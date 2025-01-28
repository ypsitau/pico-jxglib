add_library(jxglib_FATMgr INTERFACE)

target_sources(jxglib_FATMgr INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/ff15a/source/ff.c)

target_include_directories(jxglib_FATMgr INTERFACE
	${CMAKE_CURRENT_LIST_DIR}
	${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(jxglib_FATMgr INTERFACE
	jxglib_Common)

target_sources(jxglib_FATMgr INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/src/FATMgr.cpp)
