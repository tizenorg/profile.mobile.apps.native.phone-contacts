CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

SET(PH_SPEED_DIAL_EDJ_DIR "${CMAKE_APP_RES_DIR}/speeddial/edje")

ADD_CUSTOM_TARGET(ph-speeddial-layout.edj
	COMMAND edje_cc ${EDJE_CFLAGS}
	-id ${CMAKE_CURRENT_SOURCE_DIR}/..
	${CMAKE_CURRENT_SOURCE_DIR}/edje/ph-speeddial-layout.edc ph-speeddial-layout.edj
	DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/edje/ph-speeddial-layout.edc
)
ADD_DEPENDENCIES(ph-speeddial ph-speeddial-layout.edj)
INSTALL(FILES ph-speeddial-layout.edj DESTINATION ${PH_SPEED_DIAL_EDJ_DIR})
