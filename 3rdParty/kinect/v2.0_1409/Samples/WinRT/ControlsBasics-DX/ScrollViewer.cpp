//------------------------------------------------------------------------------
// <copyright file="ScrollViewer.cpp" company="Microsoft">
//     2D Scroll Viewer, transform is applied by child elements
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//------------------------------------------------------------------------------

#include "pch.h"
#include "ScrollViewer.h"

using namespace Windows::Foundation;

//--------------------------------------------------------------------------------------
// Name: ScrollViewer()
// Desc: Constructor
//--------------------------------------------------------------------------------------
ScrollViewer::ScrollViewer(KinectGestureSettings kinectGestureSettings, UIElement^ parent)
    :   UIElement(parent)
{
    m_isInertialScrolling = false;
    m_scrollOffset = Point(0, 0);
    m_cumulativeStretch = Point(0, 0);

    m_manipulatableModel = ref new Microsoft::Kinect::Toolkit::Input::ManipulatableModel(kinectGestureSettings, this);
    
    m_manipulatableModel->GestureRecognizer->AutoProcessInertia = true;

    // Register all the delegates
    // These events will raise on the thread calling the gesture recognizer
    m_tokenManipulationStarted = m_manipulatableModel->GestureRecognizer->ManipulationStarted +=
        ref new TypedEventHandler<KinectGestureRecognizer^, KinectManipulationStartedEventArgs^>(this, &ScrollViewer::OnManipulationStarted);

    m_tokenManipulationUpdated = m_manipulatableModel->GestureRecognizer->ManipulationUpdated +=
        ref new TypedEventHandler<KinectGestureRecognizer^, KinectManipulationUpdatedEventArgs^>(this, &ScrollViewer::OnManipulationUpdated);

    m_tokenManipulationInertiaStarting = m_manipulatableModel->GestureRecognizer->ManipulationInertiaStarting +=
        ref new TypedEventHandler<KinectGestureRecognizer^, KinectManipulationInertiaStartingEventArgs^>(this, &ScrollViewer::OnManipulationInertiaStarting);

    m_tokenManipulationCompleted = m_manipulatableModel->GestureRecognizer->ManipulationCompleted +=
        ref new TypedEventHandler<KinectGestureRecognizer^, KinectManipulationCompletedEventArgs^>(this, &ScrollViewer::OnManipulationCompleted);

    SetManipulationTransform(XMMatrixIdentity());
    SetInitialPosition(XMFLOAT2(0, 0));
}

//--------------------------------------------------------------------------------------
// Name: ScrollViewer()
// Desc: Destructor
//--------------------------------------------------------------------------------------
ScrollViewer::~ScrollViewer()
{
    if (m_tokenManipulationStarted.Value != 0)
    {
        m_manipulatableModel->GestureRecognizer->ManipulationStarted -= m_tokenManipulationStarted;
    }
    if (m_tokenManipulationUpdated.Value != 0)
    {
        m_manipulatableModel->GestureRecognizer->ManipulationUpdated -= m_tokenManipulationUpdated;
    }
    if (m_tokenManipulationInertiaStarting.Value != 0)
    {
        m_manipulatableModel->GestureRecognizer->ManipulationInertiaStarting -= m_tokenManipulationInertiaStarting;
    }
    if (m_tokenManipulationCompleted.Value != 0)
    {
        m_manipulatableModel->GestureRecognizer->ManipulationCompleted -= m_tokenManipulationCompleted;
    }
    if (ManipulatableModel->HasCapture)
    {
        ManipulatableModel->CompleteGesture();
    }
}

//--------------------------------------------------------------------------------------
// Name: Initialize()
// Desc: Initialize the scrollviewer's extent and client size
//--------------------------------------------------------------------------------------
void ScrollViewer::Initialize(float extentWidth, float extentHeight, float clientWidth, float clientHeight)
{
    m_extentSize.x = extentWidth;
    m_extentSize.y = extentHeight;
    m_clientSize.x = clientWidth;
    m_clientSize.y = clientHeight;
    m_scrollRange.x = extentWidth - clientWidth;
    m_scrollRange.y = extentHeight - clientHeight;

    m_manipulatableModel->Initialize(m_extentSize.x, m_extentSize.y, m_clientSize.x, m_clientSize.y);
}

