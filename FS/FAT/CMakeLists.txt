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
		FF_FS_READONLY
		FF_FS_MINIMIZE
		FF_USE_FIND
		FF_USE_MKFS
		FF_USE_FASTSEEK
		FF_USE_EXPAND
		FF_USE_CHMOD
		FF_USE_LABEL
		FF_USE_FORWARD
		FF_USE_STRFUNC
		FF_PRINT_LLI
		FF_PRINT_FLOAT
		FF_STRF_ENCODE
		FF_CODE_PAGE
		FF_USE_LFN
		FF_MAX_LFN
		FF_LFN_UNICODE
		FF_LFN_BUF
		FF_SFN_BUF
		FF_FS_RPATH
		FF_VOLUMES
		FF_STR_VOLUME_ID
		FF_VOLUME_STRS
		FF_MULTI_PARTITION
		FF_MIN_SS
		FF_MAX_SS
		FF_LBA64
		FF_MIN_GPT
		FF_USE_TRIM
		FF_FS_TINY
		FF_FS_EXFAT
		FF_FS_NORTC
		FF_NORTC_MON
		FF_NORTC_MDAY
		FF_NORTC_YEAR
		FF_FS_NOFSINFO
		FF_FS_LOCK
		FF_FS_REENTRANT
		FF_FS_TIMEOUT
	)
	cmake_parse_arguments("" ""  "${VARS}" "" ${ARGN})
	jxglib_set_if_undefined(_FFCONF_DEF			5380)
	jxglib_set_if_undefined(_FF_FS_READONLY		0)
	jxglib_set_if_undefined(_FF_FS_MINIMIZE		0)
	jxglib_set_if_undefined(_FF_USE_FIND		0)
	jxglib_set_if_undefined(_FF_USE_MKFS		0)
	jxglib_set_if_undefined(_FF_USE_FASTSEEK	0)
	jxglib_set_if_undefined(_FF_USE_EXPAND		0)
	jxglib_set_if_undefined(_FF_USE_CHMOD		0)
	jxglib_set_if_undefined(_FF_USE_LABEL		0)
	jxglib_set_if_undefined(_FF_USE_FORWARD		0)
	jxglib_set_if_undefined(_FF_USE_STRFUNC		1)
	jxglib_set_if_undefined(_FF_PRINT_LLI		0)
	jxglib_set_if_undefined(_FF_PRINT_FLOAT		0)
	jxglib_set_if_undefined(_FF_STRF_ENCODE		3)
	jxglib_set_if_undefined(_FF_CODE_PAGE		932)
	jxglib_set_if_undefined(_FF_USE_LFN			0)
	jxglib_set_if_undefined(_FF_MAX_LFN			255)
	jxglib_set_if_undefined(_FF_LFN_UNICODE		0)
	jxglib_set_if_undefined(_FF_LFN_BUF			255)
	jxglib_set_if_undefined(_FF_SFN_BUF			12)
	jxglib_set_if_undefined(_FF_FS_RPATH		0)
	jxglib_set_if_undefined(_FF_VOLUMES			1)
	jxglib_set_if_undefined(_FF_STR_VOLUME_ID	0)
	jxglib_set_if_undefined(_FF_VOLUME_STRS		\"RAM\",\"NAND\",\"CF\",\"SD\",\"SD2\",\"USB\",\"USB2\",\"USB3\")
	jxglib_set_if_undefined(_FF_MULTI_PARTITION	0)
	jxglib_set_if_undefined(_FF_MIN_SS			512)
	jxglib_set_if_undefined(_FF_MAX_SS			512)
	jxglib_set_if_undefined(_FF_LBA64			0)
	jxglib_set_if_undefined(_FF_MIN_GPT			0x10000000)
	jxglib_set_if_undefined(_FF_USE_TRIM		0)
	jxglib_set_if_undefined(_FF_FS_TINY			0)
	jxglib_set_if_undefined(_FF_FS_EXFAT		0)
	jxglib_set_if_undefined(_FF_FS_NORTC		0)
	jxglib_set_if_undefined(_FF_NORTC_MON		11)
	jxglib_set_if_undefined(_FF_NORTC_MDAY		1)
	jxglib_set_if_undefined(_FF_NORTC_YEAR		2024)
	jxglib_set_if_undefined(_FF_FS_NOFSINFO		0)
	jxglib_set_if_undefined(_FF_FS_LOCK			0)
	jxglib_set_if_undefined(_FF_FS_REENTRANT	0)
	jxglib_set_if_undefined(_FF_FS_TIMEOUT		1000)
	configure_file(${JXGLIB_DIR}/FS/FAT/ffconf.h.in ${CMAKE_CURRENT_BINARY_DIR}/ffconf.h)
	target_include_directories(${target} PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
endfunction()
