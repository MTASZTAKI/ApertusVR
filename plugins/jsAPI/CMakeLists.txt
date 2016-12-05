#MIT License
#
#Copyright (c) 2016 MTA SZTAKI
#
#Permission is hereby granted, free of charge, to any person obtaining a copy
#of this software and associated documentation files (the "Software"), to deal
#in the Software without restriction, including without limitation the rights
#to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
#copies of the Software, and to permit persons to whom the Software is
#furnished to do so, subject to the following conditions:
#
#The above copyright notice and this permission notice shall be included in all
#copies or substantial portions of the Software.
#
#THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
#IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
#FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
#AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
#LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
#OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
#SOFTWARE.

message("CMAKE_SOURCE_DIR: ${CMAKE_SOURCE_DIR}")
message("CMAKE_RUNTIME_OUTPUT_DIRECTORY: ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")

# TODO: put APE_OUTPUT_DIR_DEBUG variable to CMakeLists.txt located in project root folder
set(APE_OUTPUT_DIR_DEBUG ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Debug/)
message("APE_OUTPUT_DIR_DEBUG: ${APE_OUTPUT_DIR_DEBUG}")

set(APE_OUTPUT_DIR_RELEASE ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Release/)
message("APE_OUTPUT_DIR_RELEASE: ${APE_OUTPUT_DIR_RELEASE}")

set(JS_API_DIR ${CMAKE_CURRENT_SOURCE_DIR})
message("JS_API_DIR: ${JS_API_DIR}")

set(JS_API_COMMON_DIR ${JS_API_DIR}/common)
message("JS_API_COMMON_DIR: ${JS_API_COMMON_DIR}")

set(NODE_SOURCE_DIR ${APE_SOURCE_DIR}/3rdParty/node-v0.10.46)
message("NODE_SOURCE_DIR: ${NODE_SOURCE_DIR}")

set(NODE_LIBRARY Release node Debug node)

if (NOT EXISTS ${NODE_SOURCE_DIR})
	message(FATAL_ERROR "Could not find ${NODE_SOURCE_DIR}")
endif ()

