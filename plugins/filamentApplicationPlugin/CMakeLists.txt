#MIT License
#
#Copyright (c) 2018 MTA SZTAKI
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
set(CXX_STANDARD "-std=c++17")
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(MIN_CLANG_VERSION "6.0")

if(WIN32)
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")
endif()

if (CMAKE_C_COMPILER_ID MATCHES "Clang")
    if (CMAKE_C_COMPILER_VERSION VERSION_LESS MIN_CLANG_VERSION)
        message(FATAL_ERROR "Detected C compiler Clang ${CMAKE_C_COMPILER_VERSION} < ${MIN_CLANG_VERSION}")
    endif()
elseif (NOT MSVC)
    message(FATAL_ERROR "Detected C compiler ${CMAKE_C_COMPILER_ID} is unsupported")
endif()

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS MIN_CLANG_VERSION)
        message(FATAL_ERROR "Detected CXX compiler Clang ${CMAKE_CXX_COMPILER_VERSION} < ${MIN_CLANG_VERSION}")
    endif()
elseif (NOT MSVC)
    message(FATAL_ERROR "Detected CXX compiler ${CMAKE_CXX_COMPILER_ID} is unsupported")
endif()

if (MSVC)
    set(CXX_STANDARD "/std:c++latest")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CXX_STANDARD} /W0 /Zc:__cplusplus")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CXX_STANDARD} -fstrict-aliasing -Wno-unknown-pragmas -Wno-unused-function")
endif()

set(PLUGIN_NAME "apeFilamentApplicationPlugin")

include("network/prepare_curl.cmake")

find_package(OpenGL REQUIRED)
if(WIN32)
	set(FILAMENT_DOWNLOAD_URL "http://srv.mvv.sztaki.hu/Ape/3rdPartiesBin/filament/windows/filament-1.9.9-build.zip")
	set(FILAMENT_DOWNLOAD_URL_SOURCE "http://srv.mvv.sztaki.hu/Ape/3rdPartiesBin/filament/windows/filament-1.9.9.zip")
	set(FILAMENT_PREBUILD_DIR ${CMAKE_BINARY_DIR}/filamentApp/build)
	set(FILAMENT_PREBUILD_ZIP ${CMAKE_BINARY_DIR}/filamentApp/filament-1.9.9-build.zip)
	set(FILAMENT_SOURCE_ZIP ${CMAKE_BINARY_DIR}/filamentApp/filament-1.9.9.zip)
elseif(APPLE)
	set(FILAMENT_DOWNLOAD_URL "http://srv.mvv.sztaki.hu/Ape/3rdPartiesBin/filament/osx/filament-v1.9.9-build.zip")
    set(FILAMENT_DOWNLOAD_URL_SOURCE "http://srv.mvv.sztaki.hu/Ape/3rdPartiesBin/filament/windows/filament-1.9.9.zip")
	set(FILAMENT_PREBUILD_DIR ${CMAKE_BINARY_DIR}/filamentApp)
	set(FILAMENT_PREBUILD_ZIP ${CMAKE_BINARY_DIR}/filamentApp/filament-v1.9.9-mac.zip)
    set(FILAMENT_SOURCE_ZIP ${CMAKE_BINARY_DIR}/filamentApp/filament-1.9.9.zip)
endif()

if (NOT EXISTS ${FILAMENT_PREBUILD_DIR})
		file(MAKE_DIRECTORY ${FILAMENT_PREBUILD_DIR})
endif ()

set(FILAMENT_READY FALSE)
if(WIN32)
	if(EXISTS ${FILAMENT_PREBUILD_DIR}/filament-1.9.9-build/libs/filamentapp/Debug/filamentapp.lib)
		set(FILAMENT_READY TRUE)
		message (STATUS "Filament is installed" )
	else ()
		set(FILAMENT_READY FALSE)
		message (STATUS "Filament is not installed" )
	endif ()
elseif(APPLE)
	if(EXISTS ${FILAMENT_PREBUILD_DIR}/filament-1.9.9-build/filament/Debug/libfilament.a)
		set(FILAMENT_READY TRUE)
		message (STATUS "Filament is installed" )
	else ()
		set(FILAMENT_READY FALSE)
		message (STATUS "Filament is not installed" )
	endif ()
endif()
if (NOT FILAMENT_READY)
	if(NOT EXISTS "${FILAMENT_PREBUILD_ZIP}")
		message(STATUS "Downloading ${FILAMENT_DOWNLOAD_URL_SOURCE}...")
		file(
			DOWNLOAD "${FILAMENT_DOWNLOAD_URL_SOURCE}" "${FILAMENT_SOURCE_ZIP}"
			SHOW_PROGRESS
		)
	endif()
	message(STATUS "Extracting ${FILAMENT_SOURCE_ZIP}...")
	execute_process(
		COMMAND ${CMAKE_COMMAND} -E tar xzf "${FILAMENT_SOURCE_ZIP}"
		WORKING_DIRECTORY ${FILAMENT_PREBUILD_DIR}
	)
	if(NOT EXISTS "${FILAMENT_PREBUILD_ZIP}")
		message(STATUS "Downloading ${FILAMENT_DOWNLOAD_URL}...")
		file(
			DOWNLOAD "${FILAMENT_DOWNLOAD_URL}" "${FILAMENT_PREBUILD_ZIP}"
			SHOW_PROGRESS
		)
	endif()
	message(STATUS "Extracting ${FILAMENT_PREBUILD_ZIP}...")
	execute_process(
		COMMAND ${CMAKE_COMMAND} -E tar xzf "${FILAMENT_PREBUILD_ZIP}"
		WORKING_DIRECTORY ${FILAMENT_PREBUILD_DIR}
	)
