cmake_minimum_required (VERSION 3.10)
set( CMAKE_VERBOSE_MAKEFILE ON )

set(CMAKE_DISABLE_SOURCE_CHANGES ON)
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)

project (PurpleHLE)

#properly disable insource building, as CMAKE_DISABLE_IN_SOURCE_BUILD never works for me
if ("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")
    message(SEND_ERROR "Insource building is disabled.")
endif()

get_filename_component( CORE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/src/core" ABSOLUTE )
get_filename_component( MODULES_PATH "${CMAKE_CURRENT_SOURCE_DIR}/src/modules" ABSOLUTE )
get_filename_component( STARLET_PATH "${CMAKE_CURRENT_SOURCE_DIR}/src/starlet" ABSOLUTE )

set(SOURCES
	"${CORE_PATH}/main.cpp"
	"${MODULES_PATH}/11Zip/elzip.cpp"
	"${MODULES_PATH}/11Zip/unzipper.cpp"
)

add_executable(Starlet ${SOURCES})
