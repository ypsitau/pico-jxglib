add_library(jxglib_AdapterLVGL INTERFACE)

add_subdirectory(lvgl)

target_include_directories(jxglib_AdapterLVGL INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/include)

target_compile_definitions(lvgl PRIVATE
	LV_CONF_PATH="${CMAKE_CURRENT_LIST_DIR}/src/lv_conf.h")

target_compile_definitions(lvgl_examples PRIVATE
	LV_CONF_PATH="${CMAKE_CURRENT_LIST_DIR}/src/lv_conf.h")

target_compile_definitions(lvgl_demos PRIVATE
	LV_CONF_PATH="${CMAKE_CURRENT_LIST_DIR}/src/lv_conf.h")

target_compile_definitions(lvgl_thorvg PRIVATE
	LV_CONF_PATH="${CMAKE_CURRENT_LIST_DIR}/src/lv_conf.h")

target_compile_definitions(jxglib_AdapterLVGL INTERFACE
	LV_CONF_PATH="${CMAKE_CURRENT_LIST_DIR}/src/lv_conf.h")

target_link_libraries(jxglib_AdapterLVGL INTERFACE
	jxglib_Common
	lvgl)

target_sources(jxglib_AdapterLVGL INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/src/AdapterLVGL.cpp)
