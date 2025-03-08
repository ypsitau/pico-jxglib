add_library(jxglib_Editable INTERFACE)

target_include_directories(jxglib_Editable INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(jxglib_Editable INTERFACE
	jxglib_LineBuff)

target_sources(jxglib_Editable INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/src/Editable.cpp)
