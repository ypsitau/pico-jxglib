add_library(jxglib_Device_MCP4726 INTERFACE)

target_include_directories(jxglib_Device_MCP4726 INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(jxglib_Device_MCP4726 INTERFACE
	hardware_i2c
	jxglib_Common)

target_sources(jxglib_Device_MCP4726 INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/src/MCP4726.cpp)
