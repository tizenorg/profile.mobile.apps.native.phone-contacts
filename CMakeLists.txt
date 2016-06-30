cmake_minimum_required(VERSION 2.6)
project(${PACKAGE} CXX)

include(FindPkgConfig)
pkg_check_modules(TIZEN_PACKAGES REQUIRED
	badge
	capi-appfw-application
	capi-appfw-preference
	capi-appfw-widget-application
	capi-base-utils-i18n
	capi-system-system-settings
	capi-telephony
	contacts-service2
	dlog
	efl-extension
	elementary
	feedback
	libtzplatform-config
	notification
	phonenumber-utils
	sqlite3
	storage
	widget_service
)

set(INCLUDES      "${TIZEN_PACKAGES_INCLUDE_DIRS};inc")
set(LIBRARY_DIRS  "${TIZEN_PACKAGES_LIBRARY_DIRS}")
set(LIBRARIES     "${TIZEN_PACKAGES_LIBRARIES};dl;rt;")

include_directories(${INCLUDES})
link_directories(${LIBRARY_DIRS})

set(CMAKE_INSTALL_RPATH ${LIB_DIR})
set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)

set(COMPILER_FLAGS "-fpie -fPIC -Wall")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${COMPILER_FLAGS} -std=c++11")

add_definitions("-DTEXT_DOMAIN=\"${PACKAGE}\"")
add_definitions("-DRES_DIR=\"${RES_DIR}/\"")
add_definitions("-DLOCALE_DIR=\"${LOCALE_DIR}\"")

add_subdirectory(lib-apps-common)
add_subdirectory(lib-common)
add_subdirectory(lib-contacts)
add_subdirectory(lib-phone)
add_subdirectory(lib-logs)
add_subdirectory(contacts-app)
add_subdirectory(contacts-widget)
