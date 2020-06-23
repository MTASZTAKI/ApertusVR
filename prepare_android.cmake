find_library(log-lib log)
find_library(android-lib android)

set(PREBUILT_ANDROID_LIBS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/android/libs)
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

