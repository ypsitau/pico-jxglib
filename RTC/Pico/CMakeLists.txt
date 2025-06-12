add_library(jxglib_RTC_Pico INTERFACE)

target_include_directories(jxglib_RTC_Pico INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(jxglib_RTC_Pico INTERFACE
	hardware_rtc
	jxglib_RTC)

target_sources(jxglib_RTC_Pico INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/src/Pico.cpp)
