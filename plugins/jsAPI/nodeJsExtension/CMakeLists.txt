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

# Set target properties
set(MY_TARGET_NAME ApeNodeJsExtension)
set(MY_TARGET_OUTPUT "/node_modules/apertusvr/")

set(BUILD_PATH_DEBUG ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Debug)
set(BUILD_PATH_RELEASE ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Release)

set(TARGET_MODULE_PATH_DEBUG ${BUILD_PATH_DEBUG}/${MY_TARGET_OUTPUT})
set(TARGET_MODULE_PATH_RELEASE ${BUILD_PATH_RELEASE}/${MY_TARGET_OUTPUT})

# Add all header and cpp files in the directory to the project
set (HEADERS
	wrappers/ApeJsBindColor.h
	wrappers/ApeJsBindDegree.h
	wrappers/ApeJsBindEuler.h
	wrappers/ApeJsBindIndexedFaceSetGeometryImpl.h
	wrappers/ApeJsBindLightImpl.h
	wrappers/ApeJsBindManager.h
	wrappers/ApeJsBindNodeImpl.h
	wrappers/ApeJsBindQuaternion.h
	wrappers/ApeJsBindRadian.h
	wrappers/ApeJsBindMatrix4.h
	wrappers/ApeJsBindTextGeometryImpl.h
	wrappers/ApeJsBindVector3.h
	wrappers/ApeManualMaterialJsBind.h
	wrappers/ApePbsPassJsBind.h
	wrappers/ApeManualPassJsBind.h
	wrappers/ApeIndexedLineSetGeometryJsBind.h
	wrappers/ApeBoxGeometryJsBind.h
	wrappers/ApeFileGeometryJsBind.h
	wrappers/ApePointCloudJsBind.h
	)

set (SOURCES
	bindings.cpp
    ${PROJECT_SOURCE_DIR}/3rdParty/nbind/src/v8/Binding.cc
    ${PROJECT_SOURCE_DIR}/3rdParty/nbind/src/v8/Buffer.cc
    ${PROJECT_SOURCE_DIR}/3rdParty/nbind/src/common.cc
    ${PROJECT_SOURCE_DIR}/3rdParty/nbind/src/reflect.cc
	)

include_directories(
	${PROJECT_SOURCE_DIR}/common/include
	${PROJECT_SOURCE_DIR}/core/sceneManager
	${PROJECT_SOURCE_DIR}/core/sceneManager/session
	${PROJECT_SOURCE_DIR}/core/system
	${PROJECT_SOURCE_DIR}/core/systemConfig
	${PROJECT_SOURCE_DIR}/core/eventManager
	${PROJECT_SOURCE_DIR}/core/pluginManager
	${NODE_SOURCE_DIR}/src
	${NODE_SOURCE_DIR}/deps/uv/include
	${NODE_SOURCE_DIR}/deps/v8/include
	${PROJECT_SOURCE_DIR}/3rdParty/nbind/include
	${PROJECT_SOURCE_DIR}/3rdParty/nan
	${PROJECT_SOURCE_DIR}/3rdParty/raknet/Source
	${PROJECT_SOURCE_DIR}/plugins/jsAPI/nodeJsExtension/wrappers
	${PROJECT_SOURCE_DIR}/macros/userInput 	
	)

# Add the dynamic library target to the project
add_library (${MY_TARGET_NAME} SHARED ${SOURCES} ${HEADERS})

# Link with dependency libraries
if (NOT WIN32)
	target_link_libraries( ${MY_TARGET_NAME} dl pthread )
endif ()

# Use nodejs lib
add_library(MY_NODEJS_LIB STATIC IMPORTED)
if (WIN32)
    set_property(TARGET MY_NODEJS_LIB PROPERTY IMPORTED_LOCATION_DEBUG ${NODE_LIB_PATH_DEBUG})
    set_property(TARGET MY_NODEJS_LIB PROPERTY IMPORTED_LOCATION_RELEASE ${NODE_LIB_PATH_RELEASE})
endif ()
# TODO_ApeNodeJsExtension MY_NODEJS_LIB under unix

# Link with dependency libraries
if(WIN32)
	target_link_libraries( ${MY_TARGET_NAME} ApeSystem ApePluginManager ApeEventManager ApeSceneManager ApeSystemConfig ApeUserInputMacro RAKNET MY_NODEJS_LIB ws2_32)
endif()

# Set target library's name
set_target_properties( ${MY_TARGET_NAME} PROPERTIES OUTPUT_NAME "nbind" )
set_target_properties( ${MY_TARGET_NAME} PROPERTIES SUFFIX ".node" )

# Set library output dir in debug for our target
set_target_properties( ${MY_TARGET_NAME} PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/${MY_TARGET_OUTPUT} )
set_target_properties( ${MY_TARGET_NAME} PROPERTIES LIBRARY_OUTPUT_DIRECTORY_DEBUG ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/Debug/${MY_TARGET_OUTPUT} )
set_target_properties( ${MY_TARGET_NAME} PROPERTIES LIBRARY_OUTPUT_DIRECTORY_RELEASE  ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/Release/${MY_TARGET_OUTPUT} )

# Set runtime output dir property for our target
set_target_properties( ${MY_TARGET_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${MY_TARGET_OUTPUT} )
set_target_properties( ${MY_TARGET_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Debug/${MY_TARGET_OUTPUT} )
set_target_properties( ${MY_TARGET_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Release/${MY_TARGET_OUTPUT} )

# Add preprocessor definitions
target_compile_definitions(${MY_TARGET_NAME} PRIVATE NODE_GYP_MODULE_NAME=nbind USING_UV_SHARED=1 USING_V8_SHARED=1 V8_DEPRECATION_WARNINGS=1 _CRT_SECURE_NO_DEPRECATE _CRT_NONSTDC_NO_DEPRECATE BUILDING_NODE_EXTENSION)

# Add compile options
if (MSVC)
	target_compile_options(${MY_TARGET_NAME} PRIVATE "$<$<CONFIG:Debug>:/bigobj>")
endif()

# Place target xxx into the specified folder in IDEs supporting it
set_property( TARGET ${MY_TARGET_NAME} PROPERTY FOLDER "Plugins/JsAPI" )

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/package.json DESTINATION ${TARGET_MODULE_PATH_DEBUG})
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/package.json DESTINATION ${TARGET_MODULE_PATH_RELEASE})
