add_library(jxglib_TSC2046 INTERFACE)

target_include_directories(jxglib_TSC2046 INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(jxglib_TSC2046 INTERFACE
	hardware_spi
	jxglib_TouchPanel
	jxglib_Drawable)

target_sources(jxglib_TSC2046 INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/src/TSC2046.cpp)
