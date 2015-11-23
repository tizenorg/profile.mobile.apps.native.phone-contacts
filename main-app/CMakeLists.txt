cmake_minimum_required(VERSION 2.6)
project(contacts CXX)

file(GLOB_RECURSE SOURCES src/*.cpp)
include_directories(
	${CMAKE_SOURCE_DIR}/lib-common/inc
	${CMAKE_SOURCE_DIR}/lib-contact/inc
	${CMAKE_SOURCE_DIR}/lib-phone/inc
	${CMAKE_CURRENT_SOURCE_DIR}/inc
)

add_executable(${PROJECT_NAME} ${SOURCES})
target_link_libraries(${PROJECT_NAME} ${LIBRARIES} -pie
	common
	phone
)

install(TARGETS ${PROJECT_NAME} DESTINATION ${BINDIR})

configure_file(tizen-manifest.xml.in ${PACKAGE}.xml)
configure_file(smack.manifest.in ${CMAKE_SOURCE_DIR}/${PACKAGE}.manifest)
configure_file(smack.efl.in ${PACKAGE}.efl)

install(DIRECTORY shared/res/ DESTINATION ${ICONDIR})
install(FILES ${PACKAGE}.xml DESTINATION ${MANIFESTDIR})
install(FILES ${PACKAGE}.efl DESTINATION ${SMACKDIR})

add_subdirectory(res)
