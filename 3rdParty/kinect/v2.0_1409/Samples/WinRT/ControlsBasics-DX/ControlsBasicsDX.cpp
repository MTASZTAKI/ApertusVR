//------------------------------------------------------------------------------
// <copyright file="ControlsBasicsDX.cpp" company="Microsoft">
//     A basic game implementation that creates a D3D11 device, provides a game loop and
//     utilizes Microsoft.Kinect.Toolkit.Input to create Kinect enabled controls.
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//------------------------------------------------------------------------------

#include "pch.h"
#include "ControlsBasicsDX.h"
#include "UIElement.h"
#include "ScrollViewer.h"

#include <collection.h>
#include "ColorPixelShader.h"
#include "VertexShader.h"
#include "SpritePixelShader.h"
#include "SpriteVertexShader.h"
#include "DDSTextureLoader.h"

static const int KinectUserViewerWidth = 121;
static const int KinectUserViewerHeight = 100;

using namespace Windows::Foundation::Collections;
using namespace WindowsPreview::Kinect::Input;

Rect ControlsBasicsDX::s_clientWindowBounds;

//--------------------------------------------------------------------------------------
// Name: ControlsBasicsDX()
// Desc: Constructor
//--------------------------------------------------------------------------------------
ControlsBasicsDX::ControlsBasicsDX()
{
    m_focusRectThickness = 8.0f;

    m_frame = 0;
    m_lastControllerInputFrame = -1;

    m_screen = (Windows::UI::ViewManagement::ApplicationView::Value ==
        Windows::UI::ViewManagement::ApplicationViewState::Snapped) ? ScreenType::SnappedFirst : ScreenType::First;

    // This sample defaults to one player being engaged at a time, using the SystemEngagement model.
    m_playerEngagementMode = PlayerEngagementMode::OnePersonSystemEngagement;

    m_kinectSensor = WindowsPreview::Kinect::KinectSensor::GetDefault();
    m_bodyFrameReader = m_kinectSensor->BodyFrameSource->OpenReader();
    m_bodyIndexFrameReader = m_kinectSensor->BodyIndexFrameSource->OpenReader();
    m_depthFrameReader = m_kinectSensor->DepthFrameSource->OpenReader();

    m_inputPointerManager = ref new Microsoft::Kinect::Toolkit::Input::InputPointerManager(m_kinectSensor);

    m_kinectCoreWindow = WindowsPreview::Kinect::Input::KinectCoreWindow::GetForCurrentThread();

    m_bodies = ref new Platform::Collections::Vector<WindowsPreview::Kinect::Body^>(m_bodyFrameReader->BodyFrameSource->BodyCount);

    m_kinectSensor->Open();
}

//--------------------------------------------------------------------------------------
// Name: CreateUI()
// Desc: Build Buttons and ScrollViewers
//--------------------------------------------------------------------------------------
void ControlsBasicsDX::CreateUI(int32 rows, int32 columns, float buttonWidth, float buttonHeight,
    float verticalMargin, float horizontalMargin,
    bool showBackButton,
    WindowsPreview::Kinect::Input::KinectGestureSettings scrollViewerGestureSettings,
    WindowsPreview::Kinect::Input::KinectGestureSettings buttonGestureSettings)
{
    // Clean up buttons and remove Tapped Event Handlers
    if (m_buttons != nullptr)
    {
        for (unsigned int i = 0; i < m_buttons->Length; ++i)
        {
            m_buttons[i]->Tapped -= m_buttonTappedTokens[i];
        }
    }

    CleanUpAllCursorsAndGestures();

    int otherButtonCount = showBackButton ? 1 : 0;

    m_zorder = ref new Platform::Array<int32>(rows * columns + otherButtonCount);
    for (unsigned int i = 0; i < m_zorder->Length; ++i)
    {
        m_zorder[i] = m_zorder->Length - 1 - i;
    }

    // Adjust for Resolution Scale
    buttonWidth *= m_resolutionScale;
    buttonHeight *= m_resolutionScale;
    horizontalMargin *= m_resolutionScale;
    verticalMargin *= m_resolutionScale;

    // Typical buttons are 208px with a 4px margin.
    // The margin should be hittable via NUI Interactions, even though it will usually appear to be part of the background.
    const float hittableMarginSize = 4.0f * m_resolutionScale;
    float buttonTotalWidth = buttonWidth + hittableMarginSize;
    float buttonTotalHeight = buttonHeight + hittableMarginSize;

    float extentWidth = buttonTotalWidth * columns + 2 * horizontalMargin;
    float extentHeight = buttonTotalHeight * rows + 2 * verticalMargin;

    m_scrollviewer = ref new ScrollViewer(scrollViewerGestureSettings);
    m_scrollviewer->Initialize(extentWidth, extentHeight, min(extentWidth, m_window->Bounds.Width), min(extentHeight, m_window->Bounds.Height));

    int numButtons = rows * columns + otherButtonCount;
    m_buttons = ref new Platform::Array<Button^>(numButtons);
    m_buttonTappedTokens = ref new Platform::Array<Windows::Foundation::EventRegistrationToken>(numButtons);
    for (int i = 0; i < numButtons; ++i)
    {
        m_buttons[i] = ref new Button(buttonGestureSettings, m_scrollviewer);
        m_buttonTappedTokens[i] = m_buttons[i]->Tapped += ref new Windows::Foundation::TypedEventHandler<Button^, ButtonEventArgs^>(this, &ControlsBasicsDX::OnButtonTapped);
    }
    
    unsigned int buttonIndex = 0;

    for (int x = 0; x < columns; ++x)
    {
        for (int y = 0; y < rows; ++y)
        {
            m_buttons[buttonIndex]->Initialize(
                (Color)(buttonIndex % (unsigned int)Color::MaxCount),
                x * buttonTotalWidth + horizontalMargin, y * buttonTotalHeight + verticalMargin,
                buttonWidth, buttonHeight,
                hittableMarginSize);

            buttonIndex++;
        }
    }
    
    m_focusedElement = m_buttons[0];

    if (showBackButton)
    {
        // scale the back button by the ratio of the current window size to the maximum window size
        float x = 136.0f * ControlsBasicsDX::CurrentClientBounds.Width / 1920.0f;
        float y = 63.0f * ControlsBasicsDX::CurrentClientBounds.Height / 1080.0f;

        // create 54x54 backbutton in top left of screen 
        m_buttons[buttonIndex] = ref new Button(buttonGestureSettings, nullptr);
        m_buttons[buttonIndex]->Initialize(
            Color::LightGray,
            x * m_resolutionScale, y * m_resolutionScale,
            54.0f * m_resolutionScale, 54.0f * m_resolutionScale,
            x * m_resolutionScale, y * m_resolutionScale, 0.0f, 0.0f); // hittableMarginSize is variable per size
      
        buttonIndex++;

        // TODO: create backButton icon via Image and spritesheet.
    }

    // Can support up to 12 cursors (1 cursor per hand in scene)
    m_cursorMap = ref new Platform::Collections::Map<uint32, CursorObject^>();
    m_bodyTrackingIdToCursorColor = ref new Platform::Collections::Map<uint64, Windows::UI::Color>();
    m_engagedHandsByUsers =
        ref new Platform::Collections::Map<uint64, Windows::Foundation::Collections::IMap<uint32, bool>^>();

    m_lastCursorColor = (uint32)Color::First;
}

