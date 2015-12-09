cmake_minimum_required(VERSION 2.6)
project(contacts CXX)

file(GLOB_RECURSE SOURCES src/*.cpp)
include_directories(
	${CMAKE_SOURCE_DIR}/lib-common/inc
	${CMAKE_SOURCE_DIR}/lib-contact/inc
	${CMAKE_SOURCE_DIR}/lib-phone/inc
	${CMAKE_SOURCE_DIR}/lib-logs/inc
	${CMAKE_CURRENT_SOURCE_DIR}/inc
)

add_executable(${PROJECT_NAME} ${SOURCES})
target_link_libraries(${PROJECT_NAME} ${LIBRARIES} -pie
	common
	contact
	phone
	logs
)

install(TARGETS ${PROJECT_NAME} DESTINATION ${BIN_DIR})

configure_file(tizen-manifest.xml.in ${PACKAGE}.xml)
configure_file(smack.manifest.in ${CMAKE_SOURCE_DIR}/${PACKAGE}.manifest)

install(DIRECTORY shared/res/ DESTINATION ${ICON_DIR})
install(FILES ${PACKAGE}.xml DESTINATION ${MANIFEST_DIR})

add_subdirectory(res)
