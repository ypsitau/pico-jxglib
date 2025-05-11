add_library(jxglib_FAT INTERFACE)

target_sources(jxglib_FAT INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/ff/ff.c
	${CMAKE_CURRENT_LIST_DIR}/src/FAT.cpp)

target_include_directories(jxglib_FAT INTERFACE
	${CMAKE_CURRENT_LIST_DIR}
	${CMAKE_CURRENT_LIST_DIR}/ff
	${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(jxglib_FAT INTERFACE
	jxglib_FS)