//--------------------------------------------------------------------------------------
// Name: SwitchToUIMode()
// Desc: Switches to specified UI Mode for this sample
//--------------------------------------------------------------------------------------
void ControlsBasicsDX::GoToScreen(ScreenType screen)
{
    switch (screen)
    {
    case ScreenType::HorizontalScrolling:
        CreateUI(3, 100, 208.0f, 208.0f, 300.0f, 100.0f,
            true,
            WindowsPreview::Kinect::Input::KinectGestureSettings::ManipulationTranslateX
            | WindowsPreview::Kinect::Input::KinectGestureSettings::ManipulationTranslateInertia,
            WindowsPreview::Kinect::Input::KinectGestureSettings::Tap
            | WindowsPreview::Kinect::Input::KinectGestureSettings::KinectHold
            );
        break;
    case ScreenType::HorizontalScrollingLargeButtons:
        CreateUI(2, 100, 416.0f, 416.0f, 124.0f, 100.0f,
            true,
            WindowsPreview::Kinect::Input::KinectGestureSettings::ManipulationTranslateX
            | WindowsPreview::Kinect::Input::KinectGestureSettings::ManipulationTranslateInertia,
            WindowsPreview::Kinect::Input::KinectGestureSettings::Tap
            | WindowsPreview::Kinect::Input::KinectGestureSettings::KinectHold
            );
        break;
    case ScreenType::Panning:
        CreateUI(32, 32, 208.0f, 208.0f, 100.0f, 100.0f,
            true,
            WindowsPreview::Kinect::Input::KinectGestureSettings::ManipulationTranslateX
            | WindowsPreview::Kinect::Input::KinectGestureSettings::ManipulationTranslateY
            | WindowsPreview::Kinect::Input::KinectGestureSettings::ManipulationTranslateInertia,
            WindowsPreview::Kinect::Input::KinectGestureSettings::Tap
            | WindowsPreview::Kinect::Input::KinectGestureSettings::KinectHold
            );
        break;
    case ScreenType::PanningSmallButtons:
        CreateUI(64, 64, 104.0f, 104.0f, 100.0f, 100.0f,
            true,
            WindowsPreview::Kinect::Input::KinectGestureSettings::ManipulationTranslateX
            | WindowsPreview::Kinect::Input::KinectGestureSettings::ManipulationTranslateY
            | WindowsPreview::Kinect::Input::KinectGestureSettings::ManipulationTranslateInertia,
            WindowsPreview::Kinect::Input::KinectGestureSettings::Tap
            | WindowsPreview::Kinect::Input::KinectGestureSettings::KinectHold
            );
        break;
    case ScreenType::FourButtonsWithMargins:
        CreateUI(2, 2, 208.0f, 208.0f, 880.0f, 1720.0f,
            true,
            WindowsPreview::Kinect::Input::KinectGestureSettings::ManipulationTranslateRailsX
            | WindowsPreview::Kinect::Input::KinectGestureSettings::ManipulationTranslateRailsY,
            WindowsPreview::Kinect::Input::KinectGestureSettings::Tap
            | WindowsPreview::Kinect::Input::KinectGestureSettings::KinectHold
            );
        break;
    case ScreenType::SnappedView:
        CreateUI(8, 1, 208.0f, 208.0f, 210.0f, 40.0f,
            true,
            WindowsPreview::Kinect::Input::KinectGestureSettings::ManipulationTranslateY,
            WindowsPreview::Kinect::Input::KinectGestureSettings::Tap
            | WindowsPreview::Kinect::Input::KinectGestureSettings::KinectHold
            );
        break;
    }
}

//--------------------------------------------------------------------------------------
// Name: ~ControlsBasicsDX()
// Desc: Destructor
//--------------------------------------------------------------------------------------
ControlsBasicsDX::~ControlsBasicsDX()
{
    if (m_pointerEntered.Value != 0)
    {
        m_kinectCoreWindow->PointerEntered -= m_pointerEntered;
    }
    if (m_pointerMoved.Value != 0)
    {
        m_kinectCoreWindow->PointerMoved -= m_pointerMoved;
    }
    if (m_pointerExited.Value != 0)
    {
        m_kinectCoreWindow->PointerExited -= m_pointerExited;
    }
    if (m_bodyFrameArrivedToken.Value != 0)
    {
        m_bodyFrameReader->FrameArrived -= m_bodyFrameArrivedToken;
    }
    if (m_logicalDpiChangedToken.Value != 0)
    {
        Windows::Graphics::Display::DisplayProperties::LogicalDpiChanged -= m_logicalDpiChangedToken;
    }
}

//--------------------------------------------------------------------------------------
// Name: Initialize()
// Desc: Initialize the Direct3D resources required to run.
//--------------------------------------------------------------------------------------
void ControlsBasicsDX::Initialize(Windows::UI::Core::CoreWindow^ window)
{
    m_window = window;

    CreateDevice();

    CreateResources();
   
    PositionKinectUserViewer();

    // Listen to PointerMoved events to understand where a user is gesturing via their hand.
    
    m_pointerEntered    = m_kinectCoreWindow->PointerEntered += 
        ref new TypedEventHandler<
            WindowsPreview::Kinect::Input::KinectCoreWindow^,
            WindowsPreview::Kinect::Input::KinectPointerEventArgs^>(
                this, &ControlsBasicsDX::OnPointerEntered);
    m_pointerMoved      = m_kinectCoreWindow->PointerMoved += 
        ref new TypedEventHandler<
            WindowsPreview::Kinect::Input::KinectCoreWindow^,
            WindowsPreview::Kinect::Input::KinectPointerEventArgs^>(
                this, &ControlsBasicsDX::OnPointerMoved);
    m_pointerExited     = m_kinectCoreWindow->PointerExited += 
        ref new TypedEventHandler<
            WindowsPreview::Kinect::Input::KinectCoreWindow^,
            WindowsPreview::Kinect::Input::KinectPointerEventArgs^>(
                this, &ControlsBasicsDX::OnPointerExited);

    m_bodyFrameArrivedToken = m_bodyFrameReader->FrameArrived +=
        ref new TypedEventHandler<
            WindowsPreview::Kinect::BodyFrameReader^,
            WindowsPreview::Kinect::BodyFrameArrivedEventArgs^>(
                this, &ControlsBasicsDX::OnBodyFrameArrived);

    // Keep track of window activation state to control rendering (or not) of focus rects.
    window->Activated +=
        ref new Windows::Foundation::TypedEventHandler<
        Windows::UI::Core::CoreWindow^, Windows::UI::Core::WindowActivatedEventArgs^>(
        this, &ControlsBasicsDX::OnActivated);

    // Adjust the UI based on size changes when snapped/filled/fullscreen.
    window->SizeChanged +=
        ref new Windows::Foundation::TypedEventHandler<
        Windows::UI::Core::CoreWindow^, Windows::UI::Core::WindowSizeChangedEventArgs^>(
        this, &ControlsBasicsDX::OnSizeChanged);

    // Listen to Keyboard Down Events
    window->KeyDown +=
        ref new Windows::Foundation::TypedEventHandler<
        Windows::UI::Core::CoreWindow^, Windows::UI::Core::KeyEventArgs^>(
        this, &ControlsBasicsDX::OnKeyDown);

    // Listen to Keyboard Up Events
    window->KeyUp +=
        ref new Windows::Foundation::TypedEventHandler<
        Windows::UI::Core::CoreWindow^, Windows::UI::Core::KeyEventArgs^>(
        this, &ControlsBasicsDX::OnKeyUp);

    // Listen to Logical DPI Changes
    m_logicalDpiChangedToken = Windows::Graphics::Display::DisplayProperties::LogicalDpiChanged
        += ref new Windows::Graphics::Display::DisplayPropertiesEventHandler(
        this, &ControlsBasicsDX::OnLogicalDpiChanged);

    GoToScreen(m_screen);
}

//--------------------------------------------------------------------------------------
// Name: LoadContent()
// Desc: Load content required for the application.
//--------------------------------------------------------------------------------------
void ControlsBasicsDX::LoadContent()
{
    // The splash screen has been dismissed, so present something while we load assets.
    Clear();
    Present();
}

//--------------------------------------------------------------------------------------
// Name: Tick()
// Desc: Executes basic ControlsBasicsDX loop
//--------------------------------------------------------------------------------------
void ControlsBasicsDX::Tick()
{
    Update();

    Render();

    ++m_frame;
}

//--------------------------------------------------------------------------------------
// Name: Update()
// Desc: Called once per frame to update data
//--------------------------------------------------------------------------------------
void ControlsBasicsDX::Update()
{
    auto depthFrame = m_depthFrameReader->AcquireLatestFrame();
    auto bodyIndexFrame = m_bodyIndexFrameReader->AcquireLatestFrame();

    if (depthFrame != nullptr && bodyIndexFrame != nullptr)
    {
        uint32 bodyIndices = 0;

        for (uint32 i = 0; i < m_bodies->Size; ++i)
        {
            auto body = m_bodies->GetAt(i);
            if (body && body->IsTracked)
            {
                if (m_engagedHandsByUsers->HasKey(body->TrackingId))
                {
                    bodyIndices = bodyIndices | (0x1 << i);
                }
            }
        }

        m_kinectUserViewerImageSource->UpdateNui(depthFrame, bodyIndexFrame, bodyIndices);
    }
}

