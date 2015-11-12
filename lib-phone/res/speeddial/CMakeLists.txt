set(EDCFILES speeddial-layout.edc)
set(EDJDIR "${RESDIR}/speeddial/edje")

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
