# raknet patch log begin
# VariableListDeltaTracker.h was changed
# ReplicaManager3.cpp was changed
# 3rdParty/raknet/Lib/LibStatic/CMakeLists.txt was changed
# 3rdParty/raknet/Source/CCRakNetSlidingWindow.cpp was changed
# raknet patch log end

set (RAKNET_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/network/3rdParty/raknet)

if(NOT ANDROID)
	if (NOT EXISTS ${CMAKE_BINARY_DIR}/raknet/build)
			file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/raknet/build)
	endif()
	set (RAKNET_BUILD_DIR ${CMAKE_BINARY_DIR}/raknet/build)
	set (RAKNET_INSTALL_DIR ${CMAKE_BINARY_DIR}/raknet/install)
	set (RAKNET_READY FALSE)

	# solve a *nix/64bit processor speciality
	if (LINUX AND CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64")
		set(EXTRA_COMPILE_OPTIONS -fPIC)
	endif ()

	# find raknet
	if (WIN32)
		message (STATUS "RakNet: prepare on Windows")
		set(RAKNET_STATIC_LIB_PATH_DEBUG "${RAKNET_BUILD_DIR}/Lib/LibStatic/Debug/RakNetLibStatic.lib")
		set(RAKNET_STATIC_LIB_PATH_RELEASE "${RAKNET_BUILD_DIR}/Lib/LibStatic/Release/RakNetLibStatic.lib")
	elseif (APPLE)
		message (STATUS "RakNet: prepare on Apple")
		set(RAKNET_STATIC_LIB_PATH_DEBUG "${RAKNET_BUILD_DIR}/Lib/LibStatic/Debug/libRakNetLibStatic.a")
		set(RAKNET_STATIC_LIB_PATH_RELEASE "${RAKNET_BUILD_DIR}/Lib/LibStatic/Release/libRakNetLibStatic.a")
	elseif (LINUX)
		message (STATUS "RakNet: prepare on Linux")
		set(RAKNET_STATIC_LIB_PATH_DEBUG "${RAKNET_BUILD_DIR}/Lib/LibStatic/libRakNetLibStatic.a")
		set(RAKNET_STATIC_LIB_PATH_RELEASE "${RAKNET_BUILD_DIR}/Lib/LibStatic/libRakNetLibStatic.a")
	endif ()

	message (STATUS "RakNet: RAKNET_STATIC_LIB_PATH_DEBUG: ${RAKNET_STATIC_LIB_PATH_DEBUG}")
	message (STATUS "RakNet: RAKNET_STATIC_LIB_PATH_RELEASE: ${RAKNET_STATIC_LIB_PATH_RELEASE}")

	if (EXISTS ${RAKNET_STATIC_LIB_PATH_DEBUG} AND EXISTS ${RAKNET_STATIC_LIB_PATH_RELEASE})
		set(RAKNET_READY TRUE)
		message (STATUS "raknet is installed" )
	else ()
		set(RAKNET_READY FALSE)
		message (STATUS "raknet is not installed" )
	endif ()

	if (NOT RAKNET_READY)
		# build raknet
		foreach (CONF ${CMAKE_CONFIGURATION_TYPES})
			message (STATUS "Configuring raknet for ${CONF}, please wait.....")
			execute_process (
					COMMAND ${CMAKE_COMMAND} -Wno-dev
					-DCMAKE_CONFIGURATION_TYPES:STRING=${CONF}
					-DCMAKE_BUILD_TYPE:STRING=${CONF}
					-DCMAKE_COMPILE_OPTIONS:STRING=${EXTRA_COMPILE_OPTIONS}
					-DCMAKE_INSTALL_PREFIX:PATH=${RAKNET_INSTALL_DIR}
					-DRAKNET_ENABLE_DLL=OFF
					-DRAKNET_ENABLE_SAMPLES=OFF
					-DRAKNET_GENERATE_INCLUDE_ONLY_DIR=ON
					-DCMAKE_OSX_ARCHITECTURES=${DS_OSX_ARCHITECTURE}
					-G ${CMAKE_GENERATOR}
					${RAKNET_SRC_DIR}
					WORKING_DIRECTORY ${RAKNET_BUILD_DIR}
					RESULT_VARIABLE error OUTPUT_VARIABLE output ERROR_VARIABLE output
					)
			reportError (output if error)
			message (STATUS "Building raknet for ${CONF}, please wait.....")
			execute_process (
				COMMAND ${CMAKE_COMMAND} --build ${RAKNET_BUILD_DIR} --config ${CONF} --target RakNetLibStatic
				WORKING_DIRECTORY ${RAKNET_BUILD_DIR}
				RESULT_VARIABLE error OUTPUT_VARIABLE output ERROR_VARIABLE output)
			reportError (output if error)
		endforeach ()
	endif ()

	# use raknet
	add_library(RAKNET STATIC IMPORTED)
	set_property(TARGET RAKNET PROPERTY IMPORTED_LOCATION_DEBUG ${RAKNET_STATIC_LIB_PATH_DEBUG})
	set_property(TARGET RAKNET PROPERTY IMPORTED_LOCATION_RELEASE ${RAKNET_STATIC_LIB_PATH_RELEASE})
	set(RAKNET_INCLUDE_DIR ${RAKNET_SRC_DIR}/Source)
else()
	set(RAKNET_READY FALSE)
	add_subdirectory(${RAKNET_SRC_DIR})
	add_library(RAKNET ALIAS RakNetLibStatic)
	set(RAKNET_INCLUDE_DIR ${RAKNET_SRC_DIR}/Source)
endif()