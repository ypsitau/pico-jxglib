add_library(jxglib_WiFi INTERFACE)

target_include_directories(jxglib_WiFi INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(jxglib_WiFi INTERFACE
	pico_cyw43_arch_lwip_poll
	jxglib_Common)

target_sources(jxglib_WiFi INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/src/WiFi.cpp)

#------------------------------------------------------------------------------
add_library(jxglib_ShellCmd_WiFi INTERFACE)

target_include_directories(jxglib_ShellCmd_WiFi INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(jxglib_ShellCmd_WiFi INTERFACE
	jxglib_WiFi
	jxglib_ShellCmd_Basic)

target_sources(jxglib_ShellCmd_WiFi INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/src/ShellCmd_WiFi.cpp)
