add_library(jxglib_Resets INTERFACE)

target_include_directories(jxglib_Resets INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(jxglib_Resets INTERFACE
	jxglib_Common)

target_sources(jxglib_Resets INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/src/Resets.cpp)
