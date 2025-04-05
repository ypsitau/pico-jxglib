add_library(jxglib_FS INTERFACE)

target_include_directories(jxglib_FS INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(jxglib_FS INTERFACE
	jxglib_Common)

target_sources(jxglib_FS INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/src/FS.cpp)

add_subdirectory(FAT)
