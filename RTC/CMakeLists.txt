add_library(jxglib_RTC INTERFACE)

target_include_directories(jxglib_RTC INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(jxglib_RTC INTERFACE
	jxglib_Common)

target_sources(jxglib_RTC INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/src/RTC.cpp)

add_subdirectory(DS323x)
