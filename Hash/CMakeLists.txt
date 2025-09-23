add_library(jxglib_Hash INTERFACE)

target_include_directories(jxglib_Hash INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/guest
	${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(jxglib_Hash INTERFACE
	jxglib_Common)

target_sources(jxglib_Hash INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/src/Hash.cpp)
