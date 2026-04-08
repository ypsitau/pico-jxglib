add_library(jxglib_SPI INTERFACE)

target_include_directories(jxglib_SPI INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(jxglib_SPI INTERFACE
	hardware_spi
	jxglib_Common)

target_sources(jxglib_SPI INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/src/SPI.cpp)

#------------------------------------------------------------------------------
add_library(jxglib_ShellCmd_SPI INTERFACE)

target_include_directories(jxglib_ShellCmd_SPI INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(jxglib_ShellCmd_SPI INTERFACE
	jxglib_SPI
	jxglib_ShellCmd_Basic)

target_sources(jxglib_ShellCmd_SPI INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/src/ShellCmd_SPI.cpp)
