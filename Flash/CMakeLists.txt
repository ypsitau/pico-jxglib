add_library(jxglib_Flash INTERFACE)

target_include_directories(jxglib_Flash INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(jxglib_Flash INTERFACE
	hardware_flash
	jxglib_Stream)

target_sources(jxglib_Flash INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/src/Flash.cpp)

#------------------------------------------------------------------------------
add_library(jxglib_ShellCmd_Flash INTERFACE)

target_include_directories(jxglib_ShellCmd_Flash INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(jxglib_ShellCmd_Flash INTERFACE
	jxglib_Flash
	jxglib_FS)

target_sources(jxglib_ShellCmd_Flash INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/src/ShellCmd_Flash.cpp)
