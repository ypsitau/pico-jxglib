add_library(jxglib_Display_SSD1306 INTERFACE)

target_include_directories(jxglib_Display_SSD1306 INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(jxglib_Display_SSD1306 INTERFACE
	hardware_i2c
	jxglib_Display)

target_sources(jxglib_Display_SSD1306 INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/src/SSD1306.cpp)

#------------------------------------------------------------------------------
add_library(jxglib_ShellCmd_Display_SSD1306 INTERFACE)

target_include_directories(jxglib_ShellCmd_Display_SSD1306 INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(jxglib_ShellCmd_Display_SSD1306 INTERFACE
	jxglib_Display_SSD1306
	jxglib_ShellCmd_Display)

target_sources(jxglib_ShellCmd_Display_SSD1306 INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/src/ShellCmd_Display_SSD1306.cpp)
