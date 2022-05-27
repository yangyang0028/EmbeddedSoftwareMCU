TARGET = Field-OrientedControl-AT32

######################################
# building variables
######################################
# debug build?
DEBUG = 1
# optimization
OPT = -Og

cur_mkfile := $(abspath $(lastword $(MAKEFILE_LIST)))
cur_makefile_path := $(patsubst %/, %, $(dir $(cur_mkfile)))
project_path := $(patsubst %/, %, $(dir $(cur_makefile_path)))
embedded_software_mcu_path := $(patsubst %/, %, $(dir $(project_path)))

ifeq ($(OS),Windows_NT)
 PLATFORM="Windows"
 GCC_PATH = $(embedded_software_mcu_path)/Tools/gcc-arm-none-eabi-win/bin
else
 ifeq ($(shell uname),Darwin)
  PLATFORM="MacOS"
 else
  PLATFORM="Unix-Like"
  GCC_PATH = $(embedded_software_mcu_path)/Tools/gcc-arm-none-eabi-unix/bin
 endif
endif

$(info PLATFORM=$(PLATFORM))

$(info cur_mkfile=$(cur_mkfile))
$(info cur_makefile_path=$(cur_makefile_path))
$(info embedded_software_mcu_path=$(embedded_software_mcu_path))

#######################################
# paths
#######################################
# Build path
BUILD_DIR = build

######################################
# source
######################################
# C sources
C_SOURCES =  \
Core/Src/at32f403a_407_clock.c \
Core/Src/at32f403a_407_int.c \
Core/Src/at32f403a_407_board.c \
$(embedded_software_mcu_path)/Libraries/AT32/drivers/src/at32f403a_407_crm.c \
$(embedded_software_mcu_path)/Libraries/AT32/cmsis/cm4/device_support/system_at32f403a_407.c \
$(embedded_software_mcu_path)/Libraries/AT32/drivers/src/at32f403a_407_gpio.c \
$(embedded_software_mcu_path)/Libraries/AT32/drivers/src/at32f403a_407_misc.c \
$(embedded_software_mcu_path)/Libraries/AT32/drivers/src/at32f403a_407_tmr.c \
$(embedded_software_mcu_path)/Libraries/AT32/drivers/src/at32f403a_407_usart.c \
$(embedded_software_mcu_path)/Libraries/AT32/drivers/src/at32f403a_407_i2c.c \
$(embedded_software_mcu_path)/Common/common.c \
$(embedded_software_mcu_path)/SoftwareLib/i2c_application_library/i2c_application.c \
$(embedded_software_mcu_path)/HardwareDriver/AS5600.c \
$(embedded_software_mcu_path)/Libraries/AT32/drivers/src/at32f403a_407_dma.c \
$(embedded_software_mcu_path)/SoftwareLib/SimpleFOC/foc_utils.c \
$(embedded_software_mcu_path)/SoftwareLib/SimpleFOC/Foc.c \
$(embedded_software_mcu_path)/SoftwareLib/Pid/Pid.c \
$(embedded_software_mcu_path)/Libraries/AT32/drivers/src/at32f403a_407_rtc.c \
$(embedded_software_mcu_path)/Libraries/AT32/drivers/src/at32f403a_407_adc.c \
main.c 

# ASM sources
ASM_SOURCES =  \
$(embedded_software_mcu_path)/Libraries/AT32/cmsis/cm4/device_support/startup/gcc/startup_at32f403a_407.s 


#######################################
# binaries
#######################################
PREFIX = arm-none-eabi-

# The gcc compiler bin path can be either defined in make command via GCC_PATH variable (> make GCC_PATH=xxx)
# either it can be added to the PATH environment variable.
ifdef GCC_PATH
CC = $(GCC_PATH)/$(PREFIX)gcc
AS = $(GCC_PATH)/$(PREFIX)gcc -x assembler-with-cpp
CP = $(GCC_PATH)/$(PREFIX)objcopy
SZ = $(GCC_PATH)/$(PREFIX)size
else
CC = $(PREFIX)gcc
AS = $(PREFIX)gcc -x assembler-with-cpp
CP = $(PREFIX)objcopy
SZ = $(PREFIX)size
endif
HEX = $(CP) -O ihex
BIN = $(CP) -O binary -S
 
