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

# python
find_package(PythonInterp)
message("Python: version: ${PYTHON_VERSION_STRING}")
if (NOT PYTHONINTERP_FOUND)
	if (WIN32)

		message("Python: Starting installer...")

		set(PYTHON_SETUP_PATH "${CMAKE_SOURCE_DIR}/3rdParty/python/python-2.7.12.msi")

		execute_process(
			COMMAND cmd /c ${PYTHON_SETUP_PATH}
			RESULT_VARIABLE PYTHON_SETUP_RESULT
		)
		if (${PYTHON_SETUP_RESULT} EQUAL 0)
			message("Python: Installation succeeded, exit code (${PYTHON_SETUP_RESULT})")
		else ()
			message("Python: Installation failed, exit code (${PYTHON_SETUP_RESULT})")
		endif ()

		find_package(PythonInterp)
		message("Python: version: ${PYTHON_VERSION_STRING}")
		if (NOT PYTHONINTERP_FOUND)
			message(FATAL_ERROR "Python: Could not find python")
		endif ()

	endif ()
endif ()

message(STATUS "CMAKE_SOURCE_DIR: ${CMAKE_SOURCE_DIR}")
message(STATUS "CMAKE_RUNTIME_OUTPUT_DIRECTORY: ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")

# TODO: put APE_OUTPUT_DIR_DEBUG variable to CMakeLists.txt located in project root folder
set(APE_OUTPUT_DIR_DEBUG ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Debug/)
message(STATUS "APE_OUTPUT_DIR_DEBUG: ${APE_OUTPUT_DIR_DEBUG}")

set(APE_OUTPUT_DIR_RELEASE ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Release/)
message(STATUS "APE_OUTPUT_DIR_RELEASE: ${APE_OUTPUT_DIR_RELEASE}")

set(JS_API_DIR ${CMAKE_CURRENT_SOURCE_DIR})
message(STATUS "JS_API_DIR: ${JS_API_DIR}")

set(NODE_VERSION "8.0.0")
message(STATUS "NODE_VERSION: ${NODE_VERSION}")

set(NODE_ROOT_DIR ${APE_SOURCE_DIR}/3rdParty/nodejs)
message(STATUS "NODE_ROOT_DIR: ${NODE_ROOT_DIR}")

set(NODE_SOURCE_DIR ${NODE_ROOT_DIR}/${NODE_VERSION}/source)
message(STATUS "NODE_SOURCE_DIR: ${NODE_SOURCE_DIR}")

set(NODE_INSTALLER_DIR ${NODE_ROOT_DIR}/${NODE_VERSION}/installer)
message(STATUS "NODE_INSTALLER_DIR: ${NODE_INSTALLER_DIR}")

set(NODE_INSTALLER_X86_PATH ${NODE_INSTALLER_DIR}/x86/node-v${NODE_VERSION}-x86.msi)
message(STATUS "NODE_INSTALLER_X86_PATH: ${NODE_INSTALLER_X86_PATH}")

set(NODE_INSTALLER_X64_PATH ${NODE_INSTALLER_DIR}/x64/node-v${NODE_VERSION}-x64.msi)
message(STATUS "NODE_INSTALLER_X64_PATH: ${NODE_INSTALLER_X64_PATH}")

set(NODE_LIBRARY Release node Debug node)

if (NOT EXISTS ${NODE_ROOT_DIR})
	message(FATAL_ERROR "Could not find ${NODE_ROOT_DIR}")
endif ()
if (NOT EXISTS ${NODE_SOURCE_DIR})
	message(FATAL_ERROR "Could not find ${NODE_SOURCE_DIR}")
endif ()

message(STATUS "CMAKE_SIZEOF_VOID_P: ${CMAKE_SIZEOF_VOID_P}")
if (${CMAKE_SIZEOF_VOID_P} EQUAL 8)
	set(HOST_ARCH_X64 1)
	message(STATUS "DetectHostPlatform: HOST_ARCH_X64")
elseif (${CMAKE_SIZEOF_VOID_P} EQUAL 4)
	set(HOST_ARCH_X86 1)
	message(STATUS "DetectHostPlatform: HOST_ARCH_X86")
endif ()

