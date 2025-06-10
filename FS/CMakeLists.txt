add_library(jxglib_FS INTERFACE)

target_include_directories(jxglib_FS INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(jxglib_FS INTERFACE
	jxglib_Stream)

target_sources(jxglib_FS INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/src/FS.cpp)

add_library(jxglib_ShellCmd_FS INTERFACE)

target_include_directories(jxglib_ShellCmd_FS INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(jxglib_ShellCmd_FS INTERFACE
	jxglib_FS
	jxglib_RTC
	jxglib_ShellCmd_Basic)

target_sources(jxglib_ShellCmd_FS INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/src/ShellCmd_FS.cpp)

add_subdirectory(FAT)
add_subdirectory(LFS)
