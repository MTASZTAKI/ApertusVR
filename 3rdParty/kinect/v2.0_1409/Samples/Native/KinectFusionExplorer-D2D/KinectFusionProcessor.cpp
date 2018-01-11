//------------------------------------------------------------------------------
// <copyright file="KinectFusionProcessor.cpp" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//------------------------------------------------------------------------------

// System includes
#include "stdafx.h"

#pragma warning(push)
#pragma warning(disable:6255)
#pragma warning(disable:6263)
#pragma warning(disable:4995)
#include "ppl.h"
#pragma warning(pop)

// Project includes
#include "KinectFusionProcessor.h"
#include "KinectFusionHelper.h"
#include "resource.h"

#define AssertOwnThread() \
    _ASSERT_EXPR(GetCurrentThreadId() == m_threadId, __FUNCTIONW__ L" called on wrong thread!");

#define AssertOtherThread() \
    _ASSERT_EXPR(GetCurrentThreadId() != m_threadId, __FUNCTIONW__ L" called on wrong thread!");

HRESULT KinectFusionProcessor::CopyDepth(
    IDepthFrame* pDepthFrame
    )
{
        // Check the frame pointer
        if (NULL == pDepthFrame)
        {
            return E_INVALIDARG;
        }

        UINT nBufferSize = 0;
        UINT16 *pBuffer = NULL;

        HRESULT hr = pDepthFrame->AccessUnderlyingBuffer(&nBufferSize, &pBuffer);
        if (FAILED(hr))
        {
            return hr;
        }

        //copy and remap depth
        const UINT bufferLength =  NUI_DEPTH_RAW_HEIGHT * NUI_DEPTH_RAW_WIDTH;
        UINT16 * pDepth = m_pDepthUndistortedPixelBuffer;
        UINT16 * pRawDepth = m_pDepthRawPixelBuffer;
        for(UINT i = 0; i < bufferLength; i++, pDepth++, pRawDepth++)
        {
            const UINT id = m_pDepthDistortionLT[i];
            *pDepth = id < bufferLength? pBuffer[id] : 0;
            *pRawDepth = pBuffer[i];
        }

        return S_OK;
}

/// <summary>
/// Constructor
/// </summary>
KinectFusionProcessor::KinectFusionProcessor() :
    m_hWnd(nullptr),
    m_msgFrameReady(WM_NULL),
    m_msgUpdateSensorStatus(WM_NULL),
    m_hThread(nullptr),
    m_threadId(0),
    m_pVolume(nullptr),
    m_hrRecreateVolume(S_OK),
    m_pNuiSensor(nullptr),
    m_cLostFrameCounter(0),
    m_bTrackingFailed(false),
    m_cFrameCounter(0),
    m_cFPSFrameCounter(0),
    m_fFrameCounterStartTime(0),
    m_cLastDepthFrameTimeStamp(0),
    m_cLastColorFrameTimeStamp(0),
    m_fMostRecentRaycastTime(0),
    m_pDepthUndistortedPixelBuffer(nullptr),
    m_pDepthRawPixelBuffer(nullptr),
    m_pColorCoordinates(nullptr),
    m_pDepthVisibilityTestMap(nullptr),
    m_pDepthDistortionMap(nullptr),
    m_pDepthDistortionLT(nullptr),
    m_pMapper(nullptr),
    m_pDepthFloatImage(nullptr),
    m_pColorImage(nullptr),
    m_pResampledColorImage(nullptr),
    m_pResampledColorImageDepthAligned(nullptr),
    m_pSmoothDepthFloatImage(nullptr),
    m_pDepthPointCloud(nullptr),
    m_pRaycastPointCloud(nullptr),
    m_pRaycastDepthFloatImage(nullptr),
    m_pShadedSurface(nullptr),
    m_pShadedSurfaceNormals(nullptr),
    m_pCapturedSurfaceColor(nullptr),
    m_pFloatDeltaFromReference(nullptr),
    m_pShadedDeltaFromReference(nullptr),
    m_bKinectFusionInitialized(false),
    m_bResetReconstruction(false),
    m_bResolveSensorConflict(false),
    m_hStopProcessingEvent(INVALID_HANDLE_VALUE),
    m_pCameraPoseFinder(nullptr),
    m_bTrackingHasFailedPreviously(false),
    m_pDownsampledDepthFloatImage(nullptr),
    m_pDownsampledSmoothDepthFloatImage(nullptr),
    m_pDownsampledDepthPointCloud(nullptr),
    m_pDownsampledShadedDeltaFromReference(nullptr),
    m_pDownsampledRaycastPointCloud(nullptr),
    m_bCalculateDeltaFrame(false),
    m_coordinateMappingChangedEvent(NULL),
    m_bHaveValidCameraParameters(false)
{
    // Initialize synchronization objects
    InitializeCriticalSection(&m_lockParams);
    InitializeCriticalSection(&m_lockFrame);
    InitializeCriticalSection(&m_lockVolume);

    m_hStopProcessingEvent = CreateEvent(
        nullptr,
        TRUE, /* bManualReset */ 
        FALSE, /* bInitialState */
        nullptr
        );

    SetIdentityMatrix(m_worldToCameraTransform);
    SetIdentityMatrix(m_defaultWorldToVolumeTransform);

    // We don't know these at object creation time, so we use nominal values.
    // These will later be updated in response to the CoordinateMappingChanged event.
    m_cameraParameters.focalLengthX = NUI_KINECT_DEPTH_NORM_FOCAL_LENGTH_X;
    m_cameraParameters.focalLengthY = NUI_KINECT_DEPTH_NORM_FOCAL_LENGTH_Y;
    m_cameraParameters.principalPointX = NUI_KINECT_DEPTH_NORM_PRINCIPAL_POINT_X;
    m_cameraParameters.principalPointY = NUI_KINECT_DEPTH_NORM_PRINCIPAL_POINT_Y;
}

/// <summary>
/// Destructor
/// </summary>
KinectFusionProcessor::~KinectFusionProcessor()
{
    AssertOtherThread();

    // Shutdown the sensor
    StopProcessing();

    // Clean up Kinect Fusion
    SafeRelease(m_pVolume);

    if (nullptr != m_pMapper)
        m_pMapper->UnsubscribeCoordinateMappingChanged(m_coordinateMappingChangedEvent);

    SafeRelease(m_pMapper);

    // Clean up Kinect Fusion Camera Pose Finder
    SafeRelease(m_pCameraPoseFinder);

    SAFE_FUSION_RELEASE_IMAGE_FRAME(m_pDepthFloatImage);
    SAFE_FUSION_RELEASE_IMAGE_FRAME(m_pColorImage);
    SAFE_FUSION_RELEASE_IMAGE_FRAME(m_pResampledColorImage);
    SAFE_FUSION_RELEASE_IMAGE_FRAME(m_pResampledColorImageDepthAligned);
    SAFE_FUSION_RELEASE_IMAGE_FRAME(m_pRaycastPointCloud);
    SAFE_FUSION_RELEASE_IMAGE_FRAME(m_pRaycastDepthFloatImage);
    SAFE_FUSION_RELEASE_IMAGE_FRAME(m_pShadedSurface);
    SAFE_FUSION_RELEASE_IMAGE_FRAME(m_pShadedSurfaceNormals);
    SAFE_FUSION_RELEASE_IMAGE_FRAME(m_pCapturedSurfaceColor);
    SAFE_FUSION_RELEASE_IMAGE_FRAME(m_pFloatDeltaFromReference);
    SAFE_FUSION_RELEASE_IMAGE_FRAME(m_pShadedDeltaFromReference);
    SAFE_FUSION_RELEASE_IMAGE_FRAME(m_pSmoothDepthFloatImage);
    SAFE_FUSION_RELEASE_IMAGE_FRAME(m_pDepthPointCloud);
    SAFE_FUSION_RELEASE_IMAGE_FRAME(m_pDownsampledDepthFloatImage);
    SAFE_FUSION_RELEASE_IMAGE_FRAME(m_pDownsampledSmoothDepthFloatImage);
    SAFE_FUSION_RELEASE_IMAGE_FRAME(m_pDownsampledDepthPointCloud);
    SAFE_FUSION_RELEASE_IMAGE_FRAME(m_pDownsampledShadedDeltaFromReference);

    // Clean up the depth pixel array
    SAFE_DELETE_ARRAY(m_pDepthUndistortedPixelBuffer);
    SAFE_DELETE_ARRAY(m_pDepthRawPixelBuffer);

    // Clean up the color coordinate array
    SAFE_DELETE_ARRAY(m_pColorCoordinates);
    SAFE_DELETE_ARRAY(m_pDepthVisibilityTestMap);
    
    SAFE_DELETE_ARRAY(m_pDepthDistortionMap);
    SAFE_DELETE_ARRAY(m_pDepthDistortionLT);

    // Clean up synchronization object
    if (m_hStopProcessingEvent != INVALID_HANDLE_VALUE)
    {
        CloseHandle(m_hStopProcessingEvent);
    }

    // done with depth frame reader
    SafeRelease(m_pDepthFrameReader);
    SafeRelease(m_pColorFrameReader);


    DeleteCriticalSection(&m_lockParams);
    DeleteCriticalSection(&m_lockFrame);
    DeleteCriticalSection(&m_lockVolume);
}

/// <summary>
/// Shuts down the sensor
/// </summary>
void KinectFusionProcessor::ShutdownSensor()
{
    AssertOwnThread();

    // Clean up Kinect
    if (m_pNuiSensor != nullptr)
    {
        m_pNuiSensor->Close();
        SafeRelease(m_pNuiSensor);
    }
}

/// <summary>
/// Starts Kinect Fusion processing.
/// </summary>
/// <param name="phThread">returns the new processing thread's handle</param>
HRESULT KinectFusionProcessor::StartProcessing()
{
    AssertOtherThread();

    if (m_hThread == nullptr)
    {
        m_hThread = CreateThread(nullptr, 0, ThreadProc, this, 0, &m_threadId);
    }

    return (m_hThread != nullptr) ? S_OK : HRESULT_FROM_WIN32(GetLastError());
}

/// <summary>
/// Stops Kinect Fusion processing.
/// </summary>
HRESULT KinectFusionProcessor::StopProcessing()
{
    AssertOtherThread();

    if (m_hThread != nullptr)
    {
        SetEvent(m_hStopProcessingEvent);

        WaitForSingleObject(m_hThread, INFINITE);
        m_hThread = nullptr;
    }

    return S_OK;
}

/// <summary>
/// Thread procedure
/// </summary>
DWORD WINAPI KinectFusionProcessor::ThreadProc(LPVOID lpParameter)
{
    return reinterpret_cast<KinectFusionProcessor*>(lpParameter)->MainLoop();
}

/// <summary>
/// Is reconstruction volume initialized
/// </summary>
bool KinectFusionProcessor::IsVolumeInitialized()
{
    AssertOtherThread();

    return nullptr != m_pVolume;
}

/// <summary>
/// Is the camera pose finder initialized and running.
/// </summary>
bool KinectFusionProcessor::IsCameraPoseFinderAvailable()
{
    return m_paramsCurrent.m_bAutoFindCameraPoseWhenLost 
        && nullptr != m_pCameraPoseFinder 
        && m_pCameraPoseFinder->GetStoredPoseCount() > 0;
}