//--------------------------------------------------------------------------------------
// Name: WindowResized()
// Desc: Called with new Window sizes (due to snapped/filled/full)
//--------------------------------------------------------------------------------------
void ScrollViewer::WindowResized(Rect windowBounds)
{
    m_clientSize.x = min(m_extentSize.x, windowBounds.Width);
    m_clientSize.y = min(m_extentSize.y, windowBounds.Height);
    m_scrollRange.x = m_extentSize.x - m_clientSize.x;
    m_scrollRange.y = m_extentSize.y - m_clientSize.y;

    m_manipulatableModel->WindowResized(windowBounds);
}

//--------------------------------------------------------------------------------------
// Name: HitTest()
// Desc: Returns true if position is within bounds
//--------------------------------------------------------------------------------------
bool ScrollViewer::HitTest(Windows::Foundation::Point position)
{
    Windows::Foundation::Rect bounds =
        Windows::Foundation::Rect(
            Windows::Foundation::Point(m_clientSize.x, m_clientSize.y),
            Windows::Foundation::Point(m_extentSize.x, m_extentSize.y));

    return bounds.Contains(position);
}

//--------------------------------------------------------------------------------------
// Name: Transform()
// Desc: Get the transform - base UIElement override
//--------------------------------------------------------------------------------------
const XMMATRIX ScrollViewer::Transform() const
{
    return __super::Transform();
}

//--------------------------------------------------------------------------------------
// Name: OnManipulationStarted()
// Desc: Handles the start of a scrolling manipulation
//--------------------------------------------------------------------------------------
void ScrollViewer::OnManipulationStarted(KinectGestureRecognizer^ sender, KinectManipulationStartedEventArgs^ args)
{
    UNREFERENCED_PARAMETER(sender);
    UNREFERENCED_PARAMETER(args);
}

//--------------------------------------------------------------------------------------
// Name: OnManipulationUpdated()
// Desc: Handles the update of a scrolling manipulation
//--------------------------------------------------------------------------------------
void ScrollViewer::OnManipulationUpdated(KinectGestureRecognizer^ sender, KinectManipulationUpdatedEventArgs^ args)
{
    UNREFERENCED_PARAMETER(sender);

    Point translationDelta = args->Delta.Translation;
    translationDelta.X *= m_clientSize.x;
    translationDelta.Y *= m_clientSize.y;

    Point newScrollOffset = Point(m_scrollOffset.X - translationDelta.X, m_scrollOffset.Y - translationDelta.Y);

    // Calculate scrolling delta
    Point delta = CalculateScrollingDelta(m_scrollOffset, newScrollOffset);

    if (!m_isInertialScrolling)
    {
        // If not in inertial scroll, calculate additional stretch delta
        Point stretchDelta = CalculateStretchDelta(m_scrollOffset, newScrollOffset);

        // Update total stretch
        m_cumulativeStretch.X += stretchDelta.X;
        m_cumulativeStretch.Y += stretchDelta.Y;

        // Frame delta comprises of scrolling and stretch deltas
        delta.X += stretchDelta.X;
        delta.Y += stretchDelta.Y;
    }

    UpdateManipulationTransform(delta);

    m_scrollOffset = newScrollOffset;
}

//--------------------------------------------------------------------------------------
// Name: ScrollByOffset()
// Desc: Programmatic scrolling to support showing a specific child element.
//--------------------------------------------------------------------------------------
void ScrollViewer::ScrollByOffset(float x, float y)
{
    Point newScrollOffset = Point(m_scrollOffset.X - x, m_scrollOffset.Y - y);

    // Calculate scrolling delta
    Point delta = CalculateScrollingDelta(m_scrollOffset, newScrollOffset);

    UpdateManipulationTransform(delta);

    m_scrollOffset = newScrollOffset;
}

//--------------------------------------------------------------------------------------
// Name: OnManipulationInertiaStarting()
// Desc: Handles the start of inertial scrolling (user lets go while scrolling with velocity).
//--------------------------------------------------------------------------------------
void ScrollViewer::OnManipulationInertiaStarting(KinectGestureRecognizer^ sender, KinectManipulationInertiaStartingEventArgs^ args)
{
    UNREFERENCED_PARAMETER(sender);
    UNREFERENCED_PARAMETER(args);

    m_isInertialScrolling = true;

    // If the scrollviewer is already stretching past its bounds, complete the manipulation
    if (abs(m_cumulativeStretch.X) > 0 || abs(m_cumulativeStretch.Y) > 0)
    {
        m_manipulatableModel->CompleteGesture();
    }
}

