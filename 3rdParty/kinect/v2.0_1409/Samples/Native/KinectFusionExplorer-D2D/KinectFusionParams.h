//------------------------------------------------------------------------------
// <copyright file="KinectFusionParams.h" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//------------------------------------------------------------------------------

#pragma once

#include "NuiKinectFusionApi.h"

enum KinectFusionMeshTypes
{
    Stl = 0,
    Obj = 1,
    Ply = 2
};

/// <summary>
/// Parameters to control the behavior of the KinectFusionProcessor.
/// </summary>
struct KinectFusionParams
{
    // Number of bytes per pixel (applies to both depth float and int-per-pixel color and raycast images)
    static const int            BytesPerPixel = 4;

    /// <summary>
    /// Constructor
    /// </summary>
    KinectFusionParams() :
        m_bPauseIntegration(false),
        m_bAutoResetReconstructionWhenLost(false),
        m_bAutoResetReconstructionOnTimeout(false), // We now try to find the camera pose, however, setting this false will no longer auto reset on .xef file playback
        m_bAutoFindCameraPoseWhenLost(true),
        m_fMinDepthThreshold(NUI_FUSION_DEFAULT_MINIMUM_DEPTH),
        m_fMaxDepthThreshold(NUI_FUSION_DEFAULT_MAXIMUM_DEPTH),
        m_bMirrorDepthFrame(false),
        m_cMaxIntegrationWeight(NUI_FUSION_DEFAULT_INTEGRATION_WEIGHT),
        m_bDisplaySurfaceNormals(false),
        m_bCaptureColor(false),
        m_cColorIntegrationInterval(3),
        m_bTranslateResetPoseByMinDepthThreshold(true),
        m_saveMeshType(Stl),
        m_cDeltaFromReferenceFrameCalculationInterval(2),
        m_cMinSuccessfulTrackingFramesForCameraPoseFinder(45), // only update the camera pose finder initially after 45 successful frames (1.5s)
        m_cMinSuccessfulTrackingFramesForCameraPoseFinderAfterFailure(200), // resume integration following 200 successful frames after tracking failure (~7s)
        m_cMaxCameraPoseFinderPoseHistory(NUI_FUSION_CAMERA_POSE_FINDER_DEFAULT_POSE_HISTORY_COUNT),
        m_cCameraPoseFinderFeatureSampleLocationsPerFrame(NUI_FUSION_CAMERA_POSE_FINDER_DEFAULT_FEATURE_LOCATIONS_PER_FRAME_COUNT),
        m_fMaxCameraPoseFinderDepthThreshold(NUI_FUSION_CAMERA_POSE_FINDER_DEFAULT_MAX_DEPTH_THRESHOLD),
        m_fCameraPoseFinderDistanceThresholdReject(1.0f), // a value of 1.0 means no rejection
        m_fCameraPoseFinderDistanceThresholdAccept(0.1f),
        m_cMaxCameraPoseFinderPoseTests(5),
        m_cCameraPoseFinderProcessFrameCalculationInterval(5),
        m_fMaxAlignToReconstructionEnergyForSuccess(0.27f),
        m_fMinAlignToReconstructionEnergyForSuccess(0.005f),
        m_fMaxAlignPointCloudsEnergyForSuccess(0.006f),
        m_fMinAlignPointCloudsEnergyForSuccess(0.0f),
        m_cSmoothingKernelWidth(1),                 // 0=just copy, 1=3x3, 2=5x5, 3=7x7, here we create a 3x3 kernel
        m_fSmoothingDistanceThreshold(0.04f),       // 4cm, could use up to around 0.1f
        m_cAlignPointCloudsImageDownsampleFactor(2),// 1 = no down sample (process at m_depthImageResolution), 2=x/2,y/2, 4=x/4,y/4
        m_fMaxTranslationDelta(0.3f),               // 0.15 - 0.3m per frame typical
        m_fMaxRotationDelta(20.0f)                  // 10-20 degrees per frame typical
    {
        // Get the depth frame size from the NUI_IMAGE_RESOLUTION enum.
        // You can use NUI_IMAGE_RESOLUTION_640x480 or NUI_IMAGE_RESOLUTION_320x240 in this sample.
        // Smaller resolutions will be faster in per-frame computations, but show less detail in reconstructions
        m_cDepthWidth = NUI_DEPTH_RAW_WIDTH;
        m_cDepthHeight = NUI_DEPTH_RAW_HEIGHT;
        m_cDepthImagePixels = m_cDepthWidth*m_cDepthHeight;

        m_cColorWidth  = 1920;
        m_cColorHeight = 1080;
        m_cColorImagePixels = m_cColorWidth*m_cColorHeight;

        // Define a cubic Kinect Fusion reconstruction volume, with the sensor at the center of the
        // front face and the volume directly in front of sensor.
        m_reconstructionParams.voxelsPerMeter = 256;    // 1000mm / 256vpm = ~3.9mm/voxel
        m_reconstructionParams.voxelCountX = 384;       // 384 / 256vpm = 1.5m wide reconstruction
        m_reconstructionParams.voxelCountY = 384;       // Memory = 384*384*384 * 4bytes per voxel
        m_reconstructionParams.voxelCountZ = 384;       // This will require a GPU with at least 256MB

        // This parameter sets whether GPU or CPU processing is used. Note that the CPU will likely be 
        // too slow for real-time processing.
        m_processorType = NUI_FUSION_RECONSTRUCTION_PROCESSOR_TYPE_AMP;

        // If GPU processing is selected, we can choose the index of the device we would like to
        // use for processing by setting this zero-based index parameter. Note that setting -1 will cause
        // automatic selection of the most suitable device (specifically the DirectX11 compatible device 
        // with largest memory), which is useful in systems with multiple GPUs when only one reconstruction
        // volume is required. Note that the automatic choice will not load balance across multiple 
        // GPUs, hence users should manually select GPU indices when multiple reconstruction volumes 
        // are required, each on a separate device.
        m_deviceIndex = -1;    // automatically choose device index for processing
    }

