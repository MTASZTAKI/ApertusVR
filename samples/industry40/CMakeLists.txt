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

add_subdirectory (configs)


set (SOURCES
	ApeIndustry40.cpp
	)

include_directories(
	${PROJECT_SOURCE_DIR}/common/include
	${RAPIDJSON_INCLUDE_DIR} 
	)

add_executable (ApeIndustry40 ${SOURCES})
target_link_libraries (ApeIndustry40 ApeSystem ApePluginManager)
set_property (TARGET ApeIndustry40 PROPERTY COMPILE_DEFINITIONS APE_SOURCE_DIR="${APE_SOURCE_DIR}")
set_property (TARGET ApeIndustry40 PROPERTY FOLDER "Samples/Industry40")

set (SOURCES_LAUNCHER 
	ApeIndustry40MultiPlayerTester.cpp
	)
	
add_executable (ApeIndustry40MultiPlayerTester ${SOURCES_LAUNCHER})
target_link_libraries (ApeIndustry40MultiPlayerTester)
set_property (TARGET ApeIndustry40MultiPlayerTester PROPERTY COMPILE_DEFINITIONS APE_BIN_DIR="${APE_BIN_DIR}" APE_SOURCE_DIR="${APE_SOURCE_DIR}")
set_property (TARGET ApeIndustry40MultiPlayerTester PROPERTY FOLDER "Samples/Industry40")



