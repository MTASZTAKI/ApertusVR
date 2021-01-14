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

find_package(OpenGL REQUIRED)
if(WIN32)
	set(FILAMENT_DOWNLOAD_URL "https://github.com/google/filament/releases/download/v1.9.9/filament-v1.9.9-windows.tgz")
	set(FILAMENT_PREBUILD_DIR ${CMAKE_BINARY_DIR}/filamentWindows/build)
	set(FILAMENT_PREBUILD_ZIP ${CMAKE_BINARY_DIR}/filamentWindows/filament-v1.9.9-windows.tgz)
elseif(APPLE)
	set(FILAMENT_DOWNLOAD_URL "https://github.com/google/filament/releases/download/v1.9.9/filament-v1.9.9-mac.tgz")
	set(FILAMENT_PREBUILD_DIR ${CMAKE_BINARY_DIR}/filamentOSX/filament_build)
	set(FILAMENT_PREBUILD_ZIP ${CMAKE_BINARY_DIR}/filamentOSX/filament-v1.9.9-mac.tgz)
endif()

if (NOT EXISTS ${FILAMENT_PREBUILD_DIR})
		file(MAKE_DIRECTORY ${FILAMENT_PREBUILD_DIR})
endif ()

set(FILAMENT_READY FALSE)
if(WIN32)
	if(EXISTS ${FILAMENT_PREBUILD_DIR}/lib/x86_64/mdd/filament.lib)
		set(FILAMENT_READY TRUE)
		message (STATUS "Filament is installed" )
	else ()
		set(FILAMENT_READY FALSE)
		message (STATUS "Filament is not installed" )
	endif ()
elseif(APPLE)
	if(EXISTS ${FILAMENT_PREBUILD_DIR}/filament/Debug/libfilament.a)
		set(FILAMENT_READY TRUE)
		message (STATUS "Filament is installed" )
	else ()
		set(FILAMENT_READY FALSE)
		message (STATUS "Filament is not installed" )
	endif ()
endif()
if (NOT FILAMENT_READY)
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
                ${FILAMENT_PREBUILD_DIR}/include
                ${CMAKE_CURRENT_SOURCE_DIR}/3rdParty/rapidjson/include
        )
elseif(APPLE)

    set (HEADERS
                apeFilamentApplicationPluginConfigs.h
                apeFilamentApplicationPlugin.h
        )
    set (SOURCES
                apeFilamentApplicationPlugin.cpp
        )
    include_directories(
        ${FILAMENT_PREBUILD_DIR}/include
        ${FILAMENT_PREBUILD_DIR}/include/imgui
	${FILAMENT_PREBUILD_DIR}/include/image
	${FILAMENT_PREBUILD_DIR}/include/imageio
	${FILAMENT_PREBUILD_DIR}/include/gltfio
        ${FILAMENT_PREBUILD_DIR}/include/getopt
        ${FILAMENT_PREBUILD_DIR}/include/libsdl2
        ${CMAKE_CURRENT_SOURCE_DIR}/3rdParty/rapidjson/include
        )
endif()
add_library (${PLUGIN_NAME} SHARED ${SOURCES} ${HEADERS})