//--------------------------------------------------------------------------------------
// Name: Update()
// Desc: Clear the backbuffer
//--------------------------------------------------------------------------------------
void ControlsBasicsDX::Clear()
{
    // Clear the views
    // Create a viewport descriptor of the full window size.
    CD3D11_VIEWPORT viewPort(0.0f, 0.0f, static_cast<float>(m_backBufferDesc.Width), static_cast<float>(m_backBufferDesc.Height));

    // Set the current viewport using the descriptor.
    m_d3dContext->RSSetViewports(1, &viewPort);

    const float clearColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    m_d3dContext->OMSetRenderTargets(1, m_renderTargetView.GetAddressOf(), m_depthStencilView.Get());
    m_d3dContext->ClearRenderTargetView(m_renderTargetView.Get(), clearColor);

    if (m_depthStencilView != nullptr)
    {
        m_d3dContext->ClearDepthStencilView(m_depthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
    }
}

//--------------------------------------------------------------------------------------
// Name: Present()
// Desc: Presents the backbuffer contents to the screen
//--------------------------------------------------------------------------------------
void ControlsBasicsDX::Present()
{
    // The first argument instructs DXGI to block until VSync, putting the application
    // to sleep until the next VSync. This ensures we don't waste any cycles rendering
    // frames that will never be displayed to the screen.
    HRESULT hr = m_swapChain->Present(1, 0);

    // If the device was removed either by a disconnect or a driver upgrade, we
    // must completely reinitialize the renderer.
    if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
    {
        Initialize(m_window.Get());
    }
    else
    {
        DX::ThrowIfFailed(hr);
    }
}

//--------------------------------------------------------------------------------------
// Name: CreateDevice()
// Desc: Create d3d devuce
//--------------------------------------------------------------------------------------
void ControlsBasicsDX::CreateDevice()
{
    UINT creationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;

    // This array defines the set of DirectX hardware feature levels this app will support.
    // Note the ordering should be preserved.
    const D3D_FEATURE_LEVEL c_featureLevels[] =
    {
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
        D3D_FEATURE_LEVEL_9_3,
        D3D_FEATURE_LEVEL_9_2,
        D3D_FEATURE_LEVEL_9_1,
    };

    // Create the DX11 API device object, and get a corresponding context.
    Microsoft::WRL::ComPtr<ID3D11Device> device;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> context;

    DX::ThrowIfFailed(
        D3D11CreateDevice(
        nullptr,                    // specify null to use the default adapter
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,                    // leave as nullptr unless software device
        creationFlags,              // optionally set debug and Direct2D compatibility flags
        c_featureLevels,              // list of feature levels this app can support
        ARRAYSIZE(c_featureLevels),   // number of entries in above list
        D3D11_SDK_VERSION,          // always set this to D3D11_SDK_VERSION
        &device,                    // returns the Direct3D device created
        &m_featureLevel,            // returns feature level of device created
        &context                    // returns the device immediate context
        )
        );

    // Get the DirectX11.1 device by QI off the DirectX11 one.
    DX::ThrowIfFailed(device.As(&m_d3dDevice));

    // And get the corresponding device context in the same way.
    DX::ThrowIfFailed(context.As(&m_d3dContext));

    // Create D2D factory
    D2D1_FACTORY_OPTIONS options;
    ZeroMemory(&options, sizeof(D2D1_FACTORY_OPTIONS));

    DX::ThrowIfFailed(
        D2D1CreateFactory(
            D2D1_FACTORY_TYPE_SINGLE_THREADED,
            __uuidof(ID2D1Factory1),
            &options,
            &m_d2dFactory
            )
        );

    // Get the underlying DXGI device of the Direct3D device.
    Microsoft::WRL::ComPtr<IDXGIDevice> dxgiDevice;
    DX::ThrowIfFailed(
        m_d3dDevice.As(&dxgiDevice)
        );

    // Create the Direct2D device object and a corresponding context.
    DX::ThrowIfFailed(
        m_d2dFactory->CreateDevice(dxgiDevice.Get(), &m_d2dDevice)
        );

    DX::ThrowIfFailed(
        m_d2dDevice->CreateDeviceContext(
            D2D1_DEVICE_CONTEXT_OPTIONS_NONE,
            &m_d2dContext
            )
        );
}

//--------------------------------------------------------------------------------------
// Name: OnSizeChanged()
// Desc: Window size has changed (snapping, fill)
//--------------------------------------------------------------------------------------
void ControlsBasicsDX::OnSizeChanged(CoreWindow^ sender, WindowSizeChangedEventArgs^ args)
{
    if (m_windowBounds != m_window->Bounds)
    {
        CreateResources();

        // Adjust any UI that is relative to Window size
        m_scrollviewer->WindowResized(m_window->Bounds);

        // ensure that the UI matches the window size (if snapped now, or unsnapped)
        if (Windows::UI::ViewManagement::ApplicationView::Value ==
            Windows::UI::ViewManagement::ApplicationViewState::Snapped)
        {
            m_screen = ScreenType::SnappedFirst;
            GoToScreen(m_screen);
        }
        else if (m_screen >= ScreenType::SnappedFirst)
        {
            m_screen = ScreenType::First;
            GoToScreen(m_screen);
        }
    }
}

//--------------------------------------------------------------------------------------
// Name: CreateResources()
// Desc: Allocate all memory resources that change on a window SizeChanged event.
//--------------------------------------------------------------------------------------
void ControlsBasicsDX::CreateResources()
{
    // Store the window bounds so the next time we get a SizeChanged event we can
    // avoid rebuilding everything if the size is identical.
    m_windowBounds = m_window.Get()->Bounds;
    m_resolutionScale = 100.0f / static_cast<float>(Windows::Graphics::Display::DisplayProperties::ResolutionScale);
    s_clientWindowBounds = Rect(0, 0, m_windowBounds.Width, m_windowBounds.Height);
    
    // If the swap chain already exists, resize it,
    // otherwise create one.
    if (m_swapChain != nullptr)
    {
        // Set context's render target to null.
        m_d3dContext->OMSetRenderTargets(0, NULL, NULL);
        m_renderTargetView = nullptr;

        // Set D2D context's render target to null
        m_d2dContext->SetTarget(nullptr);
        m_d2dTargetBitmap = nullptr;

        DX::ThrowIfFailed(m_swapChain->ResizeBuffers(2, 0, 0, DXGI_FORMAT_B8G8R8A8_UNORM, 0));
    }
    else
    {
        // First, retrieve the underlying DXGI Device from the D3D Device
        Microsoft::WRL::ComPtr<IDXGIDevice1> dxgiDevice;
        DX::ThrowIfFailed(m_d3dDevice.As(&dxgiDevice));

        // Identify the physical adapter (GPU or card) this device is running on.
        Microsoft::WRL::ComPtr<IDXGIAdapter> dxgiAdapter;
        DX::ThrowIfFailed(dxgiDevice->GetAdapter(&dxgiAdapter));

        // And obtain the factory object that created it.
        Microsoft::WRL::ComPtr<IDXGIFactory2> dxgiFactory;
        DX::ThrowIfFailed(dxgiAdapter->GetParent(__uuidof(IDXGIFactory2), &dxgiFactory));

        // Create a descriptor for the swap chain.
        DXGI_SWAP_CHAIN_DESC1 swapChainDesc = { 0 };
        swapChainDesc.Width = (UINT)(m_window->Bounds.Width);
        swapChainDesc.Height = (UINT)(m_window->Bounds.Height);
        swapChainDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
        swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        swapChainDesc.BufferCount = 2;
        swapChainDesc.Stereo = false;
        swapChainDesc.SampleDesc.Count = 1;
        swapChainDesc.SampleDesc.Quality = 0;
        swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
        swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
        swapChainDesc.Flags = 0;

        // Create a SwapChain from a CoreWindow.
        DX::ThrowIfFailed(dxgiFactory->CreateSwapChainForCoreWindow(m_d3dDevice.Get(), reinterpret_cast<IUnknown*>(CoreWindow::GetForCurrentThread()), &swapChainDesc, nullptr, &m_swapChain));

        // Ensure that DXGI does not queue more than one frame at a time. This both reduces
        // latency and ensures that the application will only render after each VSync, minimizing
        // power consumption.
        DX::ThrowIfFailed(dxgiDevice->SetMaximumFrameLatency(1));
    }

    // Obtain the backbuffer for this window which will be the final 3D rendertarget.
    Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;
    DX::ThrowIfFailed(m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), &backBuffer));

    // Create a view interface on the rendertarget to use on bind.
    DX::ThrowIfFailed(m_d3dDevice->CreateRenderTargetView(backBuffer.Get(), nullptr, &m_renderTargetView));

    // Cache the rendertarget dimensions in our helper class for convenient use.
    backBuffer->GetDesc(&m_backBufferDesc);

    const WCHAR c_fontName[] = L"Verdana";
    const FLOAT c_fontSize = 50;

    // Create write factory to create text format
    Microsoft::WRL::ComPtr<IDWriteFactory> writeFactory;
    DX::ThrowIfFailed(DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(writeFactory), reinterpret_cast<IUnknown**>(writeFactory.GetAddressOf())));

    // Create the text format
    DX::ThrowIfFailed(writeFactory->CreateTextFormat(
        c_fontName,
        NULL,
        DWRITE_FONT_WEIGHT_NORMAL,
        DWRITE_FONT_STYLE_NORMAL,
        DWRITE_FONT_STRETCH_NORMAL,
        c_fontSize,
        L"",
        m_textFormat.GetAddressOf()));

    m_textFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
    m_textFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

    // Brush for the text
    DX::ThrowIfFailed(
        m_d2dContext->CreateSolidColorBrush(
            D2D1::ColorF(D2D1::ColorF::Black),
            &m_blackBrush
            )
        );

    // Create the DXGI Surface Render Target.
    FLOAT dpiX = 0;
    FLOAT dpiY = 0;
    m_d2dFactory->GetDesktopDpi(&dpiX, &dpiY);

    // Create a Direct2D target bitmap associated with the
    // swap chain back buffer and set it as the current target.
    D2D1_BITMAP_PROPERTIES1 bitmapProperties = 
        D2D1::BitmapProperties1(
            D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
            D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED),
            dpiX,
            dpiY
            );

    Microsoft::WRL::ComPtr<IDXGISurface> dxgiBackBuffer;
    DX::ThrowIfFailed(
        m_swapChain->GetBuffer(0, IID_PPV_ARGS(&dxgiBackBuffer))
        );

    DX::ThrowIfFailed(
        m_d2dContext->CreateBitmapFromDxgiSurface(
            dxgiBackBuffer.Get(),
            &bitmapProperties,
            &m_d2dTargetBitmap
            )
        );

    m_d2dContext->SetTarget(m_d2dTargetBitmap.Get());

    // We want D3D Feature level 11 or above, stop further initialization if it isn't
    if (m_featureLevel < D3D_FEATURE_LEVEL_11_0)
    {
        return;
    }

    // Allocate a 2-D surface as the depth/stencil buffer and
    // create a DepthStencil view on this surface to use on bind.
    CD3D11_TEXTURE2D_DESC depthStencilDesc(DXGI_FORMAT_D32_FLOAT, m_backBufferDesc.Width, m_backBufferDesc.Height, 1, 1, D3D11_BIND_DEPTH_STENCIL);

    Microsoft::WRL::ComPtr<ID3D11Texture2D> depthStencil;
    DX::ThrowIfFailed(m_d3dDevice->CreateTexture2D(&depthStencilDesc, nullptr, &depthStencil));

    CD3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc(D3D11_DSV_DIMENSION_TEXTURE2D);
    DX::ThrowIfFailed(m_d3dDevice->CreateDepthStencilView(depthStencil.Get(), &depthStencilViewDesc, &m_depthStencilView));

    // Create a viewport descriptor of the full window size.
    CD3D11_VIEWPORT viewPort(0.0f, 0.0f, static_cast<float>(m_backBufferDesc.Width), static_cast<float>(m_backBufferDesc.Height));

    // Set the current viewport using the descriptor.
    m_d3dContext->RSSetViewports(1, &viewPort);

    // Create vertex shader
    DX::ThrowIfFailed(m_d3dDevice->CreateVertexShader(g_vsshader, sizeof(g_vsshader), NULL, &m_vertexShader));

    // Define the vertex input layout - add color later
    D3D11_INPUT_ELEMENT_DESC layout[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
    };

    // Create the vertex input layout (need vertex shader first)
    DX::ThrowIfFailed(m_d3dDevice->CreateInputLayout(layout, _countof(layout), g_vsshader, sizeof(g_vsshader), &m_vertexLayout));

    const PositionTexVertex quadVertices[] =
    {
        { XMFLOAT4(-1.f, -1.f, 0.f, 1.f), XMFLOAT2(0.f, 1.f) },
        { XMFLOAT4(1.f, 1.f, 0.f, 1.f), XMFLOAT2(1.f, 0.f) },
        { XMFLOAT4(1.f, -1.f, 0.f, 1.f), XMFLOAT2(1.f, 1.f) },
        { XMFLOAT4(-1.f, 1.f, 0.f, 1.f), XMFLOAT2(0.f, 0.f) },
        { XMFLOAT4(1.f, 1.f, 0.f, 1.f), XMFLOAT2(1.f, 0.f) },
        { XMFLOAT4(-1.f, -1.f, 0.f, 1.f), XMFLOAT2(0.f, 1.f) },
    };

    //create vertex buffer
    D3D11_BUFFER_DESC bd = { 0 };
    bd.ByteWidth = sizeof(PositionTexVertex)* _countof(quadVertices);
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    D3D11_SUBRESOURCE_DATA srd = { quadVertices, 0, 0 };
    m_d3dDevice->CreateBuffer(&bd, &srd, &m_vertexBuffer);

    // Create Pixel Shader
    DX::ThrowIfFailed(m_d3dDevice->CreatePixelShader(g_color_psshader, sizeof(g_color_psshader), nullptr, &m_colorPixelShader));

    D3D11_BUFFER_DESC constantBufferDesc = { 0 };
    constantBufferDesc.ByteWidth = sizeof(ConstantBuffer);
    constantBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    constantBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    constantBufferDesc.MiscFlags = 0;
    constantBufferDesc.StructureByteStride = 0;
    DX::ThrowIfFailed(m_d3dDevice->CreateBuffer(&constantBufferDesc, nullptr, &m_constantBuffer));

    //
    // Setup sprite resources to support cursor rendering
    //
    // Create sprite vertex shader
    DX::ThrowIfFailed(m_d3dDevice->CreateVertexShader(g_sprite_vsshader, sizeof(g_sprite_vsshader), NULL, &m_spriteVertexShader));
    // Create sprite pixel shader
    DX::ThrowIfFailed(m_d3dDevice->CreatePixelShader(g_sprite_psshader, sizeof(g_sprite_psshader), nullptr, &m_spritePixelShader));

    // Load cursor image
    DX::ThrowIfFailed(CreateDDSTextureFromFile(m_d3dDevice.Get(), L"CursorSpriteSheet.dds", (ID3D11Resource**)m_cursorTexture.GetAddressOf(), &m_cursorView));

    // Create sprite constant buffer
    D3D11_BUFFER_DESC spriteConstantBufferDesc = { 0 };
    spriteConstantBufferDesc.ByteWidth = sizeof(SpriteConstantBuffer);
    spriteConstantBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    spriteConstantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    spriteConstantBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    spriteConstantBufferDesc.MiscFlags = 0;
    spriteConstantBufferDesc.StructureByteStride = 0;
    DX::ThrowIfFailed(m_d3dDevice->CreateBuffer(&spriteConstantBufferDesc, nullptr, &m_spriteConstantBuffer));

    // Create texture sampler
    D3D11_SAMPLER_DESC samplerDesc;
    ZeroMemory(&samplerDesc, sizeof(samplerDesc));
    samplerDesc.Filter = D3D11_FILTER_MIN_MAG_POINT_MIP_LINEAR;
    samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
    samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
    samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
    samplerDesc.MaxLOD = 3.40282e+038f;
    samplerDesc.MaxAnisotropy = 16;
    samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    DX::ThrowIfFailed(m_d3dDevice->CreateSamplerState(&samplerDesc, &m_linearClampSampler));

    // Create a blend state for transparency
    D3D11_BLEND_DESC blendDesc;
    ZeroMemory(&blendDesc, sizeof(blendDesc));
    blendDesc.RenderTarget[0].BlendEnable = true;
    blendDesc.RenderTarget[0].SrcBlend = blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_SRC_ALPHA;
    blendDesc.RenderTarget[0].DestBlend = blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
    blendDesc.RenderTarget[0].BlendOp = blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
    blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
    DX::ThrowIfFailed(m_d3dDevice->CreateBlendState(&blendDesc, &m_blendState));

    // create depth stencil state
    D3D11_DEPTH_STENCIL_DESC stencilDesc;
    ZeroMemory(&stencilDesc, sizeof(stencilDesc));
    stencilDesc.DepthEnable = false;
    stencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
    stencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
    stencilDesc.StencilEnable = false;
    stencilDesc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
    stencilDesc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
    stencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
    stencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    stencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    stencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
    stencilDesc.BackFace = stencilDesc.FrontFace;
    DX::ThrowIfFailed(m_d3dDevice->CreateDepthStencilState(&stencilDesc, &m_depthStencilState));
}

