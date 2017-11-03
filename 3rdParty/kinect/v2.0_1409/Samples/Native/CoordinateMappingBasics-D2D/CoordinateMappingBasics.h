//------------------------------------------------------------------------------
// <copyright file="CoordinateMappingBasics.h" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//------------------------------------------------------------------------------

#pragma once

#include "resource.h"
#include "ImageRenderer.h"

class CCoordinateMappingBasics
{
    static const int        cDepthWidth  = 512;
    static const int        cDepthHeight = 424;
    static const int        cColorWidth  = 1920;
    static const int        cColorHeight = 1080;

public:
    /// <summary>
    /// Constructor
    /// </summary>
    CCoordinateMappingBasics();

    /// <summary>
    /// Destructor
    /// </summary>
    ~CCoordinateMappingBasics();

    /// <summary>
    /// Handles window messages, passes most to the class instance to handle
    /// </summary>
    /// <param name="hWnd">window message is for</param>
    /// <param name="uMsg">message</param>
    /// <param name="wParam">message data</param>
    /// <param name="lParam">additional message data</param>
    /// <returns>result of message processing</returns>
    static LRESULT CALLBACK MessageRouter(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    /// <summary>
    /// Handle windows messages for a class instance
    /// </summary>
    /// <param name="hWnd">window message is for</param>
    /// <param name="uMsg">message</param>
    /// <param name="wParam">message data</param>
    /// <param name="lParam">additional message data</param>
    /// <returns>result of message processing</returns>
    LRESULT CALLBACK        DlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    /// <summary>
    /// Creates the main window and begins processing
    /// </summary>
    /// <param name="hInstance"></param>
    /// <param name="nCmdShow"></param>
    int                     Run(HINSTANCE hInstance, int nCmdShow);

private:
    HWND                    m_hWnd;
    INT64                   m_nStartTime;
    INT64                   m_nLastCounter;
    double                  m_fFreq;
    INT64                   m_nNextStatusTime;
    DWORD                   m_nFramesSinceUpdate;
    bool                    m_bSaveScreenshot;

    // Current Kinect
    IKinectSensor*          m_pKinectSensor;
    ICoordinateMapper*      m_pCoordinateMapper;
    DepthSpacePoint*        m_pDepthCoordinates;

    // Frame reader
    IMultiSourceFrameReader*m_pMultiSourceFrameReader;

    // Direct2D
    ImageRenderer*          m_pDrawCoordinateMapping;
    ID2D1Factory*           m_pD2DFactory;
    RGBQUAD*                m_pOutputRGBX; 
    RGBQUAD*                m_pBackgroundRGBX; 
    RGBQUAD*                m_pColorRGBX;

    /// <summary>
    /// Main processing function
    /// </summary>
    void                    Update();

    /// <summary>
    /// Initializes the default Kinect sensor
    /// </summary>
    /// <returns>S_OK on success, otherwise failure code</returns>
    HRESULT                 InitializeDefaultSensor();

    /// <summary>
    /// Handle new depth and color data
    /// <param name="nTime">timestamp of frame</param>
    /// <param name="pDepthBuffer">pointer to depth frame data</param>
    /// <param name="nDepthWidth">width (in pixels) of input depth image data</param>
    /// <param name="nDepthHeight">height (in pixels) of input depth image data</param>
    /// <param name="pColorBuffer">pointer to color frame data</param>
    /// <param name="nColorWidth">width (in pixels) of input color image data</param>
    /// <param name="nColorHeight">height (in pixels) of input color image data</param>
    /// <param name="pBodyIndexBuffer">pointer to body index frame data</param>
    /// <param name="nBodyIndexWidth">width (in pixels) of input body index data</param>
    /// <param name="nBodyIndexHeight">height (in pixels) of input body index data</param>
    /// </summary>
    void                    ProcessFrame(INT64 nTime, 
                                         const UINT16* pDepthBuffer, int nDepthHeight, int nDepthWidth, 
                                         const RGBQUAD* pColorBuffer, int nColorWidth, int nColorHeight,
                                         const BYTE* pBodyIndexBuffer, int nBodyIndexWidth, int nBodyIndexHeight);

    /// <summary>
    /// Set the status bar message
    /// </summary>
    /// <param name="szMessage">message to display</param>
    /// <param name="nShowTimeMsec">time in milliseconds for which to ignore future status messages</param>
    /// <param name="bForce">force status update</param>
    bool                    SetStatusMessage(_In_z_ WCHAR* szMessage, DWORD nShowTimeMsec, bool bForce);

    /// <summary>
    /// Get the name of the file where screenshot will be stored.
    /// </summary>
    /// <param name="lpszFilePath">string buffer that will receive screenshot file name.</param>
    /// <param name="nFilePathSize">number of characters in lpszFilePath string buffer.</param>
    /// <returns>
    /// S_OK on success, otherwise failure code.
    /// </returns>
    HRESULT                 GetScreenshotFileName(_Out_writes_z_(nFilePathSize) LPWSTR lpszFilePath, UINT nFilePathSize);

    /// <summary>
    /// Save passed in image data to disk as a bitmap
    /// </summary>
    /// <param name="pBitmapBits">image data to save</param>
    /// <param name="lWidth">width (in pixels) of input image data</param>
    /// <param name="lHeight">height (in pixels) of input image data</param>
    /// <param name="wBitsPerPixel">bits per pixel of image data</param>
    /// <param name="lpszFilePath">full file path to output bitmap to</param>
    /// <returns>indicates success or failure</returns>

    HRESULT                 SaveBitmapToFile(BYTE* pBitmapBits, LONG lWidth, LONG lHeight, WORD wBitsPerPixel, LPCWSTR lpszFilePath);
    /// <summary>
    /// Load an image from a resource into a buffer
    /// </summary>
    /// <param name="resourceName">name of image resource to load</param>
    /// <param name="resourceType">type of resource to load</param>
    /// <param name="nOutputWidth">width (in pixels) of scaled output bitmap</param>
    /// <param name="nOutputHeight">height (in pixels) of scaled output bitmap</param>
    /// <param name="pOutputBuffer">buffer that will hold the loaded image</param>
    /// <returns>S_OK on success, otherwise failure code</returns>
    HRESULT                 LoadResourceImage(PCWSTR resourceName, PCWSTR resourceType, UINT nOutputWidth, UINT nOutputHeight, RGBQUAD* pOutputBuffer);
};

