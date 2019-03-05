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

# freenect2 patch log begin
# 3rdParty\multikinect\libfreenect2-master\cmake_modules\FindLibUSB.cmake was modified
# 3rdParty\multikinect\libfreenect2-master\depends\install_libusb_vs2017.cmd was added
# 3rdParty\multikinect\libfreenect2-master\src\cuda_depth_packet_processor.cu was modified
# 3rdParty\multikinect\libfreenect2-master\src\cuda_kde_depth_packet_processor.cu was modified
# freenect2 log end

set (MULTI_KINECT_HOME ${CMAKE_SOURCE_DIR}/3rdParty/multiKinect/libfreenect2-master)
set (MULTI_KINECT_READY FALSE)

#error function
function (reportError what and when)
	if (${${when}})
		message (${${what}})
	endif ()
endfunction ()
#find multiKinect
if(EXISTS ${MULTI_KINECT_HOME}/bin/Debug/freenect2d.dll AND EXISTS ${MULTI_KINECT_HOME}/bin/Release/freenect2.dll)
	set(MULTI_KINECT_READY TRUE)
	message (STATUS "multiKinect is installed" )
else ()
	set(MULTI_KINECT_READY FALSE)
	message (STATUS "multiKinect is not installed" )
endif ()
#find multiKinect deps
if(NOT EXISTS ${MULTI_KINECT_HOME}/depends/libusb_src/x64/Release/dll/libusb-1.0.dll)
	message (STATUS "libusb is not installed, try to install it..." )
	execute_process ( 
			COMMAND cmd /c install_libusb_vs2017.cmd
			WORKING_DIRECTORY ${MULTI_KINECT_HOME}/depends
			RESULT_VARIABLE error OUTPUT_VARIABLE output ERROR_VARIABLE output)
		reportError (output if error)
endif ()
if (NOT MULTI_KINECT_READY)
#build multiKinect
	foreach (CONF ${CMAKE_CONFIGURATION_TYPES})
		message (STATUS "Configuring multiKinect for ${CONF}, please wait.....")
		execute_process ( 
				COMMAND ${CMAKE_COMMAND} 
				-G ${CMAKE_GENERATOR} 
				.
				WORKING_DIRECTORY ${MULTI_KINECT_HOME}
				RESULT_VARIABLE error OUTPUT_VARIABLE output ERROR_VARIABLE output
				)		
		reportError (output if error)
		message (STATUS "Building multiKinect for ${CONF}, please wait.....")
		execute_process ( 
			COMMAND ${CMAKE_COMMAND} --build . --config ${CONF} --target ALL_BUILD
			WORKING_DIRECTORY ${MULTI_KINECT_HOME}
			RESULT_VARIABLE error OUTPUT_VARIABLE output ERROR_VARIABLE output)
		reportError (output if error)
	endforeach ()
endif ()
#use multiKinect
add_library(MY_MULTI_KINECT STATIC IMPORTED)
if (WIN32)
	set_property(TARGET MY_MULTI_KINECT PROPERTY IMPORTED_LOCATION_DEBUG ${MULTI_KINECT_HOME}/lib/Debug/freenect2d.lib)
	set_property(TARGET MY_MULTI_KINECT PROPERTY IMPORTED_LOCATION_RELEASE ${MULTI_KINECT_HOME}/lib/Release/freenect2.lib)
endif ()

#ApeMultiKinectPlugin
set (HEADERS
	ApeMultiKinectPlugin.h
	)

set (SOURCES 
	ApeMultiKinectPlugin.cpp
	)

include_directories(
	${PROJECT_SOURCE_DIR}/common/include  
	${MULTI_KINECT_HOME}/include
	${MULTI_KINECT_HOME}/include/internal
	${MULTI_KINECT_HOME}
	${RAPIDJSON_INCLUDE_DIR}
	)

# Add the dinamic library target to the project
add_library (ApeMultiKinectPlugin SHARED ${SOURCES} ${HEADERS})

# Link with dependency libraries
target_link_libraries (ApeMultiKinectPlugin ApePluginManager ApeSceneManager ApeEventManager ApeLogManager ApeSystemConfig ApeMainWindow MY_MULTI_KINECT)
	
# Place target xxx into the specified folder in IDEs supporting it
set_property (TARGET ApeMultiKinectPlugin PROPERTY COMPILE_DEFINITIONS APE_SOURCE_DIR="${APE_SOURCE_DIR}")
set_property (TARGET ApeMultiKinectPlugin PROPERTY FOLDER "Plugins")

if (WIN32)
	# debug dlls
	if(NOT EXISTS ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/debug/freenect2d.dll)
		file(COPY ${MULTI_KINECT_HOME}/bin/Debug/freenect2d.dll DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/debug)
	endif()
	if(NOT EXISTS ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/debug/libusb-1.0.dll)
		file(COPY ${MULTI_KINECT_HOME}/bin/Debug/libusb-1.0.dll DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/debug)
	endif()
	if(NOT EXISTS ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/debug/turbojpeg.dll)
		file(COPY ${MULTI_KINECT_HOME}/bin/Debug/turbojpeg.dll DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/debug)
	endif()
	# release dlls
	if(NOT EXISTS ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/release/freenect2.dll)
		file(COPY ${MULTI_KINECT_HOME}/bin/Release/freenect2.dll DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/release)
	endif()
	if(NOT EXISTS ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Release/libusb-1.0.dll)
		file(COPY ${MULTI_KINECT_HOME}/bin/Release/libusb-1.0.dll DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Release)
	endif()
	if(NOT EXISTS ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Release/turbojpeg.dll)
		file(COPY ${MULTI_KINECT_HOME}/bin/Release/turbojpeg.dll DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Release)
	endif()
endif ()




