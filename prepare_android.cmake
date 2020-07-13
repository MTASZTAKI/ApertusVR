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

find_library(log-lib log)
find_library(android-lib android)

set(PREBUILT_ANDROID_LIBS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/androidx/libs)
set(ANDROID_SYSROOT ${ANDROID_NDK}/sysroot)

if(${ANDROID_ABI} STREQUAL "arm64-v8a")
        set(TARGET_HOST "aarch64-linux-android")
elseif(${ANDROID_ABI} STREQUAL "armeabi-v7a")
        set(TARGET_HOST "arm-linux-androideabi")
elseif(${ANDROID_ABI} STREQUAL "x86")
        set(TARGET_HOST "i686-linux-android")
elseif(${ANDROID_ABI} STREQUAL "x86_64")
        set(TARGET_HOST "x86_64-linux-android")
endif()

include_directories(${ANDROID_SYSROOT}/usr/include/${TARGET_HOST})