/// <summary>
/// Main processing function
/// </summary>
DWORD KinectFusionProcessor::MainLoop()
{
    AssertOwnThread();

    // Bring in the first set of parameters
    EnterCriticalSection(&m_lockParams);
    m_paramsCurrent = m_paramsNext;
    LeaveCriticalSection(&m_lockParams);


    // Propagate any updates to the gpu index in use
    m_paramsNext.m_deviceIndex = m_paramsCurrent.m_deviceIndex;

    // Get and initialize the default Kinect sensor
    InitializeDefaultSensor();

    bool bStopProcessing = false;

    // Main loop
    while (!bStopProcessing)
    {
        if (m_coordinateMappingChangedEvent != NULL &&
            WAIT_OBJECT_0 == WaitForSingleObject((HANDLE)m_coordinateMappingChangedEvent, 0))
        {
            OnCoordinateMappingChanged();
            ResetEvent((HANDLE)m_coordinateMappingChangedEvent);
        }

        HANDLE handles[] = { m_hStopProcessingEvent };
        DWORD waitResult = WaitForMultipleObjects(ARRAYSIZE(handles), handles, FALSE, 0);

        // Get parameters and other external signals

        EnterCriticalSection(&m_lockParams);
        bool bRecreateVolume = m_paramsCurrent.VolumeChanged(m_paramsNext);
        bool bResetReconstruction = m_bResetReconstruction;
        m_bResetReconstruction = false;
        m_bResolveSensorConflict = false;

        m_paramsCurrent = m_paramsNext;
        LeaveCriticalSection(&m_lockParams);

        if (waitResult == WAIT_OBJECT_0)
        {
            bStopProcessing = true;
            break;
        }
        else
        {
            if (m_bKinectFusionInitialized && m_bHaveValidCameraParameters)
            {
                // Clear status message from previous frame
                SetStatusMessage(L"");

                EnterCriticalSection(&m_lockVolume);

                if (nullptr == m_pVolume && !FAILED(m_hrRecreateVolume))
                {
                    m_hrRecreateVolume = RecreateVolume();

                    // Set an introductory message on success
                    if (SUCCEEDED(m_hrRecreateVolume))
                    {
                        SetStatusMessage(L"Click ‘Reset Reconstruction' to clear!");
                    }
                }
                else if (bRecreateVolume)
                {
                    m_hrRecreateVolume = RecreateVolume();
                }
                else if (bResetReconstruction)
                {
                    HRESULT hr = InternalResetReconstruction();

                    if (SUCCEEDED(hr))
                    {
                        SetStatusMessage(L"Reconstruction has been reset.");
                    }
                    else
                    {
                        SetStatusMessage(L"Failed to reset reconstruction.");
                    }
                }

                bool processSucceed = ProcessDepth();

                LeaveCriticalSection(&m_lockVolume);

                if (processSucceed)
                {
                    NotifyFrameReady();
                }
            }
        }

        if (m_pNuiSensor == nullptr)
        {
            // We have no sensor: Set frame rate to zero and notify the UI
            NotifyEmptyFrame();
        }
    }

    ShutdownSensor();

    return 0;
}

/// <summary>
/// Initializes the default Kinect sensor
/// </summary>
/// <returns>indicates success or failure</returns>
HRESULT KinectFusionProcessor::InitializeDefaultSensor()
{
    HRESULT hr;

    hr = GetDefaultKinectSensor(&m_pNuiSensor);
    if (FAILED(hr))
    {
        return hr;
    }

    if (m_pNuiSensor)
    {
        // Initialize the Kinect and get the depth reader
        IDepthFrameSource* pDepthFrameSource = NULL;
        IColorFrameSource* pColorFrameSource = NULL;

        hr = m_pNuiSensor->Open();

        if (SUCCEEDED(hr))
        {
            hr = m_pNuiSensor->get_DepthFrameSource(&pDepthFrameSource);
        }

        if (SUCCEEDED(hr))
        {
            hr = pDepthFrameSource->OpenReader(&m_pDepthFrameReader);
        }

        if (SUCCEEDED(hr))
        {
            hr = m_pNuiSensor->get_CoordinateMapper(&m_pMapper);
        }

        if (SUCCEEDED(hr))
        {
            hr = m_pMapper->SubscribeCoordinateMappingChanged(&m_coordinateMappingChangedEvent);
        }

        if (SUCCEEDED(hr))
        {
            hr = m_pNuiSensor->get_ColorFrameSource(&pColorFrameSource);
        }

        if (SUCCEEDED(hr))
        {
            hr = pColorFrameSource->OpenReader(&m_pColorFrameReader);
        }

        if (SUCCEEDED(InitializeKinectFusion()))
        {
            m_bKinectFusionInitialized = true;
        }

        SafeRelease(pDepthFrameSource);
        SafeRelease(pColorFrameSource);
    }

    if (!m_pNuiSensor || FAILED(hr))
    {
        SetStatusMessage(L"No ready Kinect found!");
        return E_FAIL;
    }

    return hr;
}

///////////////////////////////////////////////////////////////////////////////////////////

/// <summary>
/// Sets the UI window handle.
/// </summary>
/// <returns>S_OK on success, otherwise failure code</returns>
HRESULT KinectFusionProcessor::SetWindow(HWND hWnd, UINT msgFrameReady, UINT msgUpdateSensorStatus)
{
    AssertOtherThread();

    m_hWnd = hWnd;
    m_msgFrameReady = msgFrameReady;
    m_msgUpdateSensorStatus = msgUpdateSensorStatus;
    return S_OK;
}

/// <summary>
/// Sets the parameters.
/// </summary>
/// <returns>S_OK on success, otherwise failure code</returns>
HRESULT KinectFusionProcessor::SetParams(const KinectFusionParams& params)
{
    AssertOtherThread();

    EnterCriticalSection(&m_lockParams);
    m_paramsNext = params;
    LeaveCriticalSection(&m_lockParams);
    return S_OK;
}

/// <summary>
/// Lock the current frame while rendering it to the screen.
/// </summary>
/// <returns>S_OK on success, otherwise failure code</returns>
HRESULT KinectFusionProcessor::LockFrame(KinectFusionProcessorFrame const** ppFrame)
{
    AssertOtherThread();

    EnterCriticalSection(&m_lockFrame);
    *ppFrame = &m_frame;

    return S_OK;
}

/// <summary>
/// Unlock the previously locked frame.
/// </summary>
/// <returns>S_OK on success, otherwise failure code</returns>
HRESULT KinectFusionProcessor::UnlockFrame()
{
    AssertOtherThread();

    LeaveCriticalSection(&m_lockFrame);

    return S_OK;
}

void UpdateIntrinsics(NUI_FUSION_IMAGE_FRAME * pImageFrame, NUI_FUSION_CAMERA_PARAMETERS * params)
{
    if (pImageFrame != nullptr && pImageFrame->pCameraParameters != nullptr && params != nullptr)
    {
        pImageFrame->pCameraParameters->focalLengthX = params->focalLengthX;
        pImageFrame->pCameraParameters->focalLengthY = params->focalLengthY;
        pImageFrame->pCameraParameters->principalPointX = params->principalPointX;
        pImageFrame->pCameraParameters->principalPointY = params->principalPointY;
    }

    // Confirm we are called correctly
    _ASSERT(pImageFrame != nullptr && pImageFrame->pCameraParameters != nullptr && params != nullptr);
}

HRESULT KinectFusionProcessor::SetupUndistortion()
{
    AssertOwnThread();

    HRESULT hr = E_UNEXPECTED;

    if (m_cameraParameters.principalPointX != 0)
    {

        const UINT width = m_paramsCurrent.m_cDepthWidth;
        const UINT height = m_paramsCurrent.m_cDepthHeight;
        const UINT depthBufferSize = width * height;

        CameraSpacePoint cameraFrameCorners[4] = //at 1 meter distance. Take into account that depth frame is mirrored
        {
            /*LT*/ { -m_cameraParameters.principalPointX / m_cameraParameters.focalLengthX, m_cameraParameters.principalPointY / m_cameraParameters.focalLengthY, 1.f }, 
            /*RT*/ { (1.f - m_cameraParameters.principalPointX) / m_cameraParameters.focalLengthX, m_cameraParameters.principalPointY / m_cameraParameters.focalLengthY, 1.f }, 
            /*LB*/ { -m_cameraParameters.principalPointX / m_cameraParameters.focalLengthX, (m_cameraParameters.principalPointY - 1.f) / m_cameraParameters.focalLengthY, 1.f }, 
            /*RB*/ { (1.f - m_cameraParameters.principalPointX) / m_cameraParameters.focalLengthX, (m_cameraParameters.principalPointY - 1.f) / m_cameraParameters.focalLengthY, 1.f }
        };

        for(UINT rowID = 0; rowID < height; rowID++)
        {
            const float rowFactor = float(rowID) / float(height - 1);
            const CameraSpacePoint rowStart = 
            {
                cameraFrameCorners[0].X + (cameraFrameCorners[2].X - cameraFrameCorners[0].X) * rowFactor,
                cameraFrameCorners[0].Y + (cameraFrameCorners[2].Y - cameraFrameCorners[0].Y) * rowFactor,
                1.f
            };

            const CameraSpacePoint rowEnd = 
            {
                cameraFrameCorners[1].X + (cameraFrameCorners[3].X - cameraFrameCorners[1].X) * rowFactor,
                cameraFrameCorners[1].Y + (cameraFrameCorners[3].Y - cameraFrameCorners[1].Y) * rowFactor,
                1.f
            };

            const float stepFactor = 1.f / float(width - 1);
            const CameraSpacePoint rowDelta = 
            {
                (rowEnd.X - rowStart.X) * stepFactor,
                (rowEnd.Y - rowStart.Y) * stepFactor,
                0
            };

            _ASSERT(width == NUI_DEPTH_RAW_WIDTH);
            CameraSpacePoint cameraCoordsRow[NUI_DEPTH_RAW_WIDTH];

            CameraSpacePoint currentPoint = rowStart;
            for(UINT i = 0; i < width; i++)
            {
                cameraCoordsRow[i] = currentPoint;
                currentPoint.X += rowDelta.X;
                currentPoint.Y += rowDelta.Y;
            }

            hr = m_pMapper->MapCameraPointsToDepthSpace(width, cameraCoordsRow, width, &m_pDepthDistortionMap[rowID * width]);
            if(FAILED(hr))
            {
                SetStatusMessage(L"Failed to initialize Kinect Coordinate Mapper.");
                return hr;
            }
        }

        if (nullptr == m_pDepthDistortionLT)
        {
            SetStatusMessage(L"Failed to initialize Kinect Fusion depth image distortion Lookup Table.");
            return E_OUTOFMEMORY;
        }

        UINT* pLT = m_pDepthDistortionLT;
        for(UINT i = 0; i < depthBufferSize; i++, pLT++)
        {
            //nearest neighbor depth lookup table 
            UINT x = UINT(m_pDepthDistortionMap[i].X + 0.5f);
            UINT y = UINT(m_pDepthDistortionMap[i].Y + 0.5f);

            *pLT = (x < width && y < height)? x + y * width : UINT_MAX; 
        } 
        m_bHaveValidCameraParameters = true;
    }
    else
    {
        m_bHaveValidCameraParameters = false;
    }
    return S_OK;
}
///////////////////////////////////////////////////////////////////////////////////////////
/// <summary>
/// Initialize Kinect Fusion volume and images for processing
/// </summary>
/// <returns>S_OK on success, otherwise failure code</returns>
HRESULT KinectFusionProcessor::OnCoordinateMappingChanged()
{
    AssertOwnThread();

    HRESULT hr = E_UNEXPECTED;

    // Calculate the down sampled image sizes, which are used for the AlignPointClouds calculation frames
    CameraIntrinsics intrinsics = {};

    m_pMapper->GetDepthCameraIntrinsics(&intrinsics);

    float focalLengthX = intrinsics.FocalLengthX / NUI_DEPTH_RAW_WIDTH;
    float focalLengthY = intrinsics.FocalLengthY / NUI_DEPTH_RAW_HEIGHT;
    float principalPointX = intrinsics.PrincipalPointX / NUI_DEPTH_RAW_WIDTH;
    float principalPointY = intrinsics.PrincipalPointY / NUI_DEPTH_RAW_HEIGHT;

    if (m_cameraParameters.focalLengthX == focalLengthX && m_cameraParameters.focalLengthY == focalLengthY &&
        m_cameraParameters.principalPointX == principalPointX && m_cameraParameters.principalPointY == principalPointY)
        return S_OK; 

    m_cameraParameters.focalLengthX = focalLengthX;
    m_cameraParameters.focalLengthY = focalLengthY;
    m_cameraParameters.principalPointX = principalPointX;
    m_cameraParameters.principalPointY = principalPointY;

    _ASSERT(m_cameraParameters.focalLengthX != 0);

    UpdateIntrinsics(m_pDepthFloatImage, &m_cameraParameters);
    UpdateIntrinsics(m_pDownsampledDepthFloatImage, &m_cameraParameters);
    UpdateIntrinsics(m_pColorImage, &m_cameraParameters);
    UpdateIntrinsics(m_pResampledColorImage, &m_cameraParameters);
    UpdateIntrinsics(m_pResampledColorImageDepthAligned, &m_cameraParameters);
    UpdateIntrinsics(m_pRaycastPointCloud, &m_cameraParameters);
    UpdateIntrinsics(m_pDownsampledRaycastPointCloud, &m_cameraParameters);
    UpdateIntrinsics(m_pRaycastDepthFloatImage, &m_cameraParameters);
    UpdateIntrinsics(m_pShadedSurface, &m_cameraParameters);
    UpdateIntrinsics(m_pShadedSurfaceNormals, &m_cameraParameters);
    UpdateIntrinsics(m_pCapturedSurfaceColor, &m_cameraParameters);
    UpdateIntrinsics(m_pFloatDeltaFromReference, &m_cameraParameters);
    UpdateIntrinsics(m_pShadedDeltaFromReference, &m_cameraParameters);
    UpdateIntrinsics(m_pDownsampledShadedDeltaFromReference, &m_cameraParameters);
    UpdateIntrinsics(m_pSmoothDepthFloatImage, &m_cameraParameters);
    UpdateIntrinsics(m_pDownsampledSmoothDepthFloatImage, &m_cameraParameters);
    UpdateIntrinsics(m_pDepthPointCloud, &m_cameraParameters);
    UpdateIntrinsics(m_pDownsampledDepthPointCloud, &m_cameraParameters);

    if (nullptr == m_pDepthUndistortedPixelBuffer)
    {
        SetStatusMessage(L"Failed to initialize Kinect Fusion depth image pixel buffer.");
        return E_OUTOFMEMORY;
    }

    if (nullptr == m_pDepthRawPixelBuffer)
    {
        SetStatusMessage(L"Failed to initialize Kinect Fusion raw depth image pixel buffer.");
        return E_OUTOFMEMORY;
    }
    
    if (nullptr == m_pColorCoordinates)
    {
    SetStatusMessage(L"Failed to initialize Kinect Fusion color image coordinate buffer.");
    return E_OUTOFMEMORY;
    }

    if (nullptr == m_pDepthVisibilityTestMap)
    {
        SetStatusMessage(L"Failed to initialize Kinect Fusion depth points visibility test buffer.");
        return E_OUTOFMEMORY;
    }

    if (nullptr == m_pDepthDistortionMap)
    {
        SetStatusMessage(L"Failed to initialize Kinect Fusion depth image distortion buffer.");
        return E_OUTOFMEMORY;
    }

    hr = SetupUndistortion();
    return hr;
}


