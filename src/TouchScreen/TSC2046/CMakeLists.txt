add_library(jxglib_TouchScreen_TSC2046 INTERFACE)

target_include_directories(jxglib_TouchScreen_TSC2046 INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(jxglib_TouchScreen_TSC2046 INTERFACE
	hardware_spi
	jxglib_TouchScreen
	jxglib_Drawable)

target_sources(jxglib_TouchScreen_TSC2046 INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/src/TSC2046.cpp)
