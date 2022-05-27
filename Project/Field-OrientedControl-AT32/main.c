/**
  **************************************************************************
  * @file     main.c
  * @version  v2.0.8
  * @date     2022-04-02
  * @brief    main program
  **************************************************************************
  *                       Copyright notice & Disclaimer
  *
  * The software Board Support Package (BSP) that is made available to
  * download from Artery official website is the copyrighted work of Artery.
  * Artery authorizes customers to use, copy, and distribute the BSP
  * software and its related documentation for the purpose of design and
  * development in conjunction with Artery microcontrollers. Use of the
  * software is governed by this copyright notice and the following disclaimer.
  *
  * THIS SOFTWARE IS PROVIDED ON "AS IS" BASIS WITHOUT WARRANTIES,
  * GUARANTEES OR REPRESENTATIONS OF ANY KIND. ARTERY EXPRESSLY DISCLAIMS,
  * TO THE FULLEST EXTENT PERMITTED BY LAW, ALL EXPRESS, IMPLIED OR
  * STATUTORY OR OTHER WARRANTIES, GUARANTEES OR REPRESENTATIONS,
  * INCLUDING BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY,
  * FITNESS FOR A PARTICULAR PURPOSE, OR NON-INFRINGEMENT.
  *
  **************************************************************************
  */

#include "at32f403a_407_board.h"
#include "at32f403a_407_clock.h"
#include "i2c_application.h"
#include "at32f403a_407_i2c.h"
#include "AS5600.h"
#include "common.h"
#include "Foc.h"
#include "Pid.h"

__IO uint16_t adc1_ordinary_valuetab[2] = {0};
__IO uint16_t dma_trans_complete_flag = 0;

gpio_init_type  gpio_init_struct = {0};
crm_clocks_freq_type crm_clocks_freq_struct = {0};

tmr_output_config_type tmr_oc_init_structure;

uint16_t ccr1_val = 500;
uint16_t prescaler_value = 0;

#define I2C_TIMEOUT                      0xffff

#define I2Cx_SPEED                       1000000

#define I2Cx_PORT                        I2C1
#define I2Cx_CLK                         CRM_I2C1_PERIPH_CLOCK

#define I2Cx_SCL_PIN                     GPIO_PINS_6
#define I2Cx_SCL_GPIO_PORT               GPIOB
#define I2Cx_SCL_GPIO_CLK                CRM_GPIOB_PERIPH_CLOCK

#define I2Cx_SDA_PIN                     GPIO_PINS_7
#define I2Cx_SDA_GPIO_PORT               GPIOB
#define I2Cx_SDA_GPIO_CLK                CRM_GPIOB_PERIPH_CLOCK

#define I2Cx_DMA_CLK                     CRM_DMA1_PERIPH_CLOCK
#define I2Cx_DMA_TX_CHANNEL              DMA1_CHANNEL6
#define I2Cx_DMA_TX_IRQn                 DMA1_Channel6_IRQn

#define I2Cx_DMA_RX_CHANNEL              DMA1_CHANNEL7
#define I2Cx_DMA_RX_IRQn                 DMA1_Channel7_IRQn

#define I2Cx_EVT_IRQn                    I2C1_EVT_IRQn
#define I2Cx_ERR_IRQn                    I2C1_ERR_IRQn

i2c_handle_type hi2cx;

void crm_configuration(void) {
  crm_periph_clock_enable(CRM_IOMUX_PERIPH_CLOCK, TRUE);
  crm_periph_clock_enable(CRM_TMR2_PERIPH_CLOCK, TRUE);
  crm_periph_clock_enable(CRM_GPIOA_PERIPH_CLOCK, TRUE);
  crm_periph_clock_enable(CRM_GPIOB_PERIPH_CLOCK, TRUE);
  crm_periph_clock_enable(CRM_GPIOC_PERIPH_CLOCK, TRUE);
}