///////////////////////////////////////////////////////////////////////////////////////////
/// <summary>
/// Initialize Kinect Fusion volume and images for processing
/// </summary>
/// <returns>S_OK on success, otherwise failure code</returns>
HRESULT KinectFusionProcessor::InitializeKinectFusion()
{
    AssertOwnThread();

    HRESULT hr = E_UNEXPECTED;

    hr = m_frame.Initialize(m_paramsCurrent.m_cDepthImagePixels);
    if (FAILED(hr))
    {
        SetStatusMessage(L"Failed to allocate frame buffers.");
        return hr;
    }

    // Check to ensure suitable DirectX11 compatible hardware exists before initializing Kinect Fusion
    WCHAR description[MAX_PATH];
    WCHAR instancePath[MAX_PATH];

    if (FAILED(hr = NuiFusionGetDeviceInfo(
        m_paramsCurrent.m_processorType, 
        m_paramsCurrent.m_deviceIndex, 
        &description[0], 
        ARRAYSIZE(description), 
        &instancePath[0],
        ARRAYSIZE(instancePath), 
        &m_frame.m_deviceMemory)))
    {
        if (hr ==  E_NUI_BADINDEX)
        {
            // This error code is returned either when the device index is out of range for the processor 
            // type or there is no DirectX11 capable device installed. As we set -1 (auto-select default) 
            // for the device index in the parameters, this indicates that there is no DirectX11 capable 
            // device. The options for users in this case are to either install a DirectX11 capable device
            // (see documentation for recommended GPUs) or to switch to non-real-time CPU based 
            // reconstruction by changing the processor type to NUI_FUSION_RECONSTRUCTION_PROCESSOR_TYPE_CPU.
            SetStatusMessage(L"No DirectX11 device detected, or invalid device index - Kinect Fusion requires a DirectX11 device for GPU-based reconstruction.");
        }
        else
        {
            SetStatusMessage(L"Failed in call to NuiFusionGetDeviceInfo.");
        }
        return hr;
    }

    const UINT width = m_paramsCurrent.m_cDepthWidth;
    const UINT height = m_paramsCurrent.m_cDepthHeight;
    const UINT depthBufferSize = width * height;

    const UINT colorWidth = m_paramsCurrent.m_cColorWidth;
    const UINT colorHeight = m_paramsCurrent.m_cColorHeight;

    // Calculate the down sampled image sizes, which are used for the AlignPointClouds calculation frames
    const UINT downsampledWidth = width / m_paramsCurrent.m_cAlignPointCloudsImageDownsampleFactor;
    const UINT downsampledHeight = height / m_paramsCurrent.m_cAlignPointCloudsImageDownsampleFactor;

    // Frame generated from the depth input
    if (FAILED(hr = CreateFrame(NUI_FUSION_IMAGE_TYPE_FLOAT, width, height, &m_pDepthFloatImage)))
    {
        return hr;
    }

    // Frames generated from the depth input
    if (FAILED(hr = CreateFrame(NUI_FUSION_IMAGE_TYPE_FLOAT, downsampledWidth, downsampledHeight, &m_pDownsampledDepthFloatImage)))
    {
        return hr;
    }

    // Frame generated from the raw color input of Kinect
    if (FAILED(hr = CreateFrame(NUI_FUSION_IMAGE_TYPE_COLOR, colorWidth, colorHeight, &m_pColorImage)))
    {
        return hr;
    }

    // Frame generated from the raw color input of Kinect for use in the camera pose finder.
    // Note color will be down-sampled to the depth size if depth and color capture resolutions differ.
    if (FAILED(hr = CreateFrame(NUI_FUSION_IMAGE_TYPE_COLOR, width, height, &m_pResampledColorImage)))
    {
        return hr;
    }

    // Frame re-sampled from the color input of Kinect, aligned to depth - this will be the same size as the depth.
    if (FAILED(hr = CreateFrame(NUI_FUSION_IMAGE_TYPE_COLOR, width, height, &m_pResampledColorImageDepthAligned)))
    {
        return hr;
    }

    // Point Cloud generated from ray-casting the volume
    if (FAILED(hr = CreateFrame(NUI_FUSION_IMAGE_TYPE_POINT_CLOUD, width, height, &m_pRaycastPointCloud)))
    {
        return hr;
    }

    // Point Cloud generated from ray-casting the volume
    if (FAILED(hr = CreateFrame(NUI_FUSION_IMAGE_TYPE_POINT_CLOUD, downsampledWidth, downsampledHeight, &m_pDownsampledRaycastPointCloud)))
    {
        return hr;
    }

    // Depth frame generated from ray-casting the volume
    if (FAILED(hr = CreateFrame(NUI_FUSION_IMAGE_TYPE_FLOAT, width, height, &m_pRaycastDepthFloatImage)))
    {
        return hr;
    }

    // Image of the raycast Volume to display
    if (FAILED(hr = CreateFrame(NUI_FUSION_IMAGE_TYPE_COLOR, width, height, &m_pShadedSurface)))
    {
        return hr;
    }

    // Image of the raycast Volume with surface normals to display
    if (FAILED(hr = CreateFrame(NUI_FUSION_IMAGE_TYPE_COLOR, width, height,  &m_pShadedSurfaceNormals)))
    {
        return hr;
    }

    // Image of the raycast Volume with the captured color to display
    if (FAILED(hr = CreateFrame(NUI_FUSION_IMAGE_TYPE_COLOR, width, height, &m_pCapturedSurfaceColor)))
    {
        return hr;
    }

    // Image of the camera tracking deltas to display
    if (FAILED(hr = CreateFrame(NUI_FUSION_IMAGE_TYPE_FLOAT, width, height, &m_pFloatDeltaFromReference)))
    {
        return hr;
    }

    // Image of the camera tracking deltas to display
    if (FAILED(hr = CreateFrame(NUI_FUSION_IMAGE_TYPE_COLOR, width, height, &m_pShadedDeltaFromReference)))
    {
        return hr;
    }

    // Image of the camera tracking deltas to display
    if (FAILED(hr = CreateFrame(NUI_FUSION_IMAGE_TYPE_COLOR, downsampledWidth, downsampledHeight, &m_pDownsampledShadedDeltaFromReference)))
    {
        return hr;
    }

    // Image from input depth for use with AlignPointClouds call
    if (FAILED(hr = CreateFrame(NUI_FUSION_IMAGE_TYPE_FLOAT, width, height, &m_pSmoothDepthFloatImage)))
    {
        return hr;
    }

    // Frames generated from smoothing the depth input
    if (FAILED(hr = CreateFrame(NUI_FUSION_IMAGE_TYPE_FLOAT, downsampledWidth, downsampledHeight, &m_pDownsampledSmoothDepthFloatImage)))
    {
        return hr;
    }

    // Image used in post pose finding success check AlignPointClouds call
    if (FAILED(hr = CreateFrame(NUI_FUSION_IMAGE_TYPE_POINT_CLOUD, width, height, &m_pDepthPointCloud)))
    {
        return hr;
    }

    // Point Cloud generated from depth input, in local camera coordinate system
    if (FAILED(hr = CreateFrame(NUI_FUSION_IMAGE_TYPE_POINT_CLOUD, downsampledWidth, downsampledHeight, &m_pDownsampledDepthPointCloud)))
    {
        return hr;
    }

    SAFE_DELETE_ARRAY(m_pDepthUndistortedPixelBuffer);
    m_pDepthUndistortedPixelBuffer = new(std::nothrow) UINT16[depthBufferSize];

    if (nullptr == m_pDepthUndistortedPixelBuffer)
    {
        SetStatusMessage(L"Failed to initialize Kinect Fusion depth image pixel buffer.");
        return E_OUTOFMEMORY;
    }

    SAFE_DELETE_ARRAY(m_pDepthRawPixelBuffer);
    m_pDepthRawPixelBuffer = new(std::nothrow) UINT16[depthBufferSize];

    if (nullptr == m_pDepthRawPixelBuffer)
    {
        SetStatusMessage(L"Failed to initialize Kinect Fusion raw depth image pixel buffer.");
        return E_OUTOFMEMORY;
    }
    
    // Color coordinate array to capture data from Kinect sensor and for color to depth mapping
    // Note: this must be the same size as the depth
    SAFE_DELETE_ARRAY(m_pColorCoordinates);
    m_pColorCoordinates = new(std::nothrow) ColorSpacePoint[depthBufferSize];

    if (nullptr == m_pColorCoordinates)
    {
    SetStatusMessage(L"Failed to initialize Kinect Fusion color image coordinate buffer.");
    return E_OUTOFMEMORY;
    }

    SAFE_DELETE_ARRAY(m_pDepthVisibilityTestMap);
    m_pDepthVisibilityTestMap = new(std::nothrow) UINT16[(colorWidth >> cVisibilityTestQuantShift) * (colorHeight >> cVisibilityTestQuantShift)]; 

    if (nullptr == m_pDepthVisibilityTestMap)
    {
        SetStatusMessage(L"Failed to initialize Kinect Fusion depth points visibility test buffer.");
        return E_OUTOFMEMORY;
    }

    SAFE_DELETE_ARRAY(m_pDepthDistortionMap);
    m_pDepthDistortionMap = new(std::nothrow) DepthSpacePoint[depthBufferSize];

    if (nullptr == m_pDepthDistortionMap)
    {
        SetStatusMessage(L"Failed to initialize Kinect Fusion depth image distortion buffer.");
        return E_OUTOFMEMORY;
    }

    SAFE_DELETE_ARRAY(m_pDepthDistortionLT);
    m_pDepthDistortionLT = new(std::nothrow) UINT[depthBufferSize];

    if (nullptr == m_pDepthDistortionLT)
    {
        SetStatusMessage(L"Failed to initialize Kinect Fusion depth image distortion Lookup Table.");
        return E_OUTOFMEMORY;
    }

    // If we have valid parameters, let's go ahead and use them.
    if (m_cameraParameters.focalLengthX != 0)
    {
        SetupUndistortion();
    }

    SafeRelease(m_pCameraPoseFinder);

    // Create the camera pose finder if necessary
        NUI_FUSION_CAMERA_POSE_FINDER_PARAMETERS cameraPoseFinderParameters;

        cameraPoseFinderParameters.featureSampleLocationsPerFrameCount = m_paramsCurrent.m_cCameraPoseFinderFeatureSampleLocationsPerFrame;
        cameraPoseFinderParameters.maxPoseHistoryCount = m_paramsCurrent.m_cMaxCameraPoseFinderPoseHistory;
        cameraPoseFinderParameters.maxDepthThreshold = m_paramsCurrent.m_fMaxCameraPoseFinderDepthThreshold;

        if (FAILED(hr = NuiFusionCreateCameraPoseFinder(
            &cameraPoseFinderParameters,
            nullptr,
            &m_pCameraPoseFinder)))
        {
            return hr;
        }
    
    m_bKinectFusionInitialized = true;
    return hr;
}

HRESULT KinectFusionProcessor::CreateFrame(
    NUI_FUSION_IMAGE_TYPE frameType,
    unsigned int imageWidth,
    unsigned int imageHeight,
    NUI_FUSION_IMAGE_FRAME** ppImageFrame)
{
    HRESULT hr = S_OK;

    if (nullptr != *ppImageFrame)
    {
        // If image size or type has changed, release the old one.
        if ((*ppImageFrame)->width != imageWidth ||
            (*ppImageFrame)->height != imageHeight ||
            (*ppImageFrame)->imageType != frameType)
        {
            static_cast<void>(NuiFusionReleaseImageFrame(*ppImageFrame));
            *ppImageFrame = nullptr;
        }
    }

    // Create a new frame as needed.
    if (nullptr == *ppImageFrame)
    {
        hr = NuiFusionCreateImageFrame(
            frameType,
            imageWidth,
            imageHeight,
            &m_cameraParameters,
            ppImageFrame);

        if (FAILED(hr))
        {
            SetStatusMessage(L"Failed to initialize Kinect Fusion image.");
        }
    }

    return hr;
}

