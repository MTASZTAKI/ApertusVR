//------------------------------------------------------------------------------
// <copyright file="ScrollViewer.h" company="Microsoft">
//     2D Scroll Viewer, transform is applied by child elements
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//------------------------------------------------------------------------------

#pragma once

#include "UIElement.h"
#include "ControlsBasicsDX.h"

using namespace WindowsPreview::Kinect::Input;

ref class ScrollViewer sealed : public UIElement
{
public:
    ScrollViewer(KinectGestureSettings kinectGestureSettings, UIElement^ parent = nullptr);
    virtual ~ScrollViewer();
    void Initialize(float extentWidth, float extentHeight, float clientWidth, float clientHeight);
    void WindowResized(Rect windowBounds);

    void ScrollByOffset(float x, float y);
    
    virtual property Microsoft::Kinect::Toolkit::Input::IInputModel^ InputModel
    {
        virtual Microsoft::Kinect::Toolkit::Input::IInputModel^ get() override
        {
            return static_cast<Microsoft::Kinect::Toolkit::Input::IInputModel^>(ManipulatableModel);
        }
    }
    
    property Microsoft::Kinect::Toolkit::Input::ManipulatableModel^ ManipulatableModel
    {
        Microsoft::Kinect::Toolkit::Input::ManipulatableModel^ get()
        {
            return m_manipulatableModel;
        }
    }

    // Returns true if point (x,y) belongs to the object. The point should be in parent coordinate system.
    virtual bool HitTest(Windows::Foundation::Point position) override;

internal:
    // Base class UIElement overrides.
    virtual const XMMATRIX Transform() const override;

private:
    void GetDeviceDescription();

    // KinectGestureRecognizer event handlers.
	void OnManipulationStarted(KinectGestureRecognizer^ sender, KinectManipulationStartedEventArgs^ args);
	void OnManipulationUpdated(KinectGestureRecognizer^ sender, KinectManipulationUpdatedEventArgs^ args);
	void OnManipulationInertiaStarting(KinectGestureRecognizer^ sender, KinectManipulationInertiaStartingEventArgs^ args);
	void OnManipulationCompleted(KinectGestureRecognizer^ sender, KinectManipulationCompletedEventArgs^ args);

    // Calculate the scrolling delta within the bounds
    Point CalculateScrollingDelta(Point prevPosition, Point newPosition);
    // Calculate the stretch delta beyond the bounds 
    Point CalculateStretchDelta(Point prevPosition, Point newPosition);

    // Helper function that clamps a position to the scrolling bounds
    Point ClampToScrollingBounds(Point position); 
    // Helper that returns the distance between the bounds and a point beyond the bounds.
    Point DistanceBeyondBounds(Point position);

    // Read-only property defining the stretch exponent
    property float StretchExponent
    {
        float get()
        {
            return 0.7f;
        }
    }

private:
    // Data
    Windows::Devices::Input::IPointerDevice^ m_pointerDevice;
    XMFLOAT2 m_extentSize;
    XMFLOAT2 m_clientSize;
    XMFLOAT2 m_scrollRange;
    Point m_scrollOffset;
    Point m_cumulativeStretch;
    bool m_isInertialScrolling;

    Microsoft::Kinect::Toolkit::Input::ManipulatableModel^ m_manipulatableModel;

    // Event tokens for KinectGestureRecognizer events
    Windows::Foundation::EventRegistrationToken m_tokenManipulationStarted;
    Windows::Foundation::EventRegistrationToken m_tokenManipulationUpdated;
    Windows::Foundation::EventRegistrationToken m_tokenManipulationInertiaStarting;
    Windows::Foundation::EventRegistrationToken m_tokenManipulationCompleted;
};