//--------------------------------------------------------------------------------------
// Name: OnManipulationCompleted()
// Desc: Handles the end of a scrolling manipulation
//--------------------------------------------------------------------------------------
void ScrollViewer::OnManipulationCompleted(KinectGestureRecognizer^ sender, KinectManipulationCompletedEventArgs^ args)
{
    UNREFERENCED_PARAMETER(sender);
    UNREFERENCED_PARAMETER(args);

    // Calculate the inverse stretch delta, so that rendered position snaps to the debt-free scrollOffset
    Point stretchBackDelta = Point(-m_cumulativeStretch.X, -m_cumulativeStretch.Y);
    UpdateManipulationTransform(stretchBackDelta);

    // On manipulation end, we clear any scrolling debt, so calculate clamped scrollOffset
    m_scrollOffset = ClampToScrollingBounds(m_scrollOffset);
    m_cumulativeStretch = Point(0, 0);
}

//--------------------------------------------------------------------------------------
// Name: CalculateScrollingDelta()
// Desc: Calculate the scrolling delta within the bounds
//--------------------------------------------------------------------------------------
Point ScrollViewer::CalculateScrollingDelta(Point prevScrollOffset, Point newScrollOffset)
{
    Point prevScrollOffsetClamped = ClampToScrollingBounds(prevScrollOffset);
    Point newScrollOffsetClamped = ClampToScrollingBounds(newScrollOffset);

    return Point(newScrollOffsetClamped.X - prevScrollOffsetClamped.X, newScrollOffsetClamped.Y - prevScrollOffsetClamped.Y);
}

//--------------------------------------------------------------------------------------
// Name: CalculateStretchDelta()
// Desc: Calculate the stretch delta beyond the bounds.
//--------------------------------------------------------------------------------------
Point ScrollViewer::CalculateStretchDelta(Point prevScrollOffset, Point newScrollOffset)
{
    // Calculate the distance beyond the bounds for the previous and current scrollOffset
    Point prevBeyondBounds = DistanceBeyondBounds(prevScrollOffset);
    Point newBeyondBounds = DistanceBeyondBounds(newScrollOffset);

    Point prevStretchDistance = Point(pow(abs(prevBeyondBounds.X), StretchExponent), pow(abs(prevBeyondBounds.Y), StretchExponent));
    Point prevStretchDelta = Point(prevBeyondBounds.X < 0.0f ? -prevStretchDistance.X : prevStretchDistance.X, prevBeyondBounds.Y < 0.0f ? -prevStretchDistance.Y : prevStretchDistance.Y);

    Point newStretchDistance = Point(pow(abs(newBeyondBounds.X), StretchExponent), pow(abs(newBeyondBounds.Y), StretchExponent));
    Point newStretchDelta = Point(newBeyondBounds.X < 0.0f ? -newStretchDistance.X : newStretchDistance.X, newBeyondBounds.Y < 0.0f ? -newStretchDistance.Y : newStretchDistance.Y);

    // Stretch function is not associative, so actual delta is difference between previous and current stretch
    return Point(newStretchDelta.X - prevStretchDelta.X, newStretchDelta.Y - prevStretchDelta.Y);
}

//--------------------------------------------------------------------------------------
// Name: DistanceBeyondBounds()
// Desc: Helper that returns the distance between the bounds and a point beyond the bounds.
//       If a component of the point is within the bounds, the resultant component is 0.
//       If beyond the bounds (above m_scrollRange): distance will be positive.
//       If beyond the bounds (below 0): distance will be negative.
//--------------------------------------------------------------------------------------
Point ScrollViewer::DistanceBeyondBounds(Point scrollOffset)
{
    return Point(
        scrollOffset.X < 0 ? scrollOffset.X : (scrollOffset.X > m_scrollRange.x  ? scrollOffset.X - m_scrollRange.x : 0),
        scrollOffset.Y < 0 ? scrollOffset.Y : (scrollOffset.Y > m_scrollRange.y  ? scrollOffset.Y - m_scrollRange.y : 0));
}

//--------------------------------------------------------------------------------------
// Name: ClampToScrollingBounds()
// Desc: Helper that returns the supplied scrollOffset clamped to within the scrolling bounds
//       ScrollOffset is clamped between 0 and -scrollBoundsSize in both x and y.
//       0 is not scrolled at all. m_scrollRange is scrolled all the way to the right or bottom.
//--------------------------------------------------------------------------------------
Point ScrollViewer::ClampToScrollingBounds(Point scrollOffset)
{
    return Point(
        max(0, min(m_scrollRange.x, scrollOffset.X)),
        max(0, min(m_scrollRange.y, scrollOffset.Y))
        );
}
