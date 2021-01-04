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

cmake_minimum_required(VERSION 3.5)
project(apertusVR)
include("functions.cmake")
include("prepare_cxx.cmake")
include("prepare_cmake.cmake")
include("prepare_ape.cmake")



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




if(ANDROID)
    include("prepare_android.cmake")
endif()

include_directories("include")

if(WIN32)
    add_subdirectory(core)
    add_subdirectory(plugins)
    add_subdirectory(include)
    add_subdirectory(samples)
    add_subdirectory(macros)
elseif(ANDORID)
    add_subdirectory(core)
    add_subdirectory(plugins)
    add_subdirectory(include)
    add_subdirectory(macros)
elseif(APPLE)
    add_subdirectory(core)
    add_subdirectory(plugins)
    add_subdirectory(macros)
    add_subdirectory(samples)
endif()
