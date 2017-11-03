//------------------------------------------------------------------------------
// <copyright file="Button.h" company="Microsoft">
//     2D button that utilizes KinectGestureRecognizer for state
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//------------------------------------------------------------------------------

#pragma once

#include "UIElement.h"

using namespace WindowsPreview::Kinect::Input;

ref class ScrollViewer;

ref class ButtonEventArgs sealed
{
public:
    ButtonEventArgs() {};
};

ref class Button sealed : UIElement
{
public:
    Button(KinectGestureSettings kinectGestureSettings, UIElement^ parent);

    virtual ~Button();
    
    event Windows::Foundation::TypedEventHandler<Button^, ButtonEventArgs^>^ Tapped;

    virtual property Microsoft::Kinect::Toolkit::Input::IInputModel^ InputModel
    {
        virtual Microsoft::Kinect::Toolkit::Input::IInputModel^ get() override
        {
            return static_cast<Microsoft::Kinect::Toolkit::Input::IInputModel^>(PressableModel);
        }
    }

    property Microsoft::Kinect::Toolkit::Input::PressableModel^ PressableModel
    {
        Microsoft::Kinect::Toolkit::Input::PressableModel^ get()
        {
            return m_pressableModel;
        }
    }

    // Programmatically cause a Tap or Hold
    void Tap();
    void Hold();
    
    virtual bool HitTest(Point position) override;

    void RenderCallback();
    
    Windows::UI::Color GetColor() { return m_color; }
    
internal:
    void Initialize(Color color, float x, float y, float width, float height, float hittableMarginWidth);
    void Initialize(Color color, float x, float y, float width, float height, 
        float hittableMarginLeft, float hittableMarginTop, float hittableMarginRight, float hittableMarginBottom);

    const float GetWidth() const { return m_width; }
    const float GetHeight() const { return m_height; }

internal:
    const XMVECTOR GetPosition() const { return Transform().r[3]; }
    Windows::Foundation::Rect GetBoundingRect() { return Windows::Foundation::Rect( XMVectorGetX(GetPosition()) - m_width / 2.0f, XMVectorGetY(GetPosition()) - m_height / 2.0f, m_width, m_height ); };

private:
    Microsoft::Kinect::Toolkit::Input::PressableModel^ m_pressableModel;

    // Event tokens for KinectGestureRecognizer events
    Windows::Foundation::EventRegistrationToken m_tokenTapped;
    Windows::Foundation::EventRegistrationToken m_tokenHolding;
    
    // KinectGestureRecognizer event handlers.
	void OnTapped(KinectGestureRecognizer^ sender, KinectTappedEventArgs^ args);
	void OnHolding(KinectGestureRecognizer^ sender, KinectHoldingEventArgs^ args);

    Windows::UI::Color m_color;
    Windows::UI::Color m_initColor;
    unsigned int m_colorIndex;

    float m_initX;              // object's initial position x, relative to view centerpoint
    float m_initY;              // object's initial position y, relative to view centerpoint
    float m_width;              // object's initial width
    float m_height;             // object's initial height
    float m_hittableMarginLeft; // object's size of margin (1 side) which HitTest() treats as part of the object
    float m_hittableMarginTop; // object's size of margin (1 side) which HitTest() treats as part of the object
    float m_hittableMarginRight; // object's size of margin (1 side) which HitTest() treats as part of the object
    float m_hittableMarginBottom; // object's size of margin (1 side) which HitTest() treats as part of the object
    int m_framesToShowSmallerSize;  // when press and held, the button grows smaller for this many frames.
};