add_library(MY_FILAMENT_APP STATIC IMPORTED)
if (WIN32)
	set_property(TARGET MY_FILAMENT_APP PROPERTY IMPORTED_LOCATION_DEBUG ${FILAMENT_PREBUILD_DIR}/lib/x86_64/mdd/filament.lib)
	set_property(TARGET MY_FILAMENT_APP PROPERTY IMPORTED_LOCATION_RELEASE ${FILAMENT_PREBUILD_DIR}/lib/x86_64/md/filament.lib)

	add_library(MY_FILAMENT_UTILS STATIC IMPORTED)
	set_property(TARGET MY_FILAMENT_UTILS PROPERTY IMPORTED_LOCATION_DEBUG ${FILAMENT_PREBUILD_DIR}/lib/x86_64/mdd/utils.lib)
	set_property(TARGET MY_FILAMENT_UTILS PROPERTY IMPORTED_LOCATION_RELEASE ${FILAMENT_PREBUILD_DIR}/lib/x86_64/md/utils.lib)

	add_library(MY_FILAMENT_GLTFIO STATIC IMPORTED)
	set_property(TARGET MY_FILAMENT_GLTFIO PROPERTY IMPORTED_LOCATION_DEBUG ${FILAMENT_PREBUILD_DIR}/lib/x86_64/mdd/gltfio.lib)
	set_property(TARGET MY_FILAMENT_GLTFIO PROPERTY IMPORTED_LOCATION_RELEASE ${FILAMENT_PREBUILD_DIR}/lib/x86_64/md/gltfio.lib)

	add_library(MY_FILAMENT_GLTFIO_CORE STATIC IMPORTED)
	set_property(TARGET MY_FILAMENT_GLTFIO_CORE PROPERTY IMPORTED_LOCATION_DEBUG ${FILAMENT_PREBUILD_DIR}/lib/x86_64/mdd/gltfio_core.lib)
	set_property(TARGET MY_FILAMENT_GLTFIO_CORE PROPERTY IMPORTED_LOCATION_RELEASE ${FILAMENT_PREBUILD_DIR}/lib/x86_64/md/gltfio_core.lib)
		
	add_library(MY_FILAMENT_DRACODEC STATIC IMPORTED)
	set_property(TARGET MY_FILAMENT_DRACODEC PROPERTY IMPORTED_LOCATION_DEBUG ${FILAMENT_PREBUILD_DIR}/lib/x86_64/mdd/dracodec.lib)
	set_property(TARGET MY_FILAMENT_DRACODEC PROPERTY IMPORTED_LOCATION_RELEASE ${FILAMENT_PREBUILD_DIR}/lib/x86_64/md/dracodec.lib)

	add_library(MY_FILAMENT_BACKEND STATIC IMPORTED)
	set_property(TARGET MY_FILAMENT_BACKEND PROPERTY IMPORTED_LOCATION_DEBUG ${FILAMENT_PREBUILD_DIR}/lib/x86_64/mdd/backend.lib)
	set_property(TARGET MY_FILAMENT_BACKEND PROPERTY IMPORTED_LOCATION_RELEASE ${FILAMENT_PREBUILD_DIR}/lib/x86_64/md/backend.lib)

	add_library(MY_FILAMENT_SHADERS STATIC IMPORTED)
	set_property(TARGET MY_FILAMENT_SHADERS PROPERTY IMPORTED_LOCATION_DEBUG ${FILAMENT_PREBUILD_DIR}/lib/x86_64/mdd/shaders.lib)
	set_property(TARGET MY_FILAMENT_SHADERS PROPERTY IMPORTED_LOCATION_RELEASE ${FILAMENT_PREBUILD_DIR}/lib/x86_64/md/shaders.lib)
	
	add_library(MY_FILAMENT_MATH STATIC IMPORTED)
	set_property(TARGET MY_FILAMENT_MATH PROPERTY IMPORTED_LOCATION_DEBUG ${FILAMENT_PREBUILD_DIR}/lib/x86_64/mdd/filamat.lib)
	set_property(TARGET MY_FILAMENT_MATH PROPERTY IMPORTED_LOCATION_RELEASE ${FILAMENT_PREBUILD_DIR}/lib/x86_64/md/filamat.lib)

	add_library(MY_FILAMENT_GEOMETRY STATIC IMPORTED)
	set_property(TARGET MY_FILAMENT_GEOMETRY PROPERTY IMPORTED_LOCATION_DEBUG ${FILAMENT_PREBUILD_DIR}/lib/x86_64/mdd/geometry.lib)
	set_property(TARGET MY_FILAMENT_GEOMETRY PROPERTY IMPORTED_LOCATION_RELEASE ${FILAMENT_PREBUILD_DIR}/lib/x86_64/md/geometry.lib)

	add_library(MY_FILAMENT_VIEWER STATIC IMPORTED)
	set_property(TARGET MY_FILAMENT_VIEWER PROPERTY IMPORTED_LOCATION_DEBUG ${FILAMENT_PREBUILD_DIR}/lib/x86_64/mdd/viewer.lib)
	set_property(TARGET MY_FILAMENT_VIEWER PROPERTY IMPORTED_LOCATION_RELEASE ${FILAMENT_PREBUILD_DIR}/lib/x86_64/md/viewer.lib)

	add_library(MY_FILAMENT_IBL STATIC IMPORTED)
	set_property(TARGET MY_FILAMENT_IBL PROPERTY IMPORTED_LOCATION_DEBUG ${FILAMENT_PREBUILD_DIR}/lib/x86_64/mdd/ibl.lib)
	set_property(TARGET MY_FILAMENT_IBL PROPERTY IMPORTED_LOCATION_RELEASE ${FILAMENT_PREBUILD_DIR}/lib/x86_64/md/ibl.lib)

	add_library(MY_FILAMENT_FLAT STATIC IMPORTED)
	set_property(TARGET MY_FILAMENT_FLAT PROPERTY IMPORTED_LOCATION_DEBUG ${FILAMENT_PREBUILD_DIR}/lib/x86_64/mdd/filaflat.lib)
	set_property(TARGET MY_FILAMENT_FLAT PROPERTY IMPORTED_LOCATION_RELEASE ${FILAMENT_PREBUILD_DIR}/lib/x86_64/md/filaflat.lib)

	add_library(MY_FILAMENT_BRIDGE STATIC IMPORTED)
	set_property(TARGET MY_FILAMENT_BRIDGE PROPERTY IMPORTED_LOCATION_DEBUG ${FILAMENT_PREBUILD_DIR}/lib/x86_64/mdd/filabridge.lib)
	set_property(TARGET MY_FILAMENT_BRIDGE PROPERTY IMPORTED_LOCATION_RELEASE ${FILAMENT_PREBUILD_DIR}/lib/x86_64/md/filabridge.lib)

	add_library(MY_FILAMENT_CAMUTILS STATIC IMPORTED)
	set_property(TARGET MY_FILAMENT_CAMUTILS PROPERTY IMPORTED_LOCATION_DEBUG ${FILAMENT_PREBUILD_DIR}/lib/x86_64/mdd/camutils.lib)
	set_property(TARGET MY_FILAMENT_CAMUTILS PROPERTY IMPORTED_LOCATION_RELEASE ${FILAMENT_PREBUILD_DIR}/lib/x86_64/md/camutils.lib)

	add_library(MY_FILAMENT_BLUEGL STATIC IMPORTED)
	set_property(TARGET MY_FILAMENT_BLUEGL PROPERTY IMPORTED_LOCATION_DEBUG ${FILAMENT_PREBUILD_DIR}/lib/x86_64/mdd/bluegl.lib)
	set_property(TARGET MY_FILAMENT_BLUEGL PROPERTY IMPORTED_LOCATION_RELEASE ${FILAMENT_PREBUILD_DIR}/lib/x86_64/md/bluegl.lib)

	add_library(MY_FILAMENT_BLUEVK STATIC IMPORTED)
	set_property(TARGET MY_FILAMENT_BLUEVK PROPERTY IMPORTED_LOCATION_DEBUG ${FILAMENT_PREBUILD_DIR}/lib/x86_64/mdd/bluevk.lib)
	set_property(TARGET MY_FILAMENT_BLUEVK PROPERTY IMPORTED_LOCATION_RELEASE ${FILAMENT_PREBUILD_DIR}/lib/x86_64/md/bluevk.lib)

	add_library(MY_FILAMENT_IMAGE STATIC IMPORTED)
	set_property(TARGET MY_FILAMENT_IMAGE PROPERTY IMPORTED_LOCATION_DEBUG ${FILAMENT_PREBUILD_DIR}/lib/x86_64/mdd/image.lib)
	set_property(TARGET MY_FILAMENT_IMAGE PROPERTY IMPORTED_LOCATION_RELEASE ${FILAMENT_PREBUILD_DIR}/lib/x86_64/md/image.lib)

	add_library(MY_FILAMENT_SMOL-V STATIC IMPORTED)
	set_property(TARGET MY_FILAMENT_SMOL-V PROPERTY IMPORTED_LOCATION_DEBUG ${FILAMENT_PREBUILD_DIR}/lib/x86_64/mdd/smol-v.lib)
	set_property(TARGET MY_FILAMENT_SMOL-V PROPERTY IMPORTED_LOCATION_RELEASE ${FILAMENT_PREBUILD_DIR}/lib/x86_64/md/smol-v.lib)

	add_library(MY_FILAMENT_MESHOPT STATIC IMPORTED)
	set_property(TARGET MY_FILAMENT_MESHOPT PROPERTY IMPORTED_LOCATION_DEBUG ${FILAMENT_PREBUILD_DIR}/lib/x86_64/mdd/meshoptimizer.lib)
	set_property(TARGET MY_FILAMENT_MESHOPT PROPERTY IMPORTED_LOCATION_RELEASE ${FILAMENT_PREBUILD_DIR}/lib/x86_64/md/meshoptimizer.lib)

	add_library(MY_FILAMENT_MATDBG STATIC IMPORTED)
	set_property(TARGET MY_FILAMENT_MATDBG PROPERTY IMPORTED_LOCATION_DEBUG ${FILAMENT_PREBUILD_DIR}/lib/x86_64/mdd/matdbg.lib)
	set_property(TARGET MY_FILAMENT_MATDBG PROPERTY IMPORTED_LOCATION_RELEASE ${FILAMENT_PREBUILD_DIR}/lib/x86_64/md/matdbg.lib)

