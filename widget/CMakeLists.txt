cmake_minimum_required(VERSION 2.6)
project(widget CXX)

file(GLOB_RECURSE SOURCES src/*.cpp)
include_directories(
	${CMAKE_SOURCE_DIR}/lib-apps-common/inc
	${CMAKE_SOURCE_DIR}/lib-apps-common/res/apps-common/edje
	${CMAKE_SOURCE_DIR}/lib-common/inc
	${CMAKE_CURRENT_SOURCE_DIR}/inc
	${CMAKE_CURRENT_SOURCE_DIR}/res/widget/edje
)

add_executable(${PROJECT_NAME} ${SOURCES})
target_link_libraries(${PROJECT_NAME} ${LIBRARIES} -pie
	apps-common
	common
)

install(TARGETS ${PROJECT_NAME} DESTINATION ${BIN_DIR})

add_subdirectory(res/widget)