void gpio_configuration(void) {
  gpio_init_type gpio_init_struct;

  gpio_default_para_init(&gpio_init_struct);

  gpio_init_struct.gpio_pins = GPIO_PINS_3 | GPIO_PINS_10;
  gpio_init_struct.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;
  gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
  gpio_init_struct.gpio_mode = GPIO_MODE_MUX;
  gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
  gpio_init(GPIOB, &gpio_init_struct);
  gpio_init_struct.gpio_pins = GPIO_PINS_15;
  gpio_init(GPIOA, &gpio_init_struct);
  gpio_init_struct.gpio_pins = GPIO_PINS_13;
  gpio_init_struct.gpio_mode = GPIO_MODE_OUTPUT;
  gpio_init(GPIOC, &gpio_init_struct);

  gpio_default_para_init(&gpio_init_struct);
  gpio_init_struct.gpio_mode = 0x03;
  gpio_init_struct.gpio_pins = GPIO_PINS_1 | GPIO_PINS_2;
  gpio_init(GPIOA, &gpio_init_struct);
}

static void dma_config(void)
{
  dma_init_type dma_init_struct;
  crm_periph_clock_enable(CRM_DMA1_PERIPH_CLOCK, TRUE);
  nvic_irq_enable(DMA1_Channel1_IRQn, 0, 0);
  dma_reset(DMA1_CHANNEL1);
  dma_default_para_init(&dma_init_struct);
  dma_init_struct.buffer_size = 2;
  dma_init_struct.direction = DMA_DIR_PERIPHERAL_TO_MEMORY;
  dma_init_struct.memory_base_addr = (uint32_t)adc1_ordinary_valuetab;
  dma_init_struct.memory_data_width = DMA_MEMORY_DATA_WIDTH_HALFWORD;
  dma_init_struct.memory_inc_enable = TRUE;
  dma_init_struct.peripheral_base_addr = (uint32_t)&(ADC1->odt);
  dma_init_struct.peripheral_data_width = DMA_PERIPHERAL_DATA_WIDTH_HALFWORD;
  dma_init_struct.peripheral_inc_enable = FALSE;
  dma_init_struct.priority = DMA_PRIORITY_HIGH;
  dma_init_struct.loop_mode_enable = TRUE;
  dma_init(DMA1_CHANNEL1, &dma_init_struct);

  dma_interrupt_enable(DMA1_CHANNEL1, DMA_FDT_INT, TRUE);
  dma_channel_enable(DMA1_CHANNEL1, TRUE);
}

static void adc_config(void)
{
  adc_base_config_type adc_base_struct;
  crm_periph_clock_enable(CRM_ADC1_PERIPH_CLOCK, TRUE);
  crm_adc_clock_div_set(CRM_ADC_DIV_8);

  /* select combine mode */
  adc_combine_mode_select(ADC_INDEPENDENT_MODE);
  adc_base_default_para_init(&adc_base_struct);
  adc_base_struct.sequence_mode = TRUE;
  adc_base_struct.repeat_mode = TRUE;
  adc_base_struct.data_align = ADC_RIGHT_ALIGNMENT;
  adc_base_struct.ordinary_channel_length = 2;
  adc_base_config(ADC1, &adc_base_struct);
  adc_ordinary_channel_set(ADC1, ADC_CHANNEL_1, 1, ADC_SAMPLETIME_7_5);
  adc_ordinary_channel_set(ADC1, ADC_CHANNEL_2, 2, ADC_SAMPLETIME_7_5);
  adc_ordinary_conversion_trigger_set(ADC1, ADC12_ORDINARY_TRIG_SOFTWARE, TRUE);
  adc_dma_mode_enable(ADC1, TRUE);

  adc_enable(ADC1, TRUE);
  adc_calibration_init(ADC1);
  while(adc_calibration_init_status_get(ADC1));
  adc_calibration_start(ADC1);
  while(adc_calibration_status_get(ADC1));
}

void AS5600Receive(uint8_t address, uint8_t *buffer, uint32_t len) {
  i2c_status_type i2c_status;
  if((i2c_status = i2c_master_receive(&hi2cx, address, buffer, len, I2C_TIMEOUT)) != I2C_OK){
    // error_handler(i2c_status);
  }
}

void AS5600Transmit(uint8_t address, uint8_t *buffer, uint32_t len) {
  i2c_status_type i2c_status;
  if((i2c_status = i2c_master_transmit(&hi2cx, address, buffer, len, I2C_TIMEOUT)) != I2C_OK) {
    // error_handler(i2c_status);
  }
}

struct AS5600 g_as5600 = {
  .Angle = 0,
  .AS5600Receive = AS5600Receive,
  .AS5600Transmit = AS5600Transmit,
};

