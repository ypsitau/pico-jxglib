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

project(test-HID-Keyboard-Mouse C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1

add_executable(test-HID-Keyboard-Mouse test-HID-Keyboard-Mouse.cpp)

pico_set_program_name(test-HID-Keyboard-Mouse "test-HID-Keyboard-Mouse")
pico_set_program_version(test-HID-Keyboard-Mouse "0.1")

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(test-HID-Keyboard-Mouse 1)
pico_enable_stdio_usb(test-HID-Keyboard-Mouse 0)

# Add the standard library to the build
target_link_libraries(test-HID-Keyboard-Mouse
	pico_stdlib
	tinyusb_device
	jxglib_USBD)

# Add the standard include files to the build
target_include_directories(test-HID-Keyboard-Mouse PRIVATE
	${CMAKE_CURRENT_LIST_DIR}
)

pico_add_extra_outputs(test-HID-Keyboard-Mouse)

add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/../.. pico-jxglib)
