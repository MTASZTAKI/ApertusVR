# cUrl
if (WIN32)
	set (CURL_READY FALSE)

	set(CURL_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/network/3rdParty/curl)
	if (NOT EXISTS ${CMAKE_BINARY_DIR}/curl/build)
		file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/curl/build)
	endif()
	set(CURL_BUILD_DIR ${CMAKE_BINARY_DIR}/curl/build)
	set(CURL_INCLUDE_DIRS ${CURL_SOURCE_DIR}/include/ ${CURL_BUILD_DIR}/include/curl)

	set(CURL_DLL_FILENAME libcurl.dll)
	set(CURL_LIB_FILENAME libcurl_imp.lib)

	set(CURL_DLL_PATH_DEBUG ${CURL_BUILD_DIR}/lib/Debug/${CURL_DLL_FILENAME})
	set(CURL_DLL_PATH_RELEASE ${CURL_BUILD_DIR}/lib/Release/${CURL_DLL_FILENAME})


	set(CURL_LIB_PATH_DEBUG ${CURL_BUILD_DIR}/lib/Debug/${CURL_LIB_FILENAME})
	set(CURL_LIB_PATH_RELEASE ${CURL_BUILD_DIR}/lib/Release/${CURL_LIB_FILENAME})


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
					${CURL_SOURCE_DIR}
					WORKING_DIRECTORY ${CURL_BUILD_DIR}
					RESULT_VARIABLE error OUTPUT_VARIABLE output ERROR_VARIABLE output
					)
			reportError(output if error)
			message(STATUS "Building curl for ${CONF}, please wait.....")
			execute_process(
				COMMAND ${CMAKE_COMMAND} --build ${CURL_BUILD_DIR} --config ${CONF} --target ALL_BUILD
				WORKING_DIRECTORY ${CURL_BUILD_DIR}
				RESULT_VARIABLE error OUTPUT_VARIABLE output ERROR_VARIABLE output)
			reportError(output if error)
		endforeach ()
	endif ()

	# use curl
	add_library(MY_CURL STATIC IMPORTED)
	set_property(TARGET MY_CURL PROPERTY IMPORTED_LOCATION_DEBUG ${CURL_LIB_PATH_DEBUG})
	set_property(TARGET MY_CURL PROPERTY IMPORTED_LOCATION_RELEASE ${CURL_LIB_PATH_RELEASE})

	file(COPY ${CURL_DLL_PATH_DEBUG} DESTINATION ${APE_OUTPUT_DIR_DEBUG})
	file(COPY ${CURL_DLL_PATH_RELEASE} DESTINATION ${APE_OUTPUT_DIR_RELEASE})
	file(COPY ${CURL_LIB_PATH_DEBUG} DESTINATION ${APE_OUTPUT_DIR_DEBUG})
	file(COPY ${CURL_LIB_PATH_RELEASE} DESTINATION ${APE_OUTPUT_DIR_RELEASE})

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
