add_library(jxglib_LABOPlatform INTERFACE)

target_include_directories(jxglib_LABOPlatform INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(jxglib_LABOPlatform INTERFACE
    jxglib_Shell jxglib_Serial
	jxglib_FAT_Flash jxglib_FAT_SDCard
	jxglib_RTC_DS3231
	jxglib_USBDevice_MSCDrive jxglib_USBDevice_CDCSerial
	jxglib_TelePlot jxglib_LogicAnalyzer)

if(PICO_BOARD MATCHES ".*_w$")
	target_link_libraries(jxglib_LABOPlatform INTERFACE
		jxglib_Net_Telnet)
endif()

target_sources(jxglib_LABOPlatform INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/src/LABOPlatform.cpp)

function(jxglib_configure_LABOPlatform TARGET)
	jxglib_configure_USBDevice(${TARGET} CFG_TUD_CDC 2 CFG_TUD_MSC 1)
	jxglib_configure_FAT(${TARGET} FF_VOLUMES 2)
	if(PICO_BOARD MATCHES ".*_w$")
		jxglib_configure_Net(${TARGET})
	endif()
endfunction()

#------------------------------------------------------------------------------
add_library(jxglib_LABOPlatform_FullCmd INTERFACE)

target_link_libraries(jxglib_LABOPlatform_FullCmd INTERFACE
	jxglib_LABOPlatform
	jxglib_ShellCmd_ADC
    jxglib_ShellCmd_Font
    jxglib_ShellCmd_FS
	jxglib_ShellCmd_GPIO
	jxglib_ShellCmd_I2C
	jxglib_ShellCmd_LED
	jxglib_ShellCmd_LogicAnalyzer
	jxglib_ShellCmd_PWM
	jxglib_ShellCmd_Resets
	jxglib_ShellCmd_SPI
	jxglib_ShellCmd_UART
)	

# Peripheral specific commands
target_link_libraries(jxglib_LABOPlatform_FullCmd INTERFACE
	jxglib_ShellCmd_Camera_OV7670
	jxglib_ShellCmd_Display_SSD1306
	jxglib_ShellCmd_Display_TFT_LCD
	jxglib_ShellCmd_Display_WS2812
	jxglib_ShellCmd_RTC
	jxglib_ShellCmd_RTC_DS3231
	jxglib_ShellCmd_Device_SDCard
	jxglib_ShellCmd_Device_WS2812
)

# Network commands for Wi-Fi enabled boards
if(PICO_BOARD MATCHES ".*_w$")
	target_link_libraries(jxglib_LABOPlatform_FullCmd INTERFACE
		jxglib_ShellCmd_Net
		jxglib_ShellCmd_NetUtil
		jxglib_ShellCmd_Net_Telnet
	)
endif()
