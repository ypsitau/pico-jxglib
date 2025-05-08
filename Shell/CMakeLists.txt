add_library(jxglib_Shell INTERFACE)

target_include_directories(jxglib_Shell INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(jxglib_Shell INTERFACE
	jxglib_Terminal)

target_sources(jxglib_Shell INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/src/Shell.cpp)

add_library(jxglib_Shell_BuiltIn INTERFACE)

target_include_directories(jxglib_Shell_BuiltIn INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(jxglib_Shell_BuiltIn INTERFACE
	jxglib_Shell)

target_sources(jxglib_Shell_BuiltIn INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/src/Shell_BuiltIn.cpp)
