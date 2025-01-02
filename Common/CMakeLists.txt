add_library(jxglib_Common INTERFACE)

target_include_directories(jxglib_Common INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(jxglib_Common INTERFACE
	hardware_adc
	hardware_pio
	hardware_pwm)

target_sources(jxglib_Common INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/src/Color.cpp
	${CMAKE_CURRENT_LIST_DIR}/src/GPIO.cpp
	${CMAKE_CURRENT_LIST_DIR}/src/KeyCode.cpp
	${CMAKE_CURRENT_LIST_DIR}/src/LineBuff.cpp
	${CMAKE_CURRENT_LIST_DIR}/src/Point.cpp
	${CMAKE_CURRENT_LIST_DIR}/src/Rect.cpp
	${CMAKE_CURRENT_LIST_DIR}/src/Size.cpp
	${CMAKE_CURRENT_LIST_DIR}/src/Util.cpp
	${CMAKE_CURRENT_LIST_DIR}/src/UTF8Decoder.cpp
	${CMAKE_CURRENT_LIST_DIR}/src/WrappedPointer.cpp)
