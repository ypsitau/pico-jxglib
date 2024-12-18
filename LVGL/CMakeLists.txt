add_library(jxglib_LVGL INTERFACE)

add_subdirectory(lvgl)

target_include_directories(jxglib_LVGL INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/include)

set(LV_CONF_BUILD_DISABLE_DEMOS 1)
set(LV_CONF_BUILD_DISABLE_EXAMPLES 1)

target_compile_definitions(lvgl PRIVATE
	LV_CONF_INCLUDE_SIMPLE=1
	LV_CONF_PATH="${CMAKE_CURRENT_LIST_DIR}/src/lv_conf.h")

target_link_libraries(jxglib_LVGL INTERFACE
	jxglib_Common
	lvgl)

target_sources(jxglib_LVGL INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/src/LVGL.cpp)
