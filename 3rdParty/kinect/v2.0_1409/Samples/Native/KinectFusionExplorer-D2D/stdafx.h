//------------------------------------------------------------------------------
// <copyright file="stdafx.h" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//------------------------------------------------------------------------------

// include file for standard system and project includes

#pragma once

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers

// Windows Header Files
#include <windows.h>

#include <Shlobj.h>

#include <memory>
#include <string>

#include <atlbase.h>
#include <tchar.h>
#include <strsafe.h>

// Direct2D Header Files
#include <d2d1.h>

#pragma comment ( lib, "d2d1.lib" )

#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif

// Safe release for interfaces
template<class Interface>
inline void SafeRelease( Interface *& pInterfaceToRelease )
{
    if ( pInterfaceToRelease != NULL )
    {
        pInterfaceToRelease->Release();
        pInterfaceToRelease = NULL;
    }
}

#ifndef SAFE_DELETE
#define SAFE_DELETE(p) { if (p) { delete (p); (p)=NULL; } }
#endif

#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(p) { if (p) { delete[] (p); (p)=NULL; } }
#endif

#ifndef SAFE_FUSION_RELEASE_IMAGE_FRAME
#define SAFE_FUSION_RELEASE_IMAGE_FRAME(p) { if (p) { static_cast<void>(NuiFusionReleaseImageFrame(p)); (p)=NULL; } }
#endif
