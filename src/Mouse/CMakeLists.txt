add_library(jxglib_Mouse INTERFACE)

target_include_directories(jxglib_Mouse INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(jxglib_Mouse INTERFACE
	jxglib_Common)

target_sources(jxglib_Mouse INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/src/Mouse.cpp)