#######################################
# CFLAGS
#######################################
# cpu
CPU = -mcpu=cortex-m4

# fpu
# NONE for Cortex-M0/M0+/M3

# float-abi


# mcu
MCU = $(CPU) -mthumb $(FPU) $(FLOAT-ABI)

# macros for gcc
# AS defines
AS_DEFS = 

# C defines
C_DEFS =  \
-DAT_START_F407_V1 \
-DAT32F403ARCT7 \
-DUSE_FULL_ASSERT \
-DAT32


# AS includes
AS_INCLUDES = 

# C includes
C_INCLUDES =  \
-ICore/Inc \
-I$(embedded_software_mcu_path)/Libraries/AT32/drivers/inc \
-I$(embedded_software_mcu_path)/Libraries/AT32/cmsis/dsp/include \
-I$(embedded_software_mcu_path)/Libraries/AT32/cmsis/cm4/device_support \
-I$(embedded_software_mcu_path)/Libraries/AT32/cmsis/cm4/core_support \
-I$(embedded_software_mcu_path)/Common/ \
-I$(embedded_software_mcu_path)/SoftwareLib/i2c_application_library/ \
-I$(embedded_software_mcu_path)/HardwareDriver/ \
-I$(embedded_software_mcu_path)/SoftwareLib/SimpleFOC \
-I$(embedded_software_mcu_path)/SoftwareLib/Pid

# compile gcc flags
ASFLAGS = $(MCU) $(AS_DEFS) $(AS_INCLUDES) $(OPT) -Wall -fdata-sections -ffunction-sections

CFLAGS = $(MCU) $(C_DEFS) $(C_INCLUDES) $(OPT) -Wall -fdata-sections -ffunction-sections

ifeq ($(DEBUG), 1)
CFLAGS += -g -gdwarf-2
endif


# Generate dependency information
CFLAGS += -MMD -MP -MF"$(@:%.o=%.d)"


#######################################
# LDFLAGS
#######################################
# link script
LDSCRIPT = $(embedded_software_mcu_path)/Libraries/AT32/cmsis/cm4/device_support/startup/gcc/linker/AT32F403AxG_FLASH.ld

# libraries
LIBS = \
-lc \
-lm \
-lnosys 

LIBDIR = 

LDFLAGS = $(MCU) -T$(LDSCRIPT) $(LIBDIR) $(LIBS) -Wl,-Map=$(BUILD_DIR)/$(TARGET).map,--cref -Wl,--gc-sections

# default action: build all
all: $(BUILD_DIR)/$(TARGET).elf $(BUILD_DIR)/$(TARGET).hex $(BUILD_DIR)/$(TARGET).bin


#######################################
# build the application
#######################################
# list of objects
OBJECTS = $(addprefix $(BUILD_DIR)/,$(notdir $(C_SOURCES:.c=.o)))
vpath %.c $(sort $(dir $(C_SOURCES)))
# list of ASM program objects
OBJECTS += $(addprefix $(BUILD_DIR)/,$(notdir $(ASM_SOURCES:.s=.o)))
vpath %.s $(sort $(dir $(ASM_SOURCES)))

$(BUILD_DIR)/%.o: %.c Makefile | $(BUILD_DIR) 
	$(CC) -c $(CFLAGS) -Wa,-a,-ad,-alms=$(BUILD_DIR)/$(notdir $(<:.c=.lst)) $< -o $@

$(BUILD_DIR)/%.o: %.s Makefile | $(BUILD_DIR)
	$(AS) -c $(CFLAGS) $< -o $@

$(BUILD_DIR)/$(TARGET).elf: $(OBJECTS) Makefile
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@
	$(SZ) $@

$(BUILD_DIR)/%.hex: $(BUILD_DIR)/%.elf | $(BUILD_DIR)
	$(HEX) $< $@
	
$(BUILD_DIR)/%.bin: $(BUILD_DIR)/%.elf | $(BUILD_DIR)
	$(BIN) $< $@	
	
$(BUILD_DIR):
	mkdir $@		

#######################################
# clean up
#######################################
clean:
	-rm -fR $(BUILD_DIR)
  
#######################################
# dependencies
#######################################
-include $(wildcard $(BUILD_DIR)/*.d)

# *** EOF ***