if (WIN32)
	if (MSVC)

		set(PF_X86 "PROGRAMFILES(X86)")
		set(PF_X64 "PROGRAMFILES")

		set(VCBUILD false)
		set(MSBUILD false)

		# -------------------------------------------------------------------------------

		###
		### DEVENV
		###
		# GET Visual Studio's Version and Path - https://en.wikipedia.org/wiki/Visual_C%2B%2B#Common_MSVC_version
		if (MSVC14) # Visual Studio 2015
			GET_FILENAME_COMPONENT(VS_DIR [HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\VisualStudio\\14.0\\Setup\\VS;ProductDir] REALPATH CACHE)
		elseif (MSVC12) # Visual Studio 2013
			GET_FILENAME_COMPONENT(VS_DIR [HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\VisualStudio\\12.0\\Setup\\VS;ProductDir] REALPATH CACHE)
		elseif (MSVC11) # Visual Studio 2012
			GET_FILENAME_COMPONENT(VS_DIR [HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\VisualStudio\\11.0\\Setup\\VS;ProductDir] REALPATH CACHE)
		elseif (MSVC10) # Visual Studio 2010
		    GET_FILENAME_COMPONENT(VS_DIR [HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\VisualStudio\\10.0\\Setup\\VS;ProductDir] REALPATH CACHE)
		elseif (MSVC90) # Visual Studio 2008
		    GET_FILENAME_COMPONENT(VS_DIR [HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\VisualStudio\\9.0\\Setup\\VS;ProductDir] REALPATH CACHE)
		elseif (MSVC80) # Visual Studio 2005
		    GET_FILENAME_COMPONENT(VS_DIR [HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\VisualStudio\\8.0\\Setup\\VS;ProductDir] REALPATH CACHE)
		endif ()
		message("Visual Studio path: ${VS_DIR}")

		# Check if devenv is installed
		set(DEVENV_PATH_X86 "${VS_DIR}/Common7/IDE/devenv.exe")
		set(DEVENV_PATH_X64 "${VS_DIR}/Common7/IDE/devenv.exe")
		set(DEVENV_PATH "")
		if (EXISTS ${DEVENV_PATH_X86})
			set(DEVENV_PATH ${DEVENV_PATH_X86})
		endif ()
		if (EXISTS ${DEVENV_PATH_X64})
			set(DEVENV_PATH ${DEVENV_PATH_X64})
		endif ()
		message("Devenv path: ${DEVENV_PATH}")

		if (NOT ${DEVENV_PATH} STREQUAL "")
			set(VCBUILD true)
		endif ()

		# -------------------------------------------------------------------------------


		if (NOT ${VCBUILD})

			# Check if MSBuildTools is installed
			set(MSBUILD_SETUP_PATH_X86 "$ENV{${PF_X86}}/Microsoft Visual C++ Build Tools/vcbuildtools_msbuild.bat")
			set(MSBUILD_SETUP_PATH_X64 "$ENV{${PF_X64}}/Microsoft Visual C++ Build Tools/vcbuildtools_msbuild.bat")
			set(MSBUILD_SETUP_PATH "")
			if (EXISTS ${MSBUILD_SETUP_PATH_X86})
				set(MSBUILD_SETUP_PATH ${MSBUILD_SETUP_PATH_X86})
			endif ()
			if (EXISTS ${MSBUILD_SETUP_PATH_X64})
				set(MSBUILD_SETUP_PATH ${MSBUILD_SETUP_PATH_X64})
			endif ()
			message("MS Build Tools Path: ${MSBUILD_SETUP_PATH}")

			if (NOT EXISTS ${MSBUILD_SETUP_PATH})

				# Install MSBuildTools
				message("MSBuildTools: Starting installer...")

				set(MS_BUILD_TOOLS_PATH "${CMAKE_SOURCE_DIR}/3rdParty/msbuildtool/BuildTools_Full.exe")

				execute_process(
					COMMAND cmd /c ${MS_BUILD_TOOLS_PATH}
					RESULT_VARIABLE MS_BUILD_TOOLS_RESULT
				)
				if (${MS_BUILD_TOOLS_RESULT} EQUAL 0)
					message("MSBuildTools: Installation succeeded, exit code (${MS_BUILD_TOOLS_RESULT})")
				else ()
					message("MSBuildTools: Installation failed, exit code (${MS_BUILD_TOOLS_RESULT})")
				endif ()

				# Check buildtools path
				if (NOT EXISTS ${MSBUILD_SETUP_PATH})
					message(FATAL_ERROR "MSBuildTools: Could not find ${MSBUILD_SETUP_PATH}")
				else ()
					set(MSBUILD true)
				endif ()

			endif ()

		endif ()


		# Build Node js
		message("ApePlugins: Prebuilding Node js...")

		set(NODE_VCBUILD_PATH "${NODE_SOURCE_DIR}/vcbuild.bat")
		set(NODE_SOLUTION_PATH "${NODE_SOURCE_DIR}/node.sln")

		set(NODE_DLL_NAME "node.dll")
		set(NODE_DLL_PATH_DEBUG "${NODE_SOURCE_DIR}/Debug/${NODE_DLL_NAME}")
		set(NODE_DLL_PATH_RELEASE "${NODE_SOURCE_DIR}/Release/${NODE_DLL_NAME}")

		set(NODE_LIB_NAME "node.lib")
		set(NODE_LIB_PATH_DEBUG "${NODE_SOURCE_DIR}/Debug/${NODE_LIB_NAME}")
		set(NODE_LIB_PATH_RELEASE "${NODE_SOURCE_DIR}/Release/${NODE_LIB_NAME}")

		#--------------------------------------------------------------------------------------------------------

		# Check files/directories
		if (NOT EXISTS ${NODE_VCBUILD_PATH})
			message(FATAL_ERROR "Could not find ${NODE_VCBUILD_PATH}")
		endif ()

		#--------------------------------------------------------------------------------------------------------

		###
		### RELEASE
		###

		if (NOT EXISTS ${NODE_DLL_PATH_RELEASE})

			# Use vcbuild to generate solution files
			message("ApePlugins: Node: Creating solution files with vcbuild.bat for RELEASE mode...")
			execute_process(
				COMMAND cmd /c ${NODE_VCBUILD_PATH} release x64 nosign
				RESULT_VARIABLE NODE_VCBUILD_RESULT
			)
			if (${NODE_VCBUILD_RESULT} EQUAL 0)
				message("ApePlugins: Node: Creating solution files succeeded, exit code (${NODE_VCBUILD_RESULT})")
			else ()
				message("ApePlugins: Node: Creating solution files failed, exit code (${NODE_VCBUILD_RESULT})")
			endif ()

			# Check generated solution file
			if (NOT EXISTS ${NODE_SOLUTION_PATH})
				message(FATAL_ERROR "Could not find ${NODE_SOLUTION_PATH}")
			endif ()

			#--------------------------------------------------------------------------------------------------------

			# Build Node js
			message("ApePlugins: Node: Setting up environment variables and building...")
			if (${VCBUILD})
				execute_process(
					COMMAND cmd /c ${DEVENV_PATH} ${NODE_SOLUTION_PATH} /Build "Release"
					RESULT_VARIABLE BUILD_RESULT
				)
			elseif (${MSBUILD})
				execute_process(
					# COMMAND cmd /c ${MSBUILD_SETUP_PATH} & msbuild ${NODE_SOLUTION_PATH} /t:Build /p:Configuration=Release
					RESULT_VARIABLE BUILD_RESULT
				)
			else ()
				message(FATAL_ERROR "Could not find devenv or msbuild")
			endif ()

			if (${BUILD_RESULT} EQUAL 0)
				message("ApePlugins: Node: Build succeeded, exit code (${BUILD_RESULT})")
			else ()
				message("ApePlugins: Node: Build failed, exit code (${BUILD_RESULT})")
			endif ()

			# Check generated dll
			if (NOT EXISTS ${NODE_DLL_PATH_RELEASE})
				message(FATAL_ERROR "Could not find ${NODE_DLL_PATH_RELEASE}")
			endif ()

			# Copy node.dll to output directory
			execute_process(
				COMMAND ${CMAKE_COMMAND} -E copy
				${NODE_DLL_PATH_RELEASE} ${APE_OUTPUT_DIR_RELEASE}/${NODE_DLL_NAME}
				RESULT_VARIABLE
				COPY_SETUP_RESULT
			)
			message("ApePlugins: Node: Copy succeeded, exit code (${COPY_SETUP_RESULT})")
			if (NOT EXISTS ${APE_OUTPUT_DIR_RELEASE}/${NODE_DLL_NAME})
				message(FATAL_ERROR "Could not find ${APE_OUTPUT_DIR_RELEASE}/${NODE_DLL_NAME}")
			endif ()

			# Check generated lib
			if (NOT EXISTS ${NODE_LIB_PATH_RELEASE})
				message(FATAL_ERROR "Could not find ${NODE_LIB_PATH_RELEASE}")
			endif ()

		endif ()

		#--------------------------------------------------------------------------------------------------------

		###
		### DEBUG
		###

		if (NOT EXISTS ${NODE_DLL_PATH_DEBUG})

			# Use vcbuild to generate solution files
			message("ApePlugins: Node: Creating solution files with vcbuild.bat for DEBUG mode...")
			execute_process(
				COMMAND cmd /c ${NODE_VCBUILD_PATH} debug x64 nosign
				RESULT_VARIABLE NODE_VCBUILD_RESULT
			)
			if (${NODE_VCBUILD_RESULT} EQUAL 0)
				message("ApePlugins: Node: Creating solution files succeeded, exit code (${NODE_VCBUILD_RESULT})")
			else ()
				message("ApePlugins: Node: Creating solution files failed, exit code (${NODE_VCBUILD_RESULT})")
			endif ()

			# Check generated solution file
			if (NOT EXISTS ${NODE_SOLUTION_PATH})
				message(FATAL_ERROR "Could not find ${NODE_SOLUTION_PATH}")
			endif ()

			#--------------------------------------------------------------------------------------------------------

			# Build Node js
			message("ApePlugins: Node: Setting up environment variables and building...")
			if (${VCBUILD})
				execute_process(
					COMMAND cmd /c ${DEVENV_PATH} ${NODE_SOLUTION_PATH} /Build "Debug"
					RESULT_VARIABLE BUILD_RESULT
				)
			elseif (${MSBUILD})
				execute_process(
					COMMAND cmd /c ${MSBUILD_SETUP_PATH} & msbuild ${NODE_SOLUTION_PATH} /t:Build /p:Configuration=Debug
					RESULT_VARIABLE BUILD_RESULT
				)
			else ()
				message(FATAL_ERROR "Could not find devenv or msbuild")
			endif ()

			if (${BUILD_RESULT} EQUAL 0)
				message("ApePlugins: Node: Build succeeded, exit code (${BUILD_RESULT})")
			else ()
				message("ApePlugins: Node: Build failed, exit code (${BUILD_RESULT})")
			endif ()

			# Check generated dll
			if (NOT EXISTS ${NODE_DLL_PATH_DEBUG})
				message(FATAL_ERROR "Could not find ${NODE_DLL_PATH_DEBUG}")
			endif ()

			# Copy node.dll to output directory
			execute_process(
				COMMAND ${CMAKE_COMMAND} -E copy
				${NODE_DLL_PATH_DEBUG} ${APE_OUTPUT_DIR_DEBUG}/${NODE_DLL_NAME}
				RESULT_VARIABLE
				COPY_SETUP_RESULT
			)
			message("ApePlugins: Node: Copy succeeded, exit code (${COPY_SETUP_RESULT})")
			if (NOT EXISTS ${APE_OUTPUT_DIR_DEBUG}/${NODE_DLL_NAME})
				message(FATAL_ERROR "Could not find ${APE_OUTPUT_DIR_DEBUG}/${NODE_DLL_NAME}")
			endif ()

			# Check generated lib
			if (NOT EXISTS ${NODE_LIB_PATH_DEBUG})
				message(FATAL_ERROR "Could not find ${NODE_LIB_PATH_DEBUG}")
			endif ()

		endif ()

	endif ()
endif ()

# TODO: build node js under unix



add_subdirectory (common)
add_subdirectory (nodeJsExtension)
add_subdirectory (nodeJsPlugin)
