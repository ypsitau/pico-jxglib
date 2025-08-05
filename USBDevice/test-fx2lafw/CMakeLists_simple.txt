# Simple fx2lafw implementation for USB communication reference
cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Pico SDK path
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()

set(PICO_BOARD pico CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(fx2lafw_simple C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable
add_executable(fx2lafw_simple 
        test-fx2lafw.c
        usb_descriptors.c
)

pico_set_program_name(fx2lafw_simple "fx2lafw_simple")
pico_set_program_version(fx2lafw_simple "1.0")

# Disable stdio over USB to avoid conflicts
pico_enable_stdio_uart(fx2lafw_simple 1)
pico_enable_stdio_usb(fx2lafw_simple 0)

# Link libraries - minimal set to avoid PIO dependencies
target_link_libraries(fx2lafw_simple
        pico_stdlib
        hardware_gpio
        tinyusb_device
        tinyusb_board
)

# Add include directories
target_include_directories(fx2lafw_simple PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
)

pico_add_extra_outputs(fx2lafw_simple)
