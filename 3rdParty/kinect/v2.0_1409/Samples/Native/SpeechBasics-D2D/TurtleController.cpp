//------------------------------------------------------------------------------
// <copyright file="TurtleController.cpp" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//------------------------------------------------------------------------------

#include "stdafx.h"
#include "TurtleController.h"

// Static initializers
const D2D1_POINT_2U TurtleController::Displacements[NumDirections] =
{
    D2D1::Point2U(static_cast<UINT32>(0), static_cast<UINT32>(-1)),  // Corresponds to DirectionUp
    D2D1::Point2U(static_cast<UINT32>(1), static_cast<UINT32>(0)),  // Corresponds to DirectionRight
    D2D1::Point2U(static_cast<UINT32>(0), static_cast<UINT32>(1)), // Corresponds to DirectionDown
    D2D1::Point2U(static_cast<UINT32>(-1), static_cast<UINT32>(0))  // Corresponds to DirectionLeft
};

// Centers turtle around origin, so it can be easily rotated and translated.
// Turtle body parts layout is done directly in an {X,Y} coordinate space where
// X is in [0.0,99.057] interval and Y is in [0.0,128.859] interval.
const D2D1_SIZE_F TurtleController::m_TurtleBodySize = D2D1::SizeF(99.057f, 128.859f);

/// <summary>
/// Constructor
/// </summary>
TurtleController::TurtleController() : 
    m_hWnd(0),
    m_pD2DFactory(NULL), 
    m_pRenderTarget(NULL),
    m_pTurtleBody(NULL),
    m_pTurtleHead(NULL),
    m_pTurtleTail(NULL),
    m_pTurtleFrontLeftFoot(NULL),
    m_pTurtleBackLeftFoot(NULL),
    m_pTurtleFrontRightFoot(NULL),
    m_pTurtleBackRightFoot(NULL),
    m_pBodyFill(NULL),
    m_LayoutSize(D2D1::SizeU()),
    m_CurrentDirection(DirectionUp),
    m_CurrentRotationAngle(0),
    m_CurrentPosition(D2D1::Point2U()),
    m_OriginCenteringTransform(D2D1::Matrix3x2F::Translation(-m_TurtleBodySize.width/2, -m_TurtleBodySize.height/2)),
    m_ScalingTransform(D2D1::Matrix3x2F::Identity())
{
}

/// <summary>
/// Destructor
/// </summary>
TurtleController::~TurtleController()
{
    DiscardResources();
    SafeRelease(m_pD2DFactory);
}

/// <summary>
/// Set the window to draw to.
/// </summary>
/// <param name="hWnd">window to draw to</param>
/// <param name="pD2DFactory">already created D2D factory object</param>
/// <returns>S_OK on success, otherwise failure code.</returns>
HRESULT TurtleController::Initialize(const HWND hWnd, ID2D1Factory* pD2DFactory)
{
    if (NULL == pD2DFactory)
    {
        return E_INVALIDARG;
    }

    m_hWnd = hWnd;

    // One factory for the entire application so save a pointer here
    m_pD2DFactory = pD2DFactory;

    m_pD2DFactory->AddRef();

    return S_OK;
}

/// <summary>
/// Draws audio panel.
/// </summary>
/// <returns>S_OK on success, otherwise failure code.</returns>
HRESULT TurtleController::Draw()
{
    // create the resources for this draw device. They will be recreated if previously lost.
    HRESULT hr = EnsureResources();

    if ( FAILED(hr) )
    {
        return hr;
    }
    
    m_pRenderTarget->BeginDraw();

    m_pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::White));

    // Put turtle in desired position and orientation
    m_pRenderTarget->SetTransform(GetTurtleTransform());

    // Draw turtle body and extremities
    m_pRenderTarget->FillGeometry(m_pTurtleBody, m_pBodyFill, NULL);
    m_pRenderTarget->FillGeometry(m_pTurtleHead, m_pBodyFill, NULL);
    m_pRenderTarget->FillGeometry(m_pTurtleTail, m_pBodyFill, NULL);
    m_pRenderTarget->FillGeometry(m_pTurtleFrontLeftFoot, m_pBodyFill, NULL);
    m_pRenderTarget->FillGeometry(m_pTurtleBackLeftFoot, m_pBodyFill, NULL);
    m_pRenderTarget->FillGeometry(m_pTurtleFrontRightFoot, m_pBodyFill, NULL);
    m_pRenderTarget->FillGeometry(m_pTurtleBackRightFoot, m_pBodyFill, NULL);
            
    hr = m_pRenderTarget->EndDraw();

    // Device lost, need to recreate the render target. We'll dispose it now and retry drawing.
    if (hr == D2DERR_RECREATE_TARGET)
    {
        hr = S_OK;
        DiscardResources();
    }

    return hr;
}