//--------------------------------------------------------------------------------------
// Name: LogicalDpiChanged(Object^ sender)
// Desc: Handle Logical DPI Change and update the resolution scale.
//--------------------------------------------------------------------------------------
void ControlsBasicsDX::OnLogicalDpiChanged(Object^ sender)
{
    CreateResources();
   
    PositionKinectUserViewer();

    GoToScreen(m_screen);
}

//--------------------------------------------------------------------------------------
// Name: Render()
// Desc: Called once per frame to render
//--------------------------------------------------------------------------------------
void ControlsBasicsDX::Render()
{
    m_kinectUserViewerImageSource->RenderNuiToTexture();

    Clear();

    // We support D3D 11 or greater. If the user is not running this, give them an error message.
    if (m_featureLevel < D3D_FEATURE_LEVEL_11_0)
    {
        const WCHAR c_incorrectD3DFeatureLevel[] = L"To run this sample, you need at least D3D 11";

        m_d2dContext->BeginDraw();
        m_d2dContext->DrawText(
            c_incorrectD3DFeatureLevel,
            _countof(c_incorrectD3DFeatureLevel),
            m_textFormat.Get(),
            D2D1::RectF(0, 0, m_windowBounds.Width, m_windowBounds.Height),
            m_blackBrush.Get());

        m_d2dContext->EndDraw();

        Present();
        return;
    }

    m_d3dContext->OMSetDepthStencilState(m_depthStencilState.Get(), 0);
    m_d3dContext->OMSetBlendState(m_blendState.Get(), nullptr, 0xFFFFFFFF);

    SetStateForColoredRectsDrawing();

    // draw all the buttons - uses colorPixelShader
    for (unsigned int i = 0; i < m_buttons->Length; ++i)
    {
        int iz = m_zorder[i];
        Button^ button = m_buttons[iz];
        button->RenderCallback(); // press/hold demo - allows button to change back to normal size after delay.

        DrawButton(button, button->GetColor(), false);
    }

    if (m_isActivated) // if this application isn't activated, don't draw focus rects for it.
    {
        // draw focus rects - uses colorPixelShader
        Button^ focusedButton = dynamic_cast<Button^>(m_focusedElement);
        Windows::UI::Color color = focusedButton->GetColor();
        DrawButton(focusedButton, focusedButton->GetColor(), true /*drawFocusRect*/, Windows::UI::Colors::Gray /*focusRectColor*/);

        // draw pointer focus rect(s) - uses colorPixelShader
        for each(auto cursorMapItem in m_cursorMap)
        {
            HitTestResult^ hitTestResult = m_inputPointerManager->GetCapturedInputModel(cursorMapItem->Key);

            if (hitTestResult != nullptr)
            {
                PressableModel^ pressCaptureModel = dynamic_cast<PressableModel^>(hitTestResult->FirstKinectAwareElement);

                if (pressCaptureModel != nullptr)
                {
                    Button^ pressCaptureElement = dynamic_cast<Button^>(pressCaptureModel->Element);
                    if (pressCaptureElement != nullptr)
                    {
                        DrawButton(pressCaptureElement, pressCaptureElement->GetColor(), true /*drawFocusRect*/, Windows::UI::Colors::Gray /*focusRectColor*/);
                    }
                }
            }
        }
    }

    SetStateForSpriteRendering();

    DrawKinectUserViewer();

    // draw cursor(s) - uses spritePixelShader
    for each(auto cursorMapItem in m_cursorMap)
    {
        CursorObject^ cursor = cursorMapItem->Value;
        DrawCursor(cursor, cursor->CursorViewModel->TintColor);
    }

    Present();
}

