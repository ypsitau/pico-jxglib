add_library(jxglib_Display_VideoTransmitter INTERFACE)

target_include_directories(jxglib_Display_VideoTransmitter INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(jxglib_Display_VideoTransmitter INTERFACE
	jxglib_USBDevice_VideoTransmitter
	jxglib_Display)

target_sources(jxglib_Display_VideoTransmitter INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/src/VideoTransmitter.cpp)

#------------------------------------------------------------------------------
#add_library(jxglib_ShellCmd_Display_VideoTransmitter INTERFACE)
#
#target_include_directories(jxglib_ShellCmd_Display_VideoTransmitter INTERFACE
#	${CMAKE_CURRENT_LIST_DIR}/include)
#
#target_link_libraries(jxglib_ShellCmd_Display_VideoTransmitter INTERFACE
#	jxglib_Shell
#	jxglib_Display_VideoTransmitter)
#
#target_sources(jxglib_ShellCmd_Display_VideoTransmitter INTERFACE
#	${CMAKE_CURRENT_LIST_DIR}/src/ShellCmd_Display_VideoTransmitter.cpp)