/// <summary>
/// Release and re-create a Kinect Fusion Reconstruction Volume
/// </summary>
/// <returns>S_OK on success, otherwise failure code</returns>
HRESULT KinectFusionProcessor::RecreateVolume()
{
    AssertOwnThread();

    HRESULT hr = S_OK;

    // Clean up Kinect Fusion
    SafeRelease(m_pVolume);

    SetIdentityMatrix(m_worldToCameraTransform);

    // Create the Kinect Fusion Reconstruction Volume
    // Here we create a color volume, enabling optional color processing in the Integrate, ProcessFrame and CalculatePointCloud calls
    hr = NuiFusionCreateColorReconstruction(
        &m_paramsCurrent.m_reconstructionParams,
        m_paramsCurrent.m_processorType,
        m_paramsCurrent.m_deviceIndex,
        &m_worldToCameraTransform,
        &m_pVolume);

    if (FAILED(hr))
    {
        if (E_NUI_GPU_FAIL == hr)
        {
            WCHAR buf[MAX_PATH];
            swprintf_s(buf, ARRAYSIZE(buf), L"Device %d not able to run Kinect Fusion, or error initializing.", m_paramsCurrent.m_deviceIndex);
            SetStatusMessage(buf);
        }
        else if (E_NUI_GPU_OUTOFMEMORY == hr)
        {
            WCHAR buf[MAX_PATH];
            swprintf_s(buf, ARRAYSIZE(buf), L"Device %d out of memory error initializing reconstruction - try a smaller reconstruction volume.", m_paramsCurrent.m_deviceIndex);
            SetStatusMessage(buf);
        }
        else if (NUI_FUSION_RECONSTRUCTION_PROCESSOR_TYPE_CPU != m_paramsCurrent.m_processorType)
        {
            WCHAR buf[MAX_PATH];
            swprintf_s(buf, ARRAYSIZE(buf), L"Failed to initialize Kinect Fusion reconstruction volume on device %d.", m_paramsCurrent.m_deviceIndex);
            SetStatusMessage(buf);
        }
        else
        {
            WCHAR buf[MAX_PATH];
            swprintf_s(buf, ARRAYSIZE(buf), L"Failed to initialize Kinect Fusion reconstruction volume on CPU %d.", m_paramsCurrent.m_deviceIndex);
            SetStatusMessage(buf);
        }

        return hr;
    }
    else
    {
        // Save the default world to volume transformation to be optionally used in ResetReconstruction
        hr = m_pVolume->GetCurrentWorldToVolumeTransform(&m_defaultWorldToVolumeTransform);
        if (FAILED(hr))
        {
            SetStatusMessage(L"Failed in call to GetCurrentWorldToVolumeTransform.");
            return hr;
        }

        if (m_paramsCurrent.m_bTranslateResetPoseByMinDepthThreshold)
        {
            // This call will set the world-volume transformation
            hr = InternalResetReconstruction();
            if (FAILED(hr))
            {
                return hr;
            }
        }
        else
        {
            // Reset pause and signal that the integration resumed
            ResetTracking();
        }

        // Map X axis to blue channel, Y axis to green channel and Z axis to red channel,
        // normalizing each to the range [0, 1].
        SetIdentityMatrix(m_worldToBGRTransform);
        m_worldToBGRTransform.M11 = m_paramsCurrent.m_reconstructionParams.voxelsPerMeter / m_paramsCurrent.m_reconstructionParams.voxelCountX;
        m_worldToBGRTransform.M22 = m_paramsCurrent.m_reconstructionParams.voxelsPerMeter / m_paramsCurrent.m_reconstructionParams.voxelCountY;
        m_worldToBGRTransform.M33 = m_paramsCurrent.m_reconstructionParams.voxelsPerMeter / m_paramsCurrent.m_reconstructionParams.voxelCountZ;
        m_worldToBGRTransform.M41 = 0.5f;
        m_worldToBGRTransform.M42 = 0.5f;
        m_worldToBGRTransform.M44 = 1.0f;

        SetStatusMessage(L"Reconstruction has been reset.");
    }

    return hr;
}

/// <summary>
/// Get Color data
/// </summary>
/// <param name="imageFrame">The color image frame to copy.</param>
/// <returns>S_OK on success, otherwise failure code</returns>
HRESULT KinectFusionProcessor::CopyColor(IColorFrame* pColorFrame)
{
    HRESULT hr = S_OK;

    if (nullptr == m_pColorImage)
    {
        SetStatusMessage(L"Error copying color texture pixels.");
        return E_FAIL;
    }

    NUI_FUSION_BUFFER *destColorBuffer = m_pColorImage->pFrameBuffer;

    if (nullptr == pColorFrame || nullptr == destColorBuffer)
    {
        return E_NOINTERFACE;
    }

    // Copy the color pixels so we can return the image frame
    hr = pColorFrame->CopyConvertedFrameDataToArray(cColorWidth * cColorHeight * sizeof(RGBQUAD), destColorBuffer->pBits, ColorImageFormat_Bgra);

    if (FAILED(hr))
    {
        SetStatusMessage(L"Error copying color texture pixels.");
        hr = E_FAIL;
    }

    return hr;
}

/// <summary>
/// Adjust color to the same space as depth
/// </summary>
/// <returns>S_OK for success, or failure code</returns>
HRESULT KinectFusionProcessor::MapColorToDepth()
{
    HRESULT hr = S_OK;

    if (nullptr == m_pColorImage || nullptr == m_pResampledColorImageDepthAligned 
        || nullptr == m_pColorCoordinates || nullptr == m_pDepthVisibilityTestMap)
    {
        return E_FAIL;
    }

    NUI_FUSION_BUFFER *srcColorBuffer = m_pColorImage->pFrameBuffer;
    NUI_FUSION_BUFFER *destColorBuffer = m_pResampledColorImageDepthAligned->pFrameBuffer;

    if (nullptr == srcColorBuffer || nullptr == destColorBuffer)
    {
        SetStatusMessage(L"Error accessing color textures.");
        return E_NOINTERFACE;
    }

    if (FAILED(hr) || srcColorBuffer->Pitch == 0)
    {
        SetStatusMessage(L"Error accessing color texture pixels.");
        return  E_FAIL;
    }

    if (FAILED(hr) || destColorBuffer->Pitch == 0)
    {
        SetStatusMessage(L"Error accessing color texture pixels.");
        return  E_FAIL;
    }

    int *rawColorData = reinterpret_cast<int*>(srcColorBuffer->pBits);
    int *colorDataInDepthFrame = reinterpret_cast<int*>(destColorBuffer->pBits);

    // Get the coordinates to convert color to depth space
    hr = m_pMapper->MapDepthFrameToColorSpace(NUI_DEPTH_RAW_WIDTH * NUI_DEPTH_RAW_HEIGHT, m_pDepthRawPixelBuffer, 
        NUI_DEPTH_RAW_WIDTH * NUI_DEPTH_RAW_HEIGHT, m_pColorCoordinates);

    if (FAILED(hr))
    {
        return hr;
    }

    // construct dense depth points visibility test map so we can test for depth points that are invisible in color space
    const UINT16* const pDepthEnd = m_pDepthRawPixelBuffer + NUI_DEPTH_RAW_WIDTH * NUI_DEPTH_RAW_HEIGHT;
    const ColorSpacePoint* pColorPoint = m_pColorCoordinates;
    const UINT testMapWidth = UINT(cColorWidth >> cVisibilityTestQuantShift);
    const UINT testMapHeight = UINT(cColorHeight >> cVisibilityTestQuantShift);
    ZeroMemory(m_pDepthVisibilityTestMap, testMapWidth * testMapHeight * sizeof(UINT16));
    for(const UINT16* pDepth = m_pDepthRawPixelBuffer; pDepth < pDepthEnd; pDepth++, pColorPoint++)
    {
        const UINT x = UINT(pColorPoint->X + 0.5f) >> cVisibilityTestQuantShift;
        const UINT y = UINT(pColorPoint->Y + 0.5f) >> cVisibilityTestQuantShift;
        if(x < testMapWidth && y < testMapHeight)
        {
            const UINT idx = y * testMapWidth + x;
            const UINT16 oldDepth = m_pDepthVisibilityTestMap[idx];
            const UINT16 newDepth = *pDepth;
            if(!oldDepth || oldDepth > newDepth)
            {
                m_pDepthVisibilityTestMap[idx] = newDepth;
            }
        }
    }


    // Loop over each row and column of the destination color image and copy from the source image
    // Note that we could also do this the other way, and convert the depth pixels into the color space, 
    // avoiding black areas in the converted color image and repeated color images in the background
    // However, then the depth would have radial and tangential distortion like the color camera image,
    // which is not ideal for Kinect Fusion reconstruction.

    if (m_paramsCurrent.m_bMirrorDepthFrame)
    {
        Concurrency::parallel_for(0u, m_paramsCurrent.m_cDepthHeight, [&](UINT y)
        {
            const UINT depthWidth = m_paramsCurrent.m_cDepthWidth;
            const UINT depthImagePixels = m_paramsCurrent.m_cDepthImagePixels;
            const UINT colorHeight = m_paramsCurrent.m_cColorHeight;
            const UINT colorWidth = m_paramsCurrent.m_cColorWidth;
            const UINT testMapWidth = UINT(colorWidth >> cVisibilityTestQuantShift);

            UINT destIndex = y * depthWidth;
            for (UINT x = 0; x < depthWidth; ++x, ++destIndex)
            {
                int pixelColor = 0;
                const UINT mappedIndex = m_pDepthDistortionLT[destIndex];
                if(mappedIndex < depthImagePixels)
                {
                    // retrieve the depth to color mapping for the current depth pixel
                    const ColorSpacePoint colorPoint = m_pColorCoordinates[mappedIndex];

                    // make sure the depth pixel maps to a valid point in color space
                    const UINT colorX = (UINT)(colorPoint.X + 0.5f);
                    const UINT colorY = (UINT)(colorPoint.Y + 0.5f);
                    if (colorX < colorWidth && colorY < colorHeight)
                    {
                        const UINT16 depthValue = m_pDepthRawPixelBuffer[mappedIndex];
                        const UINT testX = colorX >> cVisibilityTestQuantShift;
                        const UINT testY = colorY >> cVisibilityTestQuantShift;
                        const UINT testIdx = testY * testMapWidth + testX;
                        const UINT16 depthTestValue = m_pDepthVisibilityTestMap[testIdx];
                        _ASSERT(depthValue >= depthTestValue);
                        if(depthValue - depthTestValue < cDepthVisibilityTestThreshold)
                        {
                            // calculate index into color array
                            const UINT colorIndex = colorX + (colorY * colorWidth);
                            pixelColor = rawColorData[colorIndex];
                        }
                    }
                }
                colorDataInDepthFrame[destIndex] = pixelColor;
            }
        });
    }
    else
    {
        Concurrency::parallel_for(0u, m_paramsCurrent.m_cDepthHeight, [&](UINT y)
        {
            const UINT depthWidth = m_paramsCurrent.m_cDepthWidth;
            const UINT depthImagePixels = m_paramsCurrent.m_cDepthImagePixels;
            const UINT colorHeight = m_paramsCurrent.m_cColorHeight;
            const UINT colorWidth = m_paramsCurrent.m_cColorWidth;
            const UINT testMapWidth = UINT(colorWidth >> cVisibilityTestQuantShift);

            // Horizontal flip the color image as the standard depth image is flipped internally in Kinect Fusion
            // to give a viewpoint as though from behind the Kinect looking forward by default.
            UINT destIndex = y * depthWidth;
            UINT flipIndex = destIndex + depthWidth - 1;
            for (UINT x = 0; x < depthWidth; ++x, ++destIndex, --flipIndex)
            {
                int pixelColor = 0;
                const UINT mappedIndex = m_pDepthDistortionLT[destIndex];
                if(mappedIndex < depthImagePixels)
                {
                    // retrieve the depth to color mapping for the current depth pixel
                    const ColorSpacePoint colorPoint = m_pColorCoordinates[mappedIndex];

                    // make sure the depth pixel maps to a valid point in color space
                    const UINT colorX = (UINT)(colorPoint.X + 0.5f);
                    const UINT colorY = (UINT)(colorPoint.Y + 0.5f);
                    if (colorX < colorWidth && colorY < colorHeight)
                    {
                        const UINT16 depthValue = m_pDepthRawPixelBuffer[mappedIndex];
                        const UINT testX = colorX >> cVisibilityTestQuantShift;
                        const UINT testY = colorY >> cVisibilityTestQuantShift;
                        const UINT testIdx = testY * testMapWidth + testX;
                        const UINT16 depthTestValue = m_pDepthVisibilityTestMap[testIdx];
                        _ASSERT(depthValue >= depthTestValue);
                        if(depthValue - depthTestValue < cDepthVisibilityTestThreshold)
                        {
                            // calculate index into color array
                            const UINT colorIndex = colorX + (colorY * colorWidth);
                            pixelColor = rawColorData[colorIndex];
                        }
                    }
                }
                colorDataInDepthFrame[flipIndex] = pixelColor;
            }
        });
    }

    return hr;
}

