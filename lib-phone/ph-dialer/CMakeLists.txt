CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(ph-dialer CXX)

INCLUDE_DIRECTORIES(
	${CMAKE_SOURCE_DIR}/app-assist-efl/inc
	${CMAKE_SOURCE_DIR}/lib-common/inc
	${CMAKE_SOURCE_DIR}/lib-common/res/common/edje
	${CMAKE_SOURCE_DIR}/lib-phone/ph-common/inc
	${CMAKE_SOURCE_DIR}/lib-phone/ph-speeddial/inc
	${CMAKE_CURRENT_SOURCE_DIR}/src
	${CMAKE_CURRENT_SOURCE_DIR}/inc
	${CMAKE_CURRENT_SOURCE_DIR}/inc/predictive-number
	${CMAKE_CURRENT_SOURCE_DIR}/res/dialer/edje
)

FILE(GLOB_RECURSE SRCS "src/*.cpp")
ADD_LIBRARY(${PROJECT_NAME} SHARED ${SRCS})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${TIZEN_PKGS_LDFLAGS}
	"app-assist-efl"
	"contacts-common"
	"ph-common"
	"ph-speeddial"
)

INSTALL(TARGETS ${PROJECT_NAME} DESTINATION ${CMAKE_APP_LIB_DIR})

ADD_SUBDIRECTORY(res/dialer)
