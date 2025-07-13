add_library(jxglib_PIOBox INTERFACE)

target_include_directories(jxglib_PIOBox INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(jxglib_PIOBox INTERFACE
	hardware_pio
	jxglib_Common)

target_sources(jxglib_PIOBox INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/src/PIOBox.cpp)
