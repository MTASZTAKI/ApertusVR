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
target_link_libraries( ${MY_TARGET_NAME} ApePluginManager ApeEventManager ApeScene MY_NODEJS_LIB )

# Add preprocessor definitions
set_property( TARGET ${MY_TARGET_NAME} PROPERTY COMPILE_DEFINITIONS )

# Place target xxx into the specified folder in IDEs supporting it
set_property( TARGET ${MY_TARGET_NAME} PROPERTY FOLDER "Plugins/JsAPI" )


file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/js/package.json DESTINATION ${BUILD_PATH_DEBUG})
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/js/package.json DESTINATION ${BUILD_PATH_RELEASE})

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/js/server.js DESTINATION ${BUILD_PATH_DEBUG})
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/js/server.js DESTINATION ${BUILD_PATH_RELEASE})

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/js/logs DESTINATION ${TARGET_MODULE_PATH_DEBUG})
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/js/logs DESTINATION ${TARGET_MODULE_PATH_RELEASE})

set(APEJS_FILE_PATH_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/js/)

set(CONFIG_FILE_PATH_SOURCE ${APEJS_FILE_PATH_SOURCE}/configs/dev/paths.json)
file(WRITE ${CONFIG_FILE_PATH_SOURCE} "{\n")
file(APPEND ${CONFIG_FILE_PATH_SOURCE} "\t\"source\": {\n")
file(APPEND ${CONFIG_FILE_PATH_SOURCE} "\t\t\"apePath\": \"${APE_SOURCE_DIR}/\",\n")
file(APPEND ${CONFIG_FILE_PATH_SOURCE} "\t\t\"jsApiPath\": \"plugins/jsAPI/nodeJsPlugin/js/\"\n")
file(APPEND ${CONFIG_FILE_PATH_SOURCE} "\t},\n")
file(APPEND ${CONFIG_FILE_PATH_SOURCE} "\t\"build\": {\n")
file(APPEND ${CONFIG_FILE_PATH_SOURCE} "\t\t\"binPath\": \"${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/\",\n")
file(APPEND ${CONFIG_FILE_PATH_SOURCE} "\t\t\"nodeModulesPath\": \"node_modules/\",\n")
file(APPEND ${CONFIG_FILE_PATH_SOURCE} "\t\t\"apertusModulePath\": \"apertusvr/\"\n")
file(APPEND ${CONFIG_FILE_PATH_SOURCE} "\t}\n")
file(APPEND ${CONFIG_FILE_PATH_SOURCE} "}\n")

set(CONFIG_FILE_PATH_DEBUG ${BUILD_PATH_DEBUG}/config.json)
file(WRITE ${CONFIG_FILE_PATH_DEBUG} "{\n")
file(APPEND ${CONFIG_FILE_PATH_DEBUG} "\t\"configuration\": \"debug\",\n")
file(APPEND ${CONFIG_FILE_PATH_DEBUG} "\t\"sourcePath\": \"${APE_SOURCE_DIR}/\",\n")
file(APPEND ${CONFIG_FILE_PATH_DEBUG} "\t\"sourcePathJs\": \"${APEJS_FILE_PATH_SOURCE}\"\n")
file(APPEND ${CONFIG_FILE_PATH_DEBUG} "}\n")

set(CONFIG_FILE_PATH_RELEASE ${BUILD_PATH_RELEASE}/config.json)
file(WRITE ${CONFIG_FILE_PATH_RELEASE} "{\n")
file(APPEND ${CONFIG_FILE_PATH_RELEASE} "\t\"configuration\": \"release\",\n")
file(APPEND ${CONFIG_FILE_PATH_RELEASE} "\t\"sourcePath\": \"${APE_SOURCE_DIR}/\",\n")
file(APPEND ${CONFIG_FILE_PATH_RELEASE} "\t\"sourcePathJs\": \"${APEJS_FILE_PATH_SOURCE}\"\n")
file(APPEND ${CONFIG_FILE_PATH_RELEASE} "}\n")
