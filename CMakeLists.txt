cmake_minimum_required(VERSION 3.1)

project(SeekByMask)

mark_as_advanced(CMAKE_INSTALL_PREFIX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR})

set(SEEKBYMASKLIB_DIR ${CMAKE_CURRENT_LIST_DIR}/src/seekByMaskLib)
set(SEEKBYMASKAPP_DIR ${CMAKE_CURRENT_LIST_DIR}/src/seekByMaskApp)
set(SEEKBYMASKTESTS_DIR ${CMAKE_CURRENT_LIST_DIR}/src/seekByMaskTests)

option(BUILD_RESOURCE_GUARD "Build with additional protection for access per each resource (usless and just slows everything)" OFF)
option(BUILD_TASK_QUEUE "Build task queue that works in separate thread and allows queued access to resource (usless and just slows everything)" OFF)

if(BUILD_TASK_QUEUE)
  add_definitions(-DTASK_QUEUE=1)
endif()

if(BUILD_RESOURCE_GUARD)
  add_definitions(-DLOCK_READ=1)
endif()

file(GLOB SEEKBYMASKAPP_SRC 
	${SEEKBYMASKAPP_DIR}/*.cpp
	${SEEKBYMASKAPP_DIR}/include/*.h
	)
	
file(GLOB SEEKBYMASKLIB_SRC 
	${SEEKBYMASKLIB_DIR}/*.cpp
	${SEEKBYMASKLIB_DIR}/include/*.h
	)

file(GLOB SEEKBYMASKTESTS_SRC 
	${SEEKBYMASKTESTS_DIR}/*.cpp
	${SEEKBYMASKTESTS_DIR}/include/*.h
	)

add_library(seekByMaskLib STATIC ${SEEKBYMASKLIB_SRC} )

set_property(DIRECTORY ${CMAKE_CURRENT_LIST_DIR} PROPERTY VS_STARTUP_PROJECT seekByMaskApp)
set(VS_STARTUP_PROJECT seekByMaskApp)

INCLUDE_DIRECTORIES(${SEEKBYMASKTESTS_DIR}/include)
INCLUDE_DIRECTORIES(${SEEKBYMASKLIB_DIR}/include)
INCLUDE_DIRECTORIES(${SEEKBYMASKAPP_DIR}/include)

add_executable(seekByMaskApp ${SEEKBYMASKAPP_SRC} )
add_executable(seekByMaskTests ${SEEKBYMASKTESTS_SRC} )

add_dependencies(seekByMaskApp seekByMaskLib)
add_dependencies(seekByMaskTests seekByMaskLib)

target_link_libraries(seekByMaskApp seekByMaskLib)
target_link_libraries(seekByMaskTests seekByMaskLib)