/// <summary>
/// Get the next frames from Kinect, re-synchronizing depth with color if required.
/// </summary>
/// <returns>S_OK on success, otherwise failure code</returns>
HRESULT KinectFusionProcessor::GetKinectFrames(bool &colorSynchronized)
{
    HRESULT hr = S_OK;
    INT64 currentDepthFrameTime = 0;
    INT64 currentColorFrameTime = 0;
    colorSynchronized = false;   // assume we are not synchronized to start with

    ////////////////////////////////////////////////////////
    // Get an extended depth frame from Kinect

    IDepthFrame* pDepthFrame = NULL;

    hr = m_pDepthFrameReader->AcquireLatestFrame(&pDepthFrame);

    if (FAILED(hr))
    {
        SafeRelease(pDepthFrame);
        SetStatusMessage(L"Kinect depth stream get frame call failed.");
        return hr;
    }

    hr = CopyDepth(pDepthFrame);
    pDepthFrame->get_RelativeTime(&currentDepthFrameTime);
    currentDepthFrameTime /= 10000;

    SafeRelease(pDepthFrame);

    ////////////////////////////////////////////////////////
    // Get a color frame from Kinect

    if(m_paramsCurrent.m_bCaptureColor)
    {
        currentColorFrameTime = m_cLastColorFrameTimeStamp;

        IColorFrame* pColorFrame;
        hr = m_pColorFrameReader->AcquireLatestFrame(&pColorFrame);

        if (FAILED(hr))
        {
            // Here we just do not integrate color rather than reporting an error
            colorSynchronized = false;
        }
        else
        {
            if (SUCCEEDED(hr))
            {
                CopyColor(pColorFrame);
            }

            if (SUCCEEDED(hr))
            {
                hr = pColorFrame->get_RelativeTime(&currentColorFrameTime);
                currentColorFrameTime /= 10000;
            }

            SafeRelease(pColorFrame);
        }

        // Check color and depth frame timestamps to ensure they were captured at the same time
        // If not, we attempt to re-synchronize by getting a new frame from the stream that is behind.
        int timestampDiff = static_cast<int>(abs(currentColorFrameTime - currentDepthFrameTime));

        if ((timestampDiff >= cMinTimestampDifferenceForFrameReSync) && m_cSuccessfulFrameCounter > 0 && (m_paramsCurrent.m_bAutoFindCameraPoseWhenLost || m_paramsCurrent.m_bCaptureColor))
        {
            colorSynchronized = false;
        }
        else
        {
            colorSynchronized = true;
        }
    }
    ////////////////////////////////////////////////////////
    // To enable playback of a .xef file through Kinect Studio and reset of the reconstruction
    // if the .xef loops, we test for when the frame timestamp has skipped a large number. 
    // Note: this will potentially continually reset live reconstructions on slow machines which
    // cannot process a live frame in less time than the reset threshold. Increase the number of
    // milliseconds in cResetOnTimeStampSkippedMilliseconds if this is a problem.

    int cResetOnTimeStampSkippedMilliseconds = cResetOnTimeStampSkippedMillisecondsGPU;

    if (m_paramsCurrent.m_processorType == NUI_FUSION_RECONSTRUCTION_PROCESSOR_TYPE_CPU)
    {
        cResetOnTimeStampSkippedMilliseconds = cResetOnTimeStampSkippedMillisecondsCPU;
    }

    if (m_paramsCurrent.m_bAutoResetReconstructionOnTimeout && m_cFrameCounter != 0 && nullptr != m_pVolume 
        && abs(currentDepthFrameTime - m_cLastDepthFrameTimeStamp) > cResetOnTimeStampSkippedMilliseconds)
    {
        hr = InternalResetReconstruction();

        if (SUCCEEDED(hr))
        {
            SetStatusMessage(L"Reconstruction has been reset.");
        }
        else
        {
            SetStatusMessage(L"Failed to reset reconstruction.");
        }
    }

    m_cLastDepthFrameTimeStamp = currentDepthFrameTime;
    m_cLastColorFrameTimeStamp = currentColorFrameTime;

    return hr;
}

/// <summary>
/// Perform camera tracking using AlignPointClouds
/// </summary>
HRESULT KinectFusionProcessor::TrackCameraAlignPointClouds(Matrix4 &calculatedCameraPose, FLOAT &alignmentEnergy)
{
    UNREFERENCED_PARAMETER(alignmentEnergy);

    ////////////////////////////////////////////////////////
    // Down sample the depth image

    HRESULT hr = DownsampleFrameNearestNeighbor(
        m_pDepthFloatImage, 
        m_pDownsampledDepthFloatImage,
        m_paramsCurrent.m_cAlignPointCloudsImageDownsampleFactor);

    if (FAILED(hr))
    {
        SetStatusMessage(L"Kinect Fusion DownsampleFrameNearestNeighbor call failed.");
        return hr;
    }

    ////////////////////////////////////////////////////////
    // Smooth depth image

    hr = m_pVolume->SmoothDepthFloatFrame(
        m_pDownsampledDepthFloatImage, 
        m_pDownsampledSmoothDepthFloatImage, 
        m_paramsCurrent.m_cSmoothingKernelWidth, 
        m_paramsCurrent.m_fSmoothingDistanceThreshold);

    if (FAILED(hr))
    {
        SetStatusMessage(L"Kinect Fusion SmoothDepth call failed.");
        return hr;
    }

    ////////////////////////////////////////////////////////
    // Calculate Point Cloud from smoothed input Depth Image

    hr = NuiFusionDepthFloatFrameToPointCloud(
        m_pDownsampledSmoothDepthFloatImage,
        m_pDownsampledDepthPointCloud);

    if (FAILED(hr))
    {
        SetStatusMessage(L"Kinect Fusion NuiFusionDepthFloatFrameToPointCloud call failed.");
        return hr;
    }

    ////////////////////////////////////////////////////////
    // CalculatePointCloud

    // Raycast even if camera tracking failed, to enable us to visualize what is 
    // happening with the system
    hr = m_pVolume->CalculatePointCloud(
        m_pDownsampledRaycastPointCloud,
        nullptr, 
        &calculatedCameraPose);

    if (FAILED(hr))
    {
        SetStatusMessage(L"Kinect Fusion CalculatePointCloud call failed.");
        return hr;
    }

    ////////////////////////////////////////////////////////
    // Call AlignPointClouds

    HRESULT tracking = S_OK;

    // Only calculate the residual delta from reference frame every m_cDeltaFromReferenceFrameCalculationInterval
    // frames to reduce computation time
    if (m_bCalculateDeltaFrame)
    {
        tracking = NuiFusionAlignPointClouds(
            m_pDownsampledRaycastPointCloud,
            m_pDownsampledDepthPointCloud,
            NUI_FUSION_DEFAULT_ALIGN_ITERATION_COUNT,
            m_pDownsampledShadedDeltaFromReference,
            &calculatedCameraPose); 

        // Up sample the delta from reference image to display as the original resolution
        hr = UpsampleFrameNearestNeighbor(
            m_pDownsampledShadedDeltaFromReference, 
            m_pShadedDeltaFromReference, 
            m_paramsCurrent.m_cAlignPointCloudsImageDownsampleFactor);

        if (FAILED(hr))
        {
            SetStatusMessage(L"Kinect Fusion UpsampleFrameNearestNeighbor call failed.");
            return hr;
        }
    }
    else
    {
        tracking = NuiFusionAlignPointClouds(
            m_pDownsampledRaycastPointCloud,
            m_pDownsampledDepthPointCloud,
            NUI_FUSION_DEFAULT_ALIGN_ITERATION_COUNT,
            NULL,
            &calculatedCameraPose); 
    }

    if (!FAILED(tracking))
    {
        // Perform additional transform magnitude check
        // Camera Tracking has converged but did we get a sensible pose estimate?
        // see if relative rotation and translation exceed thresholds 
        if (CameraTransformFailed(
            m_worldToCameraTransform,
            calculatedCameraPose,
            m_paramsCurrent.m_fMaxTranslationDelta,
            m_paramsCurrent.m_fMaxRotationDelta))
        {
            // We calculated too large a move for this to be a sensible estimate,
            // quite possibly the camera tracking drifted. Force camera pose finding.
            hr = E_NUI_FUSION_TRACKING_ERROR;

            SetStatusMessage(
                L"Kinect Fusion AlignPointClouds camera tracking failed "
                L"in transform magnitude check!");
        }
    }
    else
    {
        hr = tracking;
    }

    return hr;
}

/// <summary>
/// Perform camera tracking using AlignDepthFloatToReconstruction
/// </summary>
/// <returns>S_OK on success, otherwise failure code</returns>
HRESULT KinectFusionProcessor::TrackCameraAlignDepthFloatToReconstruction(Matrix4 &calculatedCameraPose, FLOAT &alignmentEnergy)
{
    HRESULT hr = S_OK;

    // Only calculate the residual delta from reference frame every m_cDeltaFromReferenceFrameCalculationInterval
    // frames to reduce computation time
    HRESULT tracking = S_OK;

    if (m_bCalculateDeltaFrame)
    {
        tracking = m_pVolume->AlignDepthFloatToReconstruction(
            m_pDepthFloatImage,
            NUI_FUSION_DEFAULT_ALIGN_ITERATION_COUNT,
            m_pFloatDeltaFromReference,
            &alignmentEnergy,
            &calculatedCameraPose);
    }
    else
    {
        tracking = m_pVolume->AlignDepthFloatToReconstruction(
            m_pDepthFloatImage,
            NUI_FUSION_DEFAULT_ALIGN_ITERATION_COUNT,
            nullptr,
            &alignmentEnergy,
            &calculatedCameraPose);
    }

    bool trackingSuccess = !(FAILED(tracking) || alignmentEnergy > m_paramsCurrent.m_fMaxAlignToReconstructionEnergyForSuccess || (alignmentEnergy == 0.0f && m_cSuccessfulFrameCounter > 1));

    if (trackingSuccess)
    {
        // Get the camera pose
        m_pVolume->GetCurrentWorldToCameraTransform(&calculatedCameraPose);
    }
    else
    {
        if (FAILED(tracking))
        {
            hr = tracking;
        }
        else
        {
            // We failed in the energy check
            hr = E_NUI_FUSION_TRACKING_ERROR;
        }
    }

    return hr;
}

