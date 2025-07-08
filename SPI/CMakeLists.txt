add_library(jxglib_SPI INTERFACE)

target_include_directories(jxglib_SPI INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(jxglib_SPI INTERFACE
	hardware_spi
	jxglib_Common)

target_sources(jxglib_SPI INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/src/SPI.cpp)
