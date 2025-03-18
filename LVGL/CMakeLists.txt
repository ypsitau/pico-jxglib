add_library(jxglib_LVGL INTERFACE)

#set(LV_CONF_BUILD_DISABLE_THORVG_INTERNAL ON)
#set(LV_CONF_BUILD_DISABLE_EXAMPLES ON)
#set(LV_CONF_BUILD_DISABLE_DEMOS ON)

add_subdirectory(lvgl EXCLUDE_FROM_ALL)

target_include_directories(jxglib_LVGL INTERFACE
	${CMAKE_CURRENT_LIST_DIR}
	${CMAKE_CURRENT_LIST_DIR}/include)

target_compile_definitions(jxglib_LVGL INTERFACE
	LV_CONF_PATH="${CMAKE_CURRENT_LIST_DIR}/src/lv_conf.h")

target_compile_definitions(lvgl PRIVATE
	LV_CONF_PATH="${CMAKE_CURRENT_LIST_DIR}/src/lv_conf.h")

target_compile_definitions(lvgl_examples PRIVATE
	LV_CONF_PATH="${CMAKE_CURRENT_LIST_DIR}/src/lv_conf.h")

target_compile_definitions(lvgl_demos PRIVATE
	LV_CONF_PATH="${CMAKE_CURRENT_LIST_DIR}/src/lv_conf.h")

target_compile_definitions(lvgl_thorvg PRIVATE
	LV_CONF_PATH="${CMAKE_CURRENT_LIST_DIR}/src/lv_conf.h")

target_link_libraries(jxglib_LVGL INTERFACE
	jxglib_Mouse
	jxglib_Keyboard
	jxglib_TouchScreen
	lvgl)

target_sources(jxglib_LVGL INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/src/LVGL.cpp
	${CMAKE_CURRENT_LIST_DIR}/src/Adapter.cpp)
