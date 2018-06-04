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
get_filename_component( STARLET_ARMV6_PATH "${STARLET_PATH}/armv6" ABSOLUTE )
get_filename_component( TINYXML_PATH "${MODULES_PATH}/TinyXML" ABSOLUTE )
get_filename_component( WXINCLUDE_PATH "${MODULES_PATH}/wxWidgets" ABSOLUTE )

set(SOURCES
	"${CORE_PATH}/main.cpp"
	"${MODULES_PATH}/zip/elzip.cpp"
	"${MODULES_PATH}/zip/unzipper.cpp"
	"${MODULES_PATH}/plist/Plist.cpp"
	"${MODULES_PATH}/plist/pugixml.cpp"
	"${MODULES_PATH}/plist/PlistDate.cpp"
)

set(TINYXMLSRC
	"${TINYXML_PATH}/tinystr.cpp"
	"${TINYXML_PATH}/tinyxml.cpp"
	"${TINYXML_PATH}/tinyxmlerror.cpp"
	"${TINYXML_PATH}/tinyxmlparser.cpp"
)

ADD_LIBRARY( MyStaticLib STATIC ${TINYXMLSRC})

add_executable(PurpleHLE ${SOURCES})
target_include_directories(PurpleHLE PRIVATE 
	${MODULES_PATH}/zip/headers
	${MODULES_PATH}/plist/include
	${STARLET_ARMV6_PATH}/headers
	${WXINCLUDE_PATH}/include
)
target_link_libraries(PurpleHLE stdc++fs minizip TINYXMLSRC)
