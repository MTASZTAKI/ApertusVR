//------------------------------------------------------------------------------
// <copyright file="DepthHelper.h" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//------------------------------------------------------------------------------

#pragma once

namespace KinectImageProcessor
{
    public ref class DepthHelper sealed
    {
    public:
        static bool CreateImagePixelDataFromArray(
            _In_ const Platform::Array<UINT16>^ frameArray,
            _In_ Platform::WriteOnlyArray<byte>^ pixelArray);

    private:
        DepthHelper();
        static void CopyDepthBytes(_In_reads_(cbSrcSize) byte* puiSource, int cbSrcSize, _Out_writes_all_(cbDestSize) byte* pbDest, int cbDestSize);
    };
}