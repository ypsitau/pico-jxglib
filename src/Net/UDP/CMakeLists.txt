add_library(jxglib_Net_UDP INTERFACE)

target_include_directories(jxglib_Net_UDP INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(jxglib_Net_UDP INTERFACE
	jxglib_Net)

target_sources(jxglib_Net_UDP INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/src/UDP.cpp)