//--------------------------------------------------------------------------------------
// Name: SetStateForSpriteRendering()
// Desc: Initialization which must be done once each render frame if rendering sprites.
//--------------------------------------------------------------------------------------
void ControlsBasicsDX::SetStateForSpriteRendering()
{
    UINT stride = sizeof(PositionTexVertex);
    UINT offset = 0;

    m_d3dContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    m_d3dContext->IASetInputLayout(m_vertexLayout.Get());
    m_d3dContext->IASetVertexBuffers(0, 1, m_vertexBuffer.GetAddressOf(), &stride, &offset);
    m_d3dContext->VSSetShader(m_spriteVertexShader.Get(), NULL, 0);
    m_d3dContext->GSSetShader(NULL, NULL, 0);
    m_d3dContext->PSSetShader(m_spritePixelShader.Get(), NULL, 0);
    m_d3dContext->PSSetSamplers(0, 1, m_linearClampSampler.GetAddressOf());
    m_d3dContext->PSSetConstantBuffers(0, 1, m_spriteConstantBuffer.GetAddressOf());
    m_d3dContext->VSSetConstantBuffers(0, 1, m_spriteConstantBuffer.GetAddressOf());
}

//--------------------------------------------------------------------------------------
// Name: SetStateForColoredRectsDrawing()
// Desc: Initialization which must be done once each render frame if rendering color rects.
//--------------------------------------------------------------------------------------
void ControlsBasicsDX::SetStateForColoredRectsDrawing()
{
    UINT stride = sizeof(PositionTexVertex);
    UINT offset = 0;

    m_d3dContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    m_d3dContext->IASetInputLayout(m_vertexLayout.Get());
    m_d3dContext->IASetVertexBuffers(0, 1, m_vertexBuffer.GetAddressOf(), &stride, &offset);
    m_d3dContext->VSSetShader(m_vertexShader.Get(), NULL, 0);
    m_d3dContext->GSSetShader(NULL, NULL, 0);
    m_d3dContext->PSSetShader(m_colorPixelShader.Get(), NULL, 0);
    m_d3dContext->PSSetSamplers(0, 1, m_linearClampSampler.GetAddressOf());
    m_d3dContext->PSSetConstantBuffers(0, 1, m_constantBuffer.GetAddressOf());
    m_d3dContext->VSSetConstantBuffers(0, 1, m_constantBuffer.GetAddressOf());
}

//--------------------------------------------------------------------------------------
// Name: DrawButton()
// Desc: Draw button with or without a focus rect
//--------------------------------------------------------------------------------------
void ControlsBasicsDX::DrawButton(Button^ button, Windows::UI::Color buttonColor, bool drawFocusRect, Windows::UI::Color focusRectColor)
{
    auto boundingRect = button->GetBoundingRect();

    XMFLOAT4 xmButtonColor = XMFLOAT4((float)buttonColor.R / 255.0f, (float)buttonColor.G / 255.0f, (float)buttonColor.B / 255.0f, (float)buttonColor.A / 255.0f);
    XMFLOAT4 xmFocusRectColor = XMFLOAT4((float)focusRectColor.R / 255.0f, (float)focusRectColor.G / 255.0f, (float)focusRectColor.B / 255.0f, (float)focusRectColor.A / 255.0f);

    // only draw button if it is onscreen
    if (boundingRect.IntersectsWith(s_clientWindowBounds))
    {
        XMVECTOR pos = button->GetPosition();
        if (drawFocusRect)
        {
            DrawRectangle(XMVectorGetX(pos), XMVectorGetY(pos),
                button->GetWidth() + m_focusRectThickness * 2.0f, button->GetHeight() + m_focusRectThickness * 2.0f,
                xmFocusRectColor);
        }

        DrawRectangle(XMVectorGetX(pos), XMVectorGetY(pos),
            button->GetWidth(), button->GetHeight(),
            xmButtonColor);
    }
}

//--------------------------------------------------------------------------------------
// Name: DrawButton()
// Desc: Draw button with or without a focus rect
//--------------------------------------------------------------------------------------
void ControlsBasicsDX::DrawButton(Button^ button, XMFLOAT4 buttonColor, bool drawFocusRect, XMFLOAT4 focusRectColor)
{
    auto boundingRect = button->GetBoundingRect();

    // only draw button if it is onscreen
    if (boundingRect.IntersectsWith(s_clientWindowBounds))
    {
        XMVECTOR pos = button->GetPosition();
        if (drawFocusRect)
        {
            DrawRectangle(XMVectorGetX(pos), XMVectorGetY(pos),
                button->GetWidth() + m_focusRectThickness * 2.0f, button->GetHeight() + m_focusRectThickness * 2.0f,
                focusRectColor);
        }

        DrawRectangle(XMVectorGetX(pos), XMVectorGetY(pos),
            button->GetWidth(), button->GetHeight(),
            buttonColor);
    }
}

