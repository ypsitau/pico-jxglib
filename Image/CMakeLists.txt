add_library(jxglib_Image INTERFACE)

target_include_directories(jxglib_Image INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(jxglib_Image INTERFACE
	jxglib_Stream)

target_sources(jxglib_Image INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/src/Image.cpp)
