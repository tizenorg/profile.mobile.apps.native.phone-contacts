cmake_minimum_required(VERSION 2.6)
project(phone CXX)

file(GLOB_RECURSE SOURCES src/*.cpp)
include_directories(
	${CMAKE_SOURCE_DIR}/lib-apps-common/inc
	${CMAKE_SOURCE_DIR}/lib-apps-common/res/apps-common/edje
	${CMAKE_SOURCE_DIR}/lib-common/inc
	${CMAKE_CURRENT_SOURCE_DIR}/res/dialer/edje
	${CMAKE_CURRENT_SOURCE_DIR}/res/speeddial/edje
	${CMAKE_CURRENT_SOURCE_DIR}/inc
)

add_library(${PROJECT_NAME} SHARED ${SOURCES})
target_link_libraries(${PROJECT_NAME} ${LIBRARIES} apps-common common)

install(TARGETS ${PROJECT_NAME} DESTINATION ${LIB_DIR})

add_subdirectory(res)
