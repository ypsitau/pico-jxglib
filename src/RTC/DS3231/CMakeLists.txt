add_library(jxglib_RTC_DS3231 INTERFACE)

target_include_directories(jxglib_RTC_DS3231 INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(jxglib_RTC_DS3231 INTERFACE
	hardware_i2c
	jxglib_RTC)

target_sources(jxglib_RTC_DS3231 INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/src/DS3231.cpp)

#------------------------------------------------------------------------------
add_library(jxglib_ShellCmd_RTC_DS3231 INTERFACE)

target_include_directories(jxglib_ShellCmd_RTC_DS3231 INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(jxglib_ShellCmd_RTC_DS3231 INTERFACE
	jxglib_ShellCmd_RTC_DS3231
	jxglib_RTC_DS3231)

target_sources(jxglib_ShellCmd_RTC_DS3231 INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/src/ShellCmd_RTC_DS3231.cpp)