/// <summary>
/// Handle new depth data and perform Kinect Fusion processing
/// </summary>
bool KinectFusionProcessor::ProcessDepth()
{
    AssertOwnThread();

    HRESULT hr = S_OK;
    bool depthAvailable = false;
    bool raycastFrame = false;
    bool cameraPoseFinderAvailable = IsCameraPoseFinderAvailable();
    bool integrateColor = m_paramsCurrent.m_bCaptureColor && ((m_cFrameCounter % m_paramsCurrent.m_cColorIntegrationInterval) == 0);
    bool colorSynchronized = false;
    FLOAT alignmentEnergy = 1.0f;
    Matrix4 calculatedCameraPose = m_worldToCameraTransform;
    m_bCalculateDeltaFrame = (m_cFrameCounter % m_paramsCurrent.m_cDeltaFromReferenceFrameCalculationInterval == 0) 
        || (m_bTrackingHasFailedPreviously && m_cSuccessfulFrameCounter <= 2);

    // Get the next frames from Kinect
    hr = GetKinectFrames(colorSynchronized);

    if (FAILED(hr))
    {
        return false;
    }

    // Only integrate when color is synchronized with depth
    //integrateColor = integrateColor && colorSynchronized;

    ////////////////////////////////////////////////////////
    // Depth to Depth Float

    // Convert the pixels describing extended depth as unsigned short type in millimeters to depth
    // as floating point type in meters.
    if (nullptr == m_pVolume)
    {
        hr =  NuiFusionDepthToDepthFloatFrame(
            m_pDepthUndistortedPixelBuffer,
            m_paramsCurrent.m_cDepthWidth,
            m_paramsCurrent.m_cDepthHeight,
            m_pDepthFloatImage,
            m_paramsCurrent.m_fMinDepthThreshold,
            m_paramsCurrent.m_fMaxDepthThreshold,
            m_paramsCurrent.m_bMirrorDepthFrame);
    }
    else
    {
        hr = m_pVolume->DepthToDepthFloatFrame(
            m_pDepthUndistortedPixelBuffer,
            m_paramsCurrent.m_cDepthImagePixels * sizeof(UINT16),
            m_pDepthFloatImage,
            m_paramsCurrent.m_fMinDepthThreshold,
            m_paramsCurrent.m_fMaxDepthThreshold,
            m_paramsCurrent.m_bMirrorDepthFrame);
    }

    if (FAILED(hr))
    {
        SetStatusMessage(L"Kinect Fusion NuiFusionDepthToDepthFloatFrame call failed.");
        goto FinishFrame;
    }

    depthAvailable = true;

    // Return if the volume is not initialized, just drawing the depth image
    if (nullptr == m_pVolume)
    {
        SetStatusMessage(
            L"Kinect Fusion reconstruction volume not initialized. "
            L"Please try reducing volume size or restarting.");
        goto FinishFrame;
    }

    ////////////////////////////////////////////////////////
    // Perform Camera Tracking

    HRESULT tracking = E_NUI_FUSION_TRACKING_ERROR;

    if (0 != m_cFrameCounter)
    {
        // Here we can either call or TrackCameraAlignDepthFloatToReconstruction or TrackCameraAlignPointClouds
        // The TrackCameraAlignPointClouds function typically has higher performance with the camera pose finder 
        // due to its wider basin of convergence, enabling it to more robustly regain tracking from nearby poses
        // suggested by the camera pose finder after tracking is lost.
        if (m_paramsCurrent.m_bAutoFindCameraPoseWhenLost)
        {
            tracking = TrackCameraAlignPointClouds(calculatedCameraPose, alignmentEnergy);
        }
        else
        {
            // If the camera pose finder is not turned on, we use AlignDepthFloatToReconstruction
            tracking = TrackCameraAlignDepthFloatToReconstruction(calculatedCameraPose, alignmentEnergy);
        }
    }

    if (FAILED(tracking) && 0 != m_cFrameCounter)   // frame 0 always succeeds
    {
        SetTrackingFailed();

        if (!cameraPoseFinderAvailable)
        {
            if (tracking == E_NUI_FUSION_TRACKING_ERROR)
            {
                WCHAR str[MAX_PATH];
                swprintf_s(str, ARRAYSIZE(str), L"Kinect Fusion camera tracking FAILED! Align the camera to the last tracked position.");
                SetStatusMessage(str);
            }
            else
            {
                SetStatusMessage(L"Kinect Fusion camera tracking call failed!");
                goto FinishFrame;
            }
        }
        else
        {
            // Here we try to find the correct camera pose, to re-localize camera tracking.
            // We can call either the version using AlignDepthFloatToReconstruction or the version 
            // using AlignPointClouds, which typically has a higher success rate with the camera pose finder.
            //tracking = FindCameraPoseAlignDepthFloatToReconstruction();
            tracking = FindCameraPoseAlignPointClouds();

            if (FAILED(tracking) && tracking != E_NUI_FUSION_TRACKING_ERROR)
            {
                SetStatusMessage(L"Kinect Fusion FindCameraPose call failed.");
                goto FinishFrame;
            }
        }
    }
    else
    {
        if (m_bTrackingHasFailedPreviously)
        {
            WCHAR str[MAX_PATH];
            if (!m_paramsCurrent.m_bAutoFindCameraPoseWhenLost)
            {
                swprintf_s(str, ARRAYSIZE(str), L"Kinect Fusion camera tracking RECOVERED! Residual energy=%f", alignmentEnergy);
            }
            else
            {
                swprintf_s(str, ARRAYSIZE(str), L"Kinect Fusion camera tracking RECOVERED!");
            }
            SetStatusMessage(str);
        }

        m_worldToCameraTransform = calculatedCameraPose;
        SetTrackingSucceeded();
    }

    if (m_paramsCurrent.m_bAutoResetReconstructionWhenLost &&
        m_bTrackingFailed &&
        m_cLostFrameCounter >= cResetOnNumberOfLostFrames)
    {
        // Automatically Clear Volume and reset tracking if tracking fails
        hr = InternalResetReconstruction();

        if (SUCCEEDED(hr))
        {
            // Set bad tracking message
            SetStatusMessage(
                L"Kinect Fusion camera tracking failed, "
                L"automatically reset volume.");
        }
        else
        {
            SetStatusMessage(L"Kinect Fusion Reset Reconstruction call failed.");
            goto FinishFrame;
        }
    }

    ////////////////////////////////////////////////////////
    // Integrate Depth Data into volume

    // Don't integrate depth data into the volume if:
    // 1) tracking failed
    // 2) camera pose finder is off and we have paused capture
    // 3) camera pose finder is on and we are still under the m_cMinSuccessfulTrackingFramesForCameraPoseFinderAfterFailure
    //    number of successful frames count.
    bool integrateData = !m_bTrackingFailed && !m_paramsCurrent.m_bPauseIntegration && 
        (!cameraPoseFinderAvailable || (cameraPoseFinderAvailable && !(m_bTrackingHasFailedPreviously && m_cSuccessfulFrameCounter < m_paramsCurrent.m_cMinSuccessfulTrackingFramesForCameraPoseFinderAfterFailure)));

    if (integrateData)
    {
        // Reset this flag as we are now integrating data again
        m_bTrackingHasFailedPreviously = false;

        if (integrateColor)
        {
            // Map the color frame to the depth - this fills m_pResampledColorImageDepthAligned
            MapColorToDepth();

            // Integrate the depth and color data into the volume from the calculated camera pose
            hr = m_pVolume->IntegrateFrame(
                m_pDepthFloatImage,
                m_pResampledColorImageDepthAligned,
                m_paramsCurrent.m_cMaxIntegrationWeight,
                NUI_FUSION_DEFAULT_COLOR_INTEGRATION_OF_ALL_ANGLES,
                &m_worldToCameraTransform);

            m_frame.m_bColorCaptured = true;
        }
        else
        {
            // Integrate just the depth data into the volume from the calculated camera pose
            hr = m_pVolume->IntegrateFrame(
                m_pDepthFloatImage,
                nullptr,
                m_paramsCurrent.m_cMaxIntegrationWeight,
                NUI_FUSION_DEFAULT_COLOR_INTEGRATION_OF_ALL_ANGLES,
                &m_worldToCameraTransform);
        }

        if (FAILED(hr))
        {
            SetStatusMessage(L"Kinect Fusion IntegrateFrame call failed.");
            goto FinishFrame;
        }
    }

    ////////////////////////////////////////////////////////
    // Check to see if we have time to raycast

    {
        double currentTime = m_timer.AbsoluteTime();

        raycastFrame = true;

        if (raycastFrame)
        {
            m_fMostRecentRaycastTime = currentTime;
        }
    }

    if (raycastFrame)
    {
        ////////////////////////////////////////////////////////
        // CalculatePointCloud

        // Raycast even if camera tracking failed, to enable us to visualize what is 
        // happening with the system
        hr = m_pVolume->CalculatePointCloud(
            m_pRaycastPointCloud,
            (m_paramsCurrent.m_bCaptureColor ? m_pCapturedSurfaceColor : nullptr), 
            &m_worldToCameraTransform);

        if (FAILED(hr))
        {
            SetStatusMessage(L"Kinect Fusion CalculatePointCloud call failed.");
            goto FinishFrame;
        }

        ////////////////////////////////////////////////////////
        // ShadePointCloud

        if (!m_paramsCurrent.m_bCaptureColor)
        {
            hr = NuiFusionShadePointCloud(
                m_pRaycastPointCloud,
                &m_worldToCameraTransform,
                &m_worldToBGRTransform,
                m_pShadedSurface,
                m_paramsCurrent.m_bDisplaySurfaceNormals ?  m_pShadedSurfaceNormals : nullptr);

            if (FAILED(hr))
            {
                SetStatusMessage(L"Kinect Fusion NuiFusionShadePointCloud call failed.");
                goto FinishFrame;
            }
        }
    }

    ////////////////////////////////////////////////////////
    // Update camera pose finder, adding key frames to the database

    if (m_paramsCurrent.m_bAutoFindCameraPoseWhenLost && !m_bTrackingHasFailedPreviously
        && m_cSuccessfulFrameCounter > m_paramsCurrent.m_cMinSuccessfulTrackingFramesForCameraPoseFinder
        && m_cFrameCounter % m_paramsCurrent.m_cCameraPoseFinderProcessFrameCalculationInterval == 0
        && colorSynchronized)
    {    
        hr  = UpdateCameraPoseFinder();

        if (FAILED(hr))
        {
            SetStatusMessage(L"Kinect Fusion UpdateCameraPoseFinder call failed.");
            goto FinishFrame;
        }
    }

FinishFrame:

    EnterCriticalSection(&m_lockFrame);

    ////////////////////////////////////////////////////////
    // Copy the images to their frame buffers

    if (depthAvailable)
    {
        StoreImageToFrameBuffer(m_pDepthFloatImage, m_frame.m_pDepthRGBX);
    }

    if (raycastFrame)
    {
        if (m_paramsCurrent.m_bCaptureColor)
        {
            StoreImageToFrameBuffer(m_pCapturedSurfaceColor, m_frame.m_pReconstructionRGBX);
        }
        else if (m_paramsCurrent.m_bDisplaySurfaceNormals)
        {
            StoreImageToFrameBuffer(m_pShadedSurfaceNormals, m_frame.m_pReconstructionRGBX);
        }
        else
        {
            StoreImageToFrameBuffer(m_pShadedSurface, m_frame.m_pReconstructionRGBX);
        }
    }

    // Display raycast depth image when in pose finding mode
    if (m_bTrackingFailed && cameraPoseFinderAvailable)
    {
        StoreImageToFrameBuffer(m_pRaycastDepthFloatImage, m_frame.m_pTrackingDataRGBX);
    }
    else
    {
        // Don't calculate the residual delta from reference frame every frame to reduce computation time
        if (m_bCalculateDeltaFrame )
        {
            if (!m_paramsCurrent.m_bAutoFindCameraPoseWhenLost)
            {
                // Color the float residuals from the AlignDepthFloatToReconstruction
                hr  = ColorResiduals(m_pFloatDeltaFromReference, m_pShadedDeltaFromReference);
            }

            if (SUCCEEDED(hr))
            {
                StoreImageToFrameBuffer(m_pShadedDeltaFromReference, m_frame.m_pTrackingDataRGBX);
            }
        }
    }

    ////////////////////////////////////////////////////////
    // Periodically Display Fps

    if (SUCCEEDED(hr))
    {
        // Update frame counter
        m_cFrameCounter++;
        m_cFPSFrameCounter++;

        // Display fps count approximately every cTimeDisplayInterval seconds
        double elapsed = m_timer.AbsoluteTime() - m_fFrameCounterStartTime;
        if (static_cast<int>(elapsed) >= cTimeDisplayInterval)
        {
            m_frame.m_fFramesPerSecond = 0;

            // Update status display
            if (!m_bTrackingFailed)
            {
                m_frame.m_fFramesPerSecond = static_cast<float>(m_cFPSFrameCounter / elapsed);
            }

            m_cFPSFrameCounter = 0;
            m_fFrameCounterStartTime = m_timer.AbsoluteTime();
        }
    }

    m_frame.SetStatusMessage(m_statusMessage);

    LeaveCriticalSection(&m_lockFrame);

    return SUCCEEDED(hr);
}