endif ()

if(WIN32)
    set (HEADERS
                apeFilamentApplicationPluginConfigs.h
                apeFilamentApplicationPlugin.h
        )
    set (SOURCES
                apeFilamentApplicationPlugin.cpp
        )
        include_directories(
                ${FILAMENT_PREBUILD_DIR}/filament-1.9.9/filament/include
				${FILAMENT_PREBUILD_DIR}/filament-1.9.9/filament/backend/include
				${FILAMENT_PREBUILD_DIR}/filament-1.9.9/third_party/imgui
				${FILAMENT_PREBUILD_DIR}/filament-1.9.9/libs/gltfio/include
				${FILAMENT_PREBUILD_DIR}/filament-1.9.9/libs/imageio/include
				${FILAMENT_PREBUILD_DIR}/filament-1.9.9/libs/image/include
				${FILAMENT_PREBUILD_DIR}/filament-1.9.9/libs/filamentapp/include
				${FILAMENT_PREBUILD_DIR}/filament-1.9.9/libs/viewer/include
				${FILAMENT_PREBUILD_DIR}/filament-1.9.9/libs/utils/include
				${FILAMENT_PREBUILD_DIR}/filament-1.9.9/libs/math/include
				${FILAMENT_PREBUILD_DIR}/filament-1.9.9/libs/camutils/include
				${FILAMENT_PREBUILD_DIR}/filament-1.9.9/libs/filagui/include
				${FILAMENT_PREBUILD_DIR}/filament-1.9.9/libs/filabridge/include
				${FILAMENT_PREBUILD_DIR}/filament-1.9.9/third_party/getopt/include
				${FILAMENT_PREBUILD_DIR}/filament-1.9.9/third_party/libsdl2/include
				${FILAMENT_PREBUILD_DIR}/filament-1.9.9/third_party/robin-map
                			${CMAKE_CURRENT_SOURCE_DIR}/3rdParty/rapidjson/include
				${CURL_INCLUDE_DIRS}
        )
elseif(APPLE)
    set (HEADERS
                apeFilamentApplicationPluginConfigs.h
                apeFilamentApplicationPlugin.h
	        apeVLFTImgui.h
        )
    set (SOURCES
		apeFilamentApplicationPlugin.cpp
		apeVLFTImgui.cpp
        )
    include_directories(
                ${FILAMENT_PREBUILD_DIR}/filament-1.9.9/filament/include
                ${FILAMENT_PREBUILD_DIR}/filament-1.9.9/filament/backend/include
                ${FILAMENT_PREBUILD_DIR}/filament-1.9.9/third_party/imgui
                ${FILAMENT_PREBUILD_DIR}/filament-1.9.9/libs/gltfio/include
                ${FILAMENT_PREBUILD_DIR}/filament-1.9.9/libs/imageio/include
                ${FILAMENT_PREBUILD_DIR}/filament-1.9.9/libs/image/include
                ${FILAMENT_PREBUILD_DIR}/filament-1.9.9/libs/filamentapp/include
                ${FILAMENT_PREBUILD_DIR}/filament-1.9.9/libs/viewer/include
                ${FILAMENT_PREBUILD_DIR}/filament-1.9.9/libs/utils/include
                ${FILAMENT_PREBUILD_DIR}/filament-1.9.9/libs/math/include
                ${FILAMENT_PREBUILD_DIR}/filament-1.9.9/libs/camutils/include
                ${FILAMENT_PREBUILD_DIR}/filament-1.9.9/libs/filagui/include
                ${FILAMENT_PREBUILD_DIR}/filament-1.9.9/libs/filabridge/include
                ${FILAMENT_PREBUILD_DIR}/filament-1.9.9/third_party/getopt/include
                ${FILAMENT_PREBUILD_DIR}/filament-1.9.9/third_party/libsdl2/include
                ${FILAMENT_PREBUILD_DIR}/filament-1.9.9/third_party/robin-map
                ${CMAKE_CURRENT_SOURCE_DIR}/3rdParty/rapidjson/include
       	        ${CURL_INCLUDE_DIRS}
        )
endif()
add_library (${PLUGIN_NAME} SHARED ${SOURCES} ${HEADERS})


