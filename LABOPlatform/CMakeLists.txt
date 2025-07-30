add_library(jxglib_LABOPlatform INTERFACE)

target_include_directories(jxglib_LABOPlatform INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(jxglib_LABOPlatform INTERFACE
    jxglib_Shell jxglib_Serial jxglib_FAT_Flash jxglib_RTC_DS323x
	jxglib_USBDevice_MSCDrive jxglib_USBDevice_CDCSerial
	jxglib_TelePlot jxglib_LogicAnalyzer)

target_sources(jxglib_LABOPlatform INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/src/LABOPlatform.cpp)

function(jxglib_configure_LABOPlatform TARGET)
	jxglib_configure_USBDevice(${TARGET} CFG_TUD_CDC 2 CFG_TUD_MSC 1)
	jxglib_configure_FAT(${TARGET} FF_VOLUMES 1)
endfunction()

#------------------------------------------------------------------------------
add_library(jxglib_LABOPlatform_FullCmd INTERFACE)

target_link_libraries(jxglib_LABOPlatform_FullCmd INTERFACE
	jxglib_LABOPlatform
	jxglib_ShellCmd_ADC
    jxglib_ShellCmd_FS
	jxglib_ShellCmd_GPIO
	jxglib_ShellCmd_I2C
	jxglib_ShellCmd_LogicAnalyzer
	jxglib_ShellCmd_PWM
	jxglib_ShellCmd_SPI
	jxglib_ShellCmd_UART
)	
