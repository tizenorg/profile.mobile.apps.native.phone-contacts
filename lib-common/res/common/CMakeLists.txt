set(EDCFILES common-thumbnail.edc common-buttons.edc)
set(EDJDIR "${RES_DIR}/common/edje")

foreach(EDCFILE ${EDCFILES})
	get_filename_component(EDJFILE ${EDCFILE} NAME_WE)
	set(EDJFILE ${EDJFILE}.edj)

	add_custom_target(${EDJFILE}
		COMMAND edje_cc -id .. edje/${EDCFILE} ${EDJFILE}
		DEPENDS edje/${EDCFILE}
	)

	add_dependencies(${PROJECT_NAME} ${EDJFILE})
	install(FILES ${EDJFILE} DESTINATION ${EDJDIR})
endforeach(${EDCFILE})
