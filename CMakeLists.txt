cmake_minimum_required(VERSION 3.20)

set(APP_NAME NestWatcherServer)

project(${APP_NAME})

set(CMAKE_CXX_STANDARD 17)

set(SOURCE_DIR_NAME "Source")

set(SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/")
set(SUBDIRS
    ${SOURCE_DIR_NAME}
)
set(APP_SOURCE)

foreach(SUBDIR ${SUBDIRS})
	set(COMBINED_DIR ${SOURCE_DIR}${SUBDIR})
	file(GLOB GROUP_FILES "${COMBINED_DIR}/*" )
	source_group(${SUBDIR} FILES ${GROUP_FILES})
	list(APPEND APP_SOURCE ${GROUP_FILES})
endforeach()

add_executable(${APP_NAME} ${APP_SOURCE})

add_subdirectory(Thirdparty/imgui/)
target_link_libraries(${APP_NAME} PUBLIC IMGUI)
target_include_directories(${APP_NAME} PUBLIC 
    Thirdparty/imgui/Repo
)

add_subdirectory(Thirdparty/SDL/)
target_link_libraries(${APP_NAME} PUBLIC SDL3-static)
target_include_directories(${APP_NAME} PUBLIC 
    Thirdparty/SDL/include
)


target_include_directories(${APP_NAME} PUBLIC 
    ${PROJECT_SOURCE_DIR}/${SOURCE_DIR_NAME}
)