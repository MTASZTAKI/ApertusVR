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

extern "C" APE_SYSTEM_DLL_EXPORT void ApeSystemStart(char* configFolderPath);

extern "C" APE_SYSTEM_DLL_EXPORT void ApeSystemStop();

typedef void(__stdcall* ANSWERCB)(char*, int);
static ANSWERCB cb;

extern "C" APE_SYSTEM_DLL_EXPORT void ApeEventManager_RegisterCallback(ANSWERCB fp);

extern "C" APE_SYSTEM_DLL_EXPORT bool ApeSceneManager_GetIndexedFaceSet_GetVerticesSize(char* name, int* size);

extern "C" APE_SYSTEM_DLL_EXPORT bool ApeSceneManager_GetIndexedFaceSet_GetVertices(char* name, float* vertices);

extern "C" APE_SYSTEM_DLL_EXPORT bool ApeSceneManager_GetIndexedFaceSet_GetIndicesSize(char* name, int* size);

extern "C" APE_SYSTEM_DLL_EXPORT bool ApeSceneManager_GetIndexedFaceSet_GetIndices(char* name, int* indices);

extern "C" APE_SYSTEM_DLL_EXPORT bool ApeSceneManager_GetIndexedFaceSet_GetColor(char* name, float* color);

extern "C" APE_SYSTEM_DLL_EXPORT bool ApeSceneManager_GetIndexedFaceSet_GetParent(char* name, char* parent);

extern "C" APE_SYSTEM_DLL_EXPORT bool ApeSceneManager_GetNode_GetOrientation(char* name, float* orientation);

extern "C" APE_SYSTEM_DLL_EXPORT bool ApeSceneManager_GetNode_GetPosition(char* name, float* position);

extern "C" APE_SYSTEM_DLL_EXPORT bool ApeSceneManager_GetNode_GetScale(char* name, float* scale);

extern "C" APE_SYSTEM_DLL_EXPORT bool ApeSceneManager_GetNode_GetParent(char* name, char* parent);

extern "C" APE_SYSTEM_DLL_EXPORT bool ApeSceneManager_GetNode_GetChildrenVisibility(char* name, bool* visible);

extern "C" APE_SYSTEM_DLL_EXPORT bool ApeSceneManager_GetNode_DetachFromParent(char* name);

extern "C" APE_SYSTEM_DLL_EXPORT bool ApeSceneManager_CreateNode(char* name);

extern "C" APE_SYSTEM_DLL_EXPORT bool ApeSceneManager_DeleteNode(char* name);

extern "C" APE_SYSTEM_DLL_EXPORT bool ApeSceneManager_GetNode_GetCreator(char* name, char* creator);

extern "C" APE_SYSTEM_DLL_EXPORT bool ApeSceneManager_GetText_GetCaption(char* name, char* caption);

extern "C" APE_SYSTEM_DLL_EXPORT bool ApeSceneManager_GetNode_SetPosition(char* name, float* position);

extern "C" APE_SYSTEM_DLL_EXPORT bool ApeSceneManager_GetNode_SetOrientation(char* name, float* orientation);

extern "C" APE_SYSTEM_DLL_EXPORT bool ApeSceneManager_GetNode_SetScale(char* name, float* scale);

extern "C" APE_SYSTEM_DLL_EXPORT bool ApeSceneManager_GetNode_SetParentNode(char* name, char* parent);

extern "C" APE_SYSTEM_DLL_EXPORT bool ApeSceneManager_GetNode_SetChildrenVisibility(char* name, bool* visible);

extern "C" APE_SYSTEM_DLL_EXPORT bool ApeSceneManager_GetFileGeometry_CreateFileGeometry(char* name);

extern "C" APE_SYSTEM_DLL_EXPORT bool ApeSceneManager_GetFileGeometry_GetParentNode(char* name, char *parent);

extern "C" APE_SYSTEM_DLL_EXPORT bool ApeSceneManager_GetFileGeometry_SetParentNode(char* name, char *parent);

