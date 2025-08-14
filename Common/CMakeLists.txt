add_library(jxglib_Common INTERFACE)

target_include_directories(jxglib_Common INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(jxglib_Common INTERFACE
	hardware_adc
	hardware_pio
	hardware_pwm)

target_sources(jxglib_Common INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/src/Color.cpp
	${CMAKE_CURRENT_LIST_DIR}/src/DateTime.cpp
	${CMAKE_CURRENT_LIST_DIR}/src/Dict.cpp
	${CMAKE_CURRENT_LIST_DIR}/src/FixedPool.cpp
	${CMAKE_CURRENT_LIST_DIR}/src/GPIO.cpp
	${CMAKE_CURRENT_LIST_DIR}/src/Keyboard.cpp
	${CMAKE_CURRENT_LIST_DIR}/src/KeyCode.cpp
	${CMAKE_CURRENT_LIST_DIR}/src/KeyData.cpp
	${CMAKE_CURRENT_LIST_DIR}/src/KeyLayout.cpp
	${CMAKE_CURRENT_LIST_DIR}/src/Point.cpp
	${CMAKE_CURRENT_LIST_DIR}/src/Printable.cpp
	${CMAKE_CURRENT_LIST_DIR}/src/Readable.cpp
	${CMAKE_CURRENT_LIST_DIR}/src/Rect.cpp
	${CMAKE_CURRENT_LIST_DIR}/src/Size.cpp
	${CMAKE_CURRENT_LIST_DIR}/src/Stdio.cpp
	${CMAKE_CURRENT_LIST_DIR}/src/Stream.cpp
	${CMAKE_CURRENT_LIST_DIR}/src/Tickable.cpp
	${CMAKE_CURRENT_LIST_DIR}/src/Tokenizer.cpp
	${CMAKE_CURRENT_LIST_DIR}/src/Util.cpp
	${CMAKE_CURRENT_LIST_DIR}/src/UTF8.cpp
	${CMAKE_CURRENT_LIST_DIR}/src/VT100.cpp
	${CMAKE_CURRENT_LIST_DIR}/src/WrappedPointer.cpp)

#------------------------------------------------------------------------------
add_library(jxglib_GPIOInfo INTERFACE)

target_include_directories(jxglib_GPIOInfo INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(jxglib_GPIOInfo INTERFACE
	jxglib_Common)

target_sources(jxglib_GPIOInfo INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/src/GPIOInfo.cpp)

#------------------------------------------------------------------------------
add_library(jxglib_KeyboardTest INTERFACE)

target_include_directories(jxglib_KeyboardTest INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(jxglib_KeyboardTest INTERFACE
	jxglib_Common)

target_sources(jxglib_KeyboardTest INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/src/KeyboardTest.cpp)

#------------------------------------------------------------------------------
add_library(jxglib_BinaryInfo INTERFACE)

target_include_directories(jxglib_BinaryInfo INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(jxglib_BinaryInfo INTERFACE
	jxglib_GPIOInfo)

target_sources(jxglib_BinaryInfo INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/src/BinaryInfo.cpp)

#------------------------------------------------------------------------------
add_library(jxglib_ShellCmd_Basic INTERFACE)

target_include_directories(jxglib_ShellCmd_Basic INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(jxglib_ShellCmd_Basic INTERFACE
	jxglib_Shell
	jxglib_BinaryInfo)

target_sources(jxglib_ShellCmd_Basic INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/src/ShellCmd_Basic.cpp)

#------------------------------------------------------------------------------
add_library(jxglib_ShellCmd_GPIO INTERFACE)

target_include_directories(jxglib_ShellCmd_GPIO INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(jxglib_ShellCmd_GPIO INTERFACE
	jxglib_GPIOInfo
	jxglib_ShellCmd_Basic)

target_sources(jxglib_ShellCmd_GPIO INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/src/ShellCmd_GPIO.cpp)

#------------------------------------------------------------------------------
add_library(jxglib_ShellCmd_ADC INTERFACE)

target_include_directories(jxglib_ShellCmd_ADC INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(jxglib_ShellCmd_ADC INTERFACE
	jxglib_ShellCmd_GPIO)

target_sources(jxglib_ShellCmd_ADC INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/src/ShellCmd_ADC.cpp)
