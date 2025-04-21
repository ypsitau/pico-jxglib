add_library(jxglib_Common INTERFACE)

target_include_directories(jxglib_Common INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(jxglib_Common INTERFACE
	hardware_adc
	hardware_pio
	hardware_pwm)

target_sources(jxglib_Common INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/src/Color.cpp
	${CMAKE_CURRENT_LIST_DIR}/src/FixedPool.cpp
	${CMAKE_CURRENT_LIST_DIR}/src/GPIO.cpp
	${CMAKE_CURRENT_LIST_DIR}/src/Keyboard.cpp
	${CMAKE_CURRENT_LIST_DIR}/src/KeyCode.cpp
	${CMAKE_CURRENT_LIST_DIR}/src/KeyData.cpp
	${CMAKE_CURRENT_LIST_DIR}/src/KeyLayout.cpp
	${CMAKE_CURRENT_LIST_DIR}/src/Point.cpp
	${CMAKE_CURRENT_LIST_DIR}/src/Printable.cpp
	${CMAKE_CURRENT_LIST_DIR}/src/Rect.cpp
	${CMAKE_CURRENT_LIST_DIR}/src/Size.cpp
	${CMAKE_CURRENT_LIST_DIR}/src/Stdio.cpp
	${CMAKE_CURRENT_LIST_DIR}/src/Tickable.cpp
	${CMAKE_CURRENT_LIST_DIR}/src/Tokenizer.cpp
	${CMAKE_CURRENT_LIST_DIR}/src/Util.cpp
	${CMAKE_CURRENT_LIST_DIR}/src/UTF8.cpp
	${CMAKE_CURRENT_LIST_DIR}/src/VT100.cpp
	${CMAKE_CURRENT_LIST_DIR}/src/WrappedPointer.cpp)

#------------------------------------------------------------------------------
add_library(jxglib_KeyboardTest INTERFACE)

target_include_directories(jxglib_KeyboardTest INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(jxglib_KeyboardTest INTERFACE
	jxglib_Common)

target_sources(jxglib_KeyboardTest INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/src/KeyboardTest.cpp)
