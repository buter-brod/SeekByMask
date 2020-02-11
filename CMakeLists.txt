cmake_minimum_required(VERSION 3.1)

project(SeekByMask)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR})

set(SEEKBYMASKAPP_DIR ${CMAKE_CURRENT_LIST_DIR}/src)

file(GLOB SEEKBYMASKAPP_SRC 
	${SEEKBYMASKAPP_DIR}/*.cpp
	${SEEKBYMASKAPP_DIR}/include/*.h
	)
	
set_property(DIRECTORY ${CMAKE_CURRENT_LIST_DIR} PROPERTY VS_STARTUP_PROJECT seekByMaskApp)
set(VS_STARTUP_PROJECT seekByMaskApp)

INCLUDE_DIRECTORIES(${SEEKBYMASKAPP_DIR}/include)

add_executable(seekByMaskApp ${SEEKBYMASKAPP_SRC} )