if (WIN32)
	if (MSVC)

		set(PF_X86 "PROGRAMFILES(X86)")
		set(PF_X64 "PROGRAMFILES")
		set(VS_PATH_2017 "")

		set(VCBUILD false)
		set(MSBUILD false)

		SET(BINDIR32_ENV_NAME "ProgramFiles(x86)")
		SET(ENV_PROGRAMFILES_X86 $ENV{${BINDIR32_ENV_NAME}})
		SET(ENV_PROGRAMFILES $ENV{ProgramFiles})

		message (STATUS "ENV{ProgramFiles}=${ENV_PROGRAMFILES}")
		message (STATUS "ENV{ProgramFiles(x86)}=${ENV_PROGRAMFILES_X86}")

		###
		### DEVENV
		###
		# GET Visual Studio's Version and Path
		# https://en.wikipedia.org/wiki/Visual_C%2B%2B#Common_MSVC_version
		# https://blogs.msdn.microsoft.com/heaths/2017/04/21/vswhere-is-now-installed-with-visual-studio-2017/
		if (MSVC14) # Visual Studio 2015 / 2017
			message(STATUS "MSVC14")
			set(VS_WHERE_PATH "${ENV_PROGRAMFILES_X86}/Microsoft Visual Studio/Installer/vswhere.exe")
			message(STATUS "VSWhere: path: (${VS_WHERE_PATH})")
			if (EXISTS ${VS_WHERE_PATH})
				execute_process(
					COMMAND cmd /c ${VS_WHERE_PATH} -latest -requires Microsoft.Component.MSBuild -property installationPath
					RESULT_VARIABLE VS_WHERE_PATH_RESULT
					OUTPUT_VARIABLE VS_WHERE_OUTPUT
				)
				if (${VS_WHERE_PATH_RESULT} EQUAL 0)
					message(STATUS "VSWhere: exit code: '${VS_WHERE_PATH_RESULT}'")
					string(REGEX REPLACE "\n$" "" VS_WHERE_OUTPUT "${VS_WHERE_OUTPUT}")
					message(STATUS "VSWhere: stripped output: '${VS_WHERE_OUTPUT}'")
					set(VS_PATH_2017 ${VS_WHERE_OUTPUT})
				else ()
					message(STATUS "VSWhere: exit code: (${VS_WHERE_PATH_RESULT})")
				endif ()
			else ()
				message(STATUS "VSWhere: path (${VS_WHERE_PATH_RESULT}) does not exist")
			endif ()

			message(STATUS "VS_PATH_2017: ${VS_PATH_2017}")

			if (NOT ${VS_PATH_2017} STREQUAL "")
				set(VS_DIR ${VS_PATH_2017})
			else ()
				GET_FILENAME_COMPONENT(VS_DIR [HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\VisualStudio\\14.0\\Setup\\VS;ProductDir] REALPATH CACHE)
			endif ()
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
		message(STATUS "Visual Studio path: ${VS_DIR}")

		# Check if devenv is installed
		set(DEVENV_PATH "${VS_DIR}/Common7/IDE/devenv.exe")
		message(STATUS "Devenv path: '${DEVENV_PATH}'")
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
			message(STATUS "MS Build Tools Path: ${MSBUILD_SETUP_PATH}")

			if (NOT EXISTS ${MSBUILD_SETUP_PATH})

				# Install MSBuildTools
				message(STATUS "MSBuildTools: Starting installer...")

				set(MS_BUILD_TOOLS_PATH "${CMAKE_SOURCE_DIR}/3rdParty/msbuildtools/BuildTools_Full.exe")

				execute_process(
					COMMAND cmd /c ${MS_BUILD_TOOLS_PATH}
					RESULT_VARIABLE MS_BUILD_TOOLS_RESULT
				)
				if (${MS_BUILD_TOOLS_RESULT} EQUAL 0)
					message(STATUS "MSBuildTools: Installation succeeded, exit code (${MS_BUILD_TOOLS_RESULT})")
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
		message(STATUS "ApePlugins: Prebuilding Node js...")

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

		if (EXISTS ${NODE_DLL_PATH_RELEASE})
			message(STATUS "ApePlugins: Skip building NodeJs, dll found in NodeJs/bin/release folder...")
			if (NOT EXISTS ${APE_OUTPUT_DIR_RELEASE}/${NODE_DLL_NAME})
				# Copy node.dll to output directory
				execute_process(
					COMMAND ${CMAKE_COMMAND} -E copy
					${NODE_DLL_PATH_RELEASE} ${APE_OUTPUT_DIR_RELEASE}/${NODE_DLL_NAME}
					RESULT_VARIABLE COPY_SETUP_RESULT
				)
				message("ApePlugins: Node: Copy succeeded, exit code (${COPY_SETUP_RESULT})")
			endif ()
		else ()

			# Use vcbuild to generate solution files
			message("ApePlugins: Node: Creating solution files with vcbuild.bat for RELEASE mode...")
			execute_process(
				COMMAND cmd /c ${NODE_VCBUILD_PATH} release x64 nosign dll vs2017
				RESULT_VARIABLE NODE_VCBUILD_RESULT
			)
			if (${NODE_VCBUILD_RESULT} EQUAL 0)
				message(STATUS "ApePlugins: Node: Creating solution files succeeded, exit code (${NODE_VCBUILD_RESULT})")
			else ()
				message("ApePlugins: Node: Creating solution files failed, exit code (${NODE_VCBUILD_RESULT})")
			endif ()

			# Check generated solution file
			if (NOT EXISTS ${NODE_SOLUTION_PATH})
				message(FATAL_ERROR "Could not find ${NODE_SOLUTION_PATH}")
			endif ()

			#--------------------------------------------------------------------------------------------------------

			# Build Node js
			message(STATUS "ApePlugins: Node: Setting up environment variables and building...")
			if (${VCBUILD})
				execute_process(
					COMMAND cmd /c ${DEVENV_PATH} ${NODE_SOLUTION_PATH} /Build "Release"
					RESULT_VARIABLE BUILD_RESULT
				)
			elseif (${MSBUILD})
				execute_process(
					COMMAND cmd /c ${MSBUILD_SETUP_PATH} & msbuild ${NODE_SOLUTION_PATH} /t:Build /p:Configuration=Release
					RESULT_VARIABLE BUILD_RESULT
				)
			else ()
				message(FATAL_ERROR "Could not find devenv or msbuild")
			endif ()

			if (${BUILD_RESULT} EQUAL 0)
				message(STATUS "ApePlugins: Node: Build succeeded, exit code (${BUILD_RESULT})")
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

		if (EXISTS ${NODE_DLL_PATH_DEBUG})
			message(STATUS "ApePlugins: Skip building NodeJs, dll found in NodeJs/bin/debug folder...")
			if (NOT EXISTS ${APE_OUTPUT_DIR_DEBUG}/${NODE_DLL_NAME})
				# Copy node.dll to output directory
				execute_process(
					COMMAND ${CMAKE_COMMAND} -E copy
					${NODE_DLL_PATH_DEBUG} ${APE_OUTPUT_DIR_DEBUG}/${NODE_DLL_NAME}
					RESULT_VARIABLE
					COPY_SETUP_RESULT
				)
				message("ApePlugins: Node: Copy succeeded, exit code (${COPY_SETUP_RESULT})")
			endif ()
		else ()

			# Use vcbuild to generate solution files
			message("ApePlugins: Node: Creating solution files with vcbuild.bat for DEBUG mode...")
			execute_process(
				COMMAND cmd /c ${NODE_VCBUILD_PATH} debug x64 nosign dll vs2017
				RESULT_VARIABLE NODE_VCBUILD_RESULT
			)
			if (${NODE_VCBUILD_RESULT} EQUAL 0)
				message(STATUS "ApePlugins: Node: Creating solution files succeeded, exit code (${NODE_VCBUILD_RESULT})")
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
				message("ApePlugins: VC BUILD")
				execute_process(
					COMMAND cmd /c ${DEVENV_PATH} ${NODE_SOLUTION_PATH} /Build "Debug"
					RESULT_VARIABLE BUILD_RESULT
				)
			elseif (${MSBUILD})
				message("ApePlugins: MS BUILD")
				execute_process(
					COMMAND cmd /c ${MSBUILD_SETUP_PATH} & msbuild ${NODE_SOLUTION_PATH} /t:Build /p:Configuration=Debug
					RESULT_VARIABLE BUILD_RESULT
				)
			else ()
				message(FATAL_ERROR "Could not find devenv or msbuild")
			endif ()

			if (${BUILD_RESULT} EQUAL 0)
				message(STATUS "ApePlugins: Node: Build succeeded, exit code (${BUILD_RESULT})")
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

		#--------------------------------------------------------------------------------------------------------

		###
		### NPM
		###

		message(STATUS "ApePlugins: npm version: ${RESULT_VARIABLE}")
		execute_process(
			COMMAND cmd /c "npm --version"
			RESULT_VARIABLE BUILD_RESULT
		)

		if (${BUILD_RESULT} EQUAL 0)
			message(STATUS "ApePlugins: npm is installed.")
		else ()
			message("ApePlugins: npm is not installed, starting installer...")

			if (HOST_ARCH_X64)
				execute_process(
					COMMAND cmd /c ${NODE_INSTALLER_X64_PATH}
					RESULT_VARIABLE BUILD_RESULT
				)
			elseif (HOST_ARCH_X86)
				execute_process(
					COMMAND cmd /c ${NODE_INSTALLER_X86_PATH}
					RESULT_VARIABLE BUILD_RESULT
				)
			endif ()

			if (${BUILD_RESULT} EQUAL 0)
				message(STATUS "ApePlugins: Node-installer: Installation succeeded, exit code (${BUILD_RESULT})")
				message(FATAL_ERROR "ApePlugins: Node-installer: npm should be added to path by the installer. Please restart CMake gui and reconfigure!")
			else ()
				message(FATAL_ERROR "ApePlugins: Node-installer: Installation failed, exit code (${BUILD_RESULT})")
			endif ()

		endif ()

	endif ()
endif ()

# TODO: build node js under unix



add_subdirectory (nodeJsExtension)
add_subdirectory (nodeJsPlugin)
