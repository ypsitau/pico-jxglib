add_library(jxglib_DMA INTERFACE)

target_include_directories(jxglib_DMA INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(jxglib_DMA INTERFACE
	hardware_dma
	jxglib_Common)

target_sources(jxglib_DMA INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/src/DMA.cpp)
