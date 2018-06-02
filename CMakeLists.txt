cmake_minimum_required (VERSION 3.8)
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
	"${MODULES_PATH}/zip/elzip.cpp"
	"${MODULES_PATH}/zip/unzipper.cpp"
	"${MODULES_PATH}/plist/Plist.cpp"
	"${MODULES_PATH}/plist/pugixml.cpp"
	"${MODULES_PATH}/plist/PlistDate.cpp"
)

add_executable(Starlet ${SOURCES})
target_include_directories(Starlet PRIVATE 
	${MODULES_PATH}/zip/headers
	${MODULES_PATH}/plist/include
)
target_link_libraries(Starlet stdc++fs minizip)
