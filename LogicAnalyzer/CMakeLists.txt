add_library(jxglib_LogicAnalyzer INTERFACE)

target_include_directories(jxglib_LogicAnalyzer INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(jxglib_LogicAnalyzer INTERFACE
	jxglib_DMA)

target_sources(jxglib_LogicAnalyzer INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/src/LogicAnalyzer.cpp)

#------------------------------------------------------------------------------
add_library(jxglib_ShellCmd_LogicAnalyzer INTERFACE)

target_include_directories(jxglib_ShellCmd_LogicAnalyzer INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(jxglib_ShellCmd_LogicAnalyzer INTERFACE
	jxglib_LogicAnalyzer
	jxglib_ShellCmd_Basic)

target_sources(jxglib_ShellCmd_LogicAnalyzer INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/src/ShellCmd_LogicAnalyzer.cpp)
