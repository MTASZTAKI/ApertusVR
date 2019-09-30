//------------------------------------------------------------------------------
// <copyright file="Button.cpp" company="Microsoft">
//     2D button that utilizes KinectGestureRecognizer for state
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//------------------------------------------------------------------------------

#include "pch.h"
#include "Button.h"

#include "ControlsBasicsDX.h"

using namespace Microsoft::Kinect::Toolkit::Input;
using namespace Windows::Foundation;

//--------------------------------------------------------------------------------------
// Name: Button()
// Desc: Constructor
//--------------------------------------------------------------------------------------
Button::Button(KinectGestureSettings kinectGestureSettings, UIElement^ parent) :
    UIElement(parent),
    m_colorIndex(0),
    m_color(ColorValue[0]),
    m_initColor(ColorValue[0]),
    m_initX(0),
    m_initY(0),
    m_width(0),
    m_height(0),
    m_hittableMarginLeft(0),
    m_hittableMarginTop(0),
    m_hittableMarginRight(0),
    m_hittableMarginBottom(0)
{
    m_pressableModel = ref new Microsoft::Kinect::Toolkit::Input::PressableModel(kinectGestureSettings, this);
}

//--------------------------------------------------------------------------------------
// Name: ~Button()
// Desc: Destructor
//--------------------------------------------------------------------------------------
Button::~Button()
{
    if (m_tokenTapped.Value != 0)
    {
        m_pressableModel->GestureRecognizer->Tapped -= m_tokenTapped;
    }

    if (m_tokenHolding.Value != 0)
    {
        m_pressableModel->GestureRecognizer->Holding -= m_tokenHolding;
    }

    if (m_pressableModel->HasCapture)
    {
        m_pressableModel->CompleteGesture();
    }
}

//--------------------------------------------------------------------------------------
// Name: Initialize()
// Desc: Set initial color, position, and size
//--------------------------------------------------------------------------------------
void Button::Initialize(Color color, float x, float y, float width, float height, float hittableMarginSize)
{
    Initialize(color, x, y, width, height, hittableMarginSize, hittableMarginSize, hittableMarginSize, hittableMarginSize);
}

//--------------------------------------------------------------------------------------
// Name: Initialize()
// Desc: Set initial color, position, and size
//--------------------------------------------------------------------------------------
void Button::Initialize(Color color, float x, float y, float width, float height, 
                        float hittableMarginLeft, 
                        float hittableMarginTop, 
                        float hittableMarginRight, 
                        float hittableMarginBottom 
                        )
{
    m_tokenTapped = m_pressableModel->GestureRecognizer->Tapped +=
        ref new TypedEventHandler<WindowsPreview::Kinect::Input::KinectGestureRecognizer^, WindowsPreview::Kinect::Input::KinectTappedEventArgs^>(this, &Button::OnTapped);
    
    m_tokenHolding = m_pressableModel->GestureRecognizer->Holding +=
        ref new TypedEventHandler<WindowsPreview::Kinect::Input::KinectGestureRecognizer^, WindowsPreview::Kinect::Input::KinectHoldingEventArgs^>(this, &Button::OnHolding);

    m_colorIndex = (unsigned int)color;
    m_color = ColorValue[m_colorIndex];
    m_initColor = ColorValue[m_colorIndex];
    // Normalize rectangle
    if (width < 0)
    {
        x += width;
        width = -width;
    }
    if (height < 0)
    {
        y += height;
        height = -height;
    }

    m_initX = x;
    m_initY = y;
    m_width = width;
    m_height = height;
    m_hittableMarginLeft = hittableMarginLeft;
    m_hittableMarginTop = hittableMarginTop;
    m_hittableMarginRight = hittableMarginRight;
    m_hittableMarginBottom = hittableMarginBottom;
    float visualHalfWidth = m_width * 0.5f;
    float visualHalfHeight = m_height * 0.5f;
    Rect clientWindowBounds = ControlsBasicsDX::CurrentClientBounds;

    PressableModel->SetLocation(
        (m_initX - visualHalfWidth) / clientWindowBounds.Width,
        (m_initY - visualHalfHeight) / clientWindowBounds.Height,
        (visualHalfWidth * 2) / clientWindowBounds.Width,
        (visualHalfHeight * 2) / clientWindowBounds.Height,
        m_hittableMarginLeft,
        m_hittableMarginTop,
        m_hittableMarginRight,
        m_hittableMarginBottom);

    SetInitialPosition(XMFLOAT2(m_initX + m_width/2, m_initY + m_height/2));
    SetManipulationTransform(XMMatrixIdentity());
}

