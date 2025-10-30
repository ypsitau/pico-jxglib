add_library(jxglib_Image INTERFACE)

target_include_directories(jxglib_Image INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(jxglib_Image INTERFACE
	jxglib_Common)

target_sources(jxglib_Image INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/src/Image.cpp)

#------------------------------------------------------------------------------
add_library(jxglib_ImageFile INTERFACE)

target_include_directories(jxglib_ImageFile INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(jxglib_ImageFile INTERFACE
	stb
	jxglib_FS
	jxglib_Image)

target_sources(jxglib_ImageFile INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/src/ImageFile.cpp)

#------------------------------------------------------------------------------
add_library(jxglib_ShellCmd_ImageFile INTERFACE)

target_include_directories(jxglib_ShellCmd_ImageFile INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(jxglib_ShellCmd_ImageFile INTERFACE
	jxglib_Display
	jxglib_ImageFile)

target_sources(jxglib_ShellCmd_ImageFile INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/src/ShellCmd_ImageFile.cpp)
