add_library(jxglib_Camera INTERFACE)

target_include_directories(jxglib_Camera INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(jxglib_Camera INTERFACE
	jxglib_Common)

target_sources(jxglib_Camera INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/src/Camera.cpp)
