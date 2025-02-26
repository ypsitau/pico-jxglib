add_library(jxglib_Tickable INTERFACE)

target_include_directories(jxglib_Tickable INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(jxglib_Tickable INTERFACE
	jxglib_Common)

target_sources(jxglib_Tickable INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/src/Tickable.cpp)
