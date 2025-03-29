add_library(jxglib_Stream INTERFACE)

target_include_directories(jxglib_Stream INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(jxglib_Stream INTERFACE
	jxglib_Common)

target_sources(jxglib_Stream INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/src/Stream.cpp)
