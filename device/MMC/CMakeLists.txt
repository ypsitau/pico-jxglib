add_library(jxglib_MMC INTERFACE)

target_include_directories(jxglib_MMC INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(jxglib_MMC INTERFACE
	hardware_spi
	jxglib_Common)

target_sources(jxglib_MMC INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/src/MMC.cpp)
