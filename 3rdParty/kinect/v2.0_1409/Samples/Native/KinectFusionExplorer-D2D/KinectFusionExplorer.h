//------------------------------------------------------------------------------
// <copyright file="KinectFusionExplorer.h" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//------------------------------------------------------------------------------

#pragma once

#include "ImageRenderer.h"
#include "KinectFusionParams.h"
#include "KinectFusionProcessor.h"

/// <summary>
/// KinectFusionExplorer sample.
/// </summary>
class CKinectFusionExplorer
{
    static const ULONGLONG          cStatusTimeoutInMilliseconds = 5000;

public:
    /// <summary>
    /// Constructor
    /// </summary>
    CKinectFusionExplorer();

    /// <summary>
    /// Destructor
    /// </summary>
    ~CKinectFusionExplorer();

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

    /// <summary>
    /// Direct2D
    /// </summary>
    ImageRenderer*              m_pDrawReconstruction;
    ImageRenderer*              m_pDrawTrackingResiduals;
    ImageRenderer*              m_pDrawDepth;
    ID2D1Factory*               m_pD2DFactory;

    /// <summary>
    /// Main processing function
    /// </summary>
    void                        Update();

    /// <summary>
    /// Save a mesh
    /// </summary>
    /// <returns>S_OK on success, otherwise failure code</returns>
    HRESULT                     SaveMeshFile(INuiFusionColorMesh *mesh, KinectFusionMeshTypes saveMeshType);

    /// <summary>
    /// Handle a completed frame from the Kinect Fusion processor.
    /// </summary>
    void                        HandleCompletedFrame();

    /// <summary>
    /// Initialize the UI controls
    /// </summary>
    void                        InitializeUIControls();

    /// <summary>
    /// Handle new UI interaction
    /// </summary>
    void                        ProcessUI(WPARAM wParam, LPARAM lParam);

    /// <summary>
    /// Update the internal variable values from the UI Horizontal sliders.
    /// </summary>
    void                        UpdateHSliders();

    /// <summary>
    /// Set the status bar message
    /// </summary>
    /// <param name="szMessage">message to display</param>
    void                        SetStatusMessage(const WCHAR* szMessage);

    /// <summary>
    /// Set the frames-per-second message
    /// </summary>
    /// <param name="fFramesPerSecond">current frame rate</param>
    void                        SetFramesPerSecond(float fFramesPerSecond);

    /// <summary>
    /// Set the index of the GPU processor device initialized, or -1 for CPU
    /// </summary>
    /// <param name="gpuIndex">The index of the initialized GPU processor device, or -1 fo CPU.</param>
    void                        SetDeviceIndexInitialized(int deviceIndex);

    /// <summary>
    /// The reconstruction parameters passed to the processor
    /// </summary>
    KinectFusionParams          m_params;

    /// <summary>
    /// The reconstruction processor
    /// </summary>
    KinectFusionProcessor       m_processor;
    bool                        m_bUIUpdated;

    bool                        m_bInitializeError;
    bool                        m_bSavingMesh;
    KinectFusionMeshTypes       m_saveMeshFormat;
    bool                        m_bColorCaptured;

    /// <summary>
    /// Most recently reported frame rate
    /// </summary>
    float                       m_fFramesPerSecond;

    /// <summary>
    /// Time since last status message update
    /// </summary>
    ULONGLONG                   m_tickLastStatus;
};
