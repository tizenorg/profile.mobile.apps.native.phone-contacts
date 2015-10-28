CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(ct-detail CXX)

INCLUDE_DIRECTORIES(
	${CMAKE_SOURCE_DIR}/app-assist-efl/inc
	${CMAKE_SOURCE_DIR}/lib-common/inc
	${CMAKE_SOURCE_DIR}/lib-common/res/common/edje
	${CMAKE_SOURCE_DIR}/lib-contact/ct-common/inc
	${CMAKE_SOURCE_DIR}/lib-contact/ct-list/inc
	${CMAKE_CURRENT_SOURCE_DIR}/inc
	${CMAKE_CURRENT_SOURCE_DIR}/src
)

FILE(GLOB SRCS "src/*.cpp")
ADD_LIBRARY(${PROJECT_NAME} SHARED ${SRCS})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${TIZEN_PKGS_LDFLAGS}
	"app-assist-efl"
	"contacts-common"
	"ct-common"
)

INSTALL(TARGETS ${PROJECT_NAME} DESTINATION ${CMAKE_APP_LIB_DIR})
