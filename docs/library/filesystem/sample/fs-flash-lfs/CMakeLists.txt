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
set(sdkVersion 2.2.0)
set(toolchainVersion 14_2_Rel1)
set(picotoolVersion 2.2.0-a4)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD pico CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(fs-flash-lfs C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1

add_executable(fs-flash-lfs fs-flash-lfs.cpp )

pico_set_program_name(fs-flash-lfs "fs-flash-lfs")
pico_set_program_version(fs-flash-lfs "0.1")

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(fs-flash-lfs 1)
pico_enable_stdio_usb(fs-flash-lfs 0)

# Add the standard library to the build
target_link_libraries(fs-flash-lfs
        pico_stdlib)

# Add the standard include files to the build
target_include_directories(fs-flash-lfs PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
)

pico_add_extra_outputs(fs-flash-lfs)

# mkdocs-start
target_link_libraries(fs-flash-lfs jxglib_Shell jxglib_ShellCmd_FS jxglib_Serial jxglib_LFS_Flash)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/../pico-jxglib pico-jxglib)
# mkdocs-end