/// <summary>
/// Makes the turtle performs the specified action.
/// </summary>
void TurtleController::DoAction(TurtleAction action)
{
    // Number of degrees in a right angle.
    const int DegreesInRightAngle = 90;

    switch (action)
    {
    case TurtleActionForward:
        m_CurrentPosition.x = (m_LayoutSize.width + m_CurrentPosition.x + (DisplacementAmount * Displacements[m_CurrentDirection].x)) % m_LayoutSize.width;
        m_CurrentPosition.y = (m_LayoutSize.height + m_CurrentPosition.y + (DisplacementAmount * Displacements[m_CurrentDirection].y)) % m_LayoutSize.height;
        break;

    case TurtleActionBackward:
        m_CurrentPosition.x = (m_LayoutSize.width + m_CurrentPosition.x - (DisplacementAmount * Displacements[m_CurrentDirection].x)) % m_LayoutSize.width;
        m_CurrentPosition.y = (m_LayoutSize.height + m_CurrentPosition.y - (DisplacementAmount * Displacements[m_CurrentDirection].y)) % m_LayoutSize.height;
        break;

    case TurtleActionTurnRight:
        m_CurrentDirection = NextDirection(m_CurrentDirection, true);

        // We take a right turn to mean a clockwise right angle rotation for the displayed turtle.
        m_CurrentRotationAngle += DegreesInRightAngle;
        break;

    case TurtleActionTurnLeft:
        m_CurrentDirection = NextDirection(m_CurrentDirection, false);

        // We take a left turn to mean a counter-clockwise right angle rotation for the displayed turtle.
        m_CurrentRotationAngle -= DegreesInRightAngle;
        break;
    }

    // Refresh turtle display after doing an action
    Draw();
}

/// <summary>
/// Dispose of Direct2d resources.
/// </summary>
void TurtleController::DiscardResources()
{
    SafeRelease(m_pRenderTarget);
    SafeRelease(m_pTurtleBody);
    SafeRelease(m_pTurtleHead);
    SafeRelease(m_pTurtleTail);
    SafeRelease(m_pTurtleFrontLeftFoot);
    SafeRelease(m_pTurtleBackLeftFoot);
    SafeRelease(m_pTurtleFrontRightFoot);
    SafeRelease(m_pTurtleBackRightFoot);
    SafeRelease(m_pBodyFill);
}

