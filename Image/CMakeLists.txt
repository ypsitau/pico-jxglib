add_library(pico_jxglib_Image INTERFACE)

target_include_directories(pico_jxglib_Image INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(pico_jxglib_Image INTERFACE)

target_sources(pico_jxglib_Image INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/src/Image.cpp)
