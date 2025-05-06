add_library(jxglib_Flash INTERFACE)

target_include_directories(jxglib_Flash INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(jxglib_Flash INTERFACE
	hardware_flash
	jxglib_Common
	jxglib_Stream)

target_sources(jxglib_Flash INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/src/Flash.cpp
	${CMAKE_CURRENT_LIST_DIR}/src/Test.cpp)
