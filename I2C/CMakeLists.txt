add_library(jxglib_I2C INTERFACE)

target_include_directories(jxglib_I2C INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(jxglib_I2C INTERFACE
	hardware_i2c
	jxglib_Common)

target_sources(jxglib_I2C INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/src/I2C.cpp)
