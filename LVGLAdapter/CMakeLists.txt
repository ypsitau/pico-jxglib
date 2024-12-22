add_library(jxglib_LVGLAdapter INTERFACE)

add_subdirectory(lvgl)

target_include_directories(jxglib_LVGLAdapter INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/include)

target_compile_definitions(lvgl PRIVATE
	LV_CONF_PATH="${CMAKE_CURRENT_LIST_DIR}/src/lv_conf.h")

target_compile_definitions(lvgl_examples PRIVATE
	LV_CONF_PATH="${CMAKE_CURRENT_LIST_DIR}/src/lv_conf.h")

target_compile_definitions(lvgl_demos PRIVATE
	LV_CONF_PATH="${CMAKE_CURRENT_LIST_DIR}/src/lv_conf.h")

target_compile_definitions(lvgl_thorvg PRIVATE
	LV_CONF_PATH="${CMAKE_CURRENT_LIST_DIR}/src/lv_conf.h")

target_compile_definitions(jxglib_LVGLAdapter INTERFACE
	LV_CONF_PATH="${CMAKE_CURRENT_LIST_DIR}/src/lv_conf.h")

target_link_libraries(jxglib_LVGLAdapter INTERFACE
	jxglib_TouchScreen
	lvgl)

target_sources(jxglib_LVGLAdapter INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/src/LVGLAdapter.cpp)
