CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(widget CXX)

FILE(GLOB SRCS src/*.cpp)

pkg_check_modules(TIZEN_WIDGET_PKGS REQUIRED
	capi-appfw-widget-application
	sqlite3
	widget_service
)

INCLUDE_DIRECTORIES(
	${CMAKE_SOURCE_DIR}/app-assist-efl/inc
	${CMAKE_SOURCE_DIR}/lib-common/inc
	${CMAKE_SOURCE_DIR}/lib-common/res/common/edje
	${CMAKE_CURRENT_SOURCE_DIR}/inc
	${CMAKE_CURRENT_SOURCE_DIR}/res/widget/edje
	${TIZEN_WIDGET_PKGS_INCLUDE_DIRS}
)

ADD_EXECUTABLE(${PROJECT_NAME} ${SRCS})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${TIZEN_WIDGET_PKGS_LDFLAGS} ${TIZEN_PKGS_LDFLAGS}
	"app-assist-efl"
	"contacts-common"
	"-pie -lm -lsqlite3"
)

INSTALL(TARGETS ${PROJECT_NAME} DESTINATION ${CMAKE_APP_BIN_DIR})

ADD_SUBDIRECTORY(res/widget)
