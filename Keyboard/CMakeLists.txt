add_library(jxglib_Keyboard INTERFACE)

target_include_directories(jxglib_Keyboard INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(jxglib_Keyboard INTERFACE
	jxglib_VT100)

target_sources(jxglib_Keyboard INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/src/Keyboard.cpp)
