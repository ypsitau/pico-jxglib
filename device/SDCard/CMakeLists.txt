add_library(jxglib_SDCard INTERFACE)

target_include_directories(jxglib_SDCard INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(jxglib_SDCard INTERFACE
	hardware_spi
	jxglib_Common)

target_sources(jxglib_SDCard INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/src/SDCard.cpp)

#------------------------------------------------------------------------------
add_library(jxglib_ShellCmd_SDCard INTERFACE)

target_include_directories(jxglib_ShellCmd_SDCard INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(jxglib_ShellCmd_SDCard INTERFACE
	jxglib_SDCard
	jxglib_ShellCmd_Basic)

target_sources(jxglib_ShellCmd_SDCard INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/src/ShellCmd_SDCard.cpp)
