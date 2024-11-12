add_library(jxglib_Button INTERFACE)

target_include_directories(jxglib_Button INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(jxglib_Button INTERFACE
	jxglib_Common)

target_sources(jxglib_Button INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/src/Button.cpp)
