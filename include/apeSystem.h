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

#include <functional>

namespace ape
{
	namespace System
	{
		APE_SYSTEM_DLL_EXPORT void Start(const char* configFolderPath, bool isBlocking, std::function<void()> userThreadFunction = std::function<void()>(), int step_interval = 20);

		APE_SYSTEM_DLL_EXPORT void Stop();
	}
}

extern "C" APE_SYSTEM_DLL_EXPORT void ApeSystemStart();

extern "C" APE_SYSTEM_DLL_EXPORT void ApeSystemStop();

extern "C" APE_SYSTEM_DLL_EXPORT int ApeSceneManager_GetIndexedFaceSet_GetSize(char name[]);

extern "C" APE_SYSTEM_DLL_EXPORT bool ApeSceneManager_GetIndexedFaceSet_GetVertices(char name[], float vertices[]);

extern "C" APE_SYSTEM_DLL_EXPORT bool ApeSceneManager_GetIndexedFaceSet_GetIndices(char name[], int indices[]);

extern "C" APE_SYSTEM_DLL_EXPORT bool ApeSceneManager_GetIndexedFaceSet_GetColor(char name[], float color[]);

extern "C" APE_SYSTEM_DLL_EXPORT bool ApeSceneManager_GetNode_GetOrientation(char name[], float orientation[]);

#endif