/// <summary>
/// Perform camera pose finding when tracking is lost using AlignPointClouds.
/// This is typically more successful than FindCameraPoseAlignDepthFloatToReconstruction.
/// </summary>
HRESULT KinectFusionProcessor::FindCameraPoseAlignPointClouds()
{
    HRESULT hr = S_OK;

    if (!IsCameraPoseFinderAvailable())
    {
        return E_FAIL;
    }

    hr = DownsampleColorFrameToDepthResolution(m_pColorImage, m_pResampledColorImage);
    if (FAILED(hr))
    {
        return hr;
    }

    // Start  kNN (k nearest neighbors) camera pose finding
    INuiFusionMatchCandidates *pMatchCandidates = nullptr;

    // Test the camera pose finder to see how similar the input images are to previously captured images.
    // This will return an error code if there are no matched frames in the camera pose finder database.
    hr = m_pCameraPoseFinder->FindCameraPose(
        m_pDepthFloatImage, 
        m_pResampledColorImage,
        &pMatchCandidates);

    if (FAILED(hr) || nullptr == pMatchCandidates)
    {
        goto FinishFrame;
    }

    unsigned int cPoses = pMatchCandidates->MatchPoseCount();

    float minDistance = 1.0f;   // initialize to the maximum normalized distance
    hr = pMatchCandidates->CalculateMinimumDistance(&minDistance);

    if (FAILED(hr) || 0 == cPoses)
    {
        goto FinishFrame;
    }

    // Check the closest frame is similar enough to our database to re-localize
    // For frames that have a larger minimum distance, standard tracking will run
    // and if this fails, tracking will be considered lost.
    if (minDistance >= m_paramsCurrent.m_fCameraPoseFinderDistanceThresholdReject)
    {
        SetStatusMessage(L"FindCameraPose exited early as not good enough pose matches.");
        hr = E_NUI_NO_MATCH;
        goto FinishFrame;
    }

    // Get the actual matched poses
    const Matrix4 *pNeighbors = nullptr;
    hr = pMatchCandidates->GetMatchPoses(&pNeighbors);

    if (FAILED(hr))
    {
        goto FinishFrame;
    }

    ////////////////////////////////////////////////////////
    // Smooth depth image

    hr = m_pVolume->SmoothDepthFloatFrame(
        m_pDepthFloatImage, 
        m_pSmoothDepthFloatImage, 
        m_paramsCurrent.m_cSmoothingKernelWidth, 
        m_paramsCurrent.m_fSmoothingDistanceThreshold); // ON GPU

    if (FAILED(hr))
    {
        SetStatusMessage(L"Kinect Fusion SmoothDepth call failed.");
        goto FinishFrame;
    }

    ////////////////////////////////////////////////////////
    // Calculate Point Cloud from smoothed input Depth Image

    hr = NuiFusionDepthFloatFrameToPointCloud(
        m_pSmoothDepthFloatImage,
        m_pDepthPointCloud);

    if (FAILED(hr))
    {
        SetStatusMessage(L"Kinect Fusion NuiFusionDepthFloatFrameToPointCloud call failed.");
        goto FinishFrame;
    }

    HRESULT tracking = S_OK;
    FLOAT alignmentEnergy = 0;

    unsigned short relocIterationCount = NUI_FUSION_DEFAULT_ALIGN_ITERATION_COUNT;

    double smallestEnergy = DBL_MAX;
    int smallestEnergyNeighborIndex = -1;

    int bestNeighborIndex = -1;
    Matrix4 bestNeighborCameraPose;
    SetIdentityMatrix(bestNeighborCameraPose);
    // Exclude very tiny alignment energy case which is unlikely to happen in reality - this is more likely a tracking error
    double bestNeighborAlignmentEnergy = m_paramsCurrent.m_fMaxAlignPointCloudsEnergyForSuccess;

    // Run alignment with best matched poses (i.e. k nearest neighbors (kNN))
    unsigned int maxTests = min(m_paramsCurrent.m_cMaxCameraPoseFinderPoseTests, cPoses);

    for (unsigned int n = 0; n < maxTests; n++)
    {
        ////////////////////////////////////////////////////////
        // Call AlignPointClouds

        Matrix4 poseProposal = pNeighbors[n];

        // Get the saved pose view by raycasting the volume
        hr = m_pVolume->CalculatePointCloud(m_pRaycastPointCloud, nullptr, &poseProposal);

        tracking = m_pVolume->AlignPointClouds(
            m_pRaycastPointCloud,
            m_pDepthPointCloud,
            relocIterationCount,
            nullptr,
            &alignmentEnergy,
            &poseProposal); 


        if (SUCCEEDED(tracking) && alignmentEnergy < bestNeighborAlignmentEnergy  && alignmentEnergy > m_paramsCurrent.m_fMinAlignPointCloudsEnergyForSuccess)
        {
            bestNeighborAlignmentEnergy = alignmentEnergy;
            bestNeighborIndex = n;

            // This is after tracking succeeds, so should be a more accurate pose to store...
            bestNeighborCameraPose = poseProposal; 
        }

        // Find smallest energy neighbor independent of tracking success
        if (alignmentEnergy < smallestEnergy)
        {
            smallestEnergy = alignmentEnergy;
            smallestEnergyNeighborIndex = n;
        }
    }

    // Use the neighbor with the smallest residual alignment energy
    // At the cost of additional processing we could also use kNN+Mean camera pose finding here
    // by calculating the mean pose of the best n matched poses and also testing this to see if the 
    // residual alignment energy is less than with kNN.
    if (bestNeighborIndex > -1)
    {
        m_worldToCameraTransform = bestNeighborCameraPose;

        // Get the saved pose view by raycasting the volume
        hr = m_pVolume->CalculatePointCloud(m_pRaycastPointCloud, nullptr, &m_worldToCameraTransform);

        if (FAILED(hr))
        {
            goto FinishFrame;
        }

        // Tracking succeeded!
        hr = S_OK;

        SetTrackingSucceeded();

        // Run a single iteration of AlignPointClouds to get the deltas frame
        hr = m_pVolume->AlignPointClouds(
            m_pRaycastPointCloud,
            m_pDepthPointCloud,
            1,
            m_pShadedDeltaFromReference,
            &alignmentEnergy,
            &bestNeighborCameraPose); 

        if (SUCCEEDED(hr))
        {
            StoreImageToFrameBuffer(m_pShadedDeltaFromReference, m_frame.m_pTrackingDataRGBX);
        }

        // Stop the residual image being displayed as we have stored our own
        m_bCalculateDeltaFrame = false;

        WCHAR str[MAX_PATH];
        swprintf_s(str, ARRAYSIZE(str), L"Camera Pose Finder SUCCESS! Residual energy=%f, %u frames stored, minimum distance=%f, best match index=%d", bestNeighborAlignmentEnergy, cPoses, minDistance, bestNeighborIndex);
        SetStatusMessage(str);
    }
    else
    {        
        m_worldToCameraTransform = pNeighbors[smallestEnergyNeighborIndex];

        // Get the smallest energy view by raycasting the volume
        hr = m_pVolume->CalculatePointCloud(m_pRaycastPointCloud, nullptr, &m_worldToCameraTransform);

        if (FAILED(hr))
        {
            goto FinishFrame;
        }

        // Camera pose finding failed - return the tracking failed error code
        hr = E_NUI_FUSION_TRACKING_ERROR;

        // Tracking Failed will be set again on the next iteration in ProcessDepth
        WCHAR str[MAX_PATH];
        swprintf_s(str, ARRAYSIZE(str), L"Camera Pose Finder FAILED! Residual energy=%f, %u frames stored, minimum distance=%f, best match index=%d", smallestEnergy, cPoses, minDistance, smallestEnergyNeighborIndex);
        SetStatusMessage(str);
    }

FinishFrame:

    SafeRelease(pMatchCandidates);

    return hr;
}


/// <summary>
/// Perform camera pose finding when tracking is lost using AlignDepthFloatToReconstruction.
/// </summary>
HRESULT KinectFusionProcessor::FindCameraPoseAlignDepthFloatToReconstruction()
{
    HRESULT hr = S_OK;

    if (!IsCameraPoseFinderAvailable())
    {
        return E_FAIL;
    }

    hr = DownsampleColorFrameToDepthResolution(m_pColorImage, m_pResampledColorImage);
    if (FAILED(hr))
    {
        return hr;
    }

    // Start  kNN (k nearest neighbors) camera pose finding
    INuiFusionMatchCandidates *pMatchCandidates = nullptr;

    // Test the camera pose finder to see how similar the input images are to previously captured images.
    // This will return an error code if there are no matched frames in the camera pose finder database.
    hr = m_pCameraPoseFinder->FindCameraPose(
        m_pDepthFloatImage, 
        m_pResampledColorImage,
        &pMatchCandidates);

    if (FAILED(hr) || nullptr == pMatchCandidates)
    {
        goto FinishFrame;
    }

    unsigned int cPoses = pMatchCandidates->MatchPoseCount();

    float minDistance = 1.0f;   // initialize to the maximum normalized distance
    hr = pMatchCandidates->CalculateMinimumDistance(&minDistance);

    if (FAILED(hr) || 0 == cPoses)
    {
        goto FinishFrame;
    }

    // Check the closest frame is similar enough to our database to re-localize
    // For frames that have a larger minimum distance, standard tracking will run
    // and if this fails, tracking will be considered lost.
    if (minDistance >= m_paramsCurrent.m_fCameraPoseFinderDistanceThresholdReject)
    {
        SetStatusMessage(L"FindCameraPose exited early as not good enough pose matches.");
        hr = E_NUI_NO_MATCH;
        goto FinishFrame;
    }

    // Get the actual matched poses
    const Matrix4 *pNeighbors = nullptr;
    hr = pMatchCandidates->GetMatchPoses(&pNeighbors);

    if (FAILED(hr))
    {
        goto FinishFrame;
    }

    HRESULT tracking = S_OK;
    FLOAT alignmentEnergy = 0;

    unsigned short relocIterationCount = NUI_FUSION_DEFAULT_ALIGN_ITERATION_COUNT;

    double smallestEnergy = DBL_MAX;
    int smallestEnergyNeighborIndex = -1;

    int bestNeighborIndex = -1;
    Matrix4 bestNeighborCameraPose;
    SetIdentityMatrix(bestNeighborCameraPose);
    // Exclude very tiny alignment energy case which is unlikely to happen in reality - this is more likely a tracking error
    double bestNeighborAlignmentEnergy = m_paramsCurrent.m_fMaxAlignToReconstructionEnergyForSuccess;

    // Run alignment with best matched poses (i.e. k nearest neighbors (kNN))
    unsigned int maxTests = min(m_paramsCurrent.m_cMaxCameraPoseFinderPoseTests, cPoses);

    for (unsigned int n = 0; n < maxTests; n++)
    {
        ////////////////////////////////////////////////////////
        // Call AlignDepthFloatToReconstruction

        // Run the camera tracking algorithm with the volume
        // this uses the raycast frame and pose to find a valid camera pose by matching the depth against the volume
        hr = SetReferenceFrame(pNeighbors[n]);

        if (FAILED(hr))
        {
            continue;
        }

        tracking = m_pVolume->AlignDepthFloatToReconstruction( 
            m_pDepthFloatImage,
            relocIterationCount,
            m_pFloatDeltaFromReference,
            &alignmentEnergy,
            &(pNeighbors[n]));

        bool relocSuccess = SUCCEEDED(tracking) && alignmentEnergy < bestNeighborAlignmentEnergy && alignmentEnergy > m_paramsCurrent.m_fMinAlignToReconstructionEnergyForSuccess;

        if (relocSuccess)
        {
            if (SUCCEEDED(tracking))
            {
                bestNeighborAlignmentEnergy = alignmentEnergy;
                bestNeighborIndex = n;

                // This is after tracking succeeds, so should be a more accurate pose to store...
                m_pVolume->GetCurrentWorldToCameraTransform(&bestNeighborCameraPose); 
            }
        }

        // Find smallest energy neighbor independent of tracking success
        if (alignmentEnergy < smallestEnergy)
        {
            smallestEnergy = alignmentEnergy;
            smallestEnergyNeighborIndex = n;
        }
    }

    // Use the neighbor with the smallest residual alignment energy
    // At the cost of additional processing we could also use kNN+Mean camera pose finding here
    // by calculating the mean pose of the best n matched poses and also testing this to see if the 
    // residual alignment energy is less than with kNN.
    if (bestNeighborIndex > -1)
    {       
        m_worldToCameraTransform = bestNeighborCameraPose;
        hr = SetReferenceFrame(m_worldToCameraTransform);

        if (FAILED(hr))
        {
            goto FinishFrame;
        }

        // Tracking succeeded!
        hr = S_OK;

        SetTrackingSucceeded();

        // Force the residual image to be displayed
        m_bCalculateDeltaFrame = true;

        WCHAR str[MAX_PATH];
        swprintf_s(str, ARRAYSIZE(str), L"Camera Pose Finder SUCCESS! Residual energy=%f, %u frames stored, minimum distance=%f, best match index=%d", bestNeighborAlignmentEnergy, cPoses, minDistance, bestNeighborIndex);
        SetStatusMessage(str);
    }
    else
    {        
        m_worldToCameraTransform = pNeighbors[smallestEnergyNeighborIndex];
        hr = SetReferenceFrame(m_worldToCameraTransform);

        if (FAILED(hr))
        {
            goto FinishFrame;
        }

        // Camera pose finding failed - return the tracking failed error code
        hr = E_NUI_FUSION_TRACKING_ERROR;

        // Tracking Failed will be set again on the next iteration in ProcessDepth
        WCHAR str[MAX_PATH];
        swprintf_s(str, ARRAYSIZE(str), L"Camera Pose Finder FAILED! Residual energy=%f, %u frames stored, minimum distance=%f, best match index=%d", smallestEnergy, cPoses, minDistance, smallestEnergyNeighborIndex);
        SetStatusMessage(str);
    }

FinishFrame:

    SafeRelease(pMatchCandidates);

    return hr;
}

