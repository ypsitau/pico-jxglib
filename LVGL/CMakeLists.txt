add_library(jxglib_LVGL INTERFACE)

#set(LV_CONF_BUILD_DISABLE_THORVG_INTERNAL ON)
#set(LV_CONF_BUILD_DISABLE_EXAMPLES ON)
#set(LV_CONF_BUILD_DISABLE_DEMOS ON)

add_subdirectory(lvgl EXCLUDE_FROM_ALL)

target_include_directories(jxglib_LVGL INTERFACE
	${CMAKE_CURRENT_LIST_DIR}
	${CMAKE_CURRENT_LIST_DIR}/include)

#target_compile_definitions(jxglib_LVGL INTERFACE
#	LV_CONF_PATH="${CMAKE_CURRENT_LIST_DIR}/src/lv_conf.h")
#
#target_compile_definitions(lvgl PRIVATE
#	LV_CONF_PATH="${CMAKE_CURRENT_LIST_DIR}/src/lv_conf.h")
#
#target_compile_definitions(lvgl_examples PRIVATE
#	LV_CONF_PATH="${CMAKE_CURRENT_LIST_DIR}/src/lv_conf.h")
#
#target_compile_definitions(lvgl_demos PRIVATE
#	LV_CONF_PATH="${CMAKE_CURRENT_LIST_DIR}/src/lv_conf.h")
#
#target_compile_definitions(lvgl_thorvg PRIVATE
#	LV_CONF_PATH="${CMAKE_CURRENT_LIST_DIR}/src/lv_conf.h")

target_link_libraries(jxglib_LVGL INTERFACE
	jxglib_Mouse
	jxglib_TouchScreen
	lvgl)

target_sources(jxglib_LVGL INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/src/LVGL.cpp
	${CMAKE_CURRENT_LIST_DIR}/src/Adapter.cpp)

function(jxglib_configure_LVGL target)
	set(VARS_OPTIONS
		LV_FONT_MONTSERRAT_8
		LV_FONT_MONTSERRAT_10
		LV_FONT_MONTSERRAT_12
		LV_FONT_MONTSERRAT_14
		LV_FONT_MONTSERRAT_16
		LV_FONT_MONTSERRAT_18
		LV_FONT_MONTSERRAT_20
		LV_FONT_MONTSERRAT_22
		LV_FONT_MONTSERRAT_24
		LV_FONT_MONTSERRAT_26
		LV_FONT_MONTSERRAT_28
		LV_FONT_MONTSERRAT_30
		LV_FONT_MONTSERRAT_32
		LV_FONT_MONTSERRAT_34
		LV_FONT_MONTSERRAT_36
		LV_FONT_MONTSERRAT_38
		LV_FONT_MONTSERRAT_40
		LV_FONT_MONTSERRAT_42
		LV_FONT_MONTSERRAT_44
		LV_FONT_MONTSERRAT_46
		LV_FONT_MONTSERRAT_48
	)
	set(VARS_ONE_VALUE
		LV_FONT_DEFAULT
	)
	cmake_parse_arguments("" "${VARS_OPTIONS}"  "${VARS_ONE_VALUE}" "" ${ARGN})
	jxglib_set01(_LV_FONT_MONTSERRAT_8)
	jxglib_set01(_LV_FONT_MONTSERRAT_10)
	jxglib_set01(_LV_FONT_MONTSERRAT_12)
	set(_LV_FONT_MONTSERRAT_14 1)
	jxglib_set01(_LV_FONT_MONTSERRAT_16)
	jxglib_set01(_LV_FONT_MONTSERRAT_18)
	jxglib_set01(_LV_FONT_MONTSERRAT_20)
	jxglib_set01(_LV_FONT_MONTSERRAT_22)
	jxglib_set01(_LV_FONT_MONTSERRAT_24)
	jxglib_set01(_LV_FONT_MONTSERRAT_26)
	jxglib_set01(_LV_FONT_MONTSERRAT_28)
	jxglib_set01(_LV_FONT_MONTSERRAT_30)
	jxglib_set01(_LV_FONT_MONTSERRAT_32)
	jxglib_set01(_LV_FONT_MONTSERRAT_34)
	jxglib_set01(_LV_FONT_MONTSERRAT_36)
	jxglib_set01(_LV_FONT_MONTSERRAT_38)
	jxglib_set01(_LV_FONT_MONTSERRAT_40)
	jxglib_set01(_LV_FONT_MONTSERRAT_42)
	jxglib_set01(_LV_FONT_MONTSERRAT_44)
	jxglib_set01(_LV_FONT_MONTSERRAT_46)
	jxglib_set01(_LV_FONT_MONTSERRAT_48)
	jxglib_set_if_undefined(_LV_FONT_DEFAULT "&lv_font_montserrat_14")
	configure_file(${JXGLIB_DIR}/LVGL/lv_conf.h.in ${CMAKE_CURRENT_BINARY_DIR}/lv_conf.h)
	target_compile_definitions(jxglib_LVGL INTERFACE
		LV_CONF_PATH="${CMAKE_CURRENT_BINARY_DIR}/lv_conf.h")
	target_compile_definitions(lvgl PRIVATE
		LV_CONF_PATH="${CMAKE_CURRENT_BINARY_DIR}/lv_conf.h")
	target_compile_definitions(lvgl_examples PRIVATE
		LV_CONF_PATH="${CMAKE_CURRENT_BINARY_DIR}/lv_conf.h")
	target_compile_definitions(lvgl_demos PRIVATE
		LV_CONF_PATH="${CMAKE_CURRENT_BINARY_DIR}/lv_conf.h")
	target_compile_definitions(lvgl_thorvg PRIVATE
		LV_CONF_PATH="${CMAKE_CURRENT_BINARY_DIR}/lv_conf.h")
endfunction()
