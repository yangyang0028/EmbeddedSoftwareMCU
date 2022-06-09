cmake_minimum_required(VERSION 3.5)

project(AT32F403ACGU7-Test-Cmake)

set(EmbeddedSoftwareMCU  ${CMAKE_CURRENT_SOURCE_DIR}/../..)

set(arm-none-eabi ${EmbeddedSoftwareMCU}/Tools/gcc-arm-none-eabi)

include( ${EmbeddedSoftwareMCU}/Tools/GccArm.cmake)

download_and_extract(
    https://developer.arm.com/-/media/Files/downloads/gnu-rm/10.3-2021.10/gcc-arm-none-eabi-10.3-2021.10-x86_64-linux.tar.bz2    
    ${EmbeddedSoftwareMCU}/Tools/gcc-arm-none-eabi-unix.tar.bz2
    MD5
    2383e4eb4ea23f248d33adc70dc3227e
    ${arm-none-eabi}
)


set(CMAKE_C_COMPILER ${arm-none-eabi}/bin/arm-none-eabi-gcc)
set(CMAKE_OBJCOPY  ${arm-none-eabi}/bin/arm-none-eabi-objcopy)
set(CMAKE_SIZE  ${arm-none-eabi}/bin/arm-none-eabi-size)

add_definitions(
-DAT32F403ARCT7
-DAT_START_F403A_V1
-DUSE_FULL_ASSERT
)

#set(CMAKE_BUILD_TYPE "Debug")
set(CMAKE_BUILD_TYPE "Release")
set(MCU_FLAGS "-mcpu=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16")
set(CMAKE_C_FLAGS "${MCU_FLAGS} -w -Wno-unknown-pragmas")
set(CMAKE_C_FLAGS_DEBUG "-O0 -g2 -ggdb")
set(CMAKE_C_FLAGS_RELEASE "-O3")

include_directories(
    Core/Inc 
    ${EmbeddedSoftwareMCU}/Libraries/AT32/drivers/inc 
    ${EmbeddedSoftwareMCU}/Libraries/AT32/cmsis/dsp/include 
    ${EmbeddedSoftwareMCU}/Libraries/AT32/cmsis/cm4/device_support 
    ${EmbeddedSoftwareMCU}/Libraries/AT32/cmsis/cm4/core_support
    ${CMAKE_CURRENT_BINARY_DIR}
)

set_property(SOURCE ${EmbeddedSoftwareMCU}/Libraries/AT32/cmsis/cm4/device_support/startup/gcc/startup_at32f403a_407.s PROPERTY LANGUAGE C)

add_library(at32_lib
    ${EmbeddedSoftwareMCU}/Libraries/AT32/cmsis/cm4/device_support/startup/gcc/startup_at32f403a_407.s
    ${EmbeddedSoftwareMCU}/Libraries/AT32/drivers/src/at32f403a_407_crm.c 
    ${EmbeddedSoftwareMCU}/Libraries/AT32/cmsis/cm4/device_support/system_at32f403a_407.c 
    ${EmbeddedSoftwareMCU}/Libraries/AT32/drivers/src/at32f403a_407_gpio.c 
    ${EmbeddedSoftwareMCU}/Libraries/AT32/drivers/src/at32f403a_407_misc.c 
    ${EmbeddedSoftwareMCU}/Libraries/AT32/drivers/src/at32f403a_407_usart.c
)

file(GLOB SRC_CORE Core/Src/*.c)

add_library(${PROJECT_NAME}
    ${SRC_CORE}
)
target_link_libraries(${PROJECT_NAME} -lm
    at32_lib
)

set(LINKER_SCRIPT ${EmbeddedSoftwareMCU}/Libraries/AT32/cmsis/cm4/device_support/startup/gcc/linker/AT32F403AxG_FLASH.ld)
set(CMAKE_EXE_LINKER_FLAGS
"-T${LINKER_SCRIPT} -Wl,-Map=${PROJECT_BINARY_DIR}/${PROJECT_NAME}.map,--cref -Wl,--gc-sections"
)

add_executable(${PROJECT_NAME}.elf main.c)
target_link_libraries(${PROJECT_NAME}.elf
    ${PROJECT_NAME}
)

set(ELF_FILE ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.elf)
set(HEX_FILE ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.hex)
set(BIN_FILE ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.bin)

add_custom_command(TARGET "${PROJECT_NAME}.elf" POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -Obinary ${ELF_FILE} ${BIN_FILE}
    COMMAND ${CMAKE_OBJCOPY} -Oihex  ${ELF_FILE} ${HEX_FILE}
    COMMENT "Building ${PROJECT_NAME}.bin and ${PROJECT_NAME}.hex"

    COMMAND ${CMAKE_COMMAND} -E copy ${HEX_FILE} "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.hex"
    COMMAND ${CMAKE_COMMAND} -E copy ${BIN_FILE} "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.bin"

    COMMAND ${CMAKE_SIZE} --format=berkeley ${PROJECT_NAME}.elf ${PROJECT_NAME}.hex
    COMMENT "Invoking: Cross ARM GNU Print Size"
)
