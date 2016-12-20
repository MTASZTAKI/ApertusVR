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

# Set target properties
set(MY_TARGET_NAME ApeNodeJsExtrension)
set(MY_TARGET_OUTPUT "/node_modules/apertusvr/")

# Add all header and cpp files in the directory to the project
set (HEADERS
	)

set (SOURCES
	apertusvr_wrap.cxx
	)

include_directories(
	${PROJECT_SOURCE_DIR}/common/include
	${JS_API_COMMON_DIR}
	${NODE_SOURCE_DIR}/src
	${NODE_SOURCE_DIR}/deps/uv/include
	${NODE_SOURCE_DIR}/deps/v8/include
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
# TODO: MY_NODEJS_LIB under unix

# Add Node lib dependency
target_link_libraries( ${MY_TARGET_NAME} ApePluginManager ApeEventManager ApeScene ApeJsAPICommon MY_NODEJS_LIB )

# Set target library's name
set_target_properties( ${MY_TARGET_NAME} PROPERTIES OUTPUT_NAME "apertusvr" )
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
set_property( TARGET ${MY_TARGET_NAME} PROPERTY COMPILE_DEFINITIONS NODE_GYP_MODULE_NAME apertusvr )
set_property( TARGET ${MY_TARGET_NAME} PROPERTY COMPILE_DEFINITIONS USING_UV_SHARED 1 )
set_property( TARGET ${MY_TARGET_NAME} PROPERTY COMPILE_DEFINITIONS USING_V8_SHARED 1 )
set_property( TARGET ${MY_TARGET_NAME} PROPERTY COMPILE_DEFINITIONS V8_DEPRECATION_WARNINGS 1 )
set_property( TARGET ${MY_TARGET_NAME} PROPERTY COMPILE_DEFINITIONS _CRT_SECURE_NO_DEPRECATE )
set_property( TARGET ${MY_TARGET_NAME} PROPERTY COMPILE_DEFINITIONS _CRT_NONSTDC_NO_DEPRECATE )
set_property( TARGET ${MY_TARGET_NAME} PROPERTY COMPILE_DEFINITIONS BUILDING_NODE_EXTENSION )

# Place target xxx into the specified folder in IDEs supporting it
set_property( TARGET ${MY_TARGET_NAME} PROPERTY FOLDER "Core/JsAPI" )
