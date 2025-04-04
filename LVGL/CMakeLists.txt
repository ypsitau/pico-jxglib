add_library(jxglib_LVGL INTERFACE)

#set(LV_CONF_BUILD_DISABLE_THORVG_INTERNAL ON)
#set(LV_CONF_BUILD_DISABLE_EXAMPLES ON)
#set(LV_CONF_BUILD_DISABLE_DEMOS ON)

add_subdirectory(lvgl EXCLUDE_FROM_ALL)

target_include_directories(jxglib_LVGL INTERFACE
	${CMAKE_CURRENT_LIST_DIR}
	${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(jxglib_LVGL INTERFACE
	jxglib_Mouse
	jxglib_TouchScreen
	lvgl)

target_sources(jxglib_LVGL INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/src/LVGL.cpp
	${CMAKE_CURRENT_LIST_DIR}/src/Adapter.cpp)

function(jxglib_configure_LVGL target)
	set(_LV_FONT_MONTSERRAT_8	0)
	set(_LV_FONT_MONTSERRAT_10	0)
	set(_LV_FONT_MONTSERRAT_12	0)
	set(_LV_FONT_MONTSERRAT_14	0)
	set(_LV_FONT_MONTSERRAT_16	0)
	set(_LV_FONT_MONTSERRAT_18	0)
	set(_LV_FONT_MONTSERRAT_20	0)
	set(_LV_FONT_MONTSERRAT_22	0)
	set(_LV_FONT_MONTSERRAT_24	0)
	set(_LV_FONT_MONTSERRAT_26	0)
	set(_LV_FONT_MONTSERRAT_28	0)
	set(_LV_FONT_MONTSERRAT_30	0)
	set(_LV_FONT_MONTSERRAT_32	0)
	set(_LV_FONT_MONTSERRAT_34	0)
	set(_LV_FONT_MONTSERRAT_36	0)
	set(_LV_FONT_MONTSERRAT_38	0)
	set(_LV_FONT_MONTSERRAT_40	0)
	set(_LV_FONT_MONTSERRAT_42	0)
	set(_LV_FONT_MONTSERRAT_44	0)
	set(_LV_FONT_MONTSERRAT_46	0)
	set(_LV_FONT_MONTSERRAT_48	0)
	set(fontNames ${ARGV})
	list(FILTER fontNames INCLUDE REGEX "^LV_FONT_MONTSERRAT_")
	#message("-----------------")
	#message("${fontNames}")
	set(firstFlag TRUE)
	foreach (fontName ${fontNames})
		if (${firstFlag})
			string(TOLOWER "${fontName}" fontNameLower)
			set(_LV_FONT_DEFAULT "&${fontNameLower}")
			set(firstFlag FALSE)
		endif()
		set("_${fontName}" 1)
	endforeach()
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
