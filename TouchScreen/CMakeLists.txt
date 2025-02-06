add_library(jxglib_TouchScreen INTERFACE)

target_include_directories(jxglib_TouchScreen INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(jxglib_TouchScreen INTERFACE
	jxglib_Common)

target_sources(jxglib_TouchScreen INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/src/TouchScreen.cpp)

add_subdirectory(TSC2046)
