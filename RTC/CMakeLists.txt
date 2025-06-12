add_library(jxglib_RTC INTERFACE)

target_include_directories(jxglib_RTC INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(jxglib_RTC INTERFACE
	jxglib_Common)

target_sources(jxglib_RTC INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/src/RTC.cpp)

#------------------------------------------------------------------------------
add_library(jxglib_ShellCmd_RTC INTERFACE)

target_include_directories(jxglib_ShellCmd_RTC INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(jxglib_ShellCmd_RTC INTERFACE
	jxglib_ShellCmd_Basic
	jxglib_RTC)

target_sources(jxglib_ShellCmd_RTC INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/src/ShellCmd_RTC.cpp)

#------------------------------------------------------------------------------
add_subdirectory(DS323x)
add_subdirectory(Pico)
