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

project(test-LCD1602 C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/../../.. pico-jxglib)

# Add executable. Default name is the project name, version 0.1

add_executable(test-LCD1602 test-LCD1602.cpp )

pico_set_program_name(test-LCD1602 "test-LCD1602")
pico_set_program_version(test-LCD1602 "0.1")

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(test-LCD1602 1)
pico_enable_stdio_usb(test-LCD1602 0)

# Add the standard library to the build
target_link_libraries(test-LCD1602
	pico_stdlib
	jxglib_LCD1602)

# Add the standard include files to the build
target_include_directories(test-LCD1602 PRIVATE
	${CMAKE_CURRENT_LIST_DIR}
	${CMAKE_CURRENT_LIST_DIR}/.. # for our common lwipopts or any other standard includes, if required
)

pico_add_extra_outputs(test-LCD1602)

