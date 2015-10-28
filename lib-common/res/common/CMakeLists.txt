CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

SET(COMMON_EDJ_DIR "${CMAKE_APP_RES_DIR}/common/edje")

ADD_CUSTOM_TARGET(common-thumbnail.edj
	COMMAND edje_cc ${EDJE_CFLAGS}
	-id ${CMAKE_CURRENT_SOURCE_DIR}/..
	${CMAKE_CURRENT_SOURCE_DIR}/edje/common-thumbnail.edc common-thumbnail.edj
	DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/edje/common-thumbnail.edc
)
ADD_DEPENDENCIES(${PROJECT_NAME} common-thumbnail.edj)
INSTALL(FILES common-thumbnail.edj DESTINATION ${COMMON_EDJ_DIR})

ADD_CUSTOM_TARGET(common-icons.edj
	COMMAND edje_cc ${EDJE_CFLAGS}
	-id ${CMAKE_CURRENT_SOURCE_DIR}/..
	${CMAKE_CURRENT_SOURCE_DIR}/edje/common-icons.edc common-icons.edj
	DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/edje/common-icons.edc
)
ADD_DEPENDENCIES(${PROJECT_NAME} common-icons.edj)
INSTALL(FILES common-icons.edj DESTINATION ${COMMON_EDJ_DIR})

ADD_CUSTOM_TARGET(common-buttons.edj
	COMMAND edje_cc ${EDJE_CFLAGS}
	-id ${CMAKE_CURRENT_SOURCE_DIR}/..
	${CMAKE_CURRENT_SOURCE_DIR}/edje/common-buttons.edc common-buttons.edj
	DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/edje/common-buttons.edc
)
ADD_DEPENDENCIES(${PROJECT_NAME} common-buttons.edj)
INSTALL(FILES common-buttons.edj DESTINATION ${COMMON_EDJ_DIR})
