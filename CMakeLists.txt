cmake_minimum_required(VERSION 2.6)
project(${PACKAGE} CXX)

include(FindPkgConfig)
pkg_check_modules(TIZEN_PACKAGES REQUIRED
	badge
	capi-appfw-application
	capi-appfw-preference
	capi-base-utils-i18n
	contacts-service2
	dlog
	efl-extension
	elementary
	feedback
	notification
)

set(INCLUDES      "${TIZEN_PACKAGES_INCLUDE_DIRS};inc")
set(LIBRARY_DIRS  "${TIZEN_PACKAGES_LIBRARY_DIRS}")
set(LIBRARIES     "${TIZEN_PACKAGES_LIBRARIES};dl;rt;")

include_directories(${INCLUDES})
link_directories(${LIBRARY_DIRS})

set(CMAKE_INSTALL_RPATH ${LIBDIR})
set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)

set(COMPILER_FLAGS "-fpie -fPIC -Wall")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${COMPILER_FLAGS} -std=c++11")

add_definitions("-DPACKAGE=\"${PACKAGE}\"")
add_definitions("-DTEXT_DOMAIN=\"${PACKAGE}\"")
add_definitions("-DRESDIR=\"${RESDIR}/\"")
add_definitions("-DLOCALEDIR=\"${LOCALEDIR}\"")

add_subdirectory(lib-common)
add_subdirectory(lib-contact)
add_subdirectory(lib-phone)
add_subdirectory(lib-logs)
add_subdirectory(main-app)
