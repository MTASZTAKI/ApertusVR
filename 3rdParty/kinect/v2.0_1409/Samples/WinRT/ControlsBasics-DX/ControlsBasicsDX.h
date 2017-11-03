//------------------------------------------------------------------------------
// <copyright file="ControlsBasicsDX.h" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//------------------------------------------------------------------------------

// A basic game implementation that creates a D3D11 device, provides a game loop and
// utilizes Microsoft.Kinect.Toolkit.Input to create Kinect enabled controls.

#pragma once

#include "pch.h"
#include "Button.h"
#include "CursorObject.h"
#include "KinectUserViewerImageSource.h"

ref class ScrollViewer;

struct PositionTexVertex
{
	XMFLOAT4 Position;
	XMFLOAT2 TexCoord;
};

struct SpriteConstantBuffer
{
	XMMATRIX Transform;
	XMFLOAT4 TargetSpriteIndex;
	XMFLOAT4 Tint;
};

struct ConstantBuffer
{
	XMMATRIX world;
	XMFLOAT4 diffuse;
};

using namespace Platform;
using namespace Microsoft::Kinect::Toolkit::Input;
using namespace Microsoft::WRL;
using namespace Windows::ApplicationModel;
using namespace Windows::ApplicationModel::Activation;
using namespace Windows::ApplicationModel::Core;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Core;
using namespace WindowsPreview::Kinect::Input;

