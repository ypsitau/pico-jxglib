add_library(jxglib_VideoTransmitter INTERFACE)

target_include_directories(jxglib_VideoTransmitter INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(jxglib_VideoTransmitter INTERFACE
	jxglib_Common)

target_sources(jxglib_VideoTransmitter INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/src/VideoTransmitter.cpp)
