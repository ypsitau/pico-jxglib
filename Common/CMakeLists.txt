add_library(pico_jxglib_Common INTERFACE)

target_include_directories(pico_jxglib_Common INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(pico_jxglib_Common INTERFACE)

target_sources(pico_jxglib_Common INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/src/Color.cpp
	${CMAKE_CURRENT_LIST_DIR}/src/KeyCode.cpp
	${CMAKE_CURRENT_LIST_DIR}/src/Point.cpp
	${CMAKE_CURRENT_LIST_DIR}/src/Rect.cpp
	${CMAKE_CURRENT_LIST_DIR}/src/Size.cpp
	${CMAKE_CURRENT_LIST_DIR}/src/Util.cpp
	${CMAKE_CURRENT_LIST_DIR}/src/UTF8Decoder.cpp)
