//------------------------------------------------------------------------------
// <copyright file="pch.h" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//------------------------------------------------------------------------------

#pragma once

#include <d3d11_1.h>
#include <d2d1_1.h>
#include <dwrite_1.h>
#include <d2d1_1helper.h>
#include <DirectXMath.h>
using namespace DirectX;

#include <agile.h>
#include <robuffer.h>

namespace DX
{
    inline void ThrowIfFailed(HRESULT hr)
    {
        if (FAILED(hr))
        {
            // Set a breakpoint on this line to catch DirectX API errors
            throw Platform::Exception::CreateException(hr);
        }
    }
}

#pragma comment( lib, "dxguid.lib")
