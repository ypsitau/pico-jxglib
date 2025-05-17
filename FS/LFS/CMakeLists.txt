add_library(jxglib_LFS INTERFACE)

target_sources(jxglib_LFS INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/littlefs/lfs.c
	${CMAKE_CURRENT_LIST_DIR}/littlefs/lfs_util.c
	${CMAKE_CURRENT_LIST_DIR}/src/LFS.cpp)

target_include_directories(jxglib_LFS INTERFACE
	${CMAKE_CURRENT_LIST_DIR}
	${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(jxglib_LFS INTERFACE
	jxglib_FS)

#------------------------------------------------------------------------------
add_library(jxglib_LFS_Flash INTERFACE)

target_sources(jxglib_LFS_Flash INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/src/LFS_Flash.cpp)

target_include_directories(jxglib_LFS_Flash INTERFACE
	${CMAKE_CURRENT_LIST_DIR}
	${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(jxglib_LFS_Flash INTERFACE
	jxglib_Flash
	jxglib_LFS)
