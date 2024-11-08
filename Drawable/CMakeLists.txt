add_library(pico_jxglib_Drawable INTERFACE)

target_include_directories(pico_jxglib_Drawable INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(pico_jxglib_Drawable INTERFACE)

target_sources(pico_jxglib_Drawable INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/src/Drawable.cpp)