extern "C" APE_SYSTEM_DLL_EXPORT bool ApeSceneManager_GetFileGeometry_PlayAnimation(char* name, char * animationID);

extern "C" APE_SYSTEM_DLL_EXPORT bool ApeSceneManager_GetFileGeometry_StopAnimation(char* name, char * animationID);

extern "C" APE_SYSTEM_DLL_EXPORT bool ApeSceneManager_GetFileGeometry_GetRunningAnimation(char* name, char * animationID);

extern "C" APE_SYSTEM_DLL_EXPORT bool ApeSceneManager_GetFileGeometry_GetStoppedAnimation(char* name, char * animationID);

extern "C" APE_SYSTEM_DLL_EXPORT bool ApeSceneManager_GetFileGeometry_SetFileName(char* name, char * fileName);

extern "C" APE_SYSTEM_DLL_EXPORT bool ApeSceneManager_GetFileGeometry_GetFileName(char* name, char * fileName);

extern "C" APE_SYSTEM_DLL_EXPORT bool ApeSceneManager_GetFileGeometry_Delete(char* name);

extern "C" APE_SYSTEM_DLL_EXPORT bool ApeSceneManager_GetGeometryIndexedLineSet_CreateIndexedLineSet(char* name);

extern "C" APE_SYSTEM_DLL_EXPORT bool ApeSceneManager_GetGeometryIndexedLineSet_GetParentNode(char* name, char* parent);

extern "C" APE_SYSTEM_DLL_EXPORT bool ApeSceneManager_GetGeometryIndexedLineSet_SetParentNode(char* name, char* parent);

extern "C" APE_SYSTEM_DLL_EXPORT bool ApeSceneManager_GetGeometryIndexedLineSet_SetParameters(char* name, float* coord, int* indices, float* colors, int* size);

extern "C" APE_SYSTEM_DLL_EXPORT bool ApeSceneManager_GetGeometryIndexedLineSet_GetParameters(char* name, float* coord, int* indices, float* colors, int* size);

extern "C" APE_SYSTEM_DLL_EXPORT bool ApeSceneManager_GetGeometryIndexedLineSet_Delete(char* name);

extern "C" APE_SYSTEM_DLL_EXPORT bool ApeSceneManager_GetGeometryClone_CreateGeometryClone(char* name);

extern "C" APE_SYSTEM_DLL_EXPORT bool ApeSceneManager_GetGeometryClone_GetParentNode(char* name, char* parent);

extern "C" APE_SYSTEM_DLL_EXPORT bool ApeSceneManager_GetGeometryClone_SetParentNode(char* name, char* parent);

extern "C" APE_SYSTEM_DLL_EXPORT bool ApeSceneManager_GetGeometryClone_GetSourceGeomtry(char* name, char* sourceGeometry);

extern "C" APE_SYSTEM_DLL_EXPORT bool ApeSceneManager_GetGeometryClone_SetSourceGeomtry(char* name, char* sourceGeometry);

extern "C" APE_SYSTEM_DLL_EXPORT bool ApeSceneManager_GetGeometryClone_GetSourceGeomtryGroupName(char* name, char* sourceGeometryGroupName);

extern "C" APE_SYSTEM_DLL_EXPORT bool ApeSceneManager_GetGeometryClone_SetSourceGeomtryGroupName(char* name, char* sourceGeometryGroupName);

extern "C" APE_SYSTEM_DLL_EXPORT bool ApeSceneManager_GetGeometryClone_Delete(char* name);

extern "C" APE_SYSTEM_DLL_EXPORT bool ApeSceneManager_GetText_CreateTextGeometry(char* name);

extern "C" APE_SYSTEM_DLL_EXPORT bool ApeSceneManager_GetText_Delete(char* name);

extern "C" APE_SYSTEM_DLL_EXPORT bool ApeSceneManager_GetText_SetCaption(char* name, char* caption);

#endif

