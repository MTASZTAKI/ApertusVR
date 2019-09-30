//------------------------------------------------------------------------------
// <copyright file="BodyHelper.h" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//------------------------------------------------------------------------------

#pragma once

namespace KinectImageProcessor
{
    // Point struct
    public value struct jointPoint
    {
        WindowsPreview::Kinect::JointType jointType;
        float x;
        float y;
    };

    public ref class BodyHelper sealed
    {
    public:
        static bool processJointLocations(
            _In_ Windows::Foundation::Collections::IMapView<WindowsPreview::Kinect::JointType, WindowsPreview::Kinect::Joint>^ jointsCollection,
            _Out_ Platform::WriteOnlyArray<jointPoint>^ jointPoints);

    private:
        BodyHelper();
    };
}