uint32_t buffer_compare(uint8_t* buffer1, uint8_t* buffer2, uint32_t len);
void error_handler(uint32_t error_code);
void i2c_lowlevel_init(i2c_handle_type* hi2c);

/**
  * @brief  error handler program
  * @param  i2c_status
  * @retval none
  */
void error_handler(uint32_t error_code)
{
  OUTPUT("i2c %d\n", error_code);
}

/**
  * @brief  compare whether the valus of buffer 1 and buffer 2 are equal.
  * @param  buffer1: buffer 1 address.
            buffer2: buffer 2 address.
  * @retval 0: equal.
  *         1: unequal.
  */
uint32_t buffer_compare(uint8_t* buffer1, uint8_t* buffer2, uint32_t len)
{
  uint32_t i;

  for(i = 0; i < len; i++)
  {
    if(buffer1[i] != buffer2[i])
    {
      return 1;
    }
  }

  return 0;
}

/**
  * @brief  initializes peripherals used by the i2c.
  * @param  none
  * @retval none
  */
void i2c_lowlevel_init(i2c_handle_type* hi2c)
{
gpio_init_type gpio_initstructure;

  if(hi2c->i2cx == I2Cx_PORT)
  {
    /* i2c periph clock enable */
    crm_periph_clock_enable(I2Cx_CLK, TRUE);
    crm_periph_clock_enable(I2Cx_SCL_GPIO_CLK, TRUE);
    crm_periph_clock_enable(I2Cx_SDA_GPIO_CLK, TRUE);

    /* gpio configuration */
    gpio_initstructure.gpio_out_type       = GPIO_OUTPUT_OPEN_DRAIN;
    gpio_initstructure.gpio_pull           = GPIO_PULL_UP;
    gpio_initstructure.gpio_mode           = GPIO_MODE_MUX;
    gpio_initstructure.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;

    /* configure i2c pins: scl */
    gpio_initstructure.gpio_pins = I2Cx_SCL_PIN;
    gpio_init(I2Cx_SCL_GPIO_PORT, &gpio_initstructure);

    /* configure i2c pins: sda */
    gpio_initstructure.gpio_pins = I2Cx_SDA_PIN;
    gpio_init(I2Cx_SDA_GPIO_PORT, &gpio_initstructure);

    /* configure and enable i2c dma channel interrupt */
    nvic_irq_enable(I2Cx_DMA_TX_IRQn, 0, 0);
    nvic_irq_enable(I2Cx_DMA_RX_IRQn, 0, 0);

    /* i2c dma tx and rx channels configuration */
    /* enable the dma clock */
    crm_periph_clock_enable(I2Cx_DMA_CLK, TRUE);

    /* i2c dma channel configuration */
    dma_reset(hi2c->dma_tx_channel);
    dma_reset(hi2c->dma_rx_channel);

    hi2c->dma_tx_channel = I2Cx_DMA_TX_CHANNEL;
    hi2c->dma_rx_channel = I2Cx_DMA_RX_CHANNEL;

    dma_default_para_init(&hi2c->dma_init_struct);
    hi2c->dma_init_struct.peripheral_inc_enable    = FALSE;
    hi2c->dma_init_struct.memory_inc_enable        = TRUE;
    hi2c->dma_init_struct.peripheral_data_width    = DMA_PERIPHERAL_DATA_WIDTH_BYTE;
    hi2c->dma_init_struct.memory_data_width        = DMA_MEMORY_DATA_WIDTH_BYTE;
    hi2c->dma_init_struct.loop_mode_enable         = FALSE;
    hi2c->dma_init_struct.priority                 = DMA_PRIORITY_VERY_HIGH;
    hi2c->dma_init_struct.direction                = DMA_DIR_MEMORY_TO_PERIPHERAL;

    dma_init(hi2c->dma_tx_channel, &hi2c->dma_init_struct);
    dma_init(hi2c->dma_rx_channel, &hi2c->dma_init_struct);

    i2c_init(hi2c->i2cx, I2C_FSMODE_DUTY_2_1, I2Cx_SPEED);

    i2c_own_address1_set(hi2c->i2cx, I2C_ADDRESS_MODE_7BIT, 0xb0);
  }
}

void OutPutPWM(float Ta, float Tb, float Tc) {
  tmr_channel_value_set(TMR2, TMR_SELECT_CHANNEL_1, (int)(Ta * 1000));
  tmr_channel_value_set(TMR2, TMR_SELECT_CHANNEL_1, (int)(Tb * 1000));
  tmr_channel_value_set(TMR2, TMR_SELECT_CHANNEL_1, (int)(Tc * 1000));
}

