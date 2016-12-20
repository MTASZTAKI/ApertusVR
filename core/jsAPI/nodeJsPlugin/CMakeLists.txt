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
set(MY_TARGET_NAME ApeNodeJsPlugin)
set(MY_TARGET_OUTPUT "/node_modules/apertusvr/")

set(BUILD_PATH_DEBUG ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Debug)
set(BUILD_PATH_RELEASE ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Release)

set(TARGET_MODULE_PATH_DEBUG ${BUILD_PATH_DEBUG}/${MY_TARGET_OUTPUT})
set(TARGET_MODULE_PATH_RELEASE ${BUILD_PATH_RELEASE}/${MY_TARGET_OUTPUT})

# Add all header and cpp files in the directory to the project
set (HEADERS
	ApeNodeJsPlugin.h
	)

set (SOURCES
	ApeNodeJsPlugin.cpp
	js/apeHttpApi.js
	js/apeServer.js
	js/package.json
	)

include_directories(
	${PROJECT_SOURCE_DIR}/common/include
	${JS_API_COMMON_DIR}
	${NODE_SOURCE_DIR}/src
	${NODE_SOURCE_DIR}/deps/uv/include
	${NODE_SOURCE_DIR}/deps/v8/include
	)

# Add the dynamic library target to the project
add_library( ${MY_TARGET_NAME} SHARED ${SOURCES} ${HEADERS} )

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

# Link dependencies
target_link_libraries( ${MY_TARGET_NAME} ApePluginManager ApeEventManager ApeScene ApeJsAPICommon MY_NODEJS_LIB )

# Add preprocessor definitions
set_property( TARGET ${MY_TARGET_NAME} PROPERTY COMPILE_DEFINITIONS )

# Place target xxx into the specified folder in IDEs supporting it
set_property( TARGET ${MY_TARGET_NAME} PROPERTY FOLDER "Core/JsAPI" )


if (NOT EXISTS ${BUILD_PATH_DEBUG}/package.json)
	file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/js/package.json DESTINATION ${BUILD_PATH_DEBUG})
endif ()
if (NOT EXISTS ${BUILD_PATH_RELEASE}/package.json)
	file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/js/package.json DESTINATION ${BUILD_PATH_RELEASE})
endif ()

if (NOT EXISTS ${BUILD_PATH_DEBUG}/apeServer.js)
	file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/js/apeServer.js DESTINATION ${BUILD_PATH_DEBUG})
endif ()
if (NOT EXISTS ${BUILD_PATH_RELEASE}/apeServer.js)
	file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/js/apeServer.js DESTINATION ${BUILD_PATH_RELEASE})
endif ()

if (NOT EXISTS ${TARGET_MODULE_PATH_DEBUG}/js/apeHttpApi.js)
	file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/js/apeHttpApi.js DESTINATION ${TARGET_MODULE_PATH_DEBUG}/js)
endif ()
if (NOT EXISTS ${TARGET_MODULE_PATH_RELEASE}/js/apeHttpApi.js)
	file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/js/apeHttpApi.js DESTINATION ${TARGET_MODULE_PATH_RELEASE}/js)
endif ()

# Run npm install command in output folder. This will install node module dependencies according to package.json
add_custom_command(
        TARGET ${MY_TARGET_NAME}
        POST_BUILD
        WORKING_DIRECTORY ${BUILD_PATH_DEBUG}
        COMMAND npm install
        )
add_custom_command(
        TARGET ${MY_TARGET_NAME}
        POST_BUILD
        WORKING_DIRECTORY ${BUILD_PATH_RELEASE}
        COMMAND npm install
        )
