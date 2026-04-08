add_library(jxglib_Interp INTERFACE)

target_include_directories(jxglib_Interp INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(jxglib_Interp INTERFACE
	pico_stdlib
	hardware_interp
	jxglib_Common)

target_sources(jxglib_Interp INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/src/Interp.cpp)
