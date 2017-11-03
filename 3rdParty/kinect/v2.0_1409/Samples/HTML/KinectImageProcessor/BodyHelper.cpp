//------------------------------------------------------------------------------
// <copyright file="BodyHelper.cpp" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//------------------------------------------------------------------------------

// BodyHelper.cpp
#include "pch.h"
#include "BodyHelper.h"
#include <robuffer.h>
#include <math.h>

using namespace KinectImageProcessor;
using namespace Platform;
using namespace Windows::Storage::Streams;
using namespace WindowsPreview::Kinect;

BodyHelper::BodyHelper()
{
}

bool BodyHelper::processJointLocations(
            _In_ Windows::Foundation::Collections::IMapView<WindowsPreview::Kinect::JointType, WindowsPreview::Kinect::Joint>^ jointsCollection,
            _Out_ Platform::WriteOnlyArray<jointPoint>^ jointPoints)
{
    if (jointsCollection == nullptr || jointPoints == nullptr)
    {
        return false;
    }

    ZeroMemory(jointPoints->Data, jointPoints->Length * sizeof(jointPoint));

    // get active kinect sensor
    KinectSensor^ sensor = WindowsPreview::Kinect::KinectSensor::GetDefault();

    // get coordinatemapper
    CoordinateMapper^ coordinateMapper = sensor->CoordinateMapper;

    for (int jointIndex = 0; jointIndex < (int)jointsCollection->Size; ++jointIndex)
    {
        // if Z coordinate is negative (joint is behind sensor), set to 0.1f or coordinateMapper will return (-Infinity, -Infinity)
        WindowsPreview::Kinect::CameraSpacePoint position = jointsCollection->Lookup((JointType)(jointIndex)).Position;
        if (position.Z < 0)
        {
            position.Z = 0.1f;
        }       

        // map each joint location to depth space
        DepthSpacePoint DepthSpacePoint = coordinateMapper->MapCameraPointToDepthSpace(position);
        jointPoints->Data[jointIndex].jointType = (JointType)(jointIndex);
        jointPoints->Data[jointIndex].x = DepthSpacePoint.X;
        jointPoints->Data[jointIndex].y = DepthSpacePoint.Y;
    }

    return true;
}