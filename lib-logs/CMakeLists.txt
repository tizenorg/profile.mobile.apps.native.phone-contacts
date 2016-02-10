cmake_minimum_required(VERSION 2.6)
project(logs CXX)

file(GLOB_RECURSE SOURCES src/*.cpp)
include_directories(
	${CMAKE_SOURCE_DIR}/lib-common/inc
	${CMAKE_CURRENT_SOURCE_DIR}/inc
	${CMAKE_CURRENT_SOURCE_DIR}/res/logs/edje
)

add_library(${PROJECT_NAME} SHARED ${SOURCES})
target_link_libraries(${PROJECT_NAME} ${LIBRARIES} common)

install(TARGETS ${PROJECT_NAME} DESTINATION ${LIB_DIR})

add_subdirectory(res/logs)