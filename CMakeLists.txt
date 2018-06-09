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

set(ARMV6SRC
	"${STARLET_ARMV6_PATH}/Misc/Application.cpp"
	"${STARLET_ARMV6_PATH}/Misc/Class.cpp"
	"${STARLET_ARMV6_PATH}/Misc/File.cpp"
	"${STARLET_ARMV6_PATH}/Misc/FuncExec.cpp"
	"${STARLET_ARMV6_PATH}/Misc/MemBlock.cpp"
	"${STARLET_ARMV6_PATH}/Misc/MiscFunc.cpp"
	"${STARLET_ARMV6_PATH}/Misc/NibReader.cpp"
	"${STARLET_ARMV6_PATH}/Misc/Objc.cpp"
	"${STARLET_ARMV6_PATH}/Misc/Object.cpp"
	"${STARLET_ARMV6_PATH}/Misc/PlistBReader.cpp"
	"${STARLET_ARMV6_PATH}/Misc/Port.cpp"
	"${STARLET_ARMV6_PATH}/Terrible Arm Interpreter/Decode.inl"
	"${STARLET_ARMV6_PATH}/Terrible Arm Interpreter/Inst_Bits.inl"
	"${STARLET_ARMV6_PATH}/Terrible Arm Interpreter/Inst_Branch.inl"
	"${STARLET_ARMV6_PATH}/Terrible Arm Interpreter/Inst_Data.inl"
	"${STARLET_ARMV6_PATH}/Terrible Arm Interpreter/Inst_Flags.inl"
	"${STARLET_ARMV6_PATH}/Terrible Arm Interpreter/Inst_Mem.inl"
	"${STARLET_ARMV6_PATH}/Terrible Arm Interpreter/Inst_Misc.inl"
	"${STARLET_ARMV6_PATH}/Terrible Arm Interpreter/Inst_Multiple.inl"
	"${STARLET_ARMV6_PATH}/Terrible Arm Interpreter/Inst_Multiply.inl"
	"${STARLET_ARMV6_PATH}/Terrible Arm Interpreter/Inst_Packed.inl"
	"${STARLET_ARMV6_PATH}/Terrible Arm Interpreter/Inst_Shift.inl"
	"${STARLET_ARMV6_PATH}/Terrible Arm Interpreter/Inst_Vfp.inl"
	"${STARLET_ARMV6_PATH}/Terrible Arm Interpreter/Inst_VfpMem.inl"
	"${STARLET_ARMV6_PATH}/Kernel/BsdKernel/MemMap.cpp"
	"${STARLET_ARMV6_PATH}/Kernel/BsdKernel/Pthread.cpp"
	"${STARLET_ARMV6_PATH}/Kernel/BsdKernel/SysCtl.cpp"
	"${STARLET_ARMV6_PATH}/Kernel/Devices/Devices.cpp"
	"${STARLET_ARMV6_PATH}/Kernel/FileSystem/BsdFile.cpp"
	"${STARLET_ARMV6_PATH}/Kernel/FileSystem/BsdRegFile.cpp"
	"${STARLET_ARMV6_PATH}/Kernel/FileSystem/BsdSeekable.cpp"
	"${STARLET_ARMV6_PATH}/Kernel/HLE/Bootstrap.cpp"
	"${STARLET_ARMV6_PATH}/Kernel/HLE/MachOHLE.cpp"
	"${STARLET_ARMV6_PATH}/Kernel/IOKit/FrameBufferPort.cpp"
	"${STARLET_ARMV6_PATH}/Kernel/IOKit/IoOpenPort.cpp"
	"${STARLET_ARMV6_PATH}/Kernel/IOKit/IoPortMap.cpp"
	"${STARLET_ARMV6_PATH}/Kernel/IOKit/IoServicePort.cpp"
	"${STARLET_ARMV6_PATH}/Kernel/IOKit/Mbx2D.cpp"
	"${STARLET_ARMV6_PATH}/Kernel/IOKit/MbxPort.cpp"
	"${STARLET_ARMV6_PATH}/Kernel/IOKit/Plist.cpp"
	"${STARLET_ARMV6_PATH}/Kernel/IOKit/Surface.cpp"
	"${STARLET_ARMV6_PATH}/Kernel/IPC/IpcMessage.cpp"
	"${STARLET_ARMV6_PATH}/Kernel/IPC/IpcName.cpp"
	"${STARLET_ARMV6_PATH}/Kernel/IPC/IpcPort.cpp"
	"${STARLET_ARMV6_PATH}/Kernel/IPC/IpcRight.cpp"
	"${STARLET_ARMV6_PATH}/Kernel/IPC/MsgRight.cpp"
	"${STARLET_ARMV6_PATH}/Kernel/MachKernel/Task.cpp"
	"${STARLET_ARMV6_PATH}/Kernel/MachKernel/Thread.cpp"
	"${STARLET_ARMV6_PATH}/Kernel/MachKernel/Timer.cpp"
	"${STARLET_ARMV6_PATH}/Kernel/MachKernel/Waitable.cpp"
	"${STARLET_ARMV6_PATH}/Kernel/Mach-O/DebugSymbols.cpp"
	"${STARLET_ARMV6_PATH}/Kernel/Mach-O/MachoReader.cpp"
	"${STARLET_ARMV6_PATH}/Kernel/Ports/ClockPort.cpp"
	"${STARLET_ARMV6_PATH}/Kernel/Ports/TaskPort.cpp"
	"${STARLET_ARMV6_PATH}/Kernel/Ports/ThreadPort.cpp"
	"${STARLET_ARMV6_PATH}/Kernel/Socket/BsdSocket.cpp"
	"${STARLET_ARMV6_PATH}/Kernel/Socket/DgramSocket.cpp"
	"${STARLET_ARMV6_PATH}/Kernel/Socket/StreamSocket.cpp"
	"${STARLET_ARMV6_PATH}/Kernel/Swi/Swi.cpp"
	"${STARLET_ARMV6_PATH}/Kernel/Swi/SwiResume.cpp"
	"${STARLET_ARMV6_PATH}/Kernel/VirtualMem/VirtualMem.cpp"
)
ADD_LIBRARY( tinyxml STATIC ${TINYXMLSRC})

add_executable(PurpleHLE ${SOURCES} ${ARMV6SRC})
target_include_directories(PurpleHLE PRIVATE 
	${MODULES_PATH}/zip/headers
	${MODULES_PATH}/plist/include
	${STARLET_ARMV6_PATH}/headers
	${WXINCLUDE_PATH}/include
)
target_link_libraries(PurpleHLE stdc++fs minizip tinyxml)
