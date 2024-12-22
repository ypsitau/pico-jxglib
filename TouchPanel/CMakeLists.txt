add_library(jxglib_TouchPanel INTERFACE)

target_include_directories(jxglib_TouchPanel INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(jxglib_TouchPanel INTERFACE
	jxglib_Common)

target_sources(jxglib_TouchPanel INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/src/TouchPanel.cpp)
