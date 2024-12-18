cmake_minimum_required(VERSION 3.12)

project(jxglib C CXX ASM)

add_subdirectory(AdapterLVGL)
add_subdirectory(Button)
add_subdirectory(Canvas)
add_subdirectory(Common)
add_subdirectory(Display)
add_subdirectory(Drawable)
add_subdirectory(Font)
add_subdirectory(Image)
add_subdirectory(PIO)
add_subdirectory(Printable)
add_subdirectory(Stream)
add_subdirectory(Terminal)
add_subdirectory(UART)

add_subdirectory(device)
