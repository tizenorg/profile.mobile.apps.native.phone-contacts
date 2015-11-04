file(GLOB PO_FILES *.po)

foreach(PO_FILE ${PO_FILES})
	get_filename_component(LANG ${PO_FILE} NAME_WE)
	set(MO_FILE ${CMAKE_CURRENT_BINARY_DIR}/${LANG}.mo)
	add_custom_command(OUTPUT ${MO_FILE}
		COMMAND msgfmt -o ${MO_FILE} ${PO_FILE}
		DEPENDS ${PO_FILE}
	)
	install(FILES ${MO_FILE} DESTINATION ${LOCALEDIR}/${LANG}/LC_MESSAGES RENAME ${PACKAGE}.mo)
	set(MO_FILES ${MO_FILES} ${MO_FILE})
endforeach(PO_FILE)

add_custom_target(po ALL DEPENDS ${MO_FILES})

