add_library(jxglib_JSON INTERFACE)

target_include_directories(jxglib_JSON INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(jxglib_JSON INTERFACE
	jxglib_Common)

target_sources(jxglib_JSON INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/src/JSON.cpp)
