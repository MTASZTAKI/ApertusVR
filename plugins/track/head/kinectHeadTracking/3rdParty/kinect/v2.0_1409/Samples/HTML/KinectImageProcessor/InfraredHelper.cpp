//------------------------------------------------------------------------------
// <copyright file="InfraredHelper.cpp" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//------------------------------------------------------------------------------

// InfraredHelper.cpp
#include "pch.h"
#include "InfraredHelper.h"
#include <robuffer.h>

using namespace KinectImageProcessor;
using namespace Platform;
using namespace Windows::Storage::Streams;

// InfraredSourceValueMaximum is the highest value that can be returned in the InfraredFrame.
// It is cast to a float for readability in the visualization code.
#define InfraredSourceValueMaximum static_cast<float>(USHRT_MAX)

// The InfraredOutputValueMinimum value is used to set the lower limit, post processing, of the
// infrared data that we will render.
// Increasing or decreasing this value sets a brightness "wall" either closer or further away.
#define InfraredOutputValueMinimum 0.01f 

// The InfraredOutputValueMaximum value is the upper limit, post processing, of the
// infrared data that we will render.
#define InfraredOutputValueMaximum 1.0f

// The InfraredSceneValueAverage value specifies the average infrared value of the scene.
// This value was selected by analyzing the average pixel intensity for a given scene.
// Depending on the visualization requirements for a given application, this value can be
// hard coded, as was done here, or calculated by averaging the intensity for each pixel prior
// to rendering.
#define InfraredSceneValueAverage 0.08f

// The InfraredSceneStandardDeviations value specifies the number of standard deviations
// to apply to InfraredSceneValueAverage. This value was selected by analyzing data
// from a given scene.
// Depending on the visualization requirements for a given application, this value can be
// hard coded, as was done here, or calculated at runtime.
#define InfraredSceneStandardDeviations 3.0f

InfraredHelper::InfraredHelper()
{
}

bool InfraredHelper::CreateImagePixelDataFromArray(
    _In_ const Platform::Array<UINT16>^ frameArray,
    _In_ Platform::WriteOnlyArray<byte>^ pixelArray)
{
    if (frameArray == nullptr || pixelArray == nullptr)
    {
        return false;
    }

    CopyInfraredBytes(reinterpret_cast<byte*>(frameArray->Data), frameArray->Length, pixelArray->Data, pixelArray->Length);

    return true;
}

void InfraredHelper::CopyInfraredBytes(_In_reads_(cbSrcSize) byte* puiSource, int cbSrcSize, _Out_writes_all_(cbDestSize) byte* pbDest, int cbDestSize)
{
    UINT16* pSrc = reinterpret_cast<UINT16*>(puiSource);
    RGBQUAD* pDest = reinterpret_cast<RGBQUAD*>(pbDest);

    // end pixel is start + width * height - 1
    const UINT16* pBufferEnd = pSrc + cbSrcSize;

    while (pSrc < pBufferEnd)
    {
        // normalize the incoming infrared data (ushort) to a float ranging from 
        // [InfraredOutputValueMinimum, InfraredOutputValueMaximum] by
        // 1. dividing the incoming value by the source maximum value
		float intensityRatio = static_cast<float>(*pSrc) / InfraredSourceValueMaximum;

        // 2. dividing by the (average scene value * standard deviations)
        intensityRatio /= InfraredSceneValueAverage * InfraredSceneStandardDeviations;
		
        // 3. limiting the value to InfraredOutputValueMaximum
        intensityRatio = min(InfraredOutputValueMaximum, intensityRatio);

        // 4. limiting the lower value InfraredOutputValueMinimym
        intensityRatio = max(InfraredOutputValueMinimum, intensityRatio);
	
		// 5. converting the normalized value to a byte and using the result
		// as the RGB components required by the image
		byte intensity = static_cast<byte>(intensityRatio * 255.0f); 
        pDest->rgbRed = intensity;
        pDest->rgbGreen = intensity;
        pDest->rgbBlue = intensity;
        pDest->rgbReserved = 255;

        // adjust pointers
        ++pDest;
        ++pSrc;
    }
}