cmake_minimum_required(VERSION 3.12)

project(jxglib C CXX ASM)

add_subdirectory(Button)
add_subdirectory(Canvas)
add_subdirectory(Common)
add_subdirectory(Display)
add_subdirectory(DMA)
add_subdirectory(Drawable)
add_subdirectory(Font)
add_subdirectory(Image)
add_subdirectory(Interp)
add_subdirectory(LineBuff)
add_subdirectory(LVGLAdapter)
add_subdirectory(PIO)
add_subdirectory(Printable)
add_subdirectory(Stream)
add_subdirectory(Terminal)
add_subdirectory(TouchScreen)
add_subdirectory(UART)

add_subdirectory(device)