// A basic game implementation that creates a D3D11 device and
// provides a game loop
ref class ControlsBasicsDX sealed
{
public:
	ControlsBasicsDX();
	virtual ~ControlsBasicsDX();

	// Initialization and management
	void Initialize(CoreWindow^ window);
	void LoadContent();

	// Basic game loop
	void Tick();
	void Update();
	void Render();

	// Rendering helpers
	void Clear();
	void Present();

protected:
	void OnPointerEntered(
		_In_ WindowsPreview::Kinect::Input::KinectCoreWindow^ sender,
		_In_ WindowsPreview::Kinect::Input::KinectPointerEventArgs^ args
		);
	void OnPointerMoved(
		_In_ WindowsPreview::Kinect::Input::KinectCoreWindow^ sender,
		_In_ WindowsPreview::Kinect::Input::KinectPointerEventArgs^ args
		);
	void OnPointerExited(
		_In_ WindowsPreview::Kinect::Input::KinectCoreWindow^ sender,
		_In_ WindowsPreview::Kinect::Input::KinectPointerEventArgs^ args
		);
	void OnKeyDown(
		_In_ Windows::UI::Core::CoreWindow^ sender,
		_In_ Windows::UI::Core::KeyEventArgs^ args
		);
	void OnKeyUp(
		_In_ Windows::UI::Core::CoreWindow^ sender,
		_In_ Windows::UI::Core::KeyEventArgs^ args
		);
	void OnBodyFrameArrived(
		_In_ WindowsPreview::Kinect::BodyFrameReader^ sender,
		_In_ WindowsPreview::Kinect::BodyFrameArrivedEventArgs^ args
		);
	void OnButtonTapped(
		_In_ Button^ sender,
		_In_ ButtonEventArgs^ args
		);

	void OnActivated(CoreWindow^ sender, WindowActivatedEventArgs^ args);
	void OnSizeChanged(CoreWindow^ sender, WindowSizeChangedEventArgs^ args);
	Microsoft::Kinect::Toolkit::Input::HitTestResult^ GetElementsAtPoint(Point point);

internal:
	enum class ScreenType
	{
		First = 0,
		HorizontalScrolling = First,
		HorizontalScrollingLargeButtons,
		Panning,
		PanningSmallButtons,
		FourButtonsWithMargins,
		MaxCountForNonSnappedViews,
		SnappedFirst,
		SnappedView = SnappedFirst,
		MaxCountForSnappedViews,
	};

	static property Rect CurrentClientBounds { 
		Rect get() { return s_clientWindowBounds; } 
	}

	void GoToScreen(ScreenType screen);
	void CreateUI(int32 rows, int32 columns, float buttonWidth, float buttonHeight,
		float verticalMargin, float horizontalMargin,
		bool showBackButton,
		WindowsPreview::Kinect::Input::KinectGestureSettings scrollViewerGestureSettings,
		WindowsPreview::Kinect::Input::KinectGestureSettings buttonGestureSettings);
	
	enum class PlayerEngagementMode
	{
		First = 0,
		OnePersonSystemEngagement = First,
		TwoPersonSystemEngagement,
		MaxCount,
	};

	bool HandleEngagement(
		WindowsPreview::Kinect::Input::KinectPointerPoint^ pointerPoint,
		PlayerEngagementMode playerEngagementMode);

	CursorObject^ GetCursor(
		WindowsPreview::Kinect::Input::KinectPointerPoint^ pointerPoint,
		PlayerEngagementMode playerEngagementMode);

	void CleanUpCursorAndGestures(uint32 pointerId, bool gesturesCleanupOnly);

	void CleanUpAllCursorsAndGestures();

	enum class GamePadDirection
	{
		None = 0,
		Left,
		Up,
		Right,
		Down,
	};

private:
	void CreateDevice();
	void CreateResources();
	void OnLogicalDpiChanged(Object^ sender);
	void SetStateForSpriteRendering();
	void SetStateForColoredRectsDrawing();
	void DrawCursor(CursorObject^ cursor, XMFLOAT4 tintColor);
	void DrawCursor(CursorObject^ cursor, Windows::UI::Color tintColor);
	void PositionKinectUserViewer();
	void DrawKinectUserViewer();

	void DrawSprite(
		ComPtr<ID3D11ShaderResourceView> textureResourceView,
		XMVECTOR position,
		int spriteFrameIndex,
		int spriteRows,
		int spriteCols,
		float spriteWidth,
		float spriteHeight,
		XMFLOAT4 tint = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
		bool mirrorHorizontally = false);

	void DrawRectFromTexture(
		ComPtr<ID3D11ShaderResourceView> textureResourceView,
		XMFLOAT2 point,
		float width,
		float height,
		float clipX,
		float clipY,
		float clipWidth,
		float clipHeight,
		XMFLOAT4 tint = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
		bool mirrorHorizontally = false);

	void DrawRectangle(float x, float y, float width, float height, const XMFLOAT4& color);
	void DrawButton(Button^ button, XMFLOAT4 buttonColor, bool drawFocusRect, XMFLOAT4 focusRectColor = XMFLOAT4());
	void DrawButton(
		Button^ button,
		Windows::UI::Color buttonColor,
		bool drawFocusRect,
		Windows::UI::Color focusRectColor = Windows::UI::Colors::Transparent);

	// Core Application state
	Agile<CoreWindow>                      m_window;

	WindowsPreview::Kinect::Input::KinectCoreWindow^ m_kinectCoreWindow;

	static Windows::Foundation::Rect       s_clientWindowBounds; // origin is always 0, 0. Height/Width same as m_windowBounds
	Windows::Foundation::Rect              m_windowBounds;

	// Direct3D Objects
	D3D_FEATURE_LEVEL                      m_featureLevel;
	ComPtr<ID3D11Device1>                  m_d3dDevice;
	ComPtr<ID3D11DeviceContext1>           m_d3dContext;

	// Direct2D Rendering Objects. Required for 2D.
	ComPtr<ID2D1Factory1>                  m_d2dFactory;
	ComPtr<ID2D1Device>                    m_d2dDevice;
	ComPtr<ID2D1DeviceContext>             m_d2dContext;
	ComPtr<ID2D1Bitmap1>                   m_d2dTargetBitmap;

	// Rendering resources                 
	ComPtr<IDXGISwapChain1>                m_swapChain;
	ComPtr<ID3D11RenderTargetView>         m_renderTargetView;
	ComPtr<ID3D11DepthStencilView>         m_depthStencilView;
	ComPtr<ID3D11Texture2D>                m_depthStencil;

	// Game state                          
	PlayerEngagementMode                   m_playerEngagementMode;
	INT64                                  m_frame;

	INT64                                  m_lastControllerInputFrame;
	int                                    m_frameDelayUntilNuiInteractionsAllowed;
	bool                                   m_latestButtonIsDown;
	float                                  m_focusRectThickness;
	int                                    m_frameDelayUntilNextControllerMoveAllowed;
	bool                                   m_isActivated;

	// D3D device resources
	ComPtr<ID3D11PixelShader>              m_colorPixelShader;
	ComPtr<ID3D11VertexShader>             m_vertexShader;
	ComPtr<ID3D11InputLayout>              m_vertexLayout;
	ComPtr<ID3D11SamplerState>             m_linearClampSampler;
	ComPtr<ID3D11Buffer>                   m_constantBuffer;
	ComPtr<ID3D11Buffer>                   m_vertexBuffer;
	ComPtr<ID3D11DepthStencilState>        m_depthStencilState;
	ComPtr<ID3D11BlendState>               m_blendState;
	ComPtr<ID3D11Buffer>                   m_spriteConstantBuffer;
	ComPtr<ID3D11VertexShader>             m_spriteVertexShader;
	ComPtr<ID3D11PixelShader>              m_spritePixelShader;
	ComPtr<ID3D11Texture2D>                m_cursorTexture;
	ComPtr<ID3D11ShaderResourceView>       m_cursorView;
	D3D11_TEXTURE2D_DESC                   m_backBufferDesc;
	ComPtr<IDWriteTextFormat>              m_textFormat;
	ComPtr<ID2D1SolidColorBrush>           m_blackBrush;

	// Scene data members
	ScreenType                             m_screen;
	IMap<uint32, CursorObject^>^           m_cursorMap;
	IMap<uint64, Windows::UI::Color>^      m_bodyTrackingIdToCursorColor;
	IMap<uint64, IMap<uint32, bool>^>^     m_engagedHandsByUsers;
	Array<Button^>^                        m_buttons;
	Array<Windows::Foundation::EventRegistrationToken>^ m_buttonTappedTokens;
	unsigned int                           m_lastCursorColor;
	Array<int32>^                          m_zorder;

	ScrollViewer^                          m_scrollviewer;
	
	UIElement^                             m_focusedElement;
	float                                  m_resolutionScale;

	std::unique_ptr<KinectUserViewerImageSource> m_kinectUserViewerImageSource;
	
	WindowsPreview::Kinect::KinectSensor^  m_kinectSensor;
	WindowsPreview::Kinect::BodyFrameReader^ m_bodyFrameReader;
	WindowsPreview::Kinect::BodyIndexFrameReader^ m_bodyIndexFrameReader;
	WindowsPreview::Kinect::DepthFrameReader^ m_depthFrameReader;
	Windows::Foundation::Collections::IVector<WindowsPreview::Kinect::Body^>^ m_bodies;

	Windows::Foundation::EventRegistrationToken m_pointerEntered;
	Windows::Foundation::EventRegistrationToken m_pointerMoved;
	Windows::Foundation::EventRegistrationToken m_pointerExited;
	Windows::Foundation::EventRegistrationToken m_bodyFrameArrivedToken;
	Windows::Foundation::EventRegistrationToken m_logicalDpiChangedToken;

	Microsoft::Kinect::Toolkit::Input::InputPointerManager^ m_inputPointerManager;
};