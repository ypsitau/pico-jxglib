add_library(jxglib_FATFS INTERFACE)

target_include_directories(jxglib_FATFS INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(jxglib_FATFS INTERFACE
	jxglib_Common)

target_sources(jxglib_FATFS INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/src/FATFS.cpp)
