add_library(jxglib_I2C INTERFACE)

target_include_directories(jxglib_I2C INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(jxglib_I2C INTERFACE
	hardware_i2c
	jxglib_Common)

target_sources(jxglib_I2C INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/src/I2C.cpp)

#------------------------------------------------------------------------------
add_library(jxglib_ShellCmd_I2C INTERFACE)

target_include_directories(jxglib_ShellCmd_I2C INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(jxglib_ShellCmd_I2C INTERFACE
	jxglib_I2C
	jxglib_GPIOInfo
	jxglib_ShellCmd_Basic)

target_sources(jxglib_ShellCmd_I2C INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/src/ShellCmd_I2C.cpp)