    /// <summary>
    /// Indicates whether the current reconstruction volume is different than the one in the params.
    /// </summary>
    bool VolumeChanged(const KinectFusionParams& params)
    {
        return
            m_reconstructionParams.voxelCountX != params.m_reconstructionParams.voxelCountX ||
            m_reconstructionParams.voxelCountY != params.m_reconstructionParams.voxelCountY ||
            m_reconstructionParams.voxelCountZ != params.m_reconstructionParams.voxelCountZ ||
            m_reconstructionParams.voxelsPerMeter != params.m_reconstructionParams.voxelsPerMeter ||
            m_processorType != params.m_processorType ||
            m_deviceIndex != params.m_deviceIndex;
    }

    /// <summary>
    /// Reconstruction Initialization parameters
    /// </summary>
    int                         m_deviceIndex;
    NUI_FUSION_RECONSTRUCTION_PROCESSOR_TYPE m_processorType;

    /// <summary>
    /// Parameter to pause integration of new frames
    /// </summary>
    bool                        m_bPauseIntegration;

    /// <summary>
    /// Depth image resolution and size
    /// </summary>
    UINT                        m_cDepthWidth;
    UINT                        m_cDepthHeight;
    UINT                        m_cDepthImagePixels;

    /// <summary>
    /// Color image resolution and size
    /// </summary>
    UINT                        m_cColorWidth;
    UINT                        m_cColorHeight;
    UINT                        m_cColorImagePixels;

    /// <summary>
    /// The Kinect Fusion Volume Parameters
    /// </summary>
    NUI_FUSION_RECONSTRUCTION_PARAMETERS m_reconstructionParams;

    /// <summary>
    /// Parameter to enable automatic reset of the reconstruction when camera tracking is lost.
    /// Set to true in the constructor to enable auto reset on cResetOnNumberOfLostFrames
    /// number of lost frames, or false to never automatically reset on loss of camera tracking.
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
    float                       m_fMinDepthThreshold;
    float                       m_fMaxDepthThreshold;
    bool                        m_bMirrorDepthFrame;
    unsigned short              m_cMaxIntegrationWeight;
    bool                        m_bDisplaySurfaceNormals;
    bool                        m_bCaptureColor;
    int							m_cColorIntegrationInterval;
    KinectFusionMeshTypes       m_saveMeshType;
    unsigned int                m_cDeltaFromReferenceFrameCalculationInterval;

