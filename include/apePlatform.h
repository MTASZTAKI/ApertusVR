/*MIT License

Copyright (c) 2018 MTA SZTAKI

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.*/

#ifndef APE_PLATFORM_H
#define APE_PLATFORM_H

#ifdef _WIN32
#define APE_PLATFORM_WIN
	#ifdef _WIN64
		#define APE_PLATFORM_WIN64
		#define APE_PLATFORM_STRING "Windows 64"
	#else
		#define APE_PLATFORM_WIN32
		#define APE_PLATFORM_STRING "Windows 32"
	#endif
#elif __APPLE__
#include "TargetConditionals.h"
	#define APE_PLATFORM_APPLE
	#if TARGET_IPHONE_SIMULATOR
		#define APE_PLATFORM_APPLE_IOS_SIMULATOR
		#define APE_PLATFORM_STRING "Apple - iOS Simulator"
	#elif TARGET_OS_IPHONE
		#define APE_PLATFORM_APPLE_IOS_DEVICE
		#define APE_PLATFORM_STRING "Apple - iOS device"
	#elif TARGET_OS_MAC
		#define APE_PLATFORM_APPLE_MACOS
		#define APE_PLATFORM_STRING "Apple - MacOS"
	#else
		#define APE_PLATFORM_APPLE_UNKNOWN
		#define APE_PLATFORM_STRING "Apple - Unknown"
    #endif
#elif __ANDROID__
#define APE_PLATFORM_LINUX
#define APE_PLATFORM_ANDROID
#define APE_PLATFORM_STRING "Android"
#elif __linux__
#define APE_PLATFORM_LINUX
	#define APE_PLATFORM_STRING "Linux"
#elif __unix__
	#define APE_PLATFORM_UNIX
	#define APE_PLATFORM_STRING "Unix"
#elif defined(_POSIX_VERSION)
	#define APE_PLATFORM_POSIX
	#define APE_PLATFORM_STRING "Posix"
#else
	#define APE_PLATFORM_UNKNOWN
	#define APE_PLATFORM_STRING "Unknown"
#endif

#endif