/// <summary>
/// Ensure necessary Direct2d resources are created
/// </summary>
/// <returns>indicates success or failure</returns>
HRESULT TurtleController::EnsureResources()
{
    HRESULT hr = S_OK;

    if (NULL == m_pRenderTarget)
    {
        // Get the panel size
        RECT displayRect = {0};
        GetWindowRect(m_hWnd, &displayRect);
        UINT displayWidth = displayRect.right - displayRect.left;
        UINT displayHeight = displayRect.bottom - displayRect.top;
        
        // Create scaling transform
        float scalingFactor = static_cast<float>(displayWidth) / ReferenceWidth;
        m_ScalingTransform = D2D1::Matrix3x2F::Scale(scalingFactor, scalingFactor);

        // Position turtle at center of display area, in layout coordinate space
        m_LayoutSize = D2D1::SizeU(ReferenceWidth, ReferenceWidth * displayHeight / displayWidth);
        m_CurrentPosition.x = m_LayoutSize.width / 2;
        m_CurrentPosition.y = m_LayoutSize.height / 2;

        D2D1_RENDER_TARGET_PROPERTIES rtProps = D2D1::RenderTargetProperties();
        rtProps.pixelFormat = D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_IGNORE);
        rtProps.usage = D2D1_RENDER_TARGET_USAGE_GDI_COMPATIBLE;

        // Create a hWnd render target, in order to render to the window set in initialize
        hr = m_pD2DFactory->CreateHwndRenderTarget(
            rtProps,
            D2D1::HwndRenderTargetProperties(m_hWnd, D2D1::SizeU(displayWidth, displayHeight)),
            &m_pRenderTarget
            );

        if (SUCCEEDED(hr))
        {
            hr = CreateMainBody();

            if (SUCCEEDED(hr))
            {
                hr = CreateHead();

                if (SUCCEEDED(hr))
                {
                    hr = CreateTail();

                    if (SUCCEEDED(hr))
                    {
                        hr = CreateFeet();

                        if (SUCCEEDED(hr))
                        {
                            hr = CreateBrushes();
                        }
                    }
                }
            }
        }
    }

    if ( FAILED(hr) )
    {
        DiscardResources();
    }

    return hr;
}

/// <summary>
/// Create geometry for main turtle body.
/// </summary>
/// <returns>S_OK on success, otherwise failure code.</returns>
HRESULT TurtleController::CreateMainBody()
{
    HRESULT hr = m_pD2DFactory->CreatePathGeometry(&m_pTurtleBody);

    if (SUCCEEDED(hr))
    {
        ID2D1GeometrySink *pGeometrySink = NULL;
        hr = m_pTurtleBody->Open(&pGeometrySink);

        if (SUCCEEDED(hr))
        {
            pGeometrySink->BeginFigure(D2D1::Point2F(49.279f,30.538f), D2D1_FIGURE_BEGIN_FILLED);
            pGeometrySink->AddBezier(D2D1::BezierSegment(D2D1::Point2F(29.813f,30.538f), D2D1::Point2F(13.975f,48.466f), D2D1::Point2F(13.975f,70.502f)));
            pGeometrySink->AddBezier(D2D1::BezierSegment(D2D1::Point2F(13.975f,92.539f), D2D1::Point2F(29.813f,110.466f), D2D1::Point2F(49.279f,110.466f)));
            pGeometrySink->AddBezier(D2D1::BezierSegment(D2D1::Point2F(68.748f,110.466f), D2D1::Point2F(84.586f,92.539f), D2D1::Point2F(84.586f,70.502f)));
            pGeometrySink->AddBezier(D2D1::BezierSegment(D2D1::Point2F(84.586f,48.466f), D2D1::Point2F(68.748f,30.538f), D2D1::Point2F(49.279f,30.538f)));
            
            pGeometrySink->EndFigure(D2D1_FIGURE_END_CLOSED);
            hr = pGeometrySink->Close();
        }

        SafeRelease(pGeometrySink);
    }

    return hr;
}

