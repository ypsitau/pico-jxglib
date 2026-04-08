add_library(jxglib_MQTT INTERFACE)

target_include_directories(jxglib_MQTT INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(jxglib_MQTT INTERFACE
	jxglib_Common)

target_sources(jxglib_MQTT INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/src/MQTT.cpp)