//--------------------------------------------------------------------------------------
// Name: HitTest(Point position)
// Desc: Object hit-testing - returns true if pointer is inside button's bounding bounds, false otherwise.
//       Also updates the KinectGestureRecognizer's BoundingRect (in client coordinates) so it knows where the button is, and can pick a good attraction point for press.
//--------------------------------------------------------------------------------------
bool Button::HitTest(Point position)
{
    float visualHalfWidth = m_width * 0.5f;
    float visualHalfHeight = m_height * 0.5f;

    Rect clientWindowBounds = ControlsBasicsDX::CurrentClientBounds;
    XMVECTOR buttonCenter = GetPosition();

    // Hit-test the point to the hittable region which incorporates the hittableMarginSize as well.
    float x = position.X - XMVectorGetX(buttonCenter);
    float y = position.Y - XMVectorGetY(buttonCenter);

    PressableModel->SetLocation(
        (XMVectorGetX(buttonCenter) - visualHalfWidth) / clientWindowBounds.Width,
        (XMVectorGetY(buttonCenter) - visualHalfHeight) / clientWindowBounds.Height,
        (visualHalfWidth * 2) / clientWindowBounds.Width,
        (visualHalfHeight * 2) / clientWindowBounds.Height,
        m_hittableMarginLeft,
        m_hittableMarginTop,
        m_hittableMarginRight,
        m_hittableMarginBottom);

    return ((x >= -visualHalfWidth - m_hittableMarginLeft) && (x <= visualHalfWidth + m_hittableMarginRight) 
        && (y >= -visualHalfHeight - m_hittableMarginTop) && (y <= visualHalfHeight + m_hittableMarginBottom));
}

//--------------------------------------------------------------------------------------
// Name: OnTapped()
// Desc: Handle tapped event by changing color
//--------------------------------------------------------------------------------------
void Button::OnTapped(KinectGestureRecognizer^ sender, KinectTappedEventArgs^ args)
{
    UNREFERENCED_PARAMETER(sender);
    UNREFERENCED_PARAMETER(args);

    Tapped(this, ref new ButtonEventArgs());
    Tap();
}

//--------------------------------------------------------------------------------------
// Name: Tap()
// Desc: Programmatically cause press to happen
//--------------------------------------------------------------------------------------
void Button::Tap()
{
    m_colorIndex = (++m_colorIndex) % ((unsigned int)Color::MaxCount - 1);

    m_color = ColorValue[m_colorIndex];
}

//--------------------------------------------------------------------------------------
// Name: OnHolding()
// Desc: Event handler called when users starts, completes or cancels a press/hold.
//       Titles should show their AppMenu if holding is completed.
//--------------------------------------------------------------------------------------
void Button::OnHolding(KinectGestureRecognizer^ sender, KinectHoldingEventArgs^ args)
{
    UNREFERENCED_PARAMETER(sender);

    switch (args->HoldingState)
    {
    case WindowsPreview::Kinect::Input::KinectHoldingState::Started:
        break;
	case WindowsPreview::Kinect::Input::KinectHoldingState::Completed:
        Hold();
        break;
	case WindowsPreview::Kinect::Input::KinectHoldingState::Canceled:
        break;
    }
}

void Button::Hold()
{
    // A title should show its AppMenu, if appropriate, when press/hold is complete.
    // In this app, we're just going to grow the button for 2 seconds.
    if (m_framesToShowSmallerSize == 0)
    {
        m_framesToShowSmallerSize = 60;
        m_width = m_width * 0.6f;
        m_height = m_height * 0.6f;
    }
}

//--------------------------------------------------------------------------------------
// Name: RenderCallback()
// Desc: This function is called every render. It allows a button to readjust to normal size
//       based on a frame counter. 
//--------------------------------------------------------------------------------------
void Button::RenderCallback()
{
    if (m_framesToShowSmallerSize > 0)
    {
        m_framesToShowSmallerSize--;
        if (m_framesToShowSmallerSize == 0)
        {
            m_width = m_width / 0.6f;
            m_height = m_height / 0.6f;
        }
    }
}