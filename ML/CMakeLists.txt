add_library(jxglib_ML INTERFACE)

target_include_directories(jxglib_ML INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(jxglib_ML INTERFACE
	jxglib_Common)

target_sources(jxglib_ML INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/src/ML.cpp)