//--------------------------------------------------------------------------------------
// Name: DrawCursor()
// Desc: Draw cursor to the backbuffer
//--------------------------------------------------------------------------------------
void ControlsBasicsDX::DrawCursor(CursorObject^ cursor, Windows::UI::Color tintColor)
{
    // Get the frame# of the frame and the frame# of the background as well
    int frameIndex = cursor->CursorViewModel->SpriteSheetForegroundIndex;

    XMFLOAT4 color = XMFLOAT4((float)tintColor.R / 255.0f, (float)tintColor.G / 255.0f, (float)tintColor.B / 255.0f, (float)tintColor.A / 255.0f);

    DrawCursor(cursor, color);
}

//--------------------------------------------------------------------------------------
// Name: DrawCursor()
// Desc: Draw cursor to the backbuffer
//--------------------------------------------------------------------------------------
void ControlsBasicsDX::DrawCursor(CursorObject^ cursor, XMFLOAT4 tintColor)
{
    // Get the frame# of the frame and the frame# of the background as well
    int frameIndex = cursor->CursorViewModel->SpriteSheetForegroundIndex;
    int backgroundFrameIndex = cursor->CursorViewModel->SpriteSheetBackgroundIndex;

    auto cursorModel = cursor->CursorViewModel->Model;

    // draw background first, if we have a background frame.
    if (backgroundFrameIndex != -1)
    {
        DrawSprite(m_cursorView, XMVectorSet(cursorModel->Position.X, cursorModel->Position.Y, 0.0f, 0.0f), backgroundFrameIndex,
            CURSOR_SPRITE_ROWS, CURSOR_SPRITE_COLS, CURSOR_WIDTH, CURSOR_HEIGHT, tintColor);
    }

    DrawSprite(m_cursorView, XMVectorSet(cursorModel->Position.X, cursorModel->Position.Y, 0.0f, 0.0f), frameIndex,
        CURSOR_SPRITE_ROWS, CURSOR_SPRITE_COLS, CURSOR_WIDTH * m_resolutionScale, CURSOR_HEIGHT * m_resolutionScale, tintColor, (cursorModel->HandType == WindowsPreview::Kinect::Input::HandType::LEFT) /* mirror horizontally */);
}

//--------------------------------------------------------------------------------------
// Name: PositionKinectUserViewer()
// Desc: Position Kinect User Viewer to the top of the screen.
//--------------------------------------------------------------------------------------
void ControlsBasicsDX::PositionKinectUserViewer()
{
    POINT upperLeft = {(long)(m_windowBounds.Width - KinectUserViewerWidth * m_resolutionScale) / 2, 0};
    m_kinectUserViewerImageSource.reset(new KinectUserViewerImageSource(upperLeft, KinectUserViewerWidth * m_resolutionScale, KinectUserViewerHeight * m_resolutionScale, false));
    m_kinectUserViewerImageSource->Initialize(m_d3dDevice.Get(), m_d3dContext.Get(), m_featureLevel);
}

//--------------------------------------------------------------------------------------
// Name: DrawKinectUserViewer()
// Desc: Draw Kinect User Viewer.
//--------------------------------------------------------------------------------------
void ControlsBasicsDX::DrawKinectUserViewer()
{
    XMFLOAT2 origin(0.0f, -(KinectUserViewerHeight * m_resolutionScale / 2.0f) / (float)(s_clientWindowBounds.Height / 2.0f) + 1.0f);

    float widthDXAdjusted = KinectUserViewerWidth * m_resolutionScale / (float)(s_clientWindowBounds.Width / 2.0f);
    float heightDXAdjusted = KinectUserViewerHeight * m_resolutionScale / (float)(s_clientWindowBounds.Height / 2.0f);

    DrawRectFromTexture(m_kinectUserViewerImageSource->GetShaderResourceView(), origin, widthDXAdjusted, heightDXAdjusted, 0.0f, 0.0f, 1.0f, 1.0f);
}

//--------------------------------------------------------------------------------------
// Name: DrawSprite()
// Desc: Draws a sprite from a spritesheet.
//--------------------------------------------------------------------------------------
void ControlsBasicsDX::DrawSprite(ComPtr<ID3D11ShaderResourceView> textureResourceView, XMVECTOR position, int spriteFrameIndex, int spriteRows, int spriteCols, float spriteWidth, float spriteHeight,
    XMFLOAT4 tint, bool mirrorHorizontally)
{
    XMFLOAT2 dXAdjusted;
    dXAdjusted.x = XMVectorGetX(position) / (float)(s_clientWindowBounds.Width / 2) - 1;
    dXAdjusted.y = -XMVectorGetY(position) / (float)(s_clientWindowBounds.Height / 2) + 1;
    float widthDXAdjusted = spriteWidth / (float)(s_clientWindowBounds.Width / 2);
    float heightDXAdjusted = spriteHeight / (float)(s_clientWindowBounds.Height / 2);

    int col = spriteFrameIndex % spriteCols;
    int row = spriteFrameIndex / spriteCols;
    float u0 = (float)col / spriteCols;
    float v0 = (float)row / spriteRows;
    float u1 = (float)(col + 1) / spriteCols;
    float v1 = (float)(row + 1) / spriteRows;

    DrawRectFromTexture(textureResourceView, dXAdjusted, widthDXAdjusted, heightDXAdjusted, u0, v0, u1, v1, tint, mirrorHorizontally);
}

//--------------------------------------------------------------------------------------
// Name: DrawRectFromTexture()
// Desc: Draw a rect (width/height/origin) from a part (u0/v0/u1/v1) of a Texture
//--------------------------------------------------------------------------------------
void ControlsBasicsDX::DrawRectFromTexture(ComPtr<ID3D11ShaderResourceView> textureResourceView, XMFLOAT2 origin, float width, float height, float u0, float v0, float u1, float v1,
    XMFLOAT4 tint, bool mirrorHorizontally)
{
    // note you should have called SetStateForSpriteRendering();

    m_d3dContext->PSSetShaderResources(0, 1, textureResourceView.GetAddressOf());


    SpriteConstantBuffer constantBufferData;
    constantBufferData.Transform = XMMatrixScaling(width * .5f, height * .5f, 1.0f) *
        XMMatrixTranslation(origin.x, origin.y, 0.0f);
    constantBufferData.TargetSpriteIndex.x = u0;
    constantBufferData.TargetSpriteIndex.y = v0;
    constantBufferData.TargetSpriteIndex.z = u1 - u0;
    constantBufferData.TargetSpriteIndex.w = v1 - v0;

    if (mirrorHorizontally)
    {
        //Flip sprites horizontally
        constantBufferData.TargetSpriteIndex.x += constantBufferData.TargetSpriteIndex.z;
        constantBufferData.TargetSpriteIndex.z = -constantBufferData.TargetSpriteIndex.z;
    }

    constantBufferData.Tint = tint;

    D3D11_MAPPED_SUBRESOURCE msr = { 0 };
    HRESULT hr = m_d3dContext->Map(m_spriteConstantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);
    if (SUCCEEDED(hr) && msr.pData != nullptr)
    {
        memcpy(msr.pData, &constantBufferData, sizeof(SpriteConstantBuffer));
        m_d3dContext->Unmap(m_spriteConstantBuffer.Get(), 0);
    }
    m_d3dContext->Draw(6, 0);
}

//--------------------------------------------------------------------------------------
// Name: DrawRectangle()
// Desc: Draw a solid colored rectangle to the backbuffer
//--------------------------------------------------------------------------------------
void ControlsBasicsDX::DrawRectangle(float x, float y, float width, float height, const XMFLOAT4& color)
{
    ConstantBuffer constantBufferData;

    XMFLOAT2 centerDXAdjusted;
    centerDXAdjusted.x = x / (s_clientWindowBounds.Width / 2) - 1;
    centerDXAdjusted.y = -y / (s_clientWindowBounds.Height / 2) + 1;
    float widthDXAdjusted = width / (s_clientWindowBounds.Width / 2);
    float heightDXAdjusted = height / (s_clientWindowBounds.Height / 2);

    constantBufferData.diffuse = color;

    constantBufferData.world = XMMatrixScaling(widthDXAdjusted / 2, heightDXAdjusted / 2, 1.0f) *
        XMMatrixTranslation(centerDXAdjusted.x, centerDXAdjusted.y, 0.0f);

    D3D11_MAPPED_SUBRESOURCE msr = { 0 };
    HRESULT hr = m_d3dContext->Map(m_constantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);
    if (SUCCEEDED(hr) && msr.pData != nullptr)
    {
        memcpy(msr.pData, &constantBufferData, sizeof(ConstantBuffer));
        m_d3dContext->Unmap(m_constantBuffer.Get(), 0);
    }

    m_d3dContext->Draw(6, 0);
}

