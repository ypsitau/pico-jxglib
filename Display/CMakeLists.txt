add_library(jxglib_Display INTERFACE)

target_include_directories(jxglib_Display INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(jxglib_Display INTERFACE
	jxglib_Drawable)

target_sources(jxglib_Display INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/src/Display.cpp)

add_subdirectory(SSD1306)
add_subdirectory(TFT_LCD)
