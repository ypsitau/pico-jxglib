add_library(jxglib_Net_Telnet INTERFACE)

target_include_directories(jxglib_Net_Telnet INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(jxglib_Net_Telnet INTERFACE
	jxglib_Net_TCP)

target_sources(jxglib_Net_Telnet INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/src/Telnet.cpp)