/// <summary>
/// Create geometry for turtle head.
/// </summary>
/// <returns>S_OK on success, otherwise failure code.</returns>
HRESULT TurtleController::CreateHead()
{
    HRESULT hr = m_pD2DFactory->CreatePathGeometry(&m_pTurtleHead);

    if (SUCCEEDED(hr))
    {
        ID2D1GeometrySink *pGeometrySink = NULL;
        hr = m_pTurtleHead->Open(&pGeometrySink);

        if (SUCCEEDED(hr))
        {
            pGeometrySink->BeginFigure(D2D1::Point2F(61.715f,29.865f), D2D1_FIGURE_BEGIN_FILLED);
            pGeometrySink->AddBezier(D2D1::BezierSegment(D2D1::Point2F(64.850f,26.697f), D2D1::Point2F(66.789f,22.343f), D2D1::Point2F(66.789f,17.534f)));
            pGeometrySink->AddBezier(D2D1::BezierSegment(D2D1::Point2F(66.789f,7.850f), D2D1::Point2F(58.939f,0.000f), D2D1::Point2F(49.256f,0.000f)));
            pGeometrySink->AddBezier(D2D1::BezierSegment(D2D1::Point2F(39.572f,0.000f), D2D1::Point2F(31.722f,7.850f), D2D1::Point2F(31.722f,17.534f)));
            pGeometrySink->AddBezier(D2D1::BezierSegment(D2D1::Point2F(31.722f,22.357f), D2D1::Point2F(33.671f,26.723f), D2D1::Point2F(36.822f,29.893f)));
            pGeometrySink->AddBezier(D2D1::BezierSegment(D2D1::Point2F(40.737f,28.375f), D2D1::Point2F(44.935f,27.538f), D2D1::Point2F(49.305f,27.538f)));
            pGeometrySink->AddBezier(D2D1::BezierSegment(D2D1::Point2F(53.648f,27.538f), D2D1::Point2F(57.820f,28.365f), D2D1::Point2F(61.715f,29.865f)));
            
            pGeometrySink->EndFigure(D2D1_FIGURE_END_CLOSED);
            hr = pGeometrySink->Close();
        }

        SafeRelease(pGeometrySink);
    }

    return hr;
}

/// <summary>
/// Create geometry for turtle tail.
/// </summary>
/// <returns>S_OK on success, otherwise failure code.</returns>
HRESULT TurtleController::CreateTail()
{
    HRESULT hr = m_pD2DFactory->CreatePathGeometry(&m_pTurtleTail);

    if (SUCCEEDED(hr))
    {
        ID2D1GeometrySink *pGeometrySink = NULL;
        hr = m_pTurtleTail->Open(&pGeometrySink);

        if (SUCCEEDED(hr))
        {
            pGeometrySink->BeginFigure(D2D1::Point2F(45.151f,113.209f), D2D1_FIGURE_BEGIN_FILLED);
            pGeometrySink->AddBezier(D2D1::BezierSegment(D2D1::Point2F(51.105f,121.359f), D2D1::Point2F(44.672f,128.859f), D2D1::Point2F(44.672f,128.859f)));
            pGeometrySink->AddBezier(D2D1::BezierSegment(D2D1::Point2F(52.939f,125.963f), D2D1::Point2F(54.227f,115.555f), D2D1::Point2F(54.418f,113.072f)));
            pGeometrySink->AddBezier(D2D1::BezierSegment(D2D1::Point2F(52.744f,113.324f), D2D1::Point2F(51.039f,113.466f), D2D1::Point2F(49.305f,113.466f)));
            pGeometrySink->AddBezier(D2D1::BezierSegment(D2D1::Point2F(47.900f,113.466f), D2D1::Point2F(46.516f,113.375f), D2D1::Point2F(45.151f,113.209f)));
            
            pGeometrySink->EndFigure(D2D1_FIGURE_END_CLOSED);
            hr = pGeometrySink->Close();
        }

        SafeRelease(pGeometrySink);
    }

    return hr;
}

/// <summary>
/// Create geometry for turtle feet.
/// </summary>
/// <returns>S_OK on success, otherwise failure code.</returns>
HRESULT TurtleController::CreateFeet()
{
    HRESULT hr = CreateFrontLeftFoot();

    if (SUCCEEDED(hr))
    {
        hr = CreateBackLeftFoot();

        if (SUCCEEDED(hr))
        {
            hr = CreateFrontRightFoot();

            if (SUCCEEDED(hr))
            {
                hr = CreateBackRightFoot();
            }
        }
    }
    
    return hr;
}