void DelayMs(uint32_t ms) {
  delay_ms(ms);
}

float GetShaftAnage() {
  AS5600GetAngle(&g_as5600);
  // OUTPUT("%d\n", g_as5600.Angle);
  return g_as5600.Angle / 4096.0 * _2PI;
}


struct PID  velocity_pid = {
    .Kp = 0.7,
    .Ki = 0.001,
    .Kd = 0.0,
};

struct FOCConfig g_foc_config = {
    .controller = Type_velocity_openloop_angle,
    .voltage_power_supply = 12.0,
    .voltage_limit = 3.0,
    .voltage_sensor_align = 1,
    .velocity_limit = 20,
    .velocity_pid = &velocity_pid,
    .pole_pairs = 11,
    .zero_electric_angle = 1.254794,
    .OutPutPWM = OutPutPWM,
    .DelayMs = DelayMs,
    .GetShaftAnage = GetShaftAnage,
};

struct FOC g_foc = {
  .foc_config = &g_foc_config,
};


int main(void) {

  nvic_priority_group_config(NVIC_PRIORITY_GROUP_4);

  system_clock_config();

  at32_board_init();

  crm_configuration();

  gpio_configuration();

  uart_print_init(115200);

  dma_config();
  adc_config();

  OUTPUT("Hello Word\n");

  gpio_pin_remap_config(TMR2_MUX_11, TRUE);
  gpio_pin_remap_config(SWJTAG_MUX_010, TRUE);

  tmr_base_init(TMR2, 999, 239);
  tmr_cnt_dir_set(TMR2, TMR_COUNT_UP);
  tmr_clock_source_div_set(TMR2, TMR_CLOCK_DIV1);

  tmr_output_default_para_init(&tmr_oc_init_structure);

  tmr_oc_init_structure.oc_mode = TMR_OUTPUT_CONTROL_PWM_MODE_B;
  tmr_oc_init_structure.oc_idle_state = FALSE;
  tmr_oc_init_structure.oc_polarity = TMR_OUTPUT_ACTIVE_HIGH;
  tmr_oc_init_structure.oc_output_state = TRUE;

  tmr_output_channel_config(TMR2, TMR_SELECT_CHANNEL_1, &tmr_oc_init_structure);
  tmr_channel_value_set(TMR2, TMR_SELECT_CHANNEL_1, ccr1_val);
  tmr_output_channel_buffer_enable(TMR2, TMR_SELECT_CHANNEL_1, TRUE);

  tmr_output_channel_config(TMR2, TMR_SELECT_CHANNEL_2, &tmr_oc_init_structure);
  tmr_channel_value_set(TMR2, TMR_SELECT_CHANNEL_2, ccr1_val);
  tmr_output_channel_buffer_enable(TMR2, TMR_SELECT_CHANNEL_2, TRUE);

  tmr_output_channel_config(TMR2, TMR_SELECT_CHANNEL_3, &tmr_oc_init_structure);
  tmr_channel_value_set(TMR2, TMR_SELECT_CHANNEL_3, ccr1_val);
  tmr_output_channel_buffer_enable(TMR2, TMR_SELECT_CHANNEL_3, TRUE);

  tmr_period_buffer_enable(TMR2, TRUE);
  tmr_counter_enable(TMR2, TRUE);

  hi2cx.i2cx = I2C1;
  
  i2c_config(&hi2cx);

  DBG_OUTPUT(INFORMATION, "Field-OrientedControl");
  DBG_OUTPUT(INFORMATION, "FOCInit %d", FOCInit(&g_foc));
  DBG_OUTPUT(INFORMATION, "FOCTestZeroElectricAngle %lf", FOCTestZeroElectricAngle(&g_foc));
  while(1) {
    adc_ordinary_software_trigger_enable(ADC1, TRUE);
    while(dma_trans_complete_flag == 0);
    OUTPUT("a1=%d, a2=%d,\n", adc1_ordinary_valuetab[0], adc1_ordinary_valuetab[1]);
    GPIOC->odt ^= GPIO_PINS_13;
    FOCMove(&g_foc, 2);
  }
}

/**
  * @}
  */

/**
  * @}
  */
