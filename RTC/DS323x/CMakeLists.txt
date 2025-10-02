add_library(jxglib_RTC_DS323x INTERFACE)

target_include_directories(jxglib_RTC_DS323x INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(jxglib_RTC_DS323x INTERFACE
	hardware_i2c
	jxglib_RTC)

target_sources(jxglib_RTC_DS323x INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/src/DS323x.cpp)

#------------------------------------------------------------------------------
add_library(jxglib_ShellCmd_RTC_DS323x INTERFACE)

target_include_directories(jxglib_ShellCmd_RTC_DS323x INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(jxglib_ShellCmd_RTC_DS323x INTERFACE
	jxglib_ShellCmd_RTC_DS323x
	jxglib_RTC_DS323x)

target_sources(jxglib_ShellCmd_RTC_DS323x INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/src/ShellCmd_RTC_DS323x.cpp)
