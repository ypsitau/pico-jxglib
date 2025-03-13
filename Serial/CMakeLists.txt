add_library(jxglib_Serial INTERFACE)

target_include_directories(jxglib_Serial INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(jxglib_Serial INTERFACE
	jxglib_VT100)

target_sources(jxglib_Serial INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/src/Serial.cpp)

add_subdirectory(TEK4010)
add_subdirectory(TelePlot)
add_subdirectory(VT100)
