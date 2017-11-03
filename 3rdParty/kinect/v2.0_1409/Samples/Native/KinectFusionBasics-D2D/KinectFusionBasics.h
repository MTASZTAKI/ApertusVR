//------------------------------------------------------------------------------
// <copyright file="KinectFusionBasics.h" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//------------------------------------------------------------------------------

#pragma once

#include "resource.h"
#include <NuiKinectFusionApi.h>
#include "ImageRenderer.h"

// For timing calls
#include "Timer.h"

class CKinectFusionBasics
{
    static const int            cBytesPerPixel = 4; // for depth float and int-per-pixel raycast images
    static const int            cResetOnTimeStampSkippedMilliseconds = 1000;  // ms
    static const int            cResetOnNumberOfLostFrames = 100;
    static const int            cStatusMessageMaxLen = MAX_PATH*2;
    static const int            cTimeDisplayInterval = 10;

public:
    /// <summary>
    /// Constructor
    /// </summary>
    CKinectFusionBasics();

    /// <summary>
    /// Destructor
    /// </summary>
    ~CKinectFusionBasics();

    /// <summary>
    /// Handles window messages, passes most to the class instance to handle
    /// </summary>
    /// <param name="hWnd">window message is for</param>
    /// <param name="uMsg">message</param>
    /// <param name="wParam">message data</param>
    /// <param name="lParam">additional message data</param>
    /// <returns>result of message processing</returns>
    static LRESULT CALLBACK     MessageRouter(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

    /// <summary>
    /// Handle windows messages for a class instance
    /// </summary>
    /// <param name="hWnd">window message is for</param>
    /// <param name="uMsg">message</param>
    /// <param name="wParam">message data</param>
    /// <param name="lParam">additional message data</param>
    /// <returns>result of message processing</returns>
    LRESULT CALLBACK            DlgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

    /// <summary>
    /// Creates the main window and begins processing
    /// </summary>
    /// <param name="hInstance"></param>
    /// <param name="nCmdShow"></param>
    int                         Run(HINSTANCE hInstance, int nCmdShow);

private:
    HWND                        m_hWnd;

    // Current Kinect
    IKinectSensor*              m_pNuiSensor;

    // Depth reader
    IDepthFrameReader*          m_pDepthFrameReader;

    UINT                         m_cDepthWidth;
    UINT                         m_cDepthHeight;
    UINT                         m_cDepthImagePixels;

    INT64                       m_lastFrameTimeStamp;
    bool                        m_bResetReconstruction;

    // Direct2D
    ImageRenderer*              m_pDrawDepth;
    ID2D1Factory*               m_pD2DFactory;
    
    BYTE*                       m_pDepthRGBX;

    /// <summary>
    /// Main processing function
    /// </summary>
    void                        Update();

    /// <summary>
    /// Create the first connected Kinect found 
    /// </summary>
    /// <returns>S_OK on success, otherwise failure code</returns>
    HRESULT                     CreateFirstConnected();

    /// <summary>
    /// Setup or update the Undistortion calculation for the connected camera
    /// </summary>
    HRESULT                     SetupUndistortion();

    /// <summary>
    /// Handle Coordinate Mapping changed event.
    /// Note, this happens after sensor connect, or when Kinect Studio connects
    /// </summary>
    HRESULT                     OnCoordinateMappingChanged();

    /// <summary>
    /// Initialize Kinect Fusion volume and images for processing
    /// </summary>
    /// <returns>S_OK on success, otherwise failure code</returns>
    HRESULT                     InitializeKinectFusion();

    /// <summary>
    /// Handle new depth data
    /// </summary>
    void                        ProcessDepth();

    /// <summary>
    /// Reset the reconstruction camera pose and clear the volume.
    /// </summary>
    /// <returns>S_OK on success, otherwise failure code</returns>
    HRESULT                     ResetReconstruction();

    /// <summary>
    /// Set the status bar message
    /// </summary>
    /// <param name="szMessage">message to display</param>
    void                        SetStatusMessage(WCHAR* szMessage);

    /// <summary>
    /// The Kinect Fusion Reconstruction Volume
    /// </summary>
    INuiFusionReconstruction*   m_pVolume;

    /// <summary>
    /// The Kinect Fusion Volume Parameters
    /// </summary>
    NUI_FUSION_RECONSTRUCTION_PARAMETERS m_reconstructionParams;

    /// <summary>
    // The Kinect Fusion Camera Transform
    /// </summary>
    Matrix4                     m_worldToCameraTransform;

    /// <summary>
    // The default Kinect Fusion World to Volume Transform
    /// </summary>
    Matrix4                     m_defaultWorldToVolumeTransform;

    /// <summary>
    /// Frames from the depth input
    /// </summary>
    UINT16*                     m_pDepthImagePixelBuffer;
    NUI_FUSION_IMAGE_FRAME*     m_pDepthFloatImage;

    /// <summary>
    /// For depth distortion correction
    /// </summary>
    ICoordinateMapper*          m_pMapper;
    DepthSpacePoint*            m_pDepthDistortionMap;
    UINT*                       m_pDepthDistortionLT;
    WAITABLE_HANDLE             m_coordinateMappingChangedEvent;

    /// <summary>
    /// Kinect camera parameters.
    /// </summary>
    NUI_FUSION_CAMERA_PARAMETERS m_cameraParameters;
    bool                        m_bHaveValidCameraParameters;

    /// <summary>
    /// Frames generated from ray-casting the Reconstruction Volume
    /// </summary>
    NUI_FUSION_IMAGE_FRAME*     m_pPointCloud;

    /// <summary>
    /// Images for display
    /// </summary>
    NUI_FUSION_IMAGE_FRAME*     m_pShadedSurface;

    /// <summary>
    /// Camera Tracking parameters
    /// </summary>
    int                         m_cLostFrameCounter;
    bool                        m_bTrackingFailed;

    /// <summary>
    /// Parameter to turn automatic reset of the reconstruction when camera tracking is lost on or off.
    /// Set to true in the constructor to enable auto reset on cResetOnNumberOfLostFrames lost frames,
    /// or set false to never automatically reset.
    /// </summary>
    bool                        m_bAutoResetReconstructionWhenLost;

    /// <summary>
    /// Parameter to enable automatic reset of the reconstruction when there is a large
    /// difference in timestamp between subsequent frames. This should usually be set true as 
    /// default to enable recorded .xef files to generate a reconstruction reset on looping of
    /// the playback or scrubbing, however, for debug purposes, it can be set false to prevent
    /// automatic reset on timeouts.
    /// </summary>
    bool                        m_bAutoResetReconstructionOnTimeout;

    /// <summary>
    /// Processing parameters
    /// </summary>
    int                         m_deviceIndex;
    NUI_FUSION_RECONSTRUCTION_PROCESSOR_TYPE m_processorType;
    bool                        m_bInitializeError;
    float                       m_fMinDepthThreshold;
    float                       m_fMaxDepthThreshold;
    bool                        m_bMirrorDepthFrame;
    unsigned short              m_cMaxIntegrationWeight;
    int                         m_cFrameCounter;
    double                      m_fStartTime;
    Timing::Timer               m_timer;

    /// <summary>
    /// Parameter to translate the reconstruction based on the minimum depth setting. When set to
    /// false, the reconstruction volume +Z axis starts at the camera lens and extends into the scene.
    /// Setting this true in the constructor will move the volume forward along +Z away from the
    /// camera by the minimum depth threshold to enable capture of very small reconstruction volumes
    /// by setting a non-identity camera transformation in the ResetReconstruction call.
    /// Small volumes should be shifted, as the Kinect hardware has a minimum sensing limit of ~0.35m,
    /// inside which no valid depth is returned, hence it is difficult to initialize and track robustly  
    /// when the majority of a small volume is inside this distance.
    /// </summary>
    bool                        m_bTranslateResetPoseByMinDepthThreshold;
};
