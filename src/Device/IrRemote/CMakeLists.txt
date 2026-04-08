add_library(jxglib_IrRemote INTERFACE)

target_include_directories(jxglib_IrRemote INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(jxglib_IrRemote INTERFACE
	jxglib_PIO)

target_sources(jxglib_IrRemote INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/src/IrRemote.cpp)
