add_library(jxglib_Resets INTERFACE)

target_include_directories(jxglib_Resets INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(jxglib_Resets INTERFACE
	hardware_resets
	jxglib_Common)

target_sources(jxglib_Resets INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/src/Resets.cpp)

#------------------------------------------------------------------------------
add_library(jxglib_ShellCmd_Resets INTERFACE)

target_include_directories(jxglib_ShellCmd_Resets INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(jxglib_ShellCmd_Resets INTERFACE
	jxglib_ShellCmd_Resets
	jxglib_ShellCmd_Basic)

target_sources(jxglib_ShellCmd_Resets INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/src/ShellCmd_Resets.cpp)
