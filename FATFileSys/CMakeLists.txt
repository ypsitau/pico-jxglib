add_library(jxglib_FATFileSys INTERFACE)

target_sources(jxglib_FATFileSys INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/ff15a/source/ff.c)

target_include_directories(jxglib_FATFileSys INTERFACE
	${CMAKE_CURRENT_LIST_DIR}
	${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(jxglib_FATFileSys INTERFACE
	jxglib_Common)

target_sources(jxglib_FATFileSys INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/src/FATFileSys.cpp)
