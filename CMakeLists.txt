CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(${PKG_NAME} C CXX)

SET(CMAKE_INSTALL_RPATH ${CMAKE_APP_LIB_DIR})
SET(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)

SET(CMAKE_APP_BIN_DIR "${CMAKE_APP_HOME_DIR}/bin")
SET(CMAKE_APP_LIB_DIR "${CMAKE_APP_HOME_DIR}/lib")
SET(CMAKE_APP_RES_DIR "${CMAKE_APP_HOME_DIR}/res")

ADD_DEFINITIONS("-DRESDIR=\"${CMAKE_APP_RES_DIR}/\"")
ADD_DEFINITIONS("-DTIZEN_DEBUG_ENABLE")

FIND_PROGRAM(UNAME NAMES uname)
EXEC_PROGRAM("${UNAME}" ARGS "-m" OUTPUT_VARIABLE "ARCH")
IF("${ARCH}" STREQUAL "armv7l")
	ADD_DEFINITIONS("-DTARGET")
	SET(CMAKE_C_FLAGS_RELEASE "-mabi=aapcs-linux -O2")
ENDIF("${ARCH}" STREQUAL "armv7l")

INCLUDE(FindPkgConfig)
pkg_check_modules(TIZEN_PKGS REQUIRED
	accounts-svc
	badge
	capi-appfw-application
	capi-appfw-preference
	capi-base-utils-i18n
	capi-media-metadata-extractor
	capi-system-system-settings
	capi-system-info
	capi-telephony
	capi-ui-efl-util
	contacts-service2
	dlog
	efl-extension
	elementary
	feedback
	libtzplatform-config
	libxml-2.0
	msg-service
	notification
	storage
)

INCLUDE_DIRECTORIES(${TIZEN_PKGS_INCLUDE_DIRS})
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -Wno-format-extra-args -fPIC -std=c++0x")

# Add your submodule directory name
ADD_SUBDIRECTORY(app-assist-efl)
ADD_SUBDIRECTORY(lib-common)
ADD_SUBDIRECTORY(lib-contact)
ADD_SUBDIRECTORY(lib-phone)
ADD_SUBDIRECTORY(main-app)
ADD_SUBDIRECTORY(widget)
