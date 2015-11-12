file(GLOB PO_FILES *.po)

foreach(PO_FILE ${PO_FILES})
	get_filename_component(LANG ${PO_FILE} NAME_WE)
	set(MO_FILE ${LANG}.mo)

	add_custom_target(${MO_FILE}
		COMMAND msgfmt -o ${MO_FILE} ${PO_FILE}
		DEPENDS ${PO_FILE}
	)

	add_dependencies(${PROJECT_NAME} ${MO_FILE})
	install(FILES ${MO_FILE} DESTINATION ${LOCALEDIR}/${LANG}/LC_MESSAGES RENAME ${PACKAGE}.mo)
endforeach(PO_FILE)
