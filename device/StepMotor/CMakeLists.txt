add_library(jxglib_StepMotor INTERFACE)

pico_generate_pio_header(jxglib_StepMotor
	${CMAKE_CURRENT_LIST_DIR}/src/RotateOut4Bits.pio)

target_include_directories(jxglib_StepMotor INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(jxglib_StepMotor INTERFACE
	hardware_pio
	jxglib_Common)

target_sources(jxglib_StepMotor INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/src/StepMotor.cpp)
