add_library(jxglib_LogicAnalyzer INTERFACE)

target_include_directories(jxglib_LogicAnalyzer INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(jxglib_LogicAnalyzer INTERFACE
	jxglib_Common)

target_sources(jxglib_LogicAnalyzer INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/src/LogicAnalyzer.cpp)
