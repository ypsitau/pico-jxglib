add_library(jxglib_Net INTERFACE)

target_include_directories(jxglib_Net INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(jxglib_Net INTERFACE
	jxglib_WiFi)

target_sources(jxglib_Net INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/src/Net.cpp
	${CMAKE_CURRENT_LIST_DIR}/src/NetUtil.cpp)

#------------------------------------------------------------------------------
add_library(jxglib_NetUtil INTERFACE)

target_include_directories(jxglib_NetUtil INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(jxglib_NetUtil INTERFACE
	jxglib_RTC
	jxglib_Net)

target_sources(jxglib_NetUtil INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/src/NetUtil.cpp)

#------------------------------------------------------------------------------
add_library(jxglib_ShellCmd_Net INTERFACE)

target_include_directories(jxglib_ShellCmd_Net INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(jxglib_ShellCmd_Net INTERFACE
	jxglib_RTC
	jxglib_NetUtil
	jxglib_ShellCmd_WiFi)

target_sources(jxglib_ShellCmd_Net INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/src/ShellCmd_Net.cpp)

#------------------------------------------------------------------------------
add_subdirectory(TCP)
add_subdirectory(UDP)
add_subdirectory(Telnet)
