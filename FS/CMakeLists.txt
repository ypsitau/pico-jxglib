add_library(jxglib_FS INTERFACE)

target_include_directories(jxglib_FS INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(jxglib_FS INTERFACE
	jxglib_RTC
	jxglib_Common)

target_sources(jxglib_FS INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/src/FS.cpp)

#------------------------------------------------------------------------------
add_library(jxglib_ShellCmd_FS INTERFACE)

target_include_directories(jxglib_ShellCmd_FS INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(jxglib_ShellCmd_FS INTERFACE
	jxglib_FS
	jxglib_ShellCmd_Flash
	jxglib_ShellCmd_RTC)

target_sources(jxglib_ShellCmd_FS INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/src/ShellCmd_FS.cpp)

add_subdirectory(FAT)
add_subdirectory(LFS)
