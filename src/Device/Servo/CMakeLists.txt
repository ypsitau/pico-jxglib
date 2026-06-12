add_library(jxglib_Device_Servo INTERFACE)

target_include_directories(jxglib_Device_Servo INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(jxglib_Device_Servo INTERFACE
	jxglib_PWM)

target_sources(jxglib_Device_Servo INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/src/SG90.cpp)

#------------------------------------------------------------------------------
add_library(jxglib_ShellCmd_Device_Servo INTERFACE)

target_include_directories(jxglib_ShellCmd_Device_Servo INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(jxglib_ShellCmd_Device_Servo INTERFACE
	jxglib_Device_Servo
	jxglib_ShellCmd_Basic)

target_sources(jxglib_ShellCmd_Device_Servo INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/src/ShellCmd_Device_Servo.cpp)
