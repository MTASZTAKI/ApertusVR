/*MIT License

Copyright (c) 2016 MTA SZTAKI

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

#ifndef APE_SYSTEM_H
#define APE_SYSTEM_H

#ifdef _WIN32
#ifdef BUILDING_APE_SYSTEM_DLL
#define APE_SYSTEM_DLL_EXPORT __declspec(dllexport)
#else
#define APE_SYSTEM_DLL_EXPORT __declspec(dllimport)
#endif
#else
#define APE_SYSTEM_DLL_EXPORT
#endif


#ifdef __cplusplus
#include <string>
#include "ApeSingleton.h"
#include "ApeISceneSession.h"

namespace Ape
{
	namespace System
	{
		extern "C" APE_SYSTEM_DLL_EXPORT void Start(const char* configFolderPath, int isBlockingMode);
		extern "C" APE_SYSTEM_DLL_EXPORT void Stop();
	}
}

#endif

#endif
