CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

SET(WIDGET_EDJ_PATH "${CMAKE_APP_RES_DIR}/widget/edje")

INSTALL(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/images DESTINATION ${CMAKE_APP_RES_DIR}/widget)

ADD_CUSTOM_TARGET(widget-layout-4x4.edj
	COMMAND edje_cc ${EDJE_CFLAGS}
	-id ${CMAKE_CURRENT_SOURCE_DIR}/..
	${CMAKE_CURRENT_SOURCE_DIR}/edje/widget-layout-4x4.edc widget-layout-4x4.edj
	DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/edje/widget-layout-4x4.edc
)
ADD_DEPENDENCIES(${PROJECT_NAME} widget-layout-4x4.edj)
INSTALL(FILES widget-layout-4x4.edj DESTINATION ${WIDGET_EDJ_PATH})

ADD_CUSTOM_TARGET(widget-layout-4x2.edj
	COMMAND edje_cc ${EDJE_CFLAGS}
	-id ${CMAKE_CURRENT_SOURCE_DIR}/..
	${CMAKE_CURRENT_SOURCE_DIR}/edje/widget-layout-4x2.edc widget-layout-4x2.edj
	DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/edje/widget-layout-4x2.edc
)
ADD_DEPENDENCIES(${PROJECT_NAME} widget-layout-4x2.edj)
INSTALL(FILES widget-layout-4x2.edj DESTINATION ${WIDGET_EDJ_PATH})

ADD_CUSTOM_TARGET(widget-item-layout.edj
	COMMAND edje_cc ${EDJE_CFLAGS}
	-id ${CMAKE_CURRENT_SOURCE_DIR}/..
	${CMAKE_CURRENT_SOURCE_DIR}/edje/widget-item-layout.edc widget-item-layout.edj
	DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/edje/widget-item-layout.edc
)
ADD_DEPENDENCIES(${PROJECT_NAME} widget-item-layout.edj)
INSTALL(FILES widget-item-layout.edj DESTINATION ${WIDGET_EDJ_PATH})
