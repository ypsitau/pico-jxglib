add_library(jxglib_Net_TCP INTERFACE)

target_include_directories(jxglib_Net_TCP INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(jxglib_Net_TCP INTERFACE
	jxglib_WiFi)

target_sources(jxglib_Net_TCP INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/src/TCP.cpp)
