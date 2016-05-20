CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

SET(PH_DIALER_EDJ_PATH "${CMAKE_APP_RES_DIR}/dialer/edje")

ADD_CUSTOM_TARGET(ph-dialer-layout.edj
	COMMAND edje_cc ${EDJE_CFLAGS}
	-id ${CMAKE_CURRENT_SOURCE_DIR}/..
	${CMAKE_CURRENT_SOURCE_DIR}/edje/ph-dialer-layout.edc ph-dialer-layout.edj
	DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/edje/ph-dialer-layout.edc
)
ADD_DEPENDENCIES(ph-dialer ph-dialer-layout.edj)
INSTALL(FILES ph-dialer-layout.edj DESTINATION ${PH_DIALER_EDJ_PATH})

ADD_CUSTOM_TARGET(ph-dialer-keypad.edj
	COMMAND edje_cc ${EDJE_CFLAGS}
	-id ${CMAKE_CURRENT_SOURCE_DIR}/..
	${CMAKE_CURRENT_SOURCE_DIR}/edje/ph-dialer-keypad.edc ph-dialer-keypad.edj
	DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/edje/ph-dialer-keypad.edc
)
ADD_DEPENDENCIES(ph-dialer ph-dialer-keypad.edj)
INSTALL(FILES ph-dialer-keypad.edj DESTINATION ${PH_DIALER_EDJ_PATH})

ADD_CUSTOM_TARGET(ph-dialer-predictive.edj
	COMMAND edje_cc ${EDJE_CFLAGS}
	-id ${CMAKE_CURRENT_SOURCE_DIR}/..
	${CMAKE_CURRENT_SOURCE_DIR}/edje/ph-dialer-predictive.edc ph-dialer-predictive.edj
	DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/edje/ph-dialer-predictive.edc
)
ADD_DEPENDENCIES(ph-dialer ph-dialer-predictive.edj)
INSTALL(FILES ph-dialer-predictive.edj DESTINATION ${PH_DIALER_EDJ_PATH})