//--------------------------------------------------------------------------------------
// Name: GetElementsAtPoint()
// Desc: Finds the first gesture recognizer and first manipulation enabled gesture recognizer
//       under a point.
//--------------------------------------------------------------------------------------
Microsoft::Kinect::Toolkit::Input::HitTestResult^ ControlsBasicsDX::GetElementsAtPoint(Windows::Foundation::Point point)
{
    UIElement^ firstKinectGestureRecognizer = nullptr;
    UIElement^ firstKinectManipulationGestureRecognizer = nullptr;

    for (unsigned int i = 0; i < m_buttons->Length; ++i)
    {
        int iz = m_zorder[i];

        // Hit testing: find the object under the pointer.
        if (m_buttons[iz]->HitTest(point))
        {
            if (m_buttons[iz]->InputModel->IsManipulatable)
            {
                firstKinectManipulationGestureRecognizer = m_buttons[iz];
            }

            if (firstKinectManipulationGestureRecognizer != nullptr)
            {
                if (firstKinectGestureRecognizer == nullptr)
                {
                    firstKinectGestureRecognizer = firstKinectManipulationGestureRecognizer;
                }

                Platform::Collections::Vector<Microsoft::Kinect::Toolkit::Input::ManipulatableModel^>^ manipulationModels = ref new Platform::Collections::Vector<Microsoft::Kinect::Toolkit::Input::ManipulatableModel^>();
                manipulationModels->Append(dynamic_cast<Microsoft::Kinect::Toolkit::Input::ManipulatableModel^>(firstKinectManipulationGestureRecognizer->InputModel));

                return ref new Microsoft::Kinect::Toolkit::Input::HitTestResult(dynamic_cast<Microsoft::Kinect::Toolkit::Input::PressableModel^>(firstKinectGestureRecognizer->InputModel), manipulationModels);
            }
            else if (!m_buttons[iz]->InputModel->IsManipulatable)
            {
                firstKinectGestureRecognizer = m_buttons[iz];
            }
        }
    }

    // scrollviewer isn't part of zOrder list in this app
    if (firstKinectGestureRecognizer == nullptr)
    {
        firstKinectGestureRecognizer = m_scrollviewer;
    }
    if (firstKinectManipulationGestureRecognizer == nullptr)
    {
        firstKinectManipulationGestureRecognizer = m_scrollviewer;
    }

    Platform::Collections::Vector<Microsoft::Kinect::Toolkit::Input::ManipulatableModel^>^ manipulationModels = ref new Platform::Collections::Vector<Microsoft::Kinect::Toolkit::Input::ManipulatableModel^>();
    manipulationModels->Append(dynamic_cast<Microsoft::Kinect::Toolkit::Input::ManipulatableModel^>(firstKinectManipulationGestureRecognizer->InputModel));

    return ref new Microsoft::Kinect::Toolkit::Input::HitTestResult(dynamic_cast<Microsoft::Kinect::Toolkit::Input::PressableModel^>(firstKinectGestureRecognizer->InputModel), manipulationModels);
}

//--------------------------------------------------------------------------------------
// Name: HandleEngagement()
// Desc: Determines which players/hands get a cursor.
//       Ensures that OverrideHandTracking is called for each engaged user.
//       Returns true if pointer is engaged, false otherwise. This determines which hand
//       pointers (in this case PointerIds) to process further
//--------------------------------------------------------------------------------------
bool ControlsBasicsDX::HandleEngagement(
    WindowsPreview::Kinect::Input::KinectPointerPoint^ pointerPoint,
    PlayerEngagementMode playerEngagementMode)
{
    bool shouldBeEngaged = pointerPoint->Properties->IsEngaged;

    if (shouldBeEngaged)
    {
        bool isUserAlreadyEngaged = m_engagedHandsByUsers->HasKey(pointerPoint->Properties->BodyTrackingId);

        if (!isUserAlreadyEngaged)
        {
            Windows::Foundation::Collections::IMap<uint32, bool>^ pointerIds =
                ref new Platform::Collections::Map<uint32, bool>();
            pointerIds->Insert(pointerPoint->PointerId, true);
            m_engagedHandsByUsers->Insert(pointerPoint->Properties->BodyTrackingId, pointerIds);
        }
        else
        {
            Windows::Foundation::Collections::IMap<uint32, bool>^ pointerIds =
                m_engagedHandsByUsers->Lookup(pointerPoint->Properties->BodyTrackingId);
            bool handIsEngaged = pointerIds->HasKey(pointerPoint->PointerId);
            if (!handIsEngaged)
            {
                pointerIds->Insert(pointerPoint->PointerId, true);
            }
        }
    }

    if (!shouldBeEngaged)
    {
        if (m_cursorMap->HasKey(pointerPoint->PointerId))
        {
            CleanUpCursorAndGestures(pointerPoint->PointerId, false /* gesturesCleanupOnly */);
        }

        bool wasEngaged = m_engagedHandsByUsers->HasKey(pointerPoint->Properties->BodyTrackingId);
        if (wasEngaged)
        {
            Windows::Foundation::Collections::IMap<uint32, bool>^ pointerIds =
                m_engagedHandsByUsers->Lookup(pointerPoint->Properties->BodyTrackingId);
            bool wasHandEngaged = pointerIds->HasKey(pointerPoint->PointerId);
            if (wasHandEngaged)
            {
                pointerIds->Remove(pointerPoint->PointerId);
                if (pointerIds->Size == 0)
                {
                    m_engagedHandsByUsers->Remove(pointerPoint->Properties->BodyTrackingId);
                }
            }
        }
    }

    return shouldBeEngaged;
}

//--------------------------------------------------------------------------------------
// Name: OnPointerMoved()
// Desc: Handle pointer moved events, this forms the logic for driving Kinect interactions.
//
//       The logic reads like so:
//         1. Hit-Test the incoming PointerPoint to find the top-most "Kinect aware" element,
//            and top-most "Kinect manipulation aware" element, these may or may not be the same element.
//         2. Route to manipulation aware elements to handle scrolling:
//              2a. If a manipulatable element has capture, route to it.
//              2b. If no manipulatable element has capture but the hit-test found one, route to it.
//         3. Route to kinect aware (but not manipulation aware) elements to handle pressing:
//              3a. If no manipulatable element has capture, and hit-test found a kinect aware element
//                  that's not captured for press, route to it to give it a chance to take capture or
//                  steal it from the current capture owner.
//              3b. If a pressable element had capture, route to it. Note: Capture may be in the process of
//                  being stolen by the newly hit-tested Kinect aware element, routing to the previous capture
//                  owner lets it take care of necessary cleanup.
//--------------------------------------------------------------------------------------
_Use_decl_annotations_
void ControlsBasicsDX::OnPointerMoved(
    WindowsPreview::Kinect::Input::KinectCoreWindow^ sender,
    WindowsPreview::Kinect::Input::KinectPointerEventArgs^ args
    )
{
    // if an application is not using SystemEngagement (IsPrimary), one needs to not process pointers
    // for 3 seconds after controller input. (Setting the KinectInteractionMode to Off only affects the IsPrimary boolean, pointers still flow)
    if (m_frameDelayUntilNuiInteractionsAllowed > 0)
    {
        return;
    }

    // Enable engagement if max # of engaged users hasn't been reached.
    // Call overridehandtracking so scrolling scenarios will work with 3 or more people in scene.
    // Clean up any gestures for hands which are still in the scene

    WindowsPreview::Kinect::Input::KinectPointerPoint^ pointerPoint = args->CurrentPoint;
    
    bool shouldBeEngaged = HandleEngagement(pointerPoint, m_playerEngagementMode);
    if (!shouldBeEngaged)
    {
        return;
    }

    CursorObject^ cursor = GetCursor(pointerPoint, m_playerEngagementMode);
    if (cursor == nullptr)
    {
        return;
    }

    Point pointerWindowPosition = m_inputPointerManager->TransformInputPointerCoordinatesToWindowCoordinates( pointerPoint->Position, m_windowBounds);
        
    Microsoft::Kinect::Toolkit::Input::HitTestResult^ hitTestResult = GetElementsAtPoint(pointerWindowPosition);
    
    Microsoft::Kinect::Toolkit::Input::HitTestResult^ capturedResult = m_inputPointerManager->HandlePointerAsCursor(pointerPoint, m_windowBounds, hitTestResult);

    Microsoft::Kinect::Toolkit::Input::ManipulatableModel^ capturedManipulationElement =
        capturedResult->ManipulationAwareElements->Size > 0 ? capturedResult->ManipulationAwareElements->GetAt(0) : nullptr;

    auto cursorModel = cursor->CursorViewModel->Model;

    cursorModel->Update(pointerWindowPosition, pointerPoint, capturedResult->FirstKinectAwareElement, capturedManipulationElement);

    cursorModel->Tick();
}

