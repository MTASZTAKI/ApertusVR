# cUrl
if (WIN32)
	set (CURL_READY FALSE)

	set(CURL_SOURCE_DIR ${APE_SOURCE_DIR}/3rdParty/curl)
	set(CURL_INCLUDE_DIRS ${CURL_SOURCE_DIR}/include/)

	set(CURL_DLL_FILENAME libcurl.dll)
	set(CURL_LIB_FILENAME libcurl_imp.lib)

	set(CURL_DLL_PATH_DEBUG ${CURL_SOURCE_DIR}/lib/Debug/${CURL_DLL_FILENAME})
	set(CURL_DLL_PATH_RELEASE ${CURL_SOURCE_DIR}/lib/Release/${CURL_DLL_FILENAME})
	set(CURL_DLL_OUTPUT_PATH_DEBUG ${APE_OUTPUT_DIR_DEBUG}/${CURL_DLL_FILENAME})
	set(CURL_DLL_OUTPUT_PATH_RELEASE ${APE_OUTPUT_DIR_RELEASE}/${CURL_DLL_FILENAME})

	set(CURL_LIB_PATH_DEBUG ${CURL_SOURCE_DIR}/lib/Debug/${CURL_LIB_FILENAME})
	set(CURL_LIB_PATH_RELEASE ${CURL_SOURCE_DIR}/lib/Release/${CURL_LIB_FILENAME})
	set(CURL_LIB_OUTPUT_PATH_DEBUG ${APE_OUTPUT_DIR_DEBUG}/${CURL_LIB_FILENAME})
	set(CURL_LIB_OUTPUT_PATH_RELEASE ${APE_OUTPUT_DIR_RELEASE}/${CURL_LIB_FILENAME})

	if (EXISTS ${CURL_LIB_PATH_DEBUG} AND EXISTS ${CURL_DLL_PATH_RELEASE})
		set(CURL_READY TRUE)
		message(STATUS "curl is installed" )
	else ()
		set(CURL_READY FALSE)
		message(STATUS "curl is not installed" )
	endif ()

	if (NOT CURL_READY)
		# build curl
		foreach (CONF ${CMAKE_CONFIGURATION_TYPES})
			message(STATUS "Configuring curl for ${CONF}, please wait.....")
			execute_process(
					COMMAND ${CMAKE_COMMAND}
					-G ${CMAKE_GENERATOR}
					.
					WORKING_DIRECTORY ${CURL_SOURCE_DIR}
					RESULT_VARIABLE error OUTPUT_VARIABLE output ERROR_VARIABLE output
					)
			reportError(output if error)
			message(STATUS "Building curl for ${CONF}, please wait.....")
			execute_process(
				COMMAND ${CMAKE_COMMAND} --build . --config ${CONF} --target ALL_BUILD
				WORKING_DIRECTORY ${CURL_SOURCE_DIR}
				RESULT_VARIABLE error OUTPUT_VARIABLE output ERROR_VARIABLE output)
			reportError(output if error)
		endforeach ()
	endif ()

	# use curl
	add_library(MY_CURL STATIC IMPORTED)
	set_property(TARGET MY_CURL PROPERTY IMPORTED_LOCATION_DEBUG ${CURL_LIB_PATH_DEBUG})
	set_property(TARGET MY_CURL PROPERTY IMPORTED_LOCATION_RELEASE ${CURL_LIB_PATH_RELEASE})

	if (NOT EXISTS ${CURL_DLL_PATH_DEBUG})
		message(FATAL_ERROR "Could not find ${CURL_DLL_PATH_DEBUG}")
	endif ()
	if (NOT EXISTS ${CURL_DLL_OUTPUT_PATH_DEBUG})
		file(COPY ${CURL_DLL_PATH_DEBUG} DESTINATION ${APE_OUTPUT_DIR_DEBUG})
	else ()
		message(STATUS "curl: ${CURL_DLL_OUTPUT_PATH_DEBUG} already exists. Skipping copy.")
	endif ()

	if (NOT EXISTS ${CURL_DLL_PATH_RELEASE})
		message(FATAL_ERROR "Could not find ${CURL_DLL_PATH_RELEASE}")
	endif ()
	if (NOT EXISTS ${CURL_DLL_OUTPUT_PATH_RELASE})
		file(COPY ${CURL_DLL_PATH_RELEASE} DESTINATION ${APE_OUTPUT_DIR_RELEASE})
	else ()
		message(STATUS "curl: ${CURL_DLL_OUTPUT_PATH_RELEASE} already exists. Skipping copy.")
	endif ()

	if (NOT EXISTS ${CURL_LIB_PATH_DEBUG})
		message(FATAL_ERROR "Could not find ${CURL_LIB_PATH_DEBUG}")
	endif ()
	if (NOT EXISTS ${CURL_LIB_OUTPUT_PATH_DEBUG})
		file(COPY ${CURL_LIB_PATH_DEBUG} DESTINATION ${APE_OUTPUT_DIR_DEBUG})
	else ()
		message(STATUS "curl: ${CURL_LIB_OUTPUT_PATH_DEBUG} already exists. Skipping copy.")
	endif ()

	if (NOT EXISTS ${CURL_LIB_PATH_RELEASE})
		message(FATAL_ERROR "Could not find ${CURL_LIB_PATH_RELEASE}")
	endif ()
	if (NOT EXISTS ${CURL_LIB_OUTPUT_PATH_RELEASE})
		file(COPY ${CURL_LIB_PATH_RELEASE} DESTINATION ${APE_OUTPUT_DIR_RELEASE})
	else ()
		message(STATUS "curl: ${CURL_LIB_OUTPUT_PATH_RELEASE} already exists. Skipping copy.")
	endif ()

elseif (APPLE OR LINUX)
	find_package(CURL REQUIRED)
	include_directories(${CURL_INCLUDE_DIRS})
	message(STATUS "curl dirs: ${CURL_INCLUDE_DIRS}")
	message(STATUS "curl libs: ${CURL_LIBRARIES}")

	if (${CURL_FOUND})
		set(CURL_READY TRUE)
		add_library(MY_CURL STATIC IMPORTED)
		set_property(TARGET MY_CURL PROPERTY IMPORTED_LOCATION_DEBUG ${CURL_LIBRARIES})
		set_property(TARGET MY_CURL PROPERTY IMPORTED_LOCATION_RELEASE ${CURL_LIBRARIES})
	endif ()
endif ()
