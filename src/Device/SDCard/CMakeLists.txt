add_library(jxglib_Device_SDCard INTERFACE)

target_include_directories(jxglib_Device_SDCard INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(jxglib_Device_SDCard INTERFACE
	hardware_spi
	jxglib_Common)

target_sources(jxglib_Device_SDCard INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/src/SDCard.cpp)

#------------------------------------------------------------------------------
add_library(jxglib_ShellCmd_Device_SDCard INTERFACE)

target_include_directories(jxglib_ShellCmd_Device_SDCard INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(jxglib_ShellCmd_Device_SDCard INTERFACE
	jxglib_FAT_SDCard
	jxglib_Device_SDCard
	jxglib_ShellCmd_Basic)

target_sources(jxglib_ShellCmd_Device_SDCard INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/src/ShellCmd_Device_SDCard.cpp)
