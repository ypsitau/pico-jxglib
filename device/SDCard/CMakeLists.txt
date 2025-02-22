add_library(jxglib_SDCard INTERFACE)

target_include_directories(jxglib_SDCard INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(jxglib_SDCard INTERFACE
	hardware_spi
	jxglib_Common)

target_sources(jxglib_SDCard INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/src/SDCard.cpp)
