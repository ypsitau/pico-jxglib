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
set(sdkVersion 2.1.0)
set(toolchainVersion 13_3_Rel1)
set(picotoolVersion 2.1.0)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD pico CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(USBMSC-tester C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1

add_executable(USBMSC-tester USBMSC-tester.cpp)

pico_set_program_name(USBMSC-tester "USBMSC-tester")
pico_set_program_version(USBMSC-tester "0.1")

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(USBMSC-tester 1)
pico_enable_stdio_usb(USBMSC-tester 0)

# Add the standard library to the build
target_link_libraries(USBMSC-tester
	pico_stdlib
	jxglib_Serial
	jxglib_FAT_USBMSC
	jxglib_ShellCmd_FS
	jxglib_ShellCmd_USBHost_MSC)

# Add the standard include files to the build
target_include_directories(USBMSC-tester PRIVATE
	${CMAKE_CURRENT_LIST_DIR}
)

pico_add_extra_outputs(USBMSC-tester)

add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/../.. pico-jxglib)

jxglib_configure_FAT(USBMSC-tester FF_VOLUMES 1)
jxglib_configure_USBHost(USBMSC-tester CFG_TUH_MSC 1)