if (WIN32)
    add_library(MY_FILAMENT STATIC IMPORTED)
	set_property(TARGET MY_FILAMENT PROPERTY IMPORTED_LOCATION_DEBUG ${FILAMENT_PREBUILD_DIR}/filament-1.9.9-build//filament/Debug/filament.lib)
	set_property(TARGET MY_FILAMENT PROPERTY IMPORTED_LOCATION_RELEASE ${FILAMENT_PREBUILD_DIR}/filament-1.9.9-build//filament/Release/filament.lib)
	
	add_library(MY_FILAMENT_APP STATIC IMPORTED)
	set_property(TARGET MY_FILAMENT_APP PROPERTY IMPORTED_LOCATION_DEBUG ${FILAMENT_PREBUILD_DIR}/filament-1.9.9-build/libs/filamentapp/Debug/filamentapp.lib)
	set_property(TARGET MY_FILAMENT_APP PROPERTY IMPORTED_LOCATION_RELEASE ${FILAMENT_PREBUILD_DIR}/filament-1.9.9-build/libs/filamentapp/Release/filamentapp.lib)

	add_library(MY_FILAMENT_UTILS STATIC IMPORTED)
	set_property(TARGET MY_FILAMENT_UTILS PROPERTY IMPORTED_LOCATION_DEBUG ${FILAMENT_PREBUILD_DIR}/filament-1.9.9-build/libs/utils/Debug/utils.lib)
	set_property(TARGET MY_FILAMENT_UTILS PROPERTY IMPORTED_LOCATION_RELEASE ${FILAMENT_PREBUILD_DIR}/filament-1.9.9-build/libs/utils/Release/utils.lib)

	add_library(MY_FILAMENT_GLTFIO STATIC IMPORTED)
	set_property(TARGET MY_FILAMENT_GLTFIO PROPERTY IMPORTED_LOCATION_DEBUG ${FILAMENT_PREBUILD_DIR}/filament-1.9.9-build/libs/gltfio/Debug/gltfio.lib)
	set_property(TARGET MY_FILAMENT_GLTFIO PROPERTY IMPORTED_LOCATION_RELEASE ${FILAMENT_PREBUILD_DIR}/filament-1.9.9-build/libs/gltfio/Release/gltfio.lib)

	add_library(MY_FILAMENT_GLTFIO_CORE STATIC IMPORTED)
	set_property(TARGET MY_FILAMENT_GLTFIO_CORE PROPERTY IMPORTED_LOCATION_DEBUG ${FILAMENT_PREBUILD_DIR}/filament-1.9.9-build/libs/gltfio/Debug/gltfio_core.lib)
	set_property(TARGET MY_FILAMENT_GLTFIO_CORE PROPERTY IMPORTED_LOCATION_RELEASE ${FILAMENT_PREBUILD_DIR}/filament-1.9.9-build/libs/gltfio/Release/gltfio_core.lib)
	
	add_library(MY_FILAMENT_GLTFIO_RES STATIC IMPORTED)
	set_property(TARGET MY_FILAMENT_GLTFIO_RES PROPERTY IMPORTED_LOCATION_DEBUG ${FILAMENT_PREBUILD_DIR}/filament-1.9.9-build/libs/gltfio/Debug/gltfio_resources.lib)
	set_property(TARGET MY_FILAMENT_GLTFIO_RES PROPERTY IMPORTED_LOCATION_RELEASE ${FILAMENT_PREBUILD_DIR}/filament-1.9.9-build/libs/gltfio/Release/gltfio_resources.lib)
		
	add_library(MY_FILAMENT_DRACODEC STATIC IMPORTED)
	set_property(TARGET MY_FILAMENT_DRACODEC PROPERTY IMPORTED_LOCATION_DEBUG ${FILAMENT_PREBUILD_DIR}/filament-1.9.9-build/third_party/draco/tnt/Debug/dracodec.lib)
	set_property(TARGET MY_FILAMENT_DRACODEC PROPERTY IMPORTED_LOCATION_RELEASE ${FILAMENT_PREBUILD_DIR}/filament-1.9.9-build/third_party/draco/tnt/Release/dracodec.lib)

	add_library(MY_FILAMENT_BACKEND STATIC IMPORTED)
	set_property(TARGET MY_FILAMENT_BACKEND PROPERTY IMPORTED_LOCATION_DEBUG ${FILAMENT_PREBUILD_DIR}/filament-1.9.9-build//filament/backend/Debug/backend.lib)
	set_property(TARGET MY_FILAMENT_BACKEND PROPERTY IMPORTED_LOCATION_RELEASE ${FILAMENT_PREBUILD_DIR}/filament-1.9.9-build//filament/backend/Release/backend.lib)

	add_library(MY_FILAMENT_SHADERS STATIC IMPORTED)
	set_property(TARGET MY_FILAMENT_SHADERS PROPERTY IMPORTED_LOCATION_DEBUG ${FILAMENT_PREBUILD_DIR}/filament-1.9.9-build/shaders/Debug/shaders.lib)
	set_property(TARGET MY_FILAMENT_SHADERS PROPERTY IMPORTED_LOCATION_RELEASE ${FILAMENT_PREBUILD_DIR}/filament-1.9.9-build/shaders/Release/shaders.lib)
	
	add_library(MY_FILAMENT_MATH STATIC IMPORTED)
	set_property(TARGET MY_FILAMENT_MATH PROPERTY IMPORTED_LOCATION_DEBUG ${FILAMENT_PREBUILD_DIR}/filament-1.9.9-build/libs/filamat/Debug/filamat.lib)
	set_property(TARGET MY_FILAMENT_MATH PROPERTY IMPORTED_LOCATION_RELEASE ${FILAMENT_PREBUILD_DIR}/filament-1.9.9-build/libs/filamat/Release/filamat.lib)

	add_library(MY_FILAMENT_GEOMETRY STATIC IMPORTED)
	set_property(TARGET MY_FILAMENT_GEOMETRY PROPERTY IMPORTED_LOCATION_DEBUG ${FILAMENT_PREBUILD_DIR}/filament-1.9.9-build/libs/geometry/Debug/geometry.lib)
	set_property(TARGET MY_FILAMENT_GEOMETRY PROPERTY IMPORTED_LOCATION_RELEASE ${FILAMENT_PREBUILD_DIR}/filament-1.9.9-build/libs/geometry/Release/geometry.lib)

	add_library(MY_FILAMENT_VIEWER STATIC IMPORTED)
	set_property(TARGET MY_FILAMENT_VIEWER PROPERTY IMPORTED_LOCATION_DEBUG ${FILAMENT_PREBUILD_DIR}/filament-1.9.9-build/libs/viewer/Debug/viewer.lib)
	set_property(TARGET MY_FILAMENT_VIEWER PROPERTY IMPORTED_LOCATION_RELEASE ${FILAMENT_PREBUILD_DIR}/filament-1.9.9-build/libs/viewer/Release/viewer.lib)

	add_library(MY_FILAMENT_IBL STATIC IMPORTED)
	set_property(TARGET MY_FILAMENT_IBL PROPERTY IMPORTED_LOCATION_DEBUG ${FILAMENT_PREBUILD_DIR}/filament-1.9.9-build/libs/ibl/Debug/ibl.lib)
	set_property(TARGET MY_FILAMENT_IBL PROPERTY IMPORTED_LOCATION_RELEASE ${FILAMENT_PREBUILD_DIR}/filament-1.9.9-build/libs/ibl/Release/ibl.lib)

	add_library(MY_FILAMENT_FLAT STATIC IMPORTED)
	set_property(TARGET MY_FILAMENT_FLAT PROPERTY IMPORTED_LOCATION_DEBUG ${FILAMENT_PREBUILD_DIR}/filament-1.9.9-build/libs/filaflat/Debug/filaflat.lib)
	set_property(TARGET MY_FILAMENT_FLAT PROPERTY IMPORTED_LOCATION_RELEASE ${FILAMENT_PREBUILD_DIR}/filament-1.9.9-build/libs/filaflat/Release/filaflat.lib)

	add_library(MY_FILAMENT_BRIDGE STATIC IMPORTED)
	set_property(TARGET MY_FILAMENT_BRIDGE PROPERTY IMPORTED_LOCATION_DEBUG ${FILAMENT_PREBUILD_DIR}/filament-1.9.9-build/libs/filabridge/Debug/filabridge.lib)
	set_property(TARGET MY_FILAMENT_BRIDGE PROPERTY IMPORTED_LOCATION_RELEASE ${FILAMENT_PREBUILD_DIR}/filament-1.9.9-build/libs/filabridge/Release/filabridge.lib)

	add_library(MY_FILAMENT_CAMUTILS STATIC IMPORTED)
	set_property(TARGET MY_FILAMENT_CAMUTILS PROPERTY IMPORTED_LOCATION_DEBUG ${FILAMENT_PREBUILD_DIR}/filament-1.9.9-build/libs/camutils/Debug/camutils.lib)
	set_property(TARGET MY_FILAMENT_CAMUTILS PROPERTY IMPORTED_LOCATION_RELEASE ${FILAMENT_PREBUILD_DIR}/filament-1.9.9-build/libs/camutils/Release/camutils.lib)

	add_library(MY_FILAMENT_BLUEGL STATIC IMPORTED)
	set_property(TARGET MY_FILAMENT_BLUEGL PROPERTY IMPORTED_LOCATION_DEBUG ${FILAMENT_PREBUILD_DIR}/filament-1.9.9-build/libs/bluegl/Debug/bluegl.lib)
	set_property(TARGET MY_FILAMENT_BLUEGL PROPERTY IMPORTED_LOCATION_RELEASE ${FILAMENT_PREBUILD_DIR}/filament-1.9.9-build/libs/bluegl/Release/bluegl.lib)

	#add_library(MY_FILAMENT_BLUEVK STATIC IMPORTED)
	#set_property(TARGET MY_FILAMENT_BLUEVK PROPERTY IMPORTED_LOCATION_DEBUG ${FILAMENT_PREBUILD_DIR}/filament-1.9.9-build/libs/bluevk/Debug/bluevk.lib)
	#set_property(TARGET MY_FILAMENT_BLUEVK PROPERTY IMPORTED_LOCATION_RELEASE ${FILAMENT_PREBUILD_DIR}/filament-1.9.9-build/libs/bluevk/Release/bluevk.lib)

	add_library(MY_FILAMENT_IMAGE STATIC IMPORTED)
	set_property(TARGET MY_FILAMENT_IMAGE PROPERTY IMPORTED_LOCATION_DEBUG ${FILAMENT_PREBUILD_DIR}/filament-1.9.9-build/libs/image/Debug/image.lib)
	set_property(TARGET MY_FILAMENT_IMAGE PROPERTY IMPORTED_LOCATION_RELEASE ${FILAMENT_PREBUILD_DIR}/filament-1.9.9-build/libs/image/Release/image.lib)
	
	add_library(MY_FILAMENT_IMAGEIO STATIC IMPORTED)
	set_property(TARGET MY_FILAMENT_IMAGEIO PROPERTY IMPORTED_LOCATION_DEBUG ${FILAMENT_PREBUILD_DIR}/filament-1.9.9-build/libs/imageio/Debug/imageio.lib)
	set_property(TARGET MY_FILAMENT_IMAGEIO PROPERTY IMPORTED_LOCATION_RELEASE ${FILAMENT_PREBUILD_DIR}/filament-1.9.9-build/libs/imageio/Release/imageio.lib)
	
	add_library(MY_FILAMENT_PNG STATIC IMPORTED)
	set_property(TARGET MY_FILAMENT_PNG PROPERTY IMPORTED_LOCATION_DEBUG ${FILAMENT_PREBUILD_DIR}/filament-1.9.9-build/third_party/libpng/tnt/Debug/png.lib)
	set_property(TARGET MY_FILAMENT_PNG PROPERTY IMPORTED_LOCATION_RELEASE ${FILAMENT_PREBUILD_DIR}/filament-1.9.9-build/third_party/libpng/tnt/Release/png.lib)

	add_library(MY_FILAMENT_SMOL-V STATIC IMPORTED)
	set_property(TARGET MY_FILAMENT_SMOL-V PROPERTY IMPORTED_LOCATION_DEBUG ${FILAMENT_PREBUILD_DIR}/filament-1.9.9-build/third_party/smol-v/tnt/Debug/smol-v.lib)
	set_property(TARGET MY_FILAMENT_SMOL-V PROPERTY IMPORTED_LOCATION_RELEASE ${FILAMENT_PREBUILD_DIR}/filament-1.9.9-build/third_party/smol-v/tnt/Release/smol-v.lib)

	add_library(MY_FILAMENT_MESHOPT STATIC IMPORTED)
	set_property(TARGET MY_FILAMENT_MESHOPT PROPERTY IMPORTED_LOCATION_DEBUG ${FILAMENT_PREBUILD_DIR}/filament-1.9.9-build/third_party/meshoptimizer/Debug/meshoptimizer.lib)
	set_property(TARGET MY_FILAMENT_MESHOPT PROPERTY IMPORTED_LOCATION_RELEASE ${FILAMENT_PREBUILD_DIR}/filament-1.9.9-build/third_party/meshoptimizer/Release/meshoptimizer.lib)

	add_library(MY_FILAMENT_MATDBG STATIC IMPORTED)
	set_property(TARGET MY_FILAMENT_MATDBG PROPERTY IMPORTED_LOCATION_DEBUG ${FILAMENT_PREBUILD_DIR}/filament-1.9.9-build/libs/matdbg/Debug/matdbg.lib)
	set_property(TARGET MY_FILAMENT_MATDBG PROPERTY IMPORTED_LOCATION_RELEASE ${FILAMENT_PREBUILD_DIR}/filament-1.9.9-build/libs/matdbg/Release/matdbg.lib)
	
	add_library(MY_FILAMENT_IMGUI STATIC IMPORTED)
	set_property(TARGET MY_FILAMENT_IMGUI PROPERTY IMPORTED_LOCATION_DEBUG ${FILAMENT_PREBUILD_DIR}/filament-1.9.9-build/third_party/imgui/tnt/Debug/imgui.lib)
	set_property(TARGET MY_FILAMENT_IMGUI PROPERTY IMPORTED_LOCATION_RELEASE ${FILAMENT_PREBUILD_DIR}/filament-1.9.9-build/third_party/imgui/tnt/Release/imgui.lib)
	
	add_library(MY_FILAMENT_SDL2 STATIC IMPORTED)
	set_property(TARGET MY_FILAMENT_SDL2 PROPERTY IMPORTED_LOCATION_DEBUG ${FILAMENT_PREBUILD_DIR}/filament-1.9.9-build/third_party/libsdl2/tnt/Debug/sdl2.lib)
	set_property(TARGET MY_FILAMENT_SDL2 PROPERTY IMPORTED_LOCATION_RELEASE ${FILAMENT_PREBUILD_DIR}/filament-1.9.9-build/third_party/libsdl2/tnt/Release/sdl2.lib)
	
	add_library(MY_FILAMENT_FILAGUI STATIC IMPORTED)
	set_property(TARGET MY_FILAMENT_FILAGUI PROPERTY IMPORTED_LOCATION_DEBUG ${FILAMENT_PREBUILD_DIR}/filament-1.9.9-build/libs/filagui/Debug/filagui.lib)
	set_property(TARGET MY_FILAMENT_FILAGUI PROPERTY IMPORTED_LOCATION_RELEASE ${FILAMENT_PREBUILD_DIR}/filament-1.9.9-build/libs/filagui/Release/filagui.lib)
	
	add_library(MY_FILAMENT_SPIRV_CROSS_GLSL STATIC IMPORTED)
	set_property(TARGET MY_FILAMENT_SPIRV_CROSS_GLSL PROPERTY IMPORTED_LOCATION_DEBUG ${FILAMENT_PREBUILD_DIR}/filament-1.9.9-build/third_party/spirv-cross/tnt/Debug/spirv-cross-glsl.lib)
	set_property(TARGET MY_FILAMENT_SPIRV_CROSS_GLSL PROPERTY IMPORTED_LOCATION_RELEASE ${FILAMENT_PREBUILD_DIR}/filament-1.9.9-build/third_party/spirv-cross/tnt/Release/spirv-cross-glsl.lib)
	
	add_library(MY_FILAMENT_SPIRV_CROSS_CORE STATIC IMPORTED)
	set_property(TARGET MY_FILAMENT_SPIRV_CROSS_CORE PROPERTY IMPORTED_LOCATION_DEBUG ${FILAMENT_PREBUILD_DIR}/filament-1.9.9-build/third_party/spirv-cross/tnt/Debug/spirv-cross-core.lib)
	set_property(TARGET MY_FILAMENT_SPIRV_CROSS_CORE PROPERTY IMPORTED_LOCATION_RELEASE ${FILAMENT_PREBUILD_DIR}/filament-1.9.9-build/third_party/spirv-cross/tnt/Release/spirv-cross-core.lib)
	
	add_library(MY_FILAMENT_SPIRV_CROSS_MSL STATIC IMPORTED)
	set_property(TARGET MY_FILAMENT_SPIRV_CROSS_MSL PROPERTY IMPORTED_LOCATION_DEBUG ${FILAMENT_PREBUILD_DIR}/filament-1.9.9-build/third_party/spirv-cross/tnt/Debug/spirv-cross-msl.lib)
	set_property(TARGET MY_FILAMENT_SPIRV_CROSS_MSL PROPERTY IMPORTED_LOCATION_RELEASE ${FILAMENT_PREBUILD_DIR}/filament-1.9.9-build/third_party/spirv-cross/tnt/Release/spirv-cross-msl.lib)
	
	add_library(MY_FILAMENT_GLSLANG STATIC IMPORTED)
	set_property(TARGET MY_FILAMENT_GLSLANG PROPERTY IMPORTED_LOCATION_DEBUG ${FILAMENT_PREBUILD_DIR}/filament-1.9.9-build/third_party/glslang/tnt/glslang/Debug/glslangd.lib)
	set_property(TARGET MY_FILAMENT_GLSLANG PROPERTY IMPORTED_LOCATION_RELEASE ${FILAMENT_PREBUILD_DIR}/filament-1.9.9-build/third_party/glslang/tnt/glslang/Release/glslang.lib)
	
	add_library(MY_FILAMENT_GLSLANG_OS STATIC IMPORTED)
	set_property(TARGET MY_FILAMENT_GLSLANG_OS PROPERTY IMPORTED_LOCATION_DEBUG ${FILAMENT_PREBUILD_DIR}/filament-1.9.9-build/third_party/glslang/tnt/glslang/OSDependent/Windows/Debug/OSDependentd.lib)
	set_property(TARGET MY_FILAMENT_GLSLANG_OS PROPERTY IMPORTED_LOCATION_RELEASE ${FILAMENT_PREBUILD_DIR}/filament-1.9.9-build/third_party/glslang/tnt/glslang/OSDependent/Windows/Release/OSDependent.lib)
	
	add_library(MY_FILAMENT_GLSLANG_COMP STATIC IMPORTED)
	set_property(TARGET MY_FILAMENT_GLSLANG_COMP PROPERTY IMPORTED_LOCATION_DEBUG ${FILAMENT_PREBUILD_DIR}/filament-1.9.9-build/third_party/glslang/tnt/OGLCompilersDLL/Debug/OGLCompilerd.lib)
	set_property(TARGET MY_FILAMENT_GLSLANG_COMP PROPERTY IMPORTED_LOCATION_RELEASE ${FILAMENT_PREBUILD_DIR}/filament-1.9.9-build/third_party/glslang/tnt/OGLCompilersDLL/Release/OGLCompiler.lib)
	
	add_library(MY_FILAMENT_SPIRV_TOOLS STATIC IMPORTED)
	set_property(TARGET MY_FILAMENT_SPIRV_TOOLS PROPERTY IMPORTED_LOCATION_DEBUG ${FILAMENT_PREBUILD_DIR}/filament-1.9.9-build/third_party/spirv-tools/source/Debug/SPIRV-Tools.lib)
	set_property(TARGET MY_FILAMENT_SPIRV_TOOLS PROPERTY IMPORTED_LOCATION_RELEASE ${FILAMENT_PREBUILD_DIR}/filament-1.9.9-build/third_party/spirv-tools/source/Release/SPIRV-Tools.lib)
	
	add_library(MY_FILAMENT_SPIRV_TOOLS_OPT STATIC IMPORTED)
	set_property(TARGET MY_FILAMENT_SPIRV_TOOLS_OPT PROPERTY IMPORTED_LOCATION_DEBUG ${FILAMENT_PREBUILD_DIR}/filament-1.9.9-build/third_party/spirv-tools/source/opt/Debug/SPIRV-Tools-opt.lib)
	set_property(TARGET MY_FILAMENT_SPIRV_TOOLS_OPT PROPERTY IMPORTED_LOCATION_RELEASE ${FILAMENT_PREBUILD_DIR}/filament-1.9.9-build/third_party/spirv-tools/source/opt/Release/SPIRV-Tools-opt.lib)
	
	add_library(MY_FILAMENT_SPIRV STATIC IMPORTED)
	set_property(TARGET MY_FILAMENT_SPIRV PROPERTY IMPORTED_LOCATION_DEBUG ${FILAMENT_PREBUILD_DIR}/filament-1.9.9-build/third_party/glslang/tnt/SPIRV/Debug/SPIRVd.lib)
	set_property(TARGET MY_FILAMENT_SPIRV PROPERTY IMPORTED_LOCATION_RELEASE ${FILAMENT_PREBUILD_DIR}/filament-1.9.9-build/third_party/glslang/tnt/SPIRV/Release/SPIRV.lib)
	
	add_library(MY_FILAMENT_SPV_REMAPPER STATIC IMPORTED)
	set_property(TARGET MY_FILAMENT_SPV_REMAPPER PROPERTY IMPORTED_LOCATION_DEBUG ${FILAMENT_PREBUILD_DIR}/filament-1.9.9-build/third_party/glslang/tnt/SPIRV/Debug/SPVRemapperd.lib)
	set_property(TARGET MY_FILAMENT_SPV_REMAPPER PROPERTY IMPORTED_LOCATION_RELEASE ${FILAMENT_PREBUILD_DIR}/filament-1.9.9-build/third_party/glslang/tnt/SPIRV/Release/SPVRemapper.lib)
	
	add_library(MY_FILAMENT_APP_RES STATIC IMPORTED)
	set_property(TARGET MY_FILAMENT_APP_RES PROPERTY IMPORTED_LOCATION_DEBUG ${FILAMENT_PREBUILD_DIR}/filament-1.9.9-build/libs/filamentapp/Debug/filamentapp-resources.lib)
	set_property(TARGET MY_FILAMENT_APP_RES PROPERTY IMPORTED_LOCATION_RELEASE ${FILAMENT_PREBUILD_DIR}/filament-1.9.9-build/libs/filamentapp/Release/filamentapp-resources.lib)
	
	add_library(MY_FILAMENT_TINYEXR STATIC IMPORTED)
	set_property(TARGET MY_FILAMENT_TINYEXR PROPERTY IMPORTED_LOCATION_DEBUG ${FILAMENT_PREBUILD_DIR}/filament-1.9.9-build/third_party/tinyexr/tnt/Debug/tinyexr.lib)
	set_property(TARGET MY_FILAMENT_TINYEXR PROPERTY IMPORTED_LOCATION_RELEASE ${FILAMENT_PREBUILD_DIR}/filament-1.9.9-build/third_party/tinyexr/tnt/Release/tinyexr.lib)
	
	add_library(MY_FILAMENT_LIBZ STATIC IMPORTED)
	set_property(TARGET MY_FILAMENT_LIBZ PROPERTY IMPORTED_LOCATION_DEBUG ${FILAMENT_PREBUILD_DIR}/filament-1.9.9-build/third_party/libz/tnt/Debug/z.lib)
	set_property(TARGET MY_FILAMENT_LIBZ PROPERTY IMPORTED_LOCATION_RELEASE ${FILAMENT_PREBUILD_DIR}/filament-1.9.9-build/third_party/libz/tnt/Release/z.lib)

elseif(APPLE)
    set(LIBS bluegl bluevk camutils filabridge filaflat filamat filamentapp filameshio filagui geometry gltfio ibl image imageio matdbg math mathio utils viewer )
    set(FILAMENT_PREBUILD_DIR ${CMAKE_BINARY_DIR}/filamentApp/filament-1.9.9-build)
    set(THIRD_PARTY_LIBS imgui smol-v tinyexr z sdl2 assimp png)
    set(LIB_LOCATIONS
        ${FILAMENT_PREBUILD_DIR}/shaders/Debug/libshaders.a
        ${FILAMENT_PREBUILD_DIR}/third_party/getopt/Debug/libgetopt.a
        
        ${FILAMENT_PREBUILD_DIR}/third_party/spirv-cross/tnt/Debug/libspirv-cross-core.a
        ${FILAMENT_PREBUILD_DIR}/third_party/spirv-cross/tnt/Debug/libspirv-cross-glsl.a
        ${FILAMENT_PREBUILD_DIR}/third_party/spirv-cross/tnt/Debug/libspirv-cross-msl.a
        ${FILAMENT_PREBUILD_DIR}/third_party/spirv-tools/source/opt/Debug/libSPIRV-Tools-opt.a
        ${FILAMENT_PREBUILD_DIR}/third_party/spirv-tools/source/Debug/libSPIRV-Tools.a
        
        
        ${FILAMENT_PREBUILD_DIR}/third_party/glslang/tnt/glslang/Debug/libglslang.a
        ${FILAMENT_PREBUILD_DIR}/third_party/glslang/tnt/SPIRV/Debug/libSPIRV.a
        ${FILAMENT_PREBUILD_DIR}/third_party/glslang/tnt/SPIRV/Debug/libSPVRemapper.a
        ${FILAMENT_PREBUILD_DIR}/third_party/glslang/tnt/OGLCompilersDLL/Debug/libOGLCompiler.a
        ${FILAMENT_PREBUILD_DIR}/third_party/glslang/tnt/glslang/OSDependent/Unix/Debug/libOSDependent.a
        
        ${FILAMENT_PREBUILD_DIR}/libs/filamentapp/Debug/libfilamentapp-resources.a
        ${FILAMENT_PREBUILD_DIR}/libs/ibl/Debug/libibl-lite.a
        ${FILAMENT_PREBUILD_DIR}/libs/gltfio/Debug/libgltfio_core.a
        ${FILAMENT_PREBUILD_DIR}/libs/gltfio/Debug/libgltfio_resources.a
        ${FILAMENT_PREBUILD_DIR}/filament/backend/Debug/libbackend.a
        ${FILAMENT_PREBUILD_DIR}/third_party/draco/tnt/Debug/libdracodec.a
        
    )
    foreach(LIB_LOC ${LIB_LOCATIONS})
    
        STRING(REGEX MATCH "lib[-a-zA-Z_1-9]+[\.]" LIB_NAME "${LIB_LOC}")
        STRING(LENGTH ${LIB_NAME} LIB_LENGTH)
        math(EXPR MY_LIB_LENGTH "${LIB_LENGTH} - 4")
        STRING(SUBSTRING ${LIB_NAME} 3 ${MY_LIB_LENGTH} MY_LIB_NAME)
        
        
        add_library(${MY_LIB_NAME} STATIC IMPORTED)
        set_property(TARGET ${MY_LIB_NAME} PROPERTY IMPORTED_LOCATION_DEBUG ${LIB_LOC})
        target_link_libraries (${PLUGIN_NAME} ${MY_LIB_NAME})
    endforeach()
    add_library(MY_FILAMENT STATIC IMPORTED)
	set_property(TARGET MY_FILAMENT PROPERTY IMPORTED_LOCATION_DEBUG ${FILAMENT_PREBUILD_DIR}/filament/Debug/libfilament.a)
	set_property(TARGET MY_FILAMENT PROPERTY IMPORTED_LOCATION_RELEASE ${FILAMENT_PREBUILD_DIR}/filament/Debug/libfilament.a)
endif ()


if(WIN32)
	target_link_libraries (${PLUGIN_NAME} apePluginManager apeSceneManager apeEventManager apeCoreConfig apeUserInputMacro ${OPENGL_LIBRARIES}
	MY_FILAMENT MY_FILAMENT_APP MY_FILAMENT_UTILS MY_FILAMENT_GLTFIO MY_FILAMENT_GLTFIO_CORE MY_FILAMENT_DRACODEC MY_FILAMENT_BACKEND MY_FILAMENT_SHADERS MY_FILAMENT_MATH MY_FILAMENT_GEOMETRY MY_FILAMENT_VIEWER
MY_FILAMENT_IBL MY_FILAMENT_FLAT MY_FILAMENT_BRIDGE MY_FILAMENT_CAMUTILS MY_FILAMENT_BLUEGL MY_FILAMENT_IMAGE MY_FILAMENT_SMOL-V MY_FILAMENT_MESHOPT MY_FILAMENT_MATDBG MY_FILAMENT_IMGUI MY_FILAMENT_SDL2 MY_FILAMENT_FILAGUI
MY_FILAMENT_SPIRV_CROSS_GLSL MY_FILAMENT_SPIRV_CROSS_CORE MY_FILAMENT_SPIRV_CROSS_MSL MY_FILAMENT_GLSLANG MY_FILAMENT_GLSLANG_OS MY_FILAMENT_GLSLANG_COMP MY_FILAMENT_SPIRV_TOOLS MY_FILAMENT_SPIRV_TOOLS_OPT
MY_FILAMENT_SPIRV MY_FILAMENT_SPV_REMAPPER MY_FILAMENT_APP_RES MY_FILAMENT_GLTFIO_RES MY_FILAMENT_IMAGEIO MY_FILAMENT_PNG MY_FILAMENT_TINYEXR MY_FILAMENT_LIBZ
Shlwapi.lib) 

elseif(APPLE)
	foreach(lib ${LIBS})
        		add_library(${lib} STATIC IMPORTED)
        		set_target_properties(${lib} PROPERTIES IMPORTED_LOCATION ${FILAMENT_PREBUILD_DIR}/libs/${lib}/Debug/lib${lib}.a)
		target_link_libraries (${PLUGIN_NAME} ${lib})
	endforeach()

	foreach(third_party_lib ${THIRD_PARTY_LIBS})
        add_library(${third_party_lib} STATIC IMPORTED)
        if(EXISTS ${FILAMENT_PREBUILD_DIR}/third_party/${third_party_lib}/tnt/Debug/lib${third_party_lib}.a)
		     set_target_properties(${third_party_lib} PROPERTIES IMPORTED_LOCATION ${FILAMENT_PREBUILD_DIR}/third_party/${third_party_lib}/tnt/Debug/lib${third_party_lib}.a)
        elseif(EXISTS ${FILAMENT_PREBUILD_DIR}/third_party/lib${third_party_lib}/tnt/Debug/lib${third_party_lib}.a)
             set_target_properties(${third_party_lib} PROPERTIES IMPORTED_LOCATION ${FILAMENT_PREBUILD_DIR}/third_party/lib${third_party_lib}/tnt/Debug/lib${third_party_lib}.a)
        else()
             set_target_properties(${third_party_lib} PROPERTIES IMPORTED_LOCATION ${FILAMENT_PREBUILD_DIR}/third_party/lib${third_party_lib}/tnt/Debug/lib${third_party_lib}.dylib)
        endif()
        target_link_libraries (${PLUGIN_NAME} ${third_party_lib})
	endforeach()
		
	target_link_libraries (${PLUGIN_NAME} apePluginManager apeSceneManager apeEventManager apeCoreConfig apeUserInputMacro ${OPENGL_LIBRARIES} MY_FILAMENT "-framework Cocoa -framework Metal -framework CoreVideo -framework QuartzCore")
    set(FILAMENT_PREBUILD_DIR ${CMAKE_BINARY_DIR}/filamentApp)
endif()
if (${CURL_READY})
	target_link_libraries (${PLUGIN_NAME} MY_CURL)
	set_property (TARGET ${PLUGIN_NAME} PROPERTY COMPILE_DEFINITIONS HTTPMANAGER_USE_CURL) 
endif ()



set_property (TARGET ${PLUGIN_NAME} PROPERTY COMPILE_DEFINITIONS APE_SOURCE_DIR="${APE_SOURCE_DIR}")

set_property (TARGET ${PLUGIN_NAME} PROPERTY FOLDER "Plugins/FilamentApplication")

file(COPY ${FILAMENT_PREBUILD_DIR}/filament-1.9.9-build/samples/default_env DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
file(COPY ${FILAMENT_PREBUILD_DIR}/filament-1.9.9-build/samples/assets DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})


