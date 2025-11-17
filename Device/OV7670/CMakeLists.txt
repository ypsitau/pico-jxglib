add_library(jxglib_OV7670 INTERFACE)

target_include_directories(jxglib_OV7670 INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(jxglib_OV7670 INTERFACE
	jxglib_Common)

target_sources(jxglib_OV7670 INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/src/OV7670.cpp)
