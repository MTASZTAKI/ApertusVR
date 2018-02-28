//------------------------------------------------------------------------------
// <copyright file="KinectFusionProcessorFrame.cpp" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//------------------------------------------------------------------------------

#include "stdafx.h"

#include "KinectFusionParams.h"
#include "KinectFusionProcessorFrame.h"

/// <summary>
/// Constructor.
/// </summary>
KinectFusionProcessorFrame::KinectFusionProcessorFrame() :
    m_pReconstructionRGBX(nullptr),
    m_pDepthRGBX(nullptr),
    m_pTrackingDataRGBX(nullptr),
    m_cbImageSize(0),
    m_fFramesPerSecond(0),
    m_bColorCaptured(false),
    m_deviceMemory(0)
{
    ZeroMemory(m_statusMessage, sizeof(m_statusMessage));
}

/// <summary>
/// Destructor.
/// </summary>
KinectFusionProcessorFrame::~KinectFusionProcessorFrame()
{
    ZeroMemory(m_statusMessage, sizeof(m_statusMessage));
    FreeBuffers();
}

/// <summary>
/// Initializes each of the frame buffers to the given image size.
/// </summary>
/// <param name="cImageSize">Number of pixels to allocate in each frame buffer.</param>
HRESULT KinectFusionProcessorFrame::Initialize(int cImageSize)
{
    HRESULT hr = S_OK;

    ZeroMemory(m_statusMessage, sizeof(m_statusMessage));

    ULONG cbImageSize = cImageSize * KinectFusionParams::BytesPerPixel;

    if (m_cbImageSize != cbImageSize)
    {
        FreeBuffers();

        m_cbImageSize = cbImageSize;
        m_pReconstructionRGBX = new(std::nothrow) BYTE[m_cbImageSize];
        m_pDepthRGBX = new(std::nothrow) BYTE[m_cbImageSize];
        m_pTrackingDataRGBX = new(std::nothrow) BYTE[m_cbImageSize];

        if (nullptr != m_pReconstructionRGBX &&
            nullptr != m_pDepthRGBX &&
            nullptr != m_pTrackingDataRGBX)
        {
            ZeroMemory(m_pReconstructionRGBX, m_cbImageSize);
            ZeroMemory(m_pDepthRGBX, m_cbImageSize);
            ZeroMemory(m_pTrackingDataRGBX, m_cbImageSize);
        }
        else
        {
            FreeBuffers();
            hr = E_OUTOFMEMORY;
        }
    }

    return hr;
}

/// <summary>
/// Sets the status message for the frame.
/// </summary>
/// <param name="szMessage">The status message.</param>
void KinectFusionProcessorFrame::SetStatusMessage(const WCHAR* szMessage)
{
    StringCchCopy(m_statusMessage, ARRAYSIZE(m_statusMessage), szMessage);
}

/// <summary>
/// Frees the frame buffers.
/// </summary>
void KinectFusionProcessorFrame::FreeBuffers()
{
    SAFE_DELETE_ARRAY(m_pReconstructionRGBX);
    SAFE_DELETE_ARRAY(m_pDepthRGBX);
    SAFE_DELETE_ARRAY(m_pTrackingDataRGBX);

    m_cbImageSize = 0;
}
