add_library(jxglib_PWM INTERFACE)

target_include_directories(jxglib_PWM INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(jxglib_PWM INTERFACE
	hardware_pwm
	jxglib_Common)

target_sources(jxglib_PWM INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/src/PWM.cpp)

#------------------------------------------------------------------------------
add_library(jxglib_ShellCmd_PWM INTERFACE)

target_include_directories(jxglib_ShellCmd_PWM INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(jxglib_ShellCmd_PWM INTERFACE
	jxglib_PWM
	jxglib_ShellCmd_GPIO)

target_sources(jxglib_ShellCmd_PWM INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/src/ShellCmd_PWM.cpp)
