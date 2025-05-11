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

function(jxglib_configure_FAT target)
	set(VARS
		CFG_TUSB_DEBUG
	)
	cmake_parse_arguments("" ""  "${VARS}" "" ${ARGN})
	jxglib_set_if_undefined(_CFG_TUSB_DEBUG						0)
	configure_file(${JXGLIB_DIR}/FS/FAT/ffconf.h.in ${CMAKE_CURRENT_BINARY_DIR}/ffconf.h)
	target_include_directories(${target} PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
endfunction()
