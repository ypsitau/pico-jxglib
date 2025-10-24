add_library(jxglib_LCD1602 INTERFACE)

target_include_directories(jxglib_LCD1602 INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(jxglib_LCD1602 INTERFACE
	hardware_i2c
	jxglib_Common)

target_sources(jxglib_LCD1602 INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/src/LCD1602.cpp)
