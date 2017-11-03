//------------------------------------------------------------------------------
// <copyright file="CoordinateMapperHelper.cpp" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//------------------------------------------------------------------------------

// CoordinateMapperHelper.cpp
#include "pch.h"
#include "CoordinateMapperHelper.h"
#include <robuffer.h>
#include <math.h>
#include <limits>

using namespace KinectImageProcessor;
using namespace Platform;
using namespace Windows::Storage::Streams;
using namespace WindowsPreview::Kinect;

CoordinateMapperHelper::CoordinateMapperHelper() :
    m_depthPoints(nullptr),
    m_coordinateMapper(nullptr),
    m_colorHeight(0),
    m_colorWidth(0),
    m_depthHeight(0),
    m_depthWidth(0)
{
    // get active kinect sensor
    KinectSensor^ sensor = WindowsPreview::Kinect::KinectSensor::GetDefault();

    // get coordinatemapper
    m_coordinateMapper = sensor->CoordinateMapper;

    m_depthWidth = sensor->DepthFrameSource->FrameDescription->Width;
    m_depthHeight = sensor->DepthFrameSource->FrameDescription->Height;

    // create color frame description for RGBa format
    FrameDescription^ colorFrameDescription = sensor->ColorFrameSource->CreateFrameDescription(ColorImageFormat::Rgba);

    m_colorWidth = colorFrameDescription->Width;
    m_colorHeight = colorFrameDescription->Height;

    m_depthPoints = ref new Array<DepthSpacePoint>(m_colorWidth * m_colorHeight);
}

bool CoordinateMapperHelper::ProcessMultiFrameBufferData(
    _In_ const Platform::Array<UINT16>^ depthDataArray,
    _In_ Windows::Storage::Streams::IBuffer^ colorDataBuffer,
    _In_ const Platform::Array<UINT8>^ bodyIndexframeArray,
    _In_ Windows::Storage::Streams::IBuffer^ outputDataBuffer)
{
    if (depthDataArray == nullptr || colorDataBuffer == nullptr || bodyIndexframeArray == nullptr || outputDataBuffer == nullptr)
    {
        return false;
    }

    // map color coordinates to depth coordinates
    m_coordinateMapper->MapColorFrameToDepthSpace(depthDataArray, m_depthPoints);

    // Get access to color buffer
    Microsoft::WRL::ComPtr<IBufferByteAccess> spColorBufferByteAccess;
    Microsoft::WRL::ComPtr<IInspectable> spColorDataBuffer = reinterpret_cast<IInspectable*>(colorDataBuffer);
    HRESULT hr = spColorDataBuffer.As(&spColorBufferByteAccess);
    
    if (FAILED(hr))
    {
        return false;
    }

    // Retrieve the color buffer data.
    int* pColorData = nullptr;
    byte* pColorByteData = nullptr;
    hr = spColorBufferByteAccess->Buffer(&pColorByteData);
    if (FAILED(hr))
    {
        return false;
    }

    pColorData = (int*)pColorByteData;

    // Get access to output buffer
    Microsoft::WRL::ComPtr<IBufferByteAccess> spOutputBufferByteAccess;
    Microsoft::WRL::ComPtr<IInspectable> spOutputDataBuffer = reinterpret_cast<IInspectable*>(outputDataBuffer);
    
    hr = spOutputDataBuffer.As(&spOutputBufferByteAccess);
    if (FAILED(hr))
    {
        return false;
    }

    // Retrieve the output buffer data.
    int* pOutputData = nullptr;
    byte* pOutputDataByte = nullptr;
    hr = spOutputBufferByteAccess->Buffer(&pOutputDataByte);
    if (FAILED(hr))
    {
        return false;
    }

    pOutputData = (int*)pOutputDataByte;
    
    DepthSpacePoint* pDepthPoints = m_depthPoints->Data;
    byte* pBodyIndexFrameArray = bodyIndexframeArray->Data;

    ZeroMemory(pOutputData, outputDataBuffer->Capacity);

    // loop over each color pixel
    int numColorPixels = m_colorWidth * m_colorHeight;
    for (int colorIndex = 0; colorIndex < numColorPixels; ++colorIndex)
    {
        DepthSpacePoint p = pDepthPoints[colorIndex];
        // Infinity means invalid point so we can skip processing on it
        if (p.X != -std::numeric_limits<float>::infinity() && p.Y != -std::numeric_limits<float>::infinity())
        {
            int depthX = static_cast<int>(p.X + 0.5f);
            int depthY = static_cast<int>(p.Y + 0.5f);

            if ((depthX >= 0 && depthX < m_depthWidth) && (depthY >= 0 && depthY < m_depthHeight))
            {
                BYTE bodyIndexPixel = pBodyIndexFrameArray[depthX + (depthY * m_depthWidth)];

                // if we're tracking a player for the current pixel, copy the pixel from the color data
                if (bodyIndexPixel != 0xff)
                {
                    pOutputData[colorIndex] = pColorData[colorIndex];
                }
            }
        }
    }

    outputDataBuffer->Length = colorDataBuffer->Length;

    return true;
}