elseif(APPLE)
    set(LIBS bluegl bluevk camutils filabridge filaflat filamat filamentapp filameshio filagui geometry gltfio ibl image imageio matdbg math mathio utils viewer )

    set(THIRD_PARTY_LIBS imgui smol-v)
    set(LIB_LOCATIONS
        ${FILAMENT_PREBUILD_DIR}/shaders/Debug/libshaders.a
        ${FILAMENT_PREBUILD_DIR}/third_party/spirv-cross/tnt/Debug/libspirv-cross-core.a
        ${FILAMENT_PREBUILD_DIR}/third_party/spirv-cross/tnt/Debug/libspirv-cross-glsl.a
        ${FILAMENT_PREBUILD_DIR}/third_party/spirv-cross/tnt/Debug/libspirv-cross-msl.a
        ${FILAMENT_PREBUILD_DIR}/third_party/spirv-tools/source/opt/Debug/libSPIRV-Tools-opt.a
        ${FILAMENT_PREBUILD_DIR}/third_party/spirv-tools/source/Debug/libSPIRV-Tools.a
        ${FILAMENT_PREBUILD_DIR}/third_party/libz/tnt/Debug/libz.a
        ${FILAMENT_PREBUILD_DIR}/third_party/libsdl2/tnt/Debug/libsdl2.dylib
        ${FILAMENT_PREBUILD_DIR}/third_party/glslang/tnt/glslang/Debug/libglslang.a
        ${FILAMENT_PREBUILD_DIR}/third_party/glslang/tnt/SPIRV/Debug/libSPIRV.a
        ${FILAMENT_PREBUILD_DIR}/third_party/glslang/tnt/SPIRV/Debug/libSPVRemapper.a
        ${FILAMENT_PREBUILD_DIR}/third_party/glslang/tnt/OGLCompilersDLL/Debug/libOGLCompiler.a
        ${FILAMENT_PREBUILD_DIR}/third_party/glslang/tnt/glslang/OSDependent/Unix/Debug/libOSDependent.a
        ${FILAMENT_PREBUILD_DIR}/third_party/libassimp/tnt/Debug/libassimp.a
        ${FILAMENT_PREBUILD_DIR}/libs/filamentapp/Debug/libfilamentapp-resources.a
        ${FILAMENT_PREBUILD_DIR}/libs/ibl/Debug/libibl-lite.a
        ${FILAMENT_PREBUILD_DIR}/libs/gltfio/Debug/libgltfio_core.a
        ${FILAMENT_PREBUILD_DIR}/libs/gltfio/Debug/libgltfio_resources.a
        ${FILAMENT_PREBUILD_DIR}/filament/backend/Debug/libbackend.a
        ${FILAMENT_PREBUILD_DIR}/third_party/draco/tnt/Debug/libdracodec.a
        ${FILAMENT_PREBUILD_DIR}/third_party/getopt/Debug/libgetopt.a
        ${FILAMENT_PREBUILD_DIR}/third_party/libpng/tnt/Debug/libpng.a
        ${FILAMENT_PREBUILD_DIR}/third_party/tinyexr/tnt/Debug/libtinyexr.a
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
    
	set_property(TARGET MY_FILAMENT_APP PROPERTY IMPORTED_LOCATION_DEBUG ${FILAMENT_PREBUILD_DIR}/filament/Debug/libfilament.a)
	set_property(TARGET MY_FILAMENT_APP PROPERTY IMPORTED_LOCATION_RELEASE ${FILAMENT_PREBUILD_DIR}/filament/Debug/libfilament.a)
endif ()


if(WIN32)
	target_link_libraries (${PLUGIN_NAME} apePluginManager apeSceneManager apeEventManager apeCoreConfig apeUserInputMacro ${OPENGL_LIBRARIES}
	MY_FILAMENT_APP MY_FILAMENT_UTILS MY_FILAMENT_GLTFIO MY_FILAMENT_GLTFIO_CORE MY_FILAMENT_DRACODEC MY_FILAMENT_BACKEND MY_FILAMENT_SHADERS MY_FILAMENT_MATH MY_FILAMENT_GEOMETRY MY_FILAMENT_VIEWER
MY_FILAMENT_IBL MY_FILAMENT_FLAT MY_FILAMENT_BRIDGE MY_FILAMENT_CAMUTILS MY_FILAMENT_BLUEGL MY_FILAMENT_BLUEVK MY_FILAMENT_IMAGE MY_FILAMENT_SMOL-V MY_FILAMENT_MESHOPT MY_FILAMENT_MATDBG
Shlwapi.lib)

elseif(APPLE)
	foreach(lib ${LIBS})
        		add_library(${lib} STATIC IMPORTED)
        		set_target_properties(${lib} PROPERTIES IMPORTED_LOCATION ${FILAMENT_PREBUILD_DIR}/libs/${lib}/Debug/lib${lib}.a)
		target_link_libraries (${PLUGIN_NAME} ${lib})
	endforeach()

	foreach(third_party_lib ${THIRD_PARTY_LIBS})
		add_library(${third_party_lib} STATIC IMPORTED)
		set_target_properties(${third_party_lib} PROPERTIES IMPORTED_LOCATION ${FILAMENT_PREBUILD_DIR}/third_party/${third_party_lib}/tnt/Debug/lib${third_party_lib}.a)
		target_link_libraries (${PLUGIN_NAME} ${third_party_lib})
	endforeach()
		
	target_link_libraries (${PLUGIN_NAME} apePluginManager apeSceneManager apeEventManager apeCoreConfig apeUserInputMacro ${OPENGL_LIBRARIES} MY_FILAMENT_APP "-framework Cocoa -framework Metal -framework CoreVideo -framework QuartzCore")
endif()

set_property (TARGET ${PLUGIN_NAME} PROPERTY COMPILE_DEFINITIONS APE_SOURCE_DIR="${APE_SOURCE_DIR}")

set_property (TARGET ${PLUGIN_NAME} PROPERTY FOLDER "Plugins/FilamentApplication")


