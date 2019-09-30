//------------------------------------------------------------------------------
// <copyright file="ImageRenderer.h" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//------------------------------------------------------------------------------

// Manages the drawing of image data

#pragma once

#include <d2d1.h>
#include <Dwrite.h>
#include <DirectXMath.h>

class ImageRenderer
{
public:
    /// <summary>
    /// Constructor
    /// </summary>
    ImageRenderer();

    /// <summary>
    /// Destructor
    /// </summary>
    virtual ~ImageRenderer();

    /// <summary>
    /// Set the window to draw to as well as the video format
    /// Implied bits per pixel is 32
    /// </summary>
    /// <param name="hWnd">window to draw to</param>
    /// <param name="pD2DFactory">already created D2D factory object</param>
    /// <param name="sourceWidth">width (in pixels) of image data to be drawn</param>
    /// <param name="sourceHeight">height (in pixels) of image data to be drawn</param>
    /// <param name="sourceStride">length (in bytes) of a single scanline</param>
    /// <returns>indicates success or failure</returns>
    HRESULT Initialize(HWND hwnd, ID2D1Factory* pD2DFactory, int sourceWidth, int sourceHeight, int sourceStride);

    /// <summary>
    /// Prepare device to begin drawing
    /// <returns>indicates success or failure</returns>
    /// </summary>
    HRESULT BeginDrawing();

    /// <summary>
    /// Ends drawing
    /// <returns>indicates success or failure</returns>
    /// </summary>    
    HRESULT EndDrawing();

    /// <summary>
    /// Draws a 32 bit per pixel image of previously specified width, height, and stride to the associated hwnd
    /// </summary>
    /// <param name="pImage">image data in RGBX format</param>
    /// <param name="cbImage">size of image data in bytes</param>
    /// <returns>indicates success or failure</returns>
    HRESULT DrawBackground(BYTE* pImage, unsigned long cbImage);

    /// <summary>
    /// Draws face frame results
    /// </summary>
    /// <param name="iFace">the index of the face frame corresponding to a specific body in the FOV</param>
    /// <param name="pFaceBox">face bounding box</param>
    /// <param name="pFacePoints">face points</param>
    /// <param name="pFaceRotation">face rotation</param>
    /// <param name="pFaceProperties">face properties</param>
    /// <param name="pFaceTextLayout">face result text layout</param>
    void DrawFaceFrameResults(int iFace, const RectI* pFaceBox, const PointF* pFacePoints, const Vector4* pFaceRotation, const DetectionResult* pFaceProperties, const D2D1_POINT_2F* pFaceTextLayout);

private:
    /// <summary>
    /// Ensure necessary Direct2d resources are created
    /// </summary>
    /// <returns>indicates success or failure</returns>
    HRESULT EnsureResources();

    /// <summary>
    /// Dispose of Direct2d resources 
    /// </summary>
    void DiscardResources();

    /// <summary>
    /// Validates face bounding box and face points to be within screen space
    /// </summary>
    /// <param name="pFaceBox">the face bounding box</param>
    /// <param name="pFacePoints">the face points</param>
    /// <returns>success or failure</returns>
    bool ValidateFaceBoxAndPoints(const RectI* pFaceBox, const PointF* pFacePoints);

    /// <summary>
    /// Converts rotation quaternion to Euler angles 
    /// And then maps them to a specified range of values to control the refresh rate
    /// </summary>
    /// <param name="pQuaternion">face rotation quaternion</param>
    /// <param name="pPitch">rotation about the X-axis</param>
    /// <param name="pYaw">rotation about the Y-axis</param>
    /// <param name="pRoll">rotation about the Z-axis</param>
    static void ExtractFaceRotationInDegrees(const Vector4* pQuaternion, int* pPitch, int* pYaw, int* pRoll);

    HWND                     m_hWnd;

    // Format information
    UINT                     m_sourceHeight;
    UINT                     m_sourceWidth;
    LONG                     m_sourceStride;

    // Direct2D 
    ID2D1Factory*            m_pD2DFactory;
    ID2D1HwndRenderTarget*   m_pRenderTarget;
    ID2D1Bitmap*             m_pBitmap;
    ID2D1SolidColorBrush*    m_pFaceBrush[BODY_COUNT];

    // DirectWrite
    IDWriteFactory*		     m_pDWriteFactory;
    IDWriteTextFormat*       m_pTextFormat;    
};