/// <summary>
/// Create geometry for front left turtle foot.
/// </summary>
/// <returns>S_OK on success, otherwise failure code.</returns>
HRESULT TurtleController::CreateFrontLeftFoot()
{
    HRESULT hr = m_pD2DFactory->CreatePathGeometry(&m_pTurtleFrontLeftFoot);

    if (SUCCEEDED(hr))
    {
        ID2D1GeometrySink *pGeometrySink = NULL;
        hr = m_pTurtleFrontLeftFoot->Open(&pGeometrySink);

        if (SUCCEEDED(hr))
        {
            pGeometrySink->BeginFigure(D2D1::Point2F(10.551f,39.545f), D2D1_FIGURE_BEGIN_FILLED);
            pGeometrySink->AddBezier(D2D1::BezierSegment(D2D1::Point2F(4.724f,39.545f), D2D1::Point2F(0.000f,44.269f), D2D1::Point2F(0.000f,50.096f)));
            pGeometrySink->AddBezier(D2D1::BezierSegment(D2D1::Point2F(0.000f,55.923f), D2D1::Point2F(4.724f,60.646f), D2D1::Point2F(10.551f,60.646f)));
            pGeometrySink->AddBezier(D2D1::BezierSegment(D2D1::Point2F(11.063f,60.646f), D2D1::Point2F(11.562f,60.597f), D2D1::Point2F(12.054f,60.527f)));
            pGeometrySink->AddBezier(D2D1::BezierSegment(D2D1::Point2F(13.354f,54.418f), D2D1::Point2F(15.818f,48.816f), D2D1::Point2F(19.172f,44.026f)));
            pGeometrySink->AddBezier(D2D1::BezierSegment(D2D1::Point2F(17.262f,41.318f), D2D1::Point2F(14.116f,39.545f), D2D1::Point2F(10.551f,39.545f)));
            pGeometrySink->EndFigure(D2D1_FIGURE_END_CLOSED);

            hr = pGeometrySink->Close();
        }

        SafeRelease(pGeometrySink);
    }

    return hr;
}

/// <summary>
/// Create geometry for back left turtle foot.
/// </summary>
/// <returns>S_OK on success, otherwise failure code.</returns>
HRESULT TurtleController::CreateBackLeftFoot()
{
    HRESULT hr = m_pD2DFactory->CreatePathGeometry(&m_pTurtleBackLeftFoot);

    if (SUCCEEDED(hr))
    {
        ID2D1GeometrySink *pGeometrySink = NULL;
        hr = m_pTurtleBackLeftFoot->Open(&pGeometrySink);

        if (SUCCEEDED(hr))
        {
            pGeometrySink->BeginFigure(D2D1::Point2F(16.551f,92.758f), D2D1_FIGURE_BEGIN_FILLED);
            pGeometrySink->AddBezier(D2D1::BezierSegment(D2D1::Point2F(11.078f,93.147f), D2D1::Point2F(6.756f,97.698f), D2D1::Point2F(6.756f,103.269f)));
            pGeometrySink->AddBezier(D2D1::BezierSegment(D2D1::Point2F(6.756f,109.097f), D2D1::Point2F(11.479f,113.820f), D2D1::Point2F(17.307f,113.820f)));
            pGeometrySink->AddBezier(D2D1::BezierSegment(D2D1::Point2F(22.251f,113.820f), D2D1::Point2F(26.389f,110.414f), D2D1::Point2F(27.532f,105.823f)));
            pGeometrySink->AddBezier(D2D1::BezierSegment(D2D1::Point2F(23.108f,102.381f), D2D1::Point2F(19.354f,97.930f), D2D1::Point2F(16.551f,92.758f)));
            pGeometrySink->EndFigure(D2D1_FIGURE_END_CLOSED);

            hr = pGeometrySink->Close();
        }

        SafeRelease(pGeometrySink);
    }

    return hr;
}

