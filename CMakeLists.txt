cmake_minimum_required(VERSION 3.1)

project(SeekByMask)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR})

set(SEEKBYMASKLIB_DIR ${CMAKE_CURRENT_LIST_DIR}/src/seekByMaskLib)
set(SEEKBYMASKAPP_DIR ${CMAKE_CURRENT_LIST_DIR}/src/seekByMaskApp)
set(SEEKBYMASKTESTS_DIR ${CMAKE_CURRENT_LIST_DIR}/src/seekByMaskTests)

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