/// <summary>
/// Performs raycasting for given pose and sets the tracking reference frame
/// </summary>
/// <param name="worldToCamera">The reference camera pose.</param>
/// <returns>S_OK on success, otherwise failure code</returns>
HRESULT KinectFusionProcessor::SetReferenceFrame(const Matrix4 &worldToCamera)
{
    HRESULT hr = S_OK;

    // Raycast to get the predicted previous frame to align against in the next frame
    hr = m_pVolume->CalculatePointCloudAndDepth(
        m_pRaycastPointCloud, 
        m_pRaycastDepthFloatImage, 
        nullptr, 
        &worldToCamera);

    if (FAILED(hr))
    {
        SetStatusMessage(L"Kinect Fusion CalculatePointCloud call failed.");
        return hr;
    }

    // Set this frame as a reference for AlignDepthFloatToReconstruction
    hr =  m_pVolume->SetAlignDepthFloatToReconstructionReferenceFrame(m_pRaycastDepthFloatImage);

    if (FAILED(hr))
    {
        SetStatusMessage(L"Kinect Fusion SetAlignDepthFloatToReconstructionReferenceFrame call failed.");
        return hr;
    }

    return hr;
}

/// <summary>
/// Update the status on tracking failure.
/// </summary>
void KinectFusionProcessor::SetTrackingFailed()
{
    m_cLostFrameCounter++;
    m_cSuccessfulFrameCounter = 0;
    m_bTrackingFailed = true;
    m_bTrackingHasFailedPreviously = true;
}

/// <summary>
/// Update the status when tracking succeeds.
/// </summary>
void KinectFusionProcessor::SetTrackingSucceeded()
{
    m_cLostFrameCounter = 0;
    m_cSuccessfulFrameCounter++;
    m_bTrackingFailed = false;
}

/// <summary>
/// Reset the tracking flags
/// </summary>
void KinectFusionProcessor::ResetTracking()
{
    m_bTrackingFailed = false;
    m_bTrackingHasFailedPreviously = false;

    m_cLostFrameCounter = 0;
    m_cSuccessfulFrameCounter = 0;

    // Reset pause and signal that the integration resumed
    m_paramsCurrent.m_bPauseIntegration = false;
    m_paramsNext.m_bPauseIntegration = false;
    m_frame.m_bColorCaptured = false;

    if (nullptr != m_pCameraPoseFinder)
    {
        m_pCameraPoseFinder->ResetCameraPoseFinder();
    }
}

/// <summary>
/// Update the camera pose finder data.
/// </summary>
/// <returns>S_OK on success, otherwise failure code</returns>
HRESULT KinectFusionProcessor::UpdateCameraPoseFinder()
{
    AssertOwnThread();

    HRESULT hr = S_OK;

    if (nullptr == m_pDepthFloatImage || nullptr == m_pColorImage  
        || nullptr == m_pResampledColorImage || nullptr == m_pCameraPoseFinder)
    {
        return E_FAIL;
    }

    hr = DownsampleColorFrameToDepthResolution(m_pColorImage, m_pResampledColorImage);
    if (FAILED(hr))
    {
        return hr;
    }

    BOOL poseHistoryTrimmed = FALSE;
    BOOL addedPose = FALSE;

    // This function will add the pose to the camera pose finding database when the input frame's minimum
    // distance to the existing database is equal to or above m_fDistanceThresholdAccept (i.e. indicating 
    // that the input has become dis-similar to the existing database and a new frame should be captured).
    // Note that the color and depth frames must be the same size, however, the horizontal mirroring
    // setting does not have to be consistent between depth and color. It does have to be consistent
    // between camera pose finder database creation and calling FindCameraPose though, hence we always
    // reset both the reconstruction and database when changing the mirror depth setting.
    hr = m_pCameraPoseFinder->ProcessFrame(
        m_pDepthFloatImage, 
        m_pResampledColorImage,
        &m_worldToCameraTransform, 
        m_paramsCurrent.m_fCameraPoseFinderDistanceThresholdAccept, 
        &addedPose, 
        &poseHistoryTrimmed);

    if (TRUE == addedPose)
    {
        WCHAR str[MAX_PATH];
        swprintf_s(str, ARRAYSIZE(str), L"Camera Pose Finder Added Frame! %u frames stored, minimum distance>=%f\n", m_pCameraPoseFinder->GetStoredPoseCount(), m_paramsCurrent.m_fCameraPoseFinderDistanceThresholdAccept);
        SetStatusMessage(str);
    }

    if (TRUE == poseHistoryTrimmed)
    {
        SetStatusMessage(L"Kinect Fusion Camera Pose Finder pose history is full, overwritten oldest pose to store current pose.");
    }

    if (FAILED(hr))
    {
        SetStatusMessage(L"Kinect Fusion Camera Pose Finder Process Frame call failed.");
    }

    return hr;
}

/// <summary>
/// Store a Kinect Fusion image to a frame buffer.
/// Accepts Depth Float, and Color image types.
/// </summary>
/// <param name="imageFrame">The image frame to store.</param>
/// <param name="buffer">The frame buffer.</param>
/// <returns>S_OK on success, otherwise failure code</returns>
HRESULT KinectFusionProcessor::StoreImageToFrameBuffer(
    const NUI_FUSION_IMAGE_FRAME* imageFrame,
    BYTE* buffer)
{
    AssertOwnThread();

    HRESULT hr = S_OK;

    if (nullptr == imageFrame || nullptr == imageFrame->pFrameBuffer || nullptr == buffer)
    {
        return E_INVALIDARG;
    }

    if (NUI_FUSION_IMAGE_TYPE_COLOR != imageFrame->imageType &&
        NUI_FUSION_IMAGE_TYPE_FLOAT != imageFrame->imageType)
    {
        return E_INVALIDARG;
    }

    if (0 == imageFrame->width || 0 == imageFrame->height)
    {
        return E_NOINTERFACE;
    }

    NUI_FUSION_BUFFER *imageFrameBuffer = imageFrame->pFrameBuffer;

    // Make sure we've received valid data
    if (imageFrameBuffer->Pitch != 0)
    {
        // Convert from floating point depth if required
        if (NUI_FUSION_IMAGE_TYPE_FLOAT == imageFrame->imageType)
        {
            // Depth ranges set here for better visualization, and map to black at 0 and white at 4m
            const FLOAT range = 4.0f;
            const FLOAT oneOverRange = (1.0f / range) * 256.0f;
            const FLOAT minRange = 0.0f;

            const float *pFloatBuffer = reinterpret_cast<float *>(imageFrameBuffer->pBits);

            Concurrency::parallel_for(0u, imageFrame->height, [&](unsigned int y)
            {
                unsigned int* pColorRow = reinterpret_cast<unsigned int*>(reinterpret_cast<unsigned char*>(buffer) + (y * imageFrameBuffer->Pitch));
                const float* pFloatRow = reinterpret_cast<const float*>(reinterpret_cast<const unsigned char*>(pFloatBuffer) + (y * imageFrameBuffer->Pitch));

                for (unsigned int x = 0; x < imageFrame->width; ++x)
                {
                    float depth = pFloatRow[x];

                    // Note: Using conditionals in this loop could degrade performance.
                    // Consider using a lookup table instead when writing production code.
                    BYTE intensity = (depth >= minRange) ?
                        static_cast<BYTE>( (int)((depth - minRange) * oneOverRange) % 256 ) :
                    0; // % 256 to enable it to wrap around after the max range

                    pColorRow[x] = (255u << 24) | (intensity << 16) | (intensity << 8 ) | intensity;

                    if (depth < 0.01f)
                    {
                        pColorRow[x] = (255u << 24) | (255u << 16);
                    }
                }
            });
        }
        else    // already in 4 bytes per int (RGBA/BGRA) format
        {
            const size_t destPixelCount =
                m_paramsCurrent.m_cDepthWidth * m_paramsCurrent.m_cDepthHeight;

            BYTE * pBuffer = imageFrameBuffer->pBits;

            // Draw the data with Direct2D
            memcpy_s(
                buffer,
                destPixelCount * KinectFusionParams::BytesPerPixel,
                pBuffer,
                imageFrame->width * imageFrame->height * KinectFusionParams::BytesPerPixel);
        }
    }
    else
    {
        return E_NOINTERFACE;
    }

    return hr;
}

/// <summary>
/// Reset the reconstruction camera pose and clear the volume on the next frame.
/// </summary>
/// <returns>S_OK on success, otherwise failure code</returns>
HRESULT KinectFusionProcessor::ResetReconstruction()
{
    AssertOtherThread();

    EnterCriticalSection(&m_lockParams);
    m_bResetReconstruction = true;
    LeaveCriticalSection(&m_lockParams);

    return S_OK;
}

/// <summary>
/// Reset the reconstruction camera pose and clear the volume.
/// </summary>
/// <returns>S_OK on success, otherwise failure code</returns>
HRESULT KinectFusionProcessor::InternalResetReconstruction()
{
    AssertOwnThread();

    if (nullptr == m_pVolume)
    {
        return E_FAIL;
    }

    HRESULT hr = S_OK;

    SetIdentityMatrix(m_worldToCameraTransform);

    // Translate the world origin away from the reconstruction volume location by an amount equal
    // to the minimum depth threshold. This ensures that some depth signal falls inside the volume.
    // If set false, the default world origin is set to the center of the front face of the 
    // volume, which has the effect of locating the volume directly in front of the initial camera
    // position with the +Z axis into the volume along the initial camera direction of view.
    if (m_paramsCurrent.m_bTranslateResetPoseByMinDepthThreshold)
    {
        Matrix4 worldToVolumeTransform = m_defaultWorldToVolumeTransform;

        // Translate the volume in the Z axis by the minDepthThreshold distance
        float minDist = (m_paramsCurrent.m_fMinDepthThreshold < m_paramsCurrent.m_fMaxDepthThreshold) ? m_paramsCurrent.m_fMinDepthThreshold : m_paramsCurrent.m_fMaxDepthThreshold;
        worldToVolumeTransform.M43 -= (minDist * m_paramsCurrent.m_reconstructionParams.voxelsPerMeter);

        hr = m_pVolume->ResetReconstruction(&m_worldToCameraTransform, &worldToVolumeTransform);
    }
    else
    {
        hr = m_pVolume->ResetReconstruction(&m_worldToCameraTransform, nullptr);
    }

    m_cFrameCounter = 0;
    m_cFPSFrameCounter = 0;
    m_fFrameCounterStartTime = m_timer.AbsoluteTime();

    EnterCriticalSection(&m_lockFrame);
    m_frame.m_fFramesPerSecond = 0;
    LeaveCriticalSection(&m_lockFrame);

    if (SUCCEEDED(hr))
    {
        ResetTracking();
    }

    return hr;
}

/// <summary>
/// Calculate a mesh for the current volume
/// </summary>
/// <param name="ppMesh">returns the new mesh</param>
HRESULT KinectFusionProcessor::CalculateMesh(INuiFusionColorMesh** ppMesh)
{
    AssertOtherThread();

    EnterCriticalSection(&m_lockVolume);

    HRESULT hr = E_FAIL;

    if (m_pVolume != nullptr)
    {
        hr = m_pVolume->CalculateMesh(1, ppMesh);

        // Set the frame counter to 0 to prevent a reset reconstruction call due to large frame 
        // timestamp change after meshing. Also reset frame time for fps counter.
        m_cFrameCounter = 0;
        m_cFPSFrameCounter = 0;
        m_fFrameCounterStartTime =  m_timer.AbsoluteTime();
    }

    LeaveCriticalSection(&m_lockVolume);

    return hr;
}

/// <summary>
/// Set the status bar message
/// </summary>
/// <param name="szMessage">message to display</param>
void KinectFusionProcessor::SetStatusMessage(WCHAR * szMessage)
{
    AssertOwnThread();

    StringCchCopy(m_statusMessage, ARRAYSIZE(m_statusMessage), szMessage);
}

/// <summary>
/// Notifies the UI window that a new frame is ready
/// </summary>
void KinectFusionProcessor::NotifyFrameReady()
{
    AssertOwnThread();

    if (m_hWnd != nullptr && m_msgFrameReady != WM_NULL)
    {
        PostMessage(m_hWnd, m_msgFrameReady, 0, 0);
    }
}

/// <summary>
/// Notifies the UI window to update, even though there is no new frame data
/// </summary>
void KinectFusionProcessor::NotifyEmptyFrame()
{
    AssertOwnThread();

    EnterCriticalSection(&m_lockFrame);
    m_frame.m_fFramesPerSecond = 0;
    m_frame.SetStatusMessage(m_statusMessage);
    LeaveCriticalSection(&m_lockFrame);

    NotifyFrameReady();
}

