//------------------------------------------------------------------------------
// <copyright file="CoordinateMapperHelper.h" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//------------------------------------------------------------------------------

#pragma once
#pragma warning(disable:28204)

namespace KinectImageProcessor
{
    public ref class CoordinateMapperHelper sealed
    {
    public:
        CoordinateMapperHelper();

        bool ProcessMultiFrameBufferData(
            _In_ const Platform::Array<UINT16>^ depthDataArray,
            _In_ Windows::Storage::Streams::IBuffer^ colorDataBuffer,
            _In_ const Platform::Array<UINT8>^ bodyIndexframeArray,
            _In_ Windows::Storage::Streams::IBuffer^ pixelDataBuffer);

    private:
        void SensorChanged();

        Platform::Array<WindowsPreview::Kinect::DepthSpacePoint>^ m_depthPoints;
        WindowsPreview::Kinect::CoordinateMapper^ m_coordinateMapper;

        int m_depthWidth;
        int m_depthHeight;
        int m_colorWidth;
        int m_colorHeight;
    };
}