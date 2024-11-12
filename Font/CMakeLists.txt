add_library(jxglib_Font INTERFACE)

target_include_directories(jxglib_Font INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(jxglib_Font INTERFACE)

target_sources(jxglib_Font INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/src/Font.cpp)