    /// <summary>
    /// Here we set a high limit on the maximum residual alignment energy where we consider the tracking
    /// to have succeeded. Typically this value would be around 0.2f to 0.3f.
    /// (Lower residual alignment energy after tracking is considered better.)
    /// </summary>
    float                       m_fMaxAlignToReconstructionEnergyForSuccess;

    /// <summary>
    /// Here we set a low limit on the residual alignment energy, below which we reject a tracking
    /// success report and believe it to have failed. Typically this value would be around 0.005f, as
    /// values below this (i.e. close to 0 which is perfect alignment) most likely come from frames
    /// where the majority of the image is obscured (i.e. 0 depth) or mis-matched (i.e. similar depths
    /// but different scene or camera pose).
    /// </summary>
    float                       m_fMinAlignToReconstructionEnergyForSuccess;

    /// <summary>
    /// Parameter to translate the reconstruction based on the minimum depth setting.
    /// When set to false, the reconstruction volume +Z axis starts at the camera lens and extends
    /// into the scene. Setting this true in the constructor will move the volume forward along +Z
    /// away from the camera by the minimum depth threshold to enable capture of very small
    /// reconstruction volumes by setting a non-identity camera transformation in the
    /// ResetReconstruction call.
    /// Small volumes may work better when shifted, as the Kinect hardware has a minimum sensing
    /// limit of ~0.35m, inside which no valid depth is returned, hence it is difficult to
    /// initialize and track robustly when the majority of a small volume is inside this distance.
    /// </summary>
    bool                        m_bTranslateResetPoseByMinDepthThreshold;

    /// <summary>
    /// Parameter to enable automatic finding of camera pose when lost. This searches back through
    /// the camera pose history where key-frames and camera poses have been stored in the camera
    /// pose finder database to propose the most likely pose matches for the current camera input.
    /// </summary>
    bool                        m_bAutoFindCameraPoseWhenLost;

    /// <summary>
    /// Camera pose finder configuration parameters
    /// m_fCameraPoseFinderDistanceThresholdReject is a threshold used following the minimum distance 
    /// calculation between the input frame and the camera pose finder database. This calculated value
    /// between 0 and 1.0f must be less than or equal to the threshold in order to run the pose finder,
    /// as the input must at least be similar to the pose finder database for a correct pose to be
    /// matched. m_fCameraPoseFinderDistanceThresholdAccept is a threshold passed to the ProcessFrame 
    /// function in the camera pose finder interface. The minimum distance between the input frame and
    /// the pose finder database must be greater than or equal to this value for a new pose to be 
    /// stored in the database, which regulates how close together poses are stored in the database.
    /// </summary>
    unsigned int                m_cCameraPoseFinderProcessFrameCalculationInterval;
    unsigned int                m_cMaxCameraPoseFinderPoseHistory;
    unsigned int                m_cCameraPoseFinderFeatureSampleLocationsPerFrame;
    float                       m_fMaxCameraPoseFinderDepthThreshold;
    float                       m_fCameraPoseFinderDistanceThresholdReject;
    float                       m_fCameraPoseFinderDistanceThresholdAccept;
    unsigned int                m_cMaxCameraPoseFinderPoseTests;
    unsigned int                m_cMinSuccessfulTrackingFramesForCameraPoseFinder;
    unsigned int                m_cMinSuccessfulTrackingFramesForCameraPoseFinderAfterFailure;

    /// <summary>
    /// Here we set a high limit on the maximum residual alignment energy where we consider the tracking
    /// with AlignPointClouds to have succeeded. Typically this value would be around 0.005f to 0.006f.
    /// (Lower residual alignment energy after relocalization is considered better.)
    /// </summary>
    float                       m_fMaxAlignPointCloudsEnergyForSuccess;

    /// <summary>
    /// Here we set a low limit on the residual alignment energy, below which we reject a tracking
    /// success report from AlignPointClouds and believe it to have failed. This can typically be around 0.
    /// </summary>
    float                       m_fMinAlignPointCloudsEnergyForSuccess;

    /// <summary>
    /// Camera pose finder AlignPointClouds Camera Tracking related parameters
    /// </summary>
    unsigned int                m_cAlignPointCloudsImageDownsampleFactor;
    unsigned int                m_cSmoothingKernelWidth;
    float                       m_fSmoothingDistanceThreshold;
    float                       m_fMaxTranslationDelta;
    float                       m_fMaxRotationDelta;
};