/// <summary>
/// Create geometry for front right turtle foot.
/// </summary>
/// <returns>S_OK on success, otherwise failure code.</returns>
HRESULT TurtleController::CreateFrontRightFoot()
{
    HRESULT hr = m_pD2DFactory->CreatePathGeometry(&m_pTurtleFrontRightFoot);

    if (SUCCEEDED(hr))
    {
        ID2D1GeometrySink *pGeometrySink = NULL;
        hr = m_pTurtleFrontRightFoot->Open(&pGeometrySink);

        if (SUCCEEDED(hr))
        {
            pGeometrySink->BeginFigure(D2D1::Point2F(99.057f,50.096f), D2D1_FIGURE_BEGIN_FILLED);
            pGeometrySink->AddBezier(D2D1::BezierSegment(D2D1::Point2F(99.057f,44.269f), D2D1::Point2F(94.332f,39.545f), D2D1::Point2F(88.506f,39.545f)));
            pGeometrySink->AddBezier(D2D1::BezierSegment(D2D1::Point2F(84.800f,39.545f), D2D1::Point2F(81.547f,41.459f), D2D1::Point2F(79.664f,44.347f)));
            pGeometrySink->AddBezier(D2D1::BezierSegment(D2D1::Point2F(82.891f,49.047f), D2D1::Point2F(85.268f,54.512f), D2D1::Point2F(86.543f,60.457f)));
            pGeometrySink->AddBezier(D2D1::BezierSegment(D2D1::Point2F(87.180f,60.577f), D2D1::Point2F(87.834f,60.646f), D2D1::Point2F(88.506f,60.646f)));
            pGeometrySink->AddBezier(D2D1::BezierSegment(D2D1::Point2F(94.332f,60.646f), D2D1::Point2F(99.057f,55.923f), D2D1::Point2F(99.057f,50.096f)));
            pGeometrySink->EndFigure(D2D1_FIGURE_END_CLOSED);

            hr = pGeometrySink->Close();
        }

        SafeRelease(pGeometrySink);
    }

    return hr;
}

/// <summary>
/// Create geometry for back right turtle foot.
/// </summary>
/// <returns>S_OK on success, otherwise failure code.</returns>
HRESULT TurtleController::CreateBackRightFoot()
{
    HRESULT hr = m_pD2DFactory->CreatePathGeometry(&m_pTurtleBackRightFoot);

    if (SUCCEEDED(hr))
    {
        ID2D1GeometrySink *pGeometrySink = NULL;
        hr = m_pTurtleBackRightFoot->Open(&pGeometrySink);

        if (SUCCEEDED(hr))
        {
            pGeometrySink->BeginFigure(D2D1::Point2F(71.447f,105.528f), D2D1_FIGURE_BEGIN_FILLED);
            pGeometrySink->AddBezier(D2D1::BezierSegment(D2D1::Point2F(72.482f,110.269f), D2D1::Point2F(76.699f,113.820f), D2D1::Point2F(81.750f,113.820f)));
            pGeometrySink->AddBezier(D2D1::BezierSegment(D2D1::Point2F(87.578f,113.820f), D2D1::Point2F(92.301f,109.097f), D2D1::Point2F(92.301f,103.269f)));
            pGeometrySink->AddBezier(D2D1::BezierSegment(D2D1::Point2F(92.301f,97.551f), D2D1::Point2F(87.749f,92.908f), D2D1::Point2F(82.071f,92.737f)));
            pGeometrySink->AddBezier(D2D1::BezierSegment(D2D1::Point2F(79.345f,97.772f), D2D1::Point2F(75.717f,102.124f), D2D1::Point2F(71.447f,105.528f)));

            pGeometrySink->EndFigure(D2D1_FIGURE_END_CLOSED);
            hr = pGeometrySink->Close();
        }

        SafeRelease(pGeometrySink);
    }

    return hr;
}

/// <summary>
/// Brushes used to fill turtle regions.
/// </summary>
/// <returns>S_OK on success, otherwise failure code.</returns>
HRESULT TurtleController::CreateBrushes()
{
    return m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(0x54308d), &m_pBodyFill);
}

/// <summary>
/// Get transformation matrix needed to get turtle body parts from layout coordinates into
/// display coordinates.
/// </summary>
/// <returns>Direct2D transform matrix.</return>
D2D1_MATRIX_3X2_F TurtleController::GetTurtleTransform()
{
    D2D1_MATRIX_3X2_F rotationTransform = D2D1::Matrix3x2F::Rotation(static_cast<float>(m_CurrentRotationAngle));
    D2D1_MATRIX_3X2_F positionTransform = D2D1::Matrix3x2F::Translation(static_cast<float>(m_CurrentPosition.x), static_cast<float>(m_CurrentPosition.y));

    return m_OriginCenteringTransform * rotationTransform * positionTransform * m_ScalingTransform;
}