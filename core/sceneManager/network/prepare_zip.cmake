set (ZIP_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/network/3rdParty/zip)

if(NOT ANDROID)
	if (NOT EXISTS ${CMAKE_BINARY_DIR}/zip/build)
			file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/zip/build)
	endif()
	set (ZIP_BUILD_DIR ${CMAKE_BINARY_DIR}/zip/build)
	set (ZIP_INSTALL_DIR ${CMAKE_BINARY_DIR}/zip/install)
	set (ZIP_READY FALSE)

	# find zip
	if (WIN32)
		message (STATUS "zip: prepare on Windows")
		set(ZIP_STATIC_LIB_PATH_DEBUG "${ZIP_BUILD_DIR}/Debug/zip.lib")
		set(ZIP_STATIC_LIB_PATH_RELEASE "${ZIP_BUILD_DIR}/Release/zip.lib")
	endif ()

	message (STATUS "zip: ZIP_STATIC_LIB_PATH_DEBUG: ${ZIP_STATIC_LIB_PATH_DEBUG}")
	message (STATUS "zip: ZIP_STATIC_LIB_PATH_RELEASE: ${ZIP_STATIC_LIB_PATH_RELEASE}")

	if (EXISTS ${ZIP_STATIC_LIB_PATH_DEBUG} AND EXISTS ${ZIP_STATIC_LIB_PATH_RELEASE})
		set(ZIP_READY TRUE)
		message (STATUS "zip is installed" )
	else ()
		set(ZIP_READY FALSE)
		message (STATUS "zip is not installed" )
	endif ()

	if (NOT ZIP_READY)
		# build zip
		foreach (CONF ${CMAKE_CONFIGURATION_TYPES})
			message (STATUS "Configuring zip for ${CONF}, please wait.....")
			execute_process (
					COMMAND ${CMAKE_COMMAND} -Wno-dev
					-DCMAKE_CONFIGURATION_TYPES:STRING=${CONF}
					-DCMAKE_BUILD_TYPE:STRING=${CONF}
					-DCMAKE_COMPILE_OPTIONS:STRING=${EXTRA_COMPILE_OPTIONS}
					-DCMAKE_INSTALL_PREFIX:PATH=${ZIP_INSTALL_DIR}
					-G ${CMAKE_GENERATOR}
					${ZIP_SRC_DIR}
					WORKING_DIRECTORY ${ZIP_BUILD_DIR}
					RESULT_VARIABLE error OUTPUT_VARIABLE output ERROR_VARIABLE output
					)
			reportError (output if error)
			message (STATUS "Building zip for ${CONF}, please wait.....")
			execute_process (
				COMMAND ${CMAKE_COMMAND} --build ${ZIP_BUILD_DIR} --config ${CONF} --target zip
				WORKING_DIRECTORY ${ZIP_BUILD_DIR}
				RESULT_VARIABLE error OUTPUT_VARIABLE output ERROR_VARIABLE output)
			reportError (output if error)
		endforeach ()
	endif ()

	# use zip
	add_library(MY_ZIP STATIC IMPORTED)
	set_property(TARGET MY_ZIP PROPERTY IMPORTED_LOCATION_DEBUG ${ZIP_STATIC_LIB_PATH_DEBUG})
	set_property(TARGET MY_ZIP PROPERTY IMPORTED_LOCATION_RELEASE ${ZIP_STATIC_LIB_PATH_RELEASE})
	set(MY_ZIP_INCLUDE_DIR ${ZIP_SRC_DIR}/src)
else()
	# add_subdirectory(${ZIP_SRC_DIR})
	# add_library(MY_ZIP ALIAS zip)
	# set(MY_ZIP_INCLUDE_DIR ${ZIP_SRC_DIR}/src)
	set(ZIP_READY FALSE)
	if (EXISTS ${PREBUILT_ANDROID_LIBS_DIR}/zip/${ANDROID_ABI}/libzip.a)
		set(ZIP_INSTALLED TRUE)
		message(STATUS "zip is installed")

		set(MY_ZIP_INCLUDE_DIR ${PREBUILT_ANDROID_LIBS_DIR}/zip/include)

		add_library(MY_ZIP STATIC IMPORTED)
		set_target_properties(MY_ZIP PROPERTIES IMPORTED_LOCATION ${PREBUILT_ANDROID_LIBS_DIR}/zip/${ANDROID_ABI}/libzip.a)
	else()
		set(ZIP_INSTALLED FALSE)
		message(FATAL_ERROR "zip is not installed.")
		# TODO: install zip from cmake...
	endif()
endif()