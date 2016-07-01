set(EDC_FILES group-item-layout.edc)
set(EDJ_DIR "${RES_DIR}/groups/edje")

foreach(EDC_FILE ${EDC_FILES})
	get_filename_component(EDJ_FILE ${EDC_FILE} NAME_WE)
	set(EDJ_FILE ${EDJ_FILE}.edj)

	add_custom_target(${EDJ_FILE}
		COMMAND edje_cc -id .. edje/${EDC_FILE} ${EDJ_FILE}
		DEPENDS edje/${EDC_FILE}
	)

	add_dependencies(${PROJECT_NAME} ${EDJ_FILE})
	install(FILES ${EDJ_FILE} DESTINATION ${EDJ_DIR})
endforeach(${EDC_FILE})