//--------------------------------------------------------------------------------------
// Name: OnPointerEntered()
// Desc: Handle pointer entering to scene
//--------------------------------------------------------------------------------------
_Use_decl_annotations_
void ControlsBasicsDX::OnPointerEntered( WindowsPreview::Kinect::Input::KinectCoreWindow^ sender,
                            WindowsPreview::Kinect::Input::KinectPointerEventArgs^ args)
{
}

//--------------------------------------------------------------------------------------
// Name: OnPointerExited()
// Desc: Handle pointer exit from scene
//--------------------------------------------------------------------------------------
_Use_decl_annotations_
void ControlsBasicsDX::OnPointerExited( WindowsPreview::Kinect::Input::KinectCoreWindow^ sender,
                            WindowsPreview::Kinect::Input::KinectPointerEventArgs^ args)
{
    auto pointerPoint = args->CurrentPoint;

    if (m_cursorMap->HasKey(pointerPoint->PointerId))
    {
        CleanUpCursorAndGestures(pointerPoint->PointerId, false /* gesturesCleanupOnly */);
    }

    bool wasEngaged = m_engagedHandsByUsers->HasKey(pointerPoint->Properties->BodyTrackingId);
    if (wasEngaged)
    {
        m_engagedHandsByUsers->Remove(pointerPoint->Properties->BodyTrackingId);
    }
}

//--------------------------------------------------------------------------------------
// Name: OnKeyDown()
// Desc: Handle Keyboard Down Events
//--------------------------------------------------------------------------------------
_Use_decl_annotations_
void ControlsBasicsDX::OnKeyDown( Windows::UI::Core::CoreWindow^ sender,
                      Windows::UI::Core::KeyEventArgs^ args)
{
    UNREFERENCED_PARAMETER(sender);
    UNREFERENCED_PARAMETER(sender);
}

//--------------------------------------------------------------------------------------
// Name: OnKeyUp()
// Desc: Handle Keyboard Up Events
//--------------------------------------------------------------------------------------
_Use_decl_annotations_
void ControlsBasicsDX::OnKeyUp( Windows::UI::Core::CoreWindow^ sender,
                    Windows::UI::Core::KeyEventArgs^ args)
{
    UNREFERENCED_PARAMETER(sender);
    
    // Switch UIScreens
    if (args->VirtualKey == Windows::System::VirtualKey::T)
    {
        CleanUpAllCursorsAndGestures();            

        ++m_screen;

        if (m_screen == ScreenType::MaxCountForNonSnappedViews)
        {
            m_screen = ScreenType::First;
        }

        GoToScreen(m_screen);
    }

    // Set One Person System Engagement Mode
    if (args->VirtualKey == Windows::System::VirtualKey::Number1)
    {
        CleanUpAllCursorsAndGestures();
        m_playerEngagementMode = PlayerEngagementMode::OnePersonSystemEngagement;
        m_kinectCoreWindow->SetKinectOnePersonSystemEngagement();
    }

    // Toggle One Person System Engagement Mode
    if (args->VirtualKey == Windows::System::VirtualKey::Number2)
    {
        CleanUpAllCursorsAndGestures();
        m_playerEngagementMode = PlayerEngagementMode::TwoPersonSystemEngagement;
        m_kinectCoreWindow->SetKinectTwoPersonSystemEngagement();
    }
}

//--------------------------------------------------------------------------------------
// Name: OnButtonTapped()
// Desc: Handles Button Tapped Events.
//--------------------------------------------------------------------------------------
_Use_decl_annotations_
void ControlsBasicsDX::OnButtonTapped(Button^ sender, ButtonEventArgs^ args)
{
    UNREFERENCED_PARAMETER(args);

    m_focusedElement = sender;
}

//--------------------------------------------------------------------------------------
// Name: OnBodyFrameArrived()
// Desc: Handle Body Frame Events
//--------------------------------------------------------------------------------------
_Use_decl_annotations_
void ControlsBasicsDX::OnBodyFrameArrived( WindowsPreview::Kinect::BodyFrameReader^ sender,
                               WindowsPreview::Kinect::BodyFrameArrivedEventArgs^ args)
{
    UNREFERENCED_PARAMETER(sender);

    auto frame = args->FrameReference->AcquireFrame();

    if (frame)
    {
        frame->GetAndRefreshBodyData(m_bodies);
    }
}

//--------------------------------------------------------------------------------------
// Name: OnActivated()
// Desc: Keep track of Window activation state
//--------------------------------------------------------------------------------------
void ControlsBasicsDX::OnActivated(CoreWindow^ sender, WindowActivatedEventArgs^ args)
{
    if (args->WindowActivationState == Windows::UI::Core::CoreWindowActivationState::Deactivated)
    {
        m_isActivated = false;
    }
    else
    {
        m_isActivated = true;
    }
}

//--------------------------------------------------------------------------------------
// Name: CleanUpAllCursorsAndGestures()
// Desc: Release all cursor instances and call CompleteGesture() on captured gesture recognizers.
//--------------------------------------------------------------------------------------
void ControlsBasicsDX::CleanUpAllCursorsAndGestures()
{
    m_inputPointerManager->CompleteGestures();

    // clear cursorMap, as we are changing engagement mode.
    if (m_cursorMap)
    {
        m_cursorMap->Clear();
    }
}

//--------------------------------------------------------------------------------------
// Name: CleanUpCursorAndGestures()
// Desc: Release cursor instance and call CompleteGesture() on captured gesture recognizers.
//--------------------------------------------------------------------------------------
void ControlsBasicsDX::CleanUpCursorAndGestures(uint32 pointerId, bool gesturesCleanupOnly)
{
    m_inputPointerManager->CompleteGestures(pointerId);
    if (!gesturesCleanupOnly)
    {
        m_cursorMap->Remove(pointerId);
    }
}

//--------------------------------------------------------------------------------------
// Name: GetCursor()
// Desc: Gets (allocates if necessary) the cursor to render for the pointerPoint & playerEngagementMode given.
//--------------------------------------------------------------------------------------
CursorObject^ ControlsBasicsDX::GetCursor(
    WindowsPreview::Kinect::Input::KinectPointerPoint^ pointerPoint,
    PlayerEngagementMode playerEngagementMode)
{
    CursorObject^ cursor = nullptr;

    if (m_cursorMap->HasKey(pointerPoint->PointerId))
    {
        cursor = m_cursorMap->Lookup(pointerPoint->PointerId);
    }
    else
    {
        cursor = ref new CursorObject(pointerPoint->Properties->HandType);

        Windows::UI::Color color = Windows::UI::Colors::White;

        if (playerEngagementMode == PlayerEngagementMode::TwoPersonSystemEngagement)
        {
            uint64 bodyTrackingId = pointerPoint->Properties->BodyTrackingId;
            if (!m_bodyTrackingIdToCursorColor->HasKey(bodyTrackingId))
            {
                // As players enter the scene, they get a unique color.
                // When a person leaves the scene, this sample ideally, should put their cursor color back in a color pool.
                ++m_lastCursorColor;
                color = ColorValue[((int)m_lastCursorColor % (int)Color::MaxCount)];
                m_bodyTrackingIdToCursorColor->Insert(bodyTrackingId, color);
            }
            else
            {
                color = m_bodyTrackingIdToCursorColor->Lookup(pointerPoint->Properties->BodyTrackingId);
            }
        }

        cursor->CursorViewModel->TintColor = color;
        m_cursorMap->Insert(pointerPoint->PointerId, cursor);
    }

    return cursor;
}