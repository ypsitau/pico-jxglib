# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)

# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.1.1)
set(toolchainVersion 14_2_Rel1)
set(picotoolVersion 2.1.1)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD pico CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(Labo C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1

add_executable(Labo Labo.cpp )

pico_set_program_name(Labo "Labo")
pico_set_program_version(Labo "0.0.1")

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(Labo 1)
pico_enable_stdio_usb(Labo 0)

# Add the standard library to the build
target_link_libraries(Labo
        pico_stdlib)

# Add the standard include files to the build
target_include_directories(Labo PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
)

pico_add_extra_outputs(Labo)

target_link_libraries(Labo
    jxglib_Shell jxglib_Serial
    jxglib_LFS_Flash jxglib_FAT_Flash jxglib_RTC_DS323x
	jxglib_USBDevice_MSCDrive jxglib_USBDevice_CDCSerial
    jxglib_ShellCmd_FS jxglib_ShellCmd_GPIO jxglib_ShellCmd_ADC jxglib_ShellCmd_PWM jxglib_ShellCmd_I2C jxglib_ShellCmd_SPI)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/../.. pico-jxglib)
jxglib_configure_USBDevice(Labo CFG_TUD_CDC 1 CFG_TUD_MSC 1)
jxglib_configure_FAT(Labo FF_VOLUMES 1)
