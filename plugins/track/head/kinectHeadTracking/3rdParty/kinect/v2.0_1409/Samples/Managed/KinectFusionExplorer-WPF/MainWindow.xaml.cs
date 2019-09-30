//------------------------------------------------------------------------------
// <copyright file="MainWindow.xaml.cs" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//------------------------------------------------------------------------------

namespace Microsoft.Samples.Kinect.KinectFusionExplorer
{
    using System;
    using System.Collections.Generic;
    using System.ComponentModel;
    using System.Globalization;
    using System.IO;
    using System.Threading;
    using System.Threading.Tasks;
    using System.Windows;
    using System.Windows.Data;
    using System.Windows.Media;
    using System.Windows.Media.Imaging;
    using System.Windows.Media.Media3D;
    using System.Windows.Threading;
    using Microsoft.Kinect;
    using Microsoft.Kinect.Fusion;
    using Wpf3DTools;

    /// <summary>
    /// The implementation of the MainWindow class.
    /// </summary>
    public partial class MainWindow : Window, INotifyPropertyChanged, IDisposable
    {
        /// <summary>
        /// The reconstruction volume processor type. This parameter sets whether AMP or CPU processing
        /// is used. Note that CPU processing will likely be too slow for real-time processing.
        /// </summary>
        private const ReconstructionProcessor ProcessorType = ReconstructionProcessor.Amp;

        /// <summary>
        /// The zero-based device index to choose for reconstruction processing if the 
        /// ReconstructionProcessor AMP options are selected.
        /// Here we automatically choose a device to use for processing by passing -1, 
        /// </summary>
        private const int DeviceToUse = -1;

        /// <summary>
        /// If set true, will automatically reset the reconstruction when MaxTrackingErrors have occurred
        /// </summary>
        private const bool AutoResetReconstructionWhenLost = false;

        /// <summary>
        /// Max tracking error count, will reset the reconstruction if tracking errors
        /// reach the number
        /// </summary>
        private const int MaxTrackingErrors = 100;

        /// <summary>
        /// Time threshold to reset the reconstruction if tracking can't be restored within it.
        /// This value is valid if GPU is used
        /// </summary>
        private const int ResetOnTimeStampSkippedMillisecondsGPU = 2000;

        /// <summary>
        /// Time threshold to reset the reconstruction if tracking can't be restored within it.
        /// This value is valid if CPU is used
        /// </summary>
        private const int ResetOnTimeStampSkippedMillisecondsCPU = 6000;

        /// <summary>
        /// Width of raw depth stream
        /// </summary>
        private const int RawDepthWidth = 512;

        /// <summary>
        /// Height of raw depth stream
        /// </summary>
        private const int RawDepthHeight = 424;

        /// <summary>
        /// Width of raw color stream
        /// </summary>
        private const int RawColorWidth = 1920;

        /// <summary>
        /// Height of raw color stream
        /// </summary>
        private const int RawColorHeight = 1080;

        /// <summary>
        /// Downsample factor of the color frame used by the depth visibility test.
        /// </summary>
        private const int ColorDownsampleFactor = 4;

        /// <summary>
        /// The height of raw depth stream if keep the w/h ratio as 4:3
        /// </summary>
        private const int RawDepthHeightWithSpecialRatio = 384;

        /// <summary>
        /// Event interval for FPS timer
        /// </summary>
        private const int FpsInterval = 5;

        /// <summary>
        /// Event interval for status bar timer
        /// </summary>
        private const int StatusBarInterval = 1;

        /// <summary>
        /// Force a point cloud calculation and render at least every 100 milliseconds.
        /// </summary>
        private const int RenderIntervalMilliseconds = 100;

        /// <summary>
        /// The frame interval where we integrate color.
        /// Capturing color has an associated processing cost, so we do not have to capture every frame here.
        /// </summary>
        private const int ColorIntegrationInterval = 1;

        /// <summary>
        /// Frame interval we calculate the deltaFromReferenceFrame 
        /// </summary>
        private const int DeltaFrameCalculationInterval = 2;

        /// <summary>
        /// Volume Cube and WPF3D Origin coordinate cross axis 3D graphics line thickness in screen pixels
        /// </summary>
        private const int LineThickness = 2;

        /// <summary>
        /// WPF3D Origin coordinate cross 3D graphics axis size in m
        /// </summary>
        private const float OriginCoordinateCrossAxisSize = 0.1f;

        /// <summary>
        /// Frame interval we update the camera pose finder database.
        /// </summary>
        private const int CameraPoseFinderProcessFrameCalculationInterval = 5;

        /// <summary>
        /// How many frames after starting tracking will will wait before starting to store
        /// image frames to the pose finder database. Here we set 45 successful frames (1.5s).
        /// </summary>
        private const int MinSuccessfulTrackingFramesForCameraPoseFinder = 45;

        /// <summary>
        /// How many frames after starting tracking will will wait before starting to store
        /// image frames to the pose finder database. Here we set 200 successful frames (~7s).
        /// </summary>
        private const int MinSuccessfulTrackingFramesForCameraPoseFinderAfterFailure = 200;

        /// <summary>
        /// Here we set a high limit on the maximum residual alignment energy where we consider the tracking
        /// to have succeeded. Typically this value would be around 0.2f to 0.3f.
        /// (Lower residual alignment energy after tracking is considered better.)
        /// </summary>
        private const float MaxAlignToReconstructionEnergyForSuccess = 0.27f;

        /// <summary>
        /// Here we set a low limit on the residual alignment energy, below which we reject a tracking
        /// success report and believe it to have failed. Typically this value would be around 0.005f, as
        /// values below this (i.e. close to 0 which is perfect alignment) most likely come from frames
        /// where the majority of the image is obscured (i.e. 0 depth) or mismatched (i.e. similar depths
        /// but different scene or camera pose).
        /// </summary>
        private const float MinAlignToReconstructionEnergyForSuccess = 0.005f;

        /// <summary>
        /// Here we set a high limit on the maximum residual alignment energy where we consider the tracking
        /// with AlignPointClouds to have succeeded. Typically this value would be around 0.005f to 0.006f.
        /// (Lower residual alignment energy after relocalization is considered better.)
        /// </summary>
        private const float MaxAlignPointCloudsEnergyForSuccess = 0.006f;

        /// <summary>
        /// Here we set a low limit on the residual alignment energy, below which we reject a tracking
        /// success report from AlignPointClouds and believe it to have failed. This can typically be around 0.
        /// </summary>
        private const float MinAlignPointCloudsEnergyForSuccess = 0.0f;

        /// <summary>
        /// The maximum number of matched poseCount we consider when finding the camera pose. 
        /// Although the matches are ranked, so we look at the highest probability match first, a higher 
        /// value has a greater chance of finding a good match overall, but has the trade-off of being 
        /// slower. Typically we test up to around the 5 best matches, after which is may be better just
        /// to try again with the next input depth frame if no good match is found.
        /// </summary>
        private const int MaxCameraPoseFinderPoseTests = 5;

        /// <summary>
        /// CameraPoseFinderDistanceThresholdReject is a threshold used following the minimum distance 
        /// calculation between the input frame and the camera pose finder database. This calculated value
        /// between 0 and 1.0f must be less than or equal to the threshold in order to run the pose finder,
        /// as the input must at least be similar to the pose finder database for a correct pose to be
        /// matched.
        /// </summary>
        private const float CameraPoseFinderDistanceThresholdReject = 1.0f; // a value of 1.0 means no rejection

        /// <summary>
        /// CameraPoseFinderDistanceThresholdAccept is a threshold passed to the ProcessFrame 
        /// function in the camera pose finder interface. The minimum distance between the input frame and
        /// the pose finder database must be greater than or equal to this value for a new pose to be 
        /// stored in the database, which regulates how close together poseCount are stored in the database.
        /// </summary>
        private const float CameraPoseFinderDistanceThresholdAccept = 0.1f;

        /// <summary>
        /// Maximum residual alignment energy where tracking is still considered successful
        /// </summary>
        private const int SmoothingKernelWidth = 1; // 0=just copy, 1=3x3, 2=5x5, 3=7x7, here we create a 3x3 kernel

        /// <summary>
        /// Maximum residual alignment energy where tracking is still considered successful
        /// </summary>
        private const float SmoothingDistanceThreshold = 0.04f; // 4cm, could use up to around 0.1f;

        /// <summary>
        /// Maximum translation threshold between successive poses when using AlignPointClouds
        /// </summary>
        private const float MaxTranslationDeltaAlignPointClouds = 0.3f; // 0.15 - 0.3m per frame typical

        /// <summary>
        /// Maximum rotation threshold between successive poses when using AlignPointClouds
        /// </summary>
        private const float MaxRotationDeltaAlignPointClouds = 20.0f; // 10-20 degrees per frame typical

        /// <summary>
        /// The factor to downsample the depth image by for AlignPointClouds
        /// </summary>
        private const int DownsampleFactor = 2;

        /// <summary>
        /// Threshold used in the visibility depth test to check if this depth value occlude an
        ///  object or not.
        /// </summary>
        private const ushort DepthVisibilityTestThreshold = 50; // 50mm

        /// <summary>
        /// Volume Cube 3D graphics line color
        /// </summary>
        private static System.Windows.Media.Color volumeCubeLineColor = System.Windows.Media.Color.FromArgb(200, 0, 200, 0);   // Green, partly transparent

        /// <summary>
        /// If set true, will automatically reset the reconstruction when the timestamp changes by
        /// ResetOnTimeStampSkippedMillisecondsGPU or ResetOnTimeStampSkippedMillisecondsCPU for the 
        /// different processor types respectively. This is useful for automatically resetting when
        /// scrubbing through a .XEF file or on loop of a .XEF file during playback. Note that setting
        /// this true may cause constant resets on slow machines that cannot process frames in less
        /// time that the reset threshold. If this occurs, set to false or increase the timeout.
        /// </summary>
        /// <remarks>
        /// We now try to find the camera pose, however, setting this false will no longer auto reset on .XEF file playback
        /// </remarks>
        private bool autoResetReconstructionOnTimeSkip = false; 

        /// <summary>
        /// Track whether Dispose has been called
        /// </summary>
        private bool disposed;

        /// <summary>
        /// Saving mesh flag
        /// </summary>
        private bool savingMesh;

        /// <summary>
        /// To display shaded surface normals frame instead of shaded surface frame
        /// </summary>
        private bool displayNormals;

        /// <summary>
        /// Capture, integrate and display color when true
        /// </summary>
        private bool captureColor;

        /// <summary>
        /// Pause or resume image integration
        /// </summary>
        private bool pauseIntegration;

        /// <summary>
        /// Depth image is mirrored
        /// </summary>
        private bool mirrorDepth;

        /// <summary>
        /// Whether render from the live Kinect camera pose or virtual camera pose
        /// </summary>
        private bool kinectView = true;

        /// <summary>
        /// Whether render the volume 3D graphics overlay
        /// </summary>
        private bool volumeGraphics;

        /// <summary>
        /// Image Width of depth frame
        /// </summary>
        private int depthWidth = 0;

        /// <summary>
        /// Image height of depth frame
        /// </summary>
        private int depthHeight = 0;

        /// <summary>
        /// Count of pixels in the depth frame
        /// </summary>
        private int depthPixelCount = 0;

        /// <summary>
        /// Image width of color frame
        /// </summary>
        private int colorWidth = 0;

        /// <summary>
        /// Image height of color frame
        /// </summary>
        private int colorHeight = 0;

        /// <summary>
        /// Count of pixels in the color frame
        /// </summary>
        private int colorPixelCount = 0;

        /// <summary>
        /// The width of the downsampled images for AlignPointClouds
        /// </summary>
        private int downsampledWidth;

        /// <summary>
        /// The height of the downsampled images for AlignPointClouds
        /// </summary>
        private int downsampledHeight;

        /// <summary>
        /// Store the width of the visibility test map.
        /// </summary>
        private int depthVisibilityTestMapWidth = 0;

        /// <summary>
        /// Store the height of the visibility test map.
        /// </summary>
        private int depthVisibilityTestMapHeight = 0;

        /// <summary>
        /// The counter for image process failures
        /// </summary>
        private int trackingErrorCount = 0;

        /// <summary>
        /// Set true when tracking fails
        /// </summary>
        private bool trackingFailed;

        /// <summary>
        /// Set true when tracking fails and stays false until integration resumes.
        /// </summary>
        private bool trackingHasFailedPreviously;

        /// <summary>
        /// Set true when the camera pose finder has stored frames in its database and is able to match camera frames.
        /// </summary>
        private bool cameraPoseFinderAvailable;

        /// <summary>
        /// The counter for image process successes
        /// </summary>
        private int successfulFrameCount;

        /// <summary>
        /// The counter for frames that have been processed
        /// </summary>
        private int processedFrameCount = 0;

        /// <summary>
        /// Timestamp of last depth frame
        /// </summary>
        private TimeSpan lastFrameTimestamp;

        /// <summary>
        /// Timer to count FPS
        /// </summary>
        private DispatcherTimer fpsTimer;

        /// <summary>
        /// Timer stamp of last computation of FPS
        /// </summary>
        private DateTime lastFPSTimestamp = DateTime.UtcNow;

        /// <summary>
        /// Timer stamp of last raycast and render
        /// </summary>
        private DateTime lastRenderTimestamp = DateTime.UtcNow;

        /// <summary>
        /// Timer used for ensuring status bar message will be displayed at least one second
        /// </summary>
        private DispatcherTimer statusBarTimer;

        /// <summary>
        /// Timer stamp of last update of status message
        /// </summary>
        private DateTime lastStatusTimestamp;

        /// <summary>
        /// A high priority message queue for status message
        /// </summary>
        private Queue<string> statusMessageQueue = new Queue<string>();

        /// <summary>
        /// Active Kinect sensor
        /// </summary>
        private KinectSensor sensor = null;

        /// <summary>
        /// Reader for depth/color/body index frames
        /// </summary>
        private MultiSourceFrameReader reader;

        /// <summary>
        /// Intermediate storage for the extended depth data received from the camera in the current frame
        /// </summary>
        private ushort[] depthImagePixels;

        /// <summary>
        /// Store the min depth value in color space. Used to prune occlusion.
        /// </summary>
        private ushort[] depthVisibilityTestMap;

        /// <summary>
        /// Intermediate storage for the color data received from the camera in 32bit color
        /// </summary>
        private byte[] colorImagePixels;

        /// <summary>
        /// Intermediate storage for the color data received from the camera in 32bit color, re-sampled to depth image size
        /// </summary>
        private int[] resampledColorImagePixels;

        /// <summary>
        /// Intermediate storage for the color data downsampled from depth image size and used in AlignPointClouds
        /// </summary>
        private int[] downsampledDeltaFromReferenceColorPixels;

        /// <summary>
        /// The Kinect Fusion volume, enabling color reconstruction
        /// </summary>
        private ColorReconstruction volume;

        /// <summary>
        /// Intermediate storage for the depth float data converted from depth image frame
        /// </summary>
        private FusionFloatImageFrame depthFloatFrame;

        /// <summary>
        /// Intermediate storage for the smoothed depth float image frame
        /// </summary>
        private FusionFloatImageFrame smoothDepthFloatFrame;

        /// <summary>
        /// Kinect color re-sampled to be the same size as the depth frame
        /// </summary>
        private FusionColorImageFrame resampledColorFrame;

        /// <summary>
        /// Kinect color mapped into depth frame
        /// </summary>
        private FusionColorImageFrame resampledColorFrameDepthAligned;

        /// <summary>
        /// Per-pixel alignment values
        /// </summary>
        private FusionFloatImageFrame deltaFromReferenceFrame;

        /// <summary>
        /// Shaded surface frame from shading point cloud frame
        /// </summary>
        private FusionColorImageFrame shadedSurfaceFrame;

        /// <summary>
        /// Shaded surface normals frame from shading point cloud frame
        /// </summary>
        private FusionColorImageFrame shadedSurfaceNormalsFrame;

        /// <summary>
        /// Calculated point cloud frame from image integration
        /// </summary>
        private FusionPointCloudImageFrame raycastPointCloudFrame;

        /// <summary>
        /// Calculated point cloud frame from input depth
        /// </summary>
        private FusionPointCloudImageFrame depthPointCloudFrame;

        /// <summary>
        /// Intermediate storage for the depth float data converted from depth image frame
        /// </summary>
        private FusionFloatImageFrame downsampledDepthFloatFrame;

        /// <summary>
        /// Intermediate storage for the depth float data following smoothing
        /// </summary>
        private FusionFloatImageFrame downsampledSmoothDepthFloatFrame;

        /// <summary>
        /// Calculated point cloud frame from image integration
        /// </summary>
        private FusionPointCloudImageFrame downsampledRaycastPointCloudFrame;

        /// <summary>
        /// Calculated point cloud frame from input depth
        /// </summary>
        private FusionPointCloudImageFrame downsampledDepthPointCloudFrame;

        /// <summary>
        /// Kinect color delta from reference frame data from AlignPointClouds
        /// </summary>
        private FusionColorImageFrame downsampledDeltaFromReferenceFrameColorFrame;

        /// <summary>
        /// Bitmap contains depth float frame data for rendering
        /// </summary>
        private WriteableBitmap depthFloatFrameBitmap;

        /// <summary>
        /// Bitmap contains delta from reference frame data for rendering
        /// </summary>
        private WriteableBitmap deltaFromReferenceFrameBitmap;

        /// <summary>
        /// Bitmap contains shaded surface frame data for rendering
        /// </summary>
        private WriteableBitmap shadedSurfaceFrameBitmap;

        /// <summary>
        /// Pixel buffer of depth float frame with pixel data in float format
        /// </summary>
        private float[] depthFloatFrameDepthPixels;

        /// <summary>
        /// Pixel buffer of delta from reference frame with pixel data in float format
        /// </summary>
        private float[] deltaFromReferenceFrameFloatPixels;

        /// <summary>
        /// Pixel buffer of depth float frame with pixel data in 32bit color
        /// </summary>
        private int[] depthFloatFramePixelsArgb;

        /// <summary>
        /// Pixel buffer of delta from reference frame in 32bit color
        /// </summary>
        private int[] deltaFromReferenceFramePixelsArgb;

        /// <summary>
        /// Pixels buffer of shaded surface frame in 32bit color
        /// </summary>
        private int[] shadedSurfaceFramePixelsArgb;

        /// <summary>
        /// Mapping of depth pixels into color image
        /// </summary>
        private ColorSpacePoint[] colorCoordinates;

        /// <summary>
        /// Mapped color pixels in depth frame of reference
        /// </summary>
        private int[] resampledColorImagePixelsAlignedToDepth;

        /// <summary>
        /// Pixel buffer of depth float frame with pixel data in float format, downsampled for AlignPointClouds
        /// </summary>
        private float[] downsampledDepthImagePixels;
 
        /// <summary>
        /// The coordinate mapper to convert between depth and color frames of reference
        /// </summary>
        private CoordinateMapper mapper;

        /// <summary>
        /// Alignment energy from AlignDepthFloatToReconstruction for current frame 
        /// </summary>
        private float alignmentEnergy;

        /// <summary>
        /// The worker thread to process the depth and color data
        /// </summary>
        private Thread workerThread = null;

        /// <summary>
        /// Event to stop worker thread
        /// </summary>
        private ManualResetEvent workerThreadStopEvent;

        /// <summary>
        /// Event to notify that depth data is ready for process
        /// </summary>
        private ManualResetEvent depthReadyEvent;

        /// <summary>
        /// Event to notify that color data is ready for process
        /// </summary>
        private ManualResetEvent colorReadyEvent;

        /// <summary>
        /// Lock object for raw pixel access
        /// </summary>
        private object rawDataLock = new object();

        /// <summary>
        /// Lock object for volume re-creation and meshing
        /// </summary>
        private object volumeLock = new object();

        /// <summary>
        /// The volume cube 3D graphical representation
        /// </summary>
        private ScreenSpaceLines3D volumeCube;

        /// <summary>
        /// The volume cube 3D graphical representation
        /// </summary>
        private ScreenSpaceLines3D volumeCubeAxisX;

        /// <summary>
        /// The volume cube 3D graphical representation
        /// </summary>
        private ScreenSpaceLines3D volumeCubeAxisY;

        /// <summary>
        /// The volume cube 3D graphical representation
        /// </summary>
        private ScreenSpaceLines3D volumeCubeAxisZ;

        /// <summary>
        /// The axis-aligned coordinate cross X axis
        /// </summary>
        private ScreenSpaceLines3D axisX;

        /// <summary>
        /// The axis-aligned coordinate cross Y axis
        /// </summary>
        private ScreenSpaceLines3D axisY;

        /// <summary>
        /// The axis-aligned coordinate cross Z axis
        /// </summary>
        private ScreenSpaceLines3D axisZ;

        /// <summary>
        /// Indicate whether the 3D view port has added the volume cube
        /// </summary>
        private bool haveAddedVolumeCube = false;

        /// <summary>
        /// Indicate whether the 3D view port has added the origin coordinate cross
        /// </summary>
        private bool haveAddedCoordinateCross = false;

        /// <summary>
        /// Flag boolean set true to force the reconstruction visualization to be updated after graphics camera movements
        /// </summary>
        private bool viewChanged = true;

        /// <summary>
        /// The virtual 3rd person camera view that can be controlled by the mouse
        /// </summary>
        private GraphicsCamera virtualCamera;

        /// <summary>
        /// The virtual 3rd person camera view that can be controlled by the mouse - start rotation
        /// </summary>
        private Quaternion virtualCameraStartRotation = Quaternion.Identity;

        /// <summary>
        /// The virtual 3rd person camera view that can be controlled by the mouse - start translation
        /// </summary>
        private Point3D virtualCameraStartTranslation = new Point3D();  // 0,0,0

        /// <summary>
        /// Flag to signal to worker thread to reset the reconstruction
        /// </summary>
        private bool resetReconstruction = false;

        /// <summary>
        /// Flag to signal to worker thread to re-create the reconstruction
        /// </summary>
        private bool recreateReconstruction = false;

        /// <summary>
        /// The transformation between the world and camera view coordinate system
        /// </summary>
        private Matrix4 worldToCameraTransform;

        /// <summary>
        /// The default transformation between the world and volume coordinate system
        /// </summary>
        private Matrix4 defaultWorldToVolumeTransform;

        /// <summary>
        /// Minimum depth distance threshold in meters. Depth pixels below this value will be
        /// returned as invalid (0). Min depth must be positive or 0.
        /// </summary>
        private float minDepthClip = FusionDepthProcessor.DefaultMinimumDepth;

        /// <summary>
        /// Maximum depth distance threshold in meters. Depth pixels above this value will be
        /// returned as invalid (0). Max depth must be greater than 0.
        /// </summary>
        private float maxDepthClip = FusionDepthProcessor.DefaultMaximumDepth;

        /// <summary>
        /// Image integration weight
        /// </summary>
        private short integrationWeight = FusionDepthProcessor.DefaultIntegrationWeight;

        /// <summary>
        /// The reconstruction volume voxel density in voxels per meter (vpm)
        /// 1000mm / 256vpm = ~3.9mm/voxel
        /// </summary>
        private float voxelsPerMeter = 256.0f;

        /// <summary>
        /// The reconstruction volume voxel resolution in the X axis
        /// At a setting of 256vpm the volume is 384 / 256 = 1.5m wide
        /// </summary>
        private int voxelsX = 384;

        /// <summary>
        /// The reconstruction volume voxel resolution in the Y axis
        /// At a setting of 256vpm the volume is 384 / 256 = 1.5m high
        /// </summary>
        private int voxelsY = 384;

        /// <summary>
        /// The reconstruction volume voxel resolution in the Z axis
        /// At a setting of 256vpm the volume is 384 / 256 = 1.5m deep
        /// </summary>
        private int voxelsZ = 384;

        /// <summary>
        /// Parameter to translate the reconstruction based on the minimum depth setting. When set to
        /// false, the reconstruction volume +Z axis starts at the camera lens and extends into the scene.
        /// Setting this true in the constructor will move the volume forward along +Z away from the
        /// camera by the minimum depth threshold to enable capture of very small reconstruction volume
        /// by setting a non-identity world-volume transformation in the ResetReconstruction call.
        /// Small volumes should be shifted, as the Kinect hardware has a minimum sensing limit of ~0.35m,
        /// inside which no valid depth is returned, hence it is difficult to initialize and track robustly  
        /// when the majority of a small volume is inside this distance.
        /// </summary>
        private bool translateResetPoseByMinDepthThreshold = true;

        /// <summary>
        /// The color mapping of the rendered reconstruction visualization. 
        /// </summary>
        private Matrix4 worldToBGRTransform = new Matrix4();

        /// <summary>
        /// The virtual camera pose - updated whenever the user interacts and moves the virtual camera.
        /// </summary>
        private Matrix4 virtualCameraWorldToCameraMatrix4 = new Matrix4();

        /// <summary>
        /// Flag set true if at some point color has been captured. 
        /// Used when writing .Ply mesh files to output vertex color.
        /// </summary>
        private bool colorCaptured;

        /// <summary>
        /// A camera pose finder to store image frames and poseCount to a database then match the input frames
        /// when tracking is lost to help us recover tracking.
        /// </summary>
        private CameraPoseFinder cameraPoseFinder;

        /// <summary>
        /// Parameter to enable automatic finding of camera pose when lost. This searches back through
        /// the camera pose history where key-frames and camera poseCount have been stored in the camera
        /// pose finder database to propose the most likely pose matches for the current camera input.
        /// </summary>
        private bool autoFindCameraPoseWhenLost = true;

        /// <summary>
        /// Initializes a new instance of the MainWindow class.
        /// </summary>
        public MainWindow()
        {
            this.InitializeComponent();
        }

        /// <summary>
        /// Finalizes an instance of the MainWindow class.
        /// This destructor will run only if the Dispose method does not get called.
        /// </summary>
        ~MainWindow()
        {
            this.Dispose(false);
        }

        #region Properties

        /// <summary>
        /// Property change event
        /// </summary>
        public event PropertyChangedEventHandler PropertyChanged;

        /// <summary>
        /// Gets or sets the timestamp of the current depth frame
        /// </summary>
        public TimeSpan RelativeTime { get; set; }

        /// <summary>
        /// Gets or sets a value indicating whether to display surface normals.
        /// </summary>
        public bool DisplayNormals
        {
            get
            {
                return this.displayNormals;
            }

            set
            {
                this.displayNormals = value;
                if (null != this.PropertyChanged)
                {
                    this.PropertyChanged.Invoke(this, new PropertyChangedEventArgs("DisplayNormals"));
                }
            }
        }

        /// <summary>
        /// Gets or sets a value indicating whether to capture color.
        /// </summary>
        public bool CaptureColor
        {
            get
            {
                return this.captureColor;
            }

            set
            {
                this.captureColor = value;

                if (null != this.PropertyChanged)
                {
                    this.PropertyChanged.Invoke(this, new PropertyChangedEventArgs("CaptureColor"));
                }
            }
        }

        /// <summary>
        /// Gets or sets a value indicating whether to pause integration.
        /// </summary>
        public bool PauseIntegration
        {
            get
            {
                return this.pauseIntegration;
            }

            set
            {
                this.pauseIntegration = value;
                if (null != this.PropertyChanged)
                {
                    this.PropertyChanged.Invoke(this, new PropertyChangedEventArgs("PauseIntegration"));
                }
            }
        }

        /// <summary>
        /// Gets or sets a value indicating whether to mirror depth.
        /// </summary>
        public bool MirrorDepth
        {
            get
            {
                return this.mirrorDepth;
            }

            set
            {
                this.mirrorDepth = value;
                if (null != this.PropertyChanged)
                {
                    this.PropertyChanged.Invoke(this, new PropertyChangedEventArgs("MirrorDepth"));
                }

                this.resetReconstruction = true;
            }
        }

        /// <summary>
        /// Gets or sets a value indicating whether enable "Kinect View".
        /// </summary>
        public bool KinectView
        {
            get
            {
                return this.kinectView;
            }

            set
            {
                this.kinectView = value;

                if (null != this.PropertyChanged)
                {
                    this.PropertyChanged.Invoke(this, new PropertyChangedEventArgs("KinectView"));
                }

                // Decide whether render the volume cube
                if (this.kinectView)
                {
                    this.virtualCamera.CameraTransformationChanged -= this.OnVirtualCameraTransformationChanged;
                    this.virtualCamera.Detach(this.shadedSurfaceImage);

                    if (this.volumeGraphics)
                    {
                        // Do not render the frustum when in Kinect camera view with volume graphics active
                        this.virtualCamera.RemoveFrustum3DGraphics();
                    }
                }
                else
                {
                    this.virtualCamera.Attach(this.shadedSurfaceImage);
                    this.virtualCamera.CameraTransformationChanged += this.OnVirtualCameraTransformationChanged;

                    if (this.volumeGraphics)
                    {
                        // Re-render the frustum if we exit the Kinect camera view with volume graphics active
                        this.virtualCamera.AddFrustum3DGraphics();
                    }

                    // Reset the virtual camera
                    this.virtualCamera.Reset();
                }

                this.viewChanged = true;

                this.GraphicsViewport.InvalidateVisual();
            }
        }

        /// <summary>
        /// Gets or sets a value indicating whether to show volume graphics.
        /// </summary>
        public bool VolumeGraphics
        {
            get
            {
                return this.volumeGraphics;
            }

            set
            {
                this.volumeGraphics = value;

                if (null != this.PropertyChanged)
                {
                    this.PropertyChanged.Invoke(this, new PropertyChangedEventArgs("VolumeGraphics"));
                }

                if (this.volumeGraphics)
                {
                    // Add the graphics to the visual tree

                    // Create axis-aligned coordinate cross 3D graphics at the WPF3D/reconstruction world origin
                    // Red is the +X axis, Green is the +Y axis, Blue is the +Z axis in the WPF3D coordinate system
                    // Note that this coordinate cross shows the WPF3D graphics coordinate system
                    // (right hand, erect so +Y up and +X right, +Z out of screen), rather than the reconstruction 
                    // volume coordinate system (right hand, rotated so +Y is down and +X is right, +Z into screen ).
                    this.CreateAxisAlignedCoordinateCross3DGraphics(new Point3D(0, 0, 0), OriginCoordinateCrossAxisSize, LineThickness);

                    // Create volume cube 3D graphics in WPF3D. The front top left corner is the actual origin of the volume
                    // voxel coordinate system, and shown with an overlaid coordinate cross.
                    // Red is the +X axis, Green is the +Y axis, Blue is the +Z axis in the voxel coordinate system
                    this.CreateCube3DGraphics(volumeCubeLineColor, LineThickness, new Vector3D(0, 0, 0));

                    this.AddVolumeCube3DGraphics();
                    this.AddAxisAlignedCoordinateCross3DGraphics();

                    if (!this.kinectView)
                    {
                        // Do not render the frustum when in Kinect camera view with volume graphics active
                        this.virtualCamera.AddFrustum3DGraphics();
                    }

                    // Add callback which is called every time WPF renders
                    System.Windows.Media.CompositionTarget.Rendering += this.CompositionTargetRendering;
                }
                else
                {
                    // Remove the graphics from the visual tree
                    this.DisposeVolumeCube3DGraphics();
                    this.DisposeAxisAlignedCoordinateCross3DGraphics();

                    this.virtualCamera.RemoveFrustum3DGraphics();

                    // Remove callback which is called every time WPF renders
                    System.Windows.Media.CompositionTarget.Rendering -= this.CompositionTargetRendering;
                }

                this.viewChanged = true;

                this.GraphicsViewport.InvalidateVisual();
            }
        }

        /// <summary>
        /// Gets or sets a value indicating whether to use the camera pose finder.
        /// </summary>
        public bool UseCameraPoseFinder
        {
            get
            {
                return this.autoFindCameraPoseWhenLost;
            }

            set
            {
                this.autoFindCameraPoseWhenLost = value;
                if (null != this.PropertyChanged)
                {
                    this.PropertyChanged.Invoke(this, new PropertyChangedEventArgs("UseCameraPoseFinder"));
                }
            }
        }

        /// <summary>
        /// Gets or sets the minimum clip depth.
        /// </summary>
        public double MinDepthClip
        {
            get
            {
                return (double)this.minDepthClip;
            }

            set
            {
                this.minDepthClip = (float)value;
                if (null != this.PropertyChanged)
                {
                    this.PropertyChanged.Invoke(this, new PropertyChangedEventArgs("MinDepthClip"));
                }
            }
        }

        /// <summary>
        /// Gets or sets the maximum clip depth.
        /// </summary>
        public double MaxDepthClip
        {
            get
            {
                return (double)this.maxDepthClip;
            }

            set
            {
                this.maxDepthClip = (float)value;
                if (null != this.PropertyChanged)
                {
                    this.PropertyChanged.Invoke(this, new PropertyChangedEventArgs("MaxDepthClip"));
                }
            }
        }

        /// <summary>
        /// Gets or sets the integration weight.
        /// </summary>
        public double IntegrationWeight
        {
            get
            {
                return (double)this.integrationWeight;
            }

            set
            {
                this.integrationWeight = (short)(value + 0.5);
                if (null != this.PropertyChanged)
                {
                    this.PropertyChanged.Invoke(this, new PropertyChangedEventArgs("IntegrationWeight"));
                }
            }
        }

        /// <summary>
        /// Gets or sets the voxels per meter value.
        /// </summary>
        public double VoxelsPerMeter
        {
            get
            {
                return (double)this.voxelsPerMeter;
            }

            set
            {
                this.voxelsPerMeter = (float)value;
                if (null != this.PropertyChanged)
                {
                    this.PropertyChanged.Invoke(this, new PropertyChangedEventArgs("VoxelsPerMeter"));
                }
            }
        }

        /// <summary>
        /// Gets or sets the X-axis volume resolution.
        /// </summary>
        public double VoxelsX
        {
            get
            {
                return (double)this.voxelsX;
            }

            set
            {
                this.voxelsX = (int)(value + 0.5);
                if (null != this.PropertyChanged)
                {
                    this.PropertyChanged.Invoke(this, new PropertyChangedEventArgs("VoxelsX"));
                }
            }
        }

        /// <summary>
        /// Gets or sets the Y-axis volume resolution.
        /// </summary>
        public double VoxelsY
        {
            get
            {
                return (double)this.voxelsY;
            }

            set
            {
                this.voxelsY = (int)(value + 0.5);
                if (null != this.PropertyChanged)
                {
                    this.PropertyChanged.Invoke(this, new PropertyChangedEventArgs("VoxelsY"));
                }
            }
        }

        /// <summary>
        /// Gets or sets the Z-axis volume resolution.
        /// </summary>
        public double VoxelsZ
        {
            get
            {
                return (double)this.voxelsZ;
            }

            set
            {
                this.voxelsZ = (int)(value + 0.5);
                if (null != this.PropertyChanged)
                {
                    this.PropertyChanged.Invoke(this, new PropertyChangedEventArgs("VoxelsZ"));
                }
            }
        }

        /// <summary>
        /// Gets a value indicating whether rendering is overdue 
        /// (i.e. time interval since last render > RenderIntervalMilliseconds)
        /// </summary>
        public bool IsRenderOverdue
        {
            get
            {
                return (DateTime.UtcNow - this.lastRenderTimestamp).TotalMilliseconds >= RenderIntervalMilliseconds;
            }
        }

        #endregion

        /// <summary>
        /// Dispose resources
        /// </summary>
        public void Dispose()
        {
            this.Dispose(true);

            // This object will be cleaned up by the Dispose method.
            GC.SuppressFinalize(this);
        }

        /// <summary>
        /// Frees all memory associated with the ReconstructionVolume and FusionImageFrames.
        /// </summary>
        /// <param name="disposing">Whether the function was called from Dispose.</param>
        protected virtual void Dispose(bool disposing)
        {
            if (!this.disposed)
            {
                if (disposing)
                {
                    if (null != this.depthReadyEvent)
                    {
                        this.depthReadyEvent.Dispose();
                    }

                    if (null != this.colorReadyEvent)
                    {
                        this.colorReadyEvent.Dispose();
                    }

                    if (null != this.workerThreadStopEvent)
                    {
                        this.workerThreadStopEvent.Dispose();
                    }

                    this.RemoveVolumeCube3DGraphics();
                    this.DisposeVolumeCube3DGraphics();

                    this.RemoveAxisAlignedCoordinateCross3DGraphics();
                    this.DisposeAxisAlignedCoordinateCross3DGraphics();

                    if (null != this.virtualCamera)
                    {
                        this.virtualCamera.CameraTransformationChanged -= this.OnVirtualCameraTransformationChanged;
                        this.virtualCamera.Detach(this.shadedSurfaceImage);     // Stop getting mouse events from the image
                        this.virtualCamera.Dispose();
                    }

                    this.SafeDisposeFusionResources();

                    if (null != this.volume)
                    {
                        this.volume.Dispose();
                    }
                }
            }

            this.disposed = true;
        }

        /// <summary>
        /// Render Fusion color frame to UI
        /// </summary>
        /// <param name="colorFrame">Fusion color frame</param>
        /// <param name="colorPixels">Pixel buffer for fusion color frame</param>
        /// <param name="bitmap">Bitmap contains color frame data for rendering</param>
        /// <param name="image">UI image component to render the color frame</param>
        private static void RenderColorImage(FusionColorImageFrame colorFrame, ref int[] colorPixels, ref WriteableBitmap bitmap, System.Windows.Controls.Image image)
        {
            if (null == image || null == colorFrame)
            {
                return;
            }

            if (null == colorPixels || colorFrame.PixelDataLength != colorPixels.Length)
            {
                // Create pixel array of correct format
                colorPixels = new int[colorFrame.PixelDataLength];
            }

            if (null == bitmap || colorFrame.Width != bitmap.Width || colorFrame.Height != bitmap.Height)
            {
                // Create bitmap of correct format
                bitmap = new WriteableBitmap(colorFrame.Width, colorFrame.Height, 96.0, 96.0, PixelFormats.Bgr32, null);

                // Set bitmap as source to UI image object
                image.Source = bitmap;
            }

            // Copy pixel data to pixel buffer
            colorFrame.CopyPixelDataTo(colorPixels);

            // Write pixels to bitmap
            bitmap.WritePixels(
                        new Int32Rect(0, 0, colorFrame.Width, colorFrame.Height),
                        colorPixels,
                        bitmap.PixelWidth * sizeof(int),
                        0);
        }

        /// <summary>
        /// Execute startup tasks
        /// </summary>
        /// <param name="sender">object sending the event</param>
        /// <param name="e">event arguments</param>
        private void WindowLoaded(object sender, RoutedEventArgs e)
        {
            int deviceMemoryKB = 0;

            // Check to ensure suitable DirectX11 compatible hardware exists before initializing Kinect Fusion
            try
            {
                string deviceDescription = string.Empty;
                string deviceInstancePath = string.Empty;

                FusionDepthProcessor.GetDeviceInfo(ProcessorType, DeviceToUse, out deviceDescription, out deviceInstancePath, out deviceMemoryKB);
            }
            catch (IndexOutOfRangeException)
            {
                // Thrown when index is out of range for processor type or there is no DirectX11 capable device installed.
                // As we set -1 (auto-select default) for the DeviceToUse above, this indicates that there is no DirectX11 
                // capable device. The options for users in this case are to either install a DirectX11 capable device 
                // (see documentation for recommended GPUs) or to switch to non-real-time CPU based reconstruction by 
                // changing ProcessorType to ReconstructionProcessor.Cpu
                this.statusBarText.Text = Properties.Resources.NoDirectX11CompatibleDeviceOrInvalidDeviceIndex;
                return;
            }
            catch (DllNotFoundException)
            {
                this.statusBarText.Text = Properties.Resources.MissingPrerequisite;
                return;
            }
            catch (InvalidOperationException ex)
            {
                this.statusBarText.Text = ex.Message;
                return;
            }

            VoxelsXSlider.Maximum = 512;
            VoxelsYSlider.Maximum = 512;
            VoxelsZSlider.Maximum = 512;

            // One sensor is supported
            this.sensor = KinectSensor.GetDefault();

            if (null == this.sensor)
            {
                this.statusBarText.Text = Properties.Resources.NoReadyKinect;
                return;
            }

            // get the coordinate mapper
            this.mapper = this.sensor.CoordinateMapper;

            // open the sensor
            this.sensor.Open();

            this.reader = this.sensor.OpenMultiSourceFrameReader(FrameSourceTypes.Depth | FrameSourceTypes.Color);

            FrameDescription depthFrameDescription = this.sensor.DepthFrameSource.FrameDescription;
            this.depthWidth = depthFrameDescription.Width;
            this.depthHeight = depthFrameDescription.Height;
            this.depthPixelCount = this.depthWidth * this.depthHeight;

            FrameDescription colorFrameDescription = this.sensor.ColorFrameSource.FrameDescription;
            this.colorWidth = colorFrameDescription.Width;
            this.colorHeight = colorFrameDescription.Height;
            this.colorPixelCount = this.colorWidth * this.colorHeight;

            this.depthVisibilityTestMapWidth = this.colorWidth / ColorDownsampleFactor;
            this.depthVisibilityTestMapHeight = this.colorHeight / ColorDownsampleFactor;
            this.depthVisibilityTestMap = new ushort[this.depthVisibilityTestMapWidth * this.depthVisibilityTestMapHeight];

            // Setup the graphics rendering

            // Create virtualCamera for non-Kinect viewpoint rendering
            // Default position is translated along Z axis, looking back at the origin
            this.virtualCameraStartTranslation = new Point3D(0, 0, this.voxelsZ / this.voxelsPerMeter);
            this.virtualCamera = new GraphicsCamera(this.virtualCameraStartTranslation, this.virtualCameraStartRotation, (float)Width / (float)Height);

            // Attach this virtual camera to the viewport
            this.GraphicsViewport.Camera = this.virtualCamera.Camera;

            // Start worker thread for depth processing
            this.StartWorkerThread();

            // Start fps timer
            this.fpsTimer = new DispatcherTimer(DispatcherPriority.Send);
            this.fpsTimer.Interval = new TimeSpan(0, 0, FpsInterval);
            this.fpsTimer.Tick += this.FpsTimerTick;
            this.fpsTimer.Start();

            // Set last fps timestamp as now
            this.lastFPSTimestamp = DateTime.UtcNow;

            // Start status bar timer
            this.statusBarTimer = new DispatcherTimer(DispatcherPriority.Send);
            this.statusBarTimer.Interval = new TimeSpan(0, 0, StatusBarInterval);
            this.statusBarTimer.Tick += this.StatusBarTimerTick;
            this.statusBarTimer.Start();

            this.lastStatusTimestamp = DateTime.Now;

            // Add an event handler to be called whenever depth and color both have new data
            this.reader.MultiSourceFrameArrived += this.Reader_MultiSourceFrameArrived;

            // Allocate frames for Kinect Fusion now a sensor is present
            this.AllocateKinectFusionResources();

            // Create the camera frustum 3D graphics in WPF3D
            this.virtualCamera.CreateFrustum3DGraphics(this.GraphicsViewport, this.depthWidth, this.depthHeight);

            // Set recreate reconstruction flag
            this.recreateReconstruction = true;

            // Show introductory message
            this.ShowStatusMessage(Properties.Resources.IntroductoryMessage);
        }

        /// <summary>
        /// Execute shutdown tasks
        /// </summary>
        /// <param name="sender">Object sending the event</param>
        /// <param name="e">Event arguments</param>
        private void WindowClosing(object sender, System.ComponentModel.CancelEventArgs e)
        {
            // Stop timer
            if (null != this.fpsTimer)
            {
                this.fpsTimer.Stop();
                this.fpsTimer.Tick -= this.FpsTimerTick;
            }

            if (null != this.statusBarTimer)
            {
                this.statusBarTimer.Stop();
                this.statusBarTimer.Tick -= this.StatusBarTimerTick;
            }

            if (this.reader != null)
            {
                this.reader.Dispose();
                this.reader = null;
            }

            if (null != this.sensor)
            {
                this.sensor.Close();
                this.sensor = null;
            }

            // Remove the camera frustum 3D graphics from WPF3D
            this.virtualCamera.DisposeFrustum3DGraphics();

            // Stop worker thread
            this.StopWorkerThread();
        }

        /// <summary>
        /// Handler for FPS timer tick
        /// </summary>
        /// <param name="sender">Object sending the event</param>
        /// <param name="e">Event arguments</param>
        private void FpsTimerTick(object sender, EventArgs e)
        {
            if (!this.savingMesh)
            {
                if (null == this.sensor)
                {
                    // Show "No ready Kinect found!" on status bar
                    this.statusBarText.Text = Properties.Resources.NoReadyKinect;
                }
                else
                {
                    // Calculate time span from last calculation of FPS
                    double intervalSeconds = (DateTime.UtcNow - this.lastFPSTimestamp).TotalSeconds;

                    // Calculate and show fps on status bar
                    this.fpsText.Text = string.Format(
                        System.Globalization.CultureInfo.InvariantCulture,
                        Properties.Resources.Fps,
                        (double)this.processedFrameCount / intervalSeconds);
                }
            }

            // Reset frame counter
            this.processedFrameCount = 0;
            this.lastFPSTimestamp = DateTime.UtcNow;
        }

        /// <summary>
        /// Reset FPS timer and counter
        /// </summary>
        private void ResetFps()
        {
            // Restart fps timer
            if (null != this.fpsTimer)
            {
                this.fpsTimer.Stop();
                this.fpsTimer.Start();
            }

            // Reset frame counter
            this.processedFrameCount = 0;
            this.lastFPSTimestamp = DateTime.UtcNow;
        }

        /// <summary>
        /// Handler for status bar timer tick
        /// </summary>
        /// <param name="sender">Object sending the event</param>
        /// <param name="e">Event arguments</param>
        private void StatusBarTimerTick(object sender, EventArgs e)
        {
            if (this.statusMessageQueue.Count > 0)
            {
                this.statusBarText.Text = this.statusMessageQueue.Dequeue();

                // Update the last timestamp of status message
                this.lastStatusTimestamp = DateTime.Now;
            }
        }

        /// <summary>
        /// Start the work thread to process incoming depth data
        /// </summary>
        private void StartWorkerThread()
        {
            if (null == this.workerThread)
            {
                // Initialize events
                this.depthReadyEvent = new ManualResetEvent(false);
                this.colorReadyEvent = new ManualResetEvent(false);
                this.workerThreadStopEvent = new ManualResetEvent(false);

                // Create worker thread and start
                this.workerThread = new Thread(this.WorkerThreadProc);
                this.workerThread.Start();
            }
        }

        /// <summary>
        /// Stop worker thread
        /// </summary>
        private void StopWorkerThread()
        {
            if (null != this.workerThread)
            {
                // Set stop event to stop thread
                this.workerThreadStopEvent.Set();

                // Wait for exit of thread
                this.workerThread.Join();
            }
        }

        /// <summary>
        /// Worker thread in which depth data is processed
        /// </summary>
        private void WorkerThreadProc()
        {
            WaitHandle[] events = new WaitHandle[2] { this.workerThreadStopEvent, this.depthReadyEvent };
            while (true)
            {
                int index = WaitHandle.WaitAny(events);

                if (0 == index)
                {
                    // Stop event has been set. Exit thread
                    break;
                }

                // Reset depth ready event
                this.depthReadyEvent.Reset();

                // Pass data to process
                this.Process();
            }
        }

        /// <summary>
        /// Event handler for multiSourceFrame arrived event
        /// </summary>
        /// <param name="sender">object sending the event</param>
        /// <param name="e">event arguments</param>
        private void Reader_MultiSourceFrameArrived(object sender, MultiSourceFrameArrivedEventArgs e)
        {
            bool validDepth = false;
            bool validColor = false;

            MultiSourceFrameReference frameReference = e.FrameReference;

            MultiSourceFrame multiSourceFrame = null;
            DepthFrame depthFrame = null;
            ColorFrame colorFrame = null;

            try
            {
                multiSourceFrame = frameReference.AcquireFrame();

                if (multiSourceFrame != null)
                {
                    // MultiSourceFrame is IDisposable
                    lock (this.rawDataLock)
                    {
                        ColorFrameReference colorFrameReference = multiSourceFrame.ColorFrameReference;
                        DepthFrameReference depthFrameReference = multiSourceFrame.DepthFrameReference;

                        colorFrame = colorFrameReference.AcquireFrame();
                        depthFrame = depthFrameReference.AcquireFrame();

                        if ((depthFrame != null) && (colorFrame != null))
                        {
                            // Save frame timestamp
                            this.RelativeTime = depthFrame.RelativeTime;

                            FrameDescription colorFrameDescription = colorFrame.FrameDescription;
                            int colorWidth = colorFrameDescription.Width;
                            int colorHeight = colorFrameDescription.Height;

                            if ((colorWidth * colorHeight * sizeof(int)) == this.colorImagePixels.Length)
                            {
                                colorFrame.CopyConvertedFrameDataToArray(this.colorImagePixels, ColorImageFormat.Bgra);

                                validColor = true;
                            }

                            FrameDescription depthFrameDescription = depthFrame.FrameDescription;
                            int depthWidth = depthFrameDescription.Width;
                            int depthHeight = depthFrameDescription.Height;

                            if ((depthWidth * depthHeight) == this.depthImagePixels.Length)
                            {
                                depthFrame.CopyFrameDataToArray(this.depthImagePixels);

                                validDepth = true;
                            }
                        }
                    }
                }
            }
            catch (Exception)
            {
                // ignore if the frame is no longer available
            }
            finally
            {
                // MultiSourceFrame, DepthFrame, ColorFrame, BodyIndexFrame are IDispoable
                if (depthFrame != null)
                {
                    depthFrame.Dispose();
                    depthFrame = null;
                }

                if (colorFrame != null)
                {
                    colorFrame.Dispose();
                    colorFrame = null;
                }

                if (multiSourceFrame != null)
                {
                    multiSourceFrame = null;
                }
            }

            if (validDepth)
            {
                // Signal worker thread to process
                this.depthReadyEvent.Set();
            }

            if (validColor)
            {
                // Signal worker thread to process
                this.colorReadyEvent.Set();
            }
        }

        /// <summary>
        /// The main Kinect Fusion process function
        /// </summary>
        private void Process()
        {
            if (this.recreateReconstruction)
            {
                lock (this.volumeLock)
                {
                    this.recreateReconstruction = false; 
                    this.RecreateReconstruction();
                }
            }

            if (this.resetReconstruction)
            {
                this.resetReconstruction = false;
                this.ResetReconstruction();
            }

            if (null != this.volume && !this.savingMesh)
            {
                try
                {
                    // Check if camera pose finder is available
                    this.cameraPoseFinderAvailable = this.IsCameraPoseFinderAvailable();

                    // Convert depth to float and render depth frame
                    this.ProcessDepthData();

                    // Track camera pose
                    this.TrackCamera();

                    // Only continue if we do not have tracking errors
                    if (0 == this.trackingErrorCount)
                    {
                        // Integrate depth
                        bool colorAvailable = this.IntegrateData();

                        // Check to see if another depth frame is already available. 
                        // If not we have time to calculate a point cloud and render, 
                        // but if so we make sure we force a render at least every 
                        // RenderIntervalMilliseconds.
                        if (!this.depthReadyEvent.WaitOne(0) || this.IsRenderOverdue)
                        {
                            // Raycast and render
                            this.RenderReconstruction();
                        }

                        // Update camera pose finder, adding key frames to the database
                        if (this.autoFindCameraPoseWhenLost && !this.trackingHasFailedPreviously
                            && this.successfulFrameCount > MinSuccessfulTrackingFramesForCameraPoseFinder
                            && this.processedFrameCount % CameraPoseFinderProcessFrameCalculationInterval == 0
                            && colorAvailable)
                        {
                            this.UpdateCameraPoseFinder();
                        }
                    }
                }
                catch (InvalidOperationException ex)
                {
                    this.ShowStatusMessage(ex.Message);
                }
            }
        }

        /// <summary>
        /// Is the camera pose finder initialized and running.
        /// </summary>
        /// <returns>Returns true if available, otherwise false</returns>
        private bool IsCameraPoseFinderAvailable()
        {
            return this.autoFindCameraPoseWhenLost
                && null != this.cameraPoseFinder
                && this.cameraPoseFinder.GetStoredPoseCount() > 0;
        }

        /// <summary>
        /// Process the depth input for camera tracking
        /// </summary>
        private void ProcessDepthData()
        {
            // To enable playback of a .xef file through Kinect Studio and reset of the reconstruction
            // if the .xef loops, we test for when the frame timestamp has skipped a large number. 
            // Note: this will potentially continually reset live reconstructions on slow machines which
            // cannot process a live frame in less time than the reset threshold. Increase the number of
            // milliseconds if this is a problem.
            if (this.autoResetReconstructionOnTimeSkip)
            {
                this.CheckResetTimeStamp(this.RelativeTime);
            }

            // Lock the depth operations
            lock (this.rawDataLock)
            {
                this.volume.DepthToDepthFloatFrame(
                    this.depthImagePixels,
                    this.depthFloatFrame,
                    this.minDepthClip,
                    this.maxDepthClip,
                    this.MirrorDepth);
            }

            // Render depth float frame
            this.Dispatcher.BeginInvoke(
                (Action)
                (() =>
                    this.RenderDepthFloatImage(
                        ref this.depthFloatFrameBitmap,
                        this.depthFloatImage)));
        }

        /// <summary>
        /// Track camera pose by aligning depth float image with reconstruction volume
        /// </summary>
        /// <param name="calculateDeltaFrame">Flag to calculate the delta frame.</param>
        /// <param name="calculatedCameraPos">The calculated camera position.</param>
        /// <returns>Returns true if tracking succeeded, false otherwise.</returns>
        private bool TrackCameraAlignDepthFloatToReconstruction(bool calculateDeltaFrame, ref Matrix4 calculatedCameraPos)
        {
            bool trackingSucceeded = false;

            // Note that here we only calculate the deltaFromReferenceFrame every 
            // DeltaFrameCalculationInterval frames to reduce computation time
            if (calculateDeltaFrame)
            {
                trackingSucceeded = this.volume.AlignDepthFloatToReconstruction(
                    this.depthFloatFrame,
                    FusionDepthProcessor.DefaultAlignIterationCount,
                    this.deltaFromReferenceFrame,
                    out this.alignmentEnergy,
                    this.worldToCameraTransform);
            }
            else
            {
                // Don't bother getting the residual delta from reference frame to cut computation time
                trackingSucceeded = this.volume.AlignDepthFloatToReconstruction(
                    this.depthFloatFrame,
                    FusionDepthProcessor.DefaultAlignIterationCount,
                    null,
                    out this.alignmentEnergy,
                    this.worldToCameraTransform);
            }

            if (!trackingSucceeded || this.alignmentEnergy > MaxAlignToReconstructionEnergyForSuccess || (this.alignmentEnergy <= MinAlignToReconstructionEnergyForSuccess && this.successfulFrameCount > 0))
            {
                trackingSucceeded = false;
            }
            else
            {
                // Tracking succeeded, get the updated camera pose
                calculatedCameraPos = this.volume.GetCurrentWorldToCameraTransform();
            }

            return trackingSucceeded;
        }

        /// <summary>
        /// Track camera pose using AlignPointClouds
        /// </summary>
        /// <param name="calculateDeltaFrame">A flag to indicate it is time to calculate the delta frame.</param>
        /// <param name="calculatedCameraPose">The calculated camera pose.</param>
        /// <returns>Returns true if tracking succeeded, false otherwise.</returns>
        private bool TrackCameraAlignPointClouds(ref bool calculateDeltaFrame, ref Matrix4 calculatedCameraPose)
        {
            bool trackingSucceeded = false;

            this.DownsampleDepthFrameNearestNeighbor(this.downsampledDepthFloatFrame, DownsampleFactor);

            // Smooth the depth frame
            this.volume.SmoothDepthFloatFrame(this.downsampledDepthFloatFrame, this.downsampledSmoothDepthFloatFrame, SmoothingKernelWidth, SmoothingDistanceThreshold);

            // Calculate point cloud from the smoothed frame
            FusionDepthProcessor.DepthFloatFrameToPointCloud(this.downsampledSmoothDepthFloatFrame, this.downsampledDepthPointCloudFrame);

            // Get the saved pose view by raycasting the volume from the current camera pose
            this.volume.CalculatePointCloud(this.downsampledRaycastPointCloudFrame, calculatedCameraPose);

            Matrix4 initialPose = calculatedCameraPose;

            // Note that here we only calculate the deltaFromReferenceFrame every 
            // DeltaFrameCalculationInterval frames to reduce computation time
            if (calculateDeltaFrame)
            {
                trackingSucceeded = FusionDepthProcessor.AlignPointClouds(
                    this.downsampledRaycastPointCloudFrame,
                    this.downsampledDepthPointCloudFrame,
                    FusionDepthProcessor.DefaultAlignIterationCount,
                    this.downsampledDeltaFromReferenceFrameColorFrame,
                    ref calculatedCameraPose);

                this.UpsampleColorDeltasFrameNearestNeighbor(DownsampleFactor);

                this.UpdateAlignDeltas();

                // Set calculateDeltaFrame to false as we are rendering it here
                calculateDeltaFrame = false;
            }
            else
            {
                // Don't bother getting the residual delta from reference frame to cut computation time
                trackingSucceeded = FusionDepthProcessor.AlignPointClouds(
                    this.downsampledRaycastPointCloudFrame,
                    this.downsampledDepthPointCloudFrame,
                    FusionDepthProcessor.DefaultAlignIterationCount,
                    null,
                    ref calculatedCameraPose);
            }

            if (trackingSucceeded)
            {
                bool failed = KinectFusionHelper.CameraTransformFailed(
                    initialPose, 
                    calculatedCameraPose,
                    MaxTranslationDeltaAlignPointClouds,
                    MaxRotationDeltaAlignPointClouds);

                if (failed)
                {
                    trackingSucceeded = false;
                }
            }

            return trackingSucceeded;
        }

        /// <summary>
        /// Track the camera pose
        /// </summary>
        private void TrackCamera()
        {
            bool calculateDeltaFrame = this.processedFrameCount % DeltaFrameCalculationInterval == 0;
            bool trackingSucceeded = false;

            // Get updated camera transform from image alignment
            Matrix4 calculatedCameraPos = this.worldToCameraTransform;

            // Here we can either call TrackCameraAlignDepthFloatToReconstruction or TrackCameraAlignPointClouds
            // The TrackCameraAlignPointClouds function typically has higher performance with the camera pose finder 
            // due to its wider basin of convergence, enabling it to more robustly regain tracking from nearby poses
            // suggested by the camera pose finder after tracking is lost.
            if (this.autoFindCameraPoseWhenLost)
            {
                // Track using AlignPointClouds
                trackingSucceeded = this.TrackCameraAlignPointClouds(ref calculateDeltaFrame, ref calculatedCameraPos);
            }
            else
            {
                // Track using AlignDepthFloatToReconstruction
                trackingSucceeded = this.TrackCameraAlignDepthFloatToReconstruction(calculateDeltaFrame, ref calculatedCameraPos);
            }

            if (!trackingSucceeded && 0 != this.successfulFrameCount)
            {
                this.SetTrackingFailed();

                if (!this.cameraPoseFinderAvailable)
                {
                    // Show tracking error on status bar
                    this.ShowStatusMessageLowPriority(Properties.Resources.CameraTrackingFailed);
                }
                else
                {
                    // Here we try to find the correct camera pose, to re-localize camera tracking.
                    // We can call either the version using AlignDepthFloatToReconstruction or the
                    // version using AlignPointClouds, which typically has a higher success rate.
                    // trackingSucceeded = this.FindCameraPoseAlignDepthFloatToReconstruction();
                    trackingSucceeded = this.FindCameraPoseAlignPointClouds();

                    if (!trackingSucceeded)
                    {
                        // Show tracking error on status bar
                        this.ShowStatusMessageLowPriority(Properties.Resources.CameraTrackingFailed);
                    }
                }
            }
            else
            {
                if (this.trackingHasFailedPreviously)
                {
                    this.ShowStatusMessageLowPriority("Kinect Fusion camera tracking RECOVERED! Residual energy=" + string.Format(CultureInfo.InvariantCulture, "{0:0.00000}", this.alignmentEnergy));
                }

                this.UpdateAlignDeltas();

                this.SetTrackingSucceeded();

                this.worldToCameraTransform = calculatedCameraPos;
            }

            if (AutoResetReconstructionWhenLost && !trackingSucceeded
                && this.trackingErrorCount >= MaxTrackingErrors)
            {
                // Bad tracking
                this.ShowStatusMessage(Properties.Resources.ResetVolumeAuto);

                // Automatically Clear Volume and reset tracking if tracking fails
                this.ResetReconstruction();
            }

            if (trackingSucceeded)
            {
                if (this.kinectView)
                {
                    Dispatcher.BeginInvoke((Action)(() => this.UpdateVirtualCameraTransform()));
                }
                else
                {
                    // Just update the frustum
                    Dispatcher.BeginInvoke((Action)(() => this.virtualCamera.UpdateFrustumTransformMatrix4(this.worldToCameraTransform)));
                }

                // Increase processed frame counter
                this.processedFrameCount++;
            }
        }

        /// <summary>
        /// Set variables if camera tracking succeeded
        /// </summary>
        private void SetTrackingFailed()
        {
            // Clear successful frame count and increment the track error count
            this.trackingFailed = true;
            this.trackingHasFailedPreviously = true;
            this.trackingErrorCount++;
            this.successfulFrameCount = 0;
        }

        /// <summary>
        /// Set variables if camera tracking succeeded
        /// </summary>
        private void SetTrackingSucceeded()
        {
            // Clear track error count and increment the successful frame count
            this.trackingFailed = false;
            this.trackingErrorCount = 0;
            this.successfulFrameCount++;
        }

        /// <summary>
        /// Reset tracking variables
        /// </summary>
        private void ResetTracking()
        {
            this.trackingFailed = false;
            this.trackingHasFailedPreviously = false;
            this.trackingErrorCount = 0;
            this.successfulFrameCount = 0;

            // Reset pause and signal that the integration resumed
            this.PauseIntegration = false;

            if (null != this.cameraPoseFinder)
            {
                this.cameraPoseFinder.ResetCameraPoseFinder();
            }
        }

        /// <summary>
        /// Process input color image to make it equal in size to the depth image
        /// </summary>
        private unsafe void ProcessColorForCameraPoseFinder()
        {
            if (null == this.resampledColorFrame || null == this.downsampledDepthImagePixels)
            {
                throw new ArgumentException("inputs null");
            }

            if (this.depthWidth != RawDepthWidth || this.depthHeight != RawDepthHeight
                || this.colorWidth != RawColorWidth || this.colorHeight != RawColorHeight)
            {
                return;
            }

            float factor = RawColorWidth / RawDepthHeightWithSpecialRatio;
            const int FilledZeroMargin = (RawDepthHeight - RawDepthHeightWithSpecialRatio) / 2;

            // Here we make use of unsafe code to just copy the whole pixel as an int for performance reasons, as we do
            // not need access to the individual rgba components.
            fixed (byte* ptrColorPixels = this.colorImagePixels)
            {
                int* rawColorPixels = (int*)ptrColorPixels;

                Parallel.For(
                    FilledZeroMargin,
                    this.depthHeight - FilledZeroMargin,
                    y =>
                    {
                        int destIndex = y * this.depthWidth;

                        for (int x = 0; x < this.depthWidth; ++x, ++destIndex)
                        {
                            int srcX = (int)(x * factor);
                            int srcY = (int)(y * factor);
                            int sourceColorIndex = (srcY * this.colorWidth) + srcX;

                            this.resampledColorImagePixels[destIndex] = rawColorPixels[sourceColorIndex];
                        }
                    });
            }

            this.resampledColorFrame.CopyPixelDataFrom(this.resampledColorImagePixels);
        }

        /// <summary>
        /// This is used to set the reference frame.
        /// </summary>
        /// <param name="pose">The pose to use.</param>
        private void SetReferenceFrame(Matrix4 pose)
        {
            // Get the saved pose view by raycasting the volume
            this.volume.CalculatePointCloudAndDepth(this.raycastPointCloudFrame, this.smoothDepthFloatFrame, null, pose);

            // Set this as the reference frame for the next call to AlignDepthFloatToReconstruction
            this.volume.SetAlignDepthFloatToReconstructionReferenceFrame(this.smoothDepthFloatFrame);
        }

        /// <summary>
        /// Perform camera pose finding when tracking is lost using AlignPointClouds.
        /// This is typically more successful than FindCameraPoseAlignDepthFloatToReconstruction.
        /// </summary>
        /// <returns>Returns true if a valid camera pose was found, otherwise false.</returns>
        private bool FindCameraPoseAlignPointClouds()
        {
            if (!this.cameraPoseFinderAvailable)
            {
                return false;
            }

            this.ProcessColorForCameraPoseFinder();

            MatchCandidates matchCandidates = this.cameraPoseFinder.FindCameraPose(
                this.depthFloatFrame,
                this.resampledColorFrame);

            if (null == matchCandidates)
            {
                return false;
            }

            int poseCount = matchCandidates.GetPoseCount();
            float minDistance = matchCandidates.CalculateMinimumDistance();

            if (0 == poseCount || minDistance >= CameraPoseFinderDistanceThresholdReject)
            {
                this.ShowStatusMessage(Properties.Resources.PoseFinderNotEnoughMatches);
                return false;
            }

            // Smooth the depth frame
            this.volume.SmoothDepthFloatFrame(this.depthFloatFrame, this.smoothDepthFloatFrame, SmoothingKernelWidth, SmoothingDistanceThreshold);

            // Calculate point cloud from the smoothed frame
            FusionDepthProcessor.DepthFloatFrameToPointCloud(this.smoothDepthFloatFrame, this.depthPointCloudFrame);

            double smallestEnergy = double.MaxValue;
            int smallestEnergyNeighborIndex = -1;

            int bestNeighborIndex = -1;
            Matrix4 bestNeighborCameraPose = Matrix4.Identity;

            double bestNeighborAlignmentEnergy = MaxAlignPointCloudsEnergyForSuccess;

            // Run alignment with best matched poseCount (i.e. k nearest neighbors (kNN))
            int maxTests = Math.Min(MaxCameraPoseFinderPoseTests, poseCount);

            var neighbors = matchCandidates.GetMatchPoses();

            for (int n = 0; n < maxTests; n++)
            {
                // Run the camera tracking algorithm with the volume
                // this uses the raycast frame and pose to find a valid camera pose by matching the raycast against the input point cloud
                Matrix4 poseProposal = neighbors[n];

                // Get the saved pose view by raycasting the volume
                this.volume.CalculatePointCloud(this.raycastPointCloudFrame, poseProposal);

                bool success = this.volume.AlignPointClouds(
                    this.raycastPointCloudFrame,
                    this.depthPointCloudFrame,
                    FusionDepthProcessor.DefaultAlignIterationCount,
                    this.resampledColorFrame,
                    out this.alignmentEnergy,
                    ref poseProposal);

                bool relocSuccess = success && this.alignmentEnergy < bestNeighborAlignmentEnergy && this.alignmentEnergy > MinAlignPointCloudsEnergyForSuccess;

                if (relocSuccess)
                {
                    bestNeighborAlignmentEnergy = this.alignmentEnergy;
                    bestNeighborIndex = n;

                    // This is after tracking succeeds, so should be a more accurate pose to store...
                    bestNeighborCameraPose = poseProposal;

                    // Update the delta image
                    this.resampledColorFrame.CopyPixelDataTo(this.deltaFromReferenceFramePixelsArgb);
                }

                // Find smallest energy neighbor independent of tracking success
                if (this.alignmentEnergy < smallestEnergy)
                {
                    smallestEnergy = this.alignmentEnergy;
                    smallestEnergyNeighborIndex = n;
                }
            }

            matchCandidates.Dispose();

            // Use the neighbor with the smallest residual alignment energy
            // At the cost of additional processing we could also use kNN+Mean camera pose finding here
            // by calculating the mean pose of the best n matched poses and also testing this to see if the 
            // residual alignment energy is less than with kNN.
            if (bestNeighborIndex > -1)
            {
                this.worldToCameraTransform = bestNeighborCameraPose;
                this.SetReferenceFrame(this.worldToCameraTransform);

                // Tracking succeeded!
                this.SetTrackingSucceeded();

                this.UpdateAlignDeltas();

                this.ShowStatusMessageLowPriority("Camera Pose Finder SUCCESS! Residual energy= " + string.Format(CultureInfo.InvariantCulture, "{0:0.00000}", bestNeighborAlignmentEnergy) + ", " + poseCount + " frames stored, minimum distance=" + minDistance + ", best match index=" + bestNeighborIndex);

                return true;
            }
            else
            {
                this.worldToCameraTransform = neighbors[smallestEnergyNeighborIndex];
                this.SetReferenceFrame(this.worldToCameraTransform);

                // Camera pose finding failed - return the tracking failed error code
                this.SetTrackingFailed();

                // Tracking Failed will be set again on the next iteration in ProcessDepth
                this.ShowStatusMessageLowPriority("Camera Pose Finder FAILED! Residual energy=" + string.Format(CultureInfo.InvariantCulture, "{0:0.00000}", smallestEnergy) + ", " + poseCount + " frames stored, minimum distance=" + minDistance + ", best match index=" + smallestEnergyNeighborIndex);

                return false;
            }
        }

        /// <summary>
        /// Perform camera pose finding when tracking is lost using AlignDepthFloatToReconstruction.
        /// </summary>
        /// <returns>Returns true if a valid camera pose was found, otherwise false.</returns>
        private bool FindCameraPoseAlignDepthFloatToReconstruction()
        {
            if (!this.cameraPoseFinderAvailable)
            {
                return false;
            }

            this.ProcessColorForCameraPoseFinder();

            MatchCandidates matchCandidates = this.cameraPoseFinder.FindCameraPose(
                this.depthFloatFrame,
                this.resampledColorFrame);

            if (null == matchCandidates)
            {
                return false;
            }

            int poseCount = matchCandidates.GetPoseCount();
            float minDistance = matchCandidates.CalculateMinimumDistance();

            if (0 == poseCount || minDistance >= CameraPoseFinderDistanceThresholdReject)
            {
                this.ShowStatusMessage(Properties.Resources.PoseFinderNotEnoughMatches);
                return false;
            }

            double smallestEnergy = double.MaxValue;
            int smallestEnergyNeighborIndex = -1;

            int bestNeighborIndex = -1;
            Matrix4 bestNeighborCameraPose = Matrix4.Identity;

            double bestNeighborAlignmentEnergy = MaxAlignToReconstructionEnergyForSuccess;

            // Run alignment with best matched poseCount (i.e. k nearest neighbors (kNN))
            int maxTests = Math.Min(MaxCameraPoseFinderPoseTests, poseCount);

            var neighbors = matchCandidates.GetMatchPoses();

            for (int n = 0; n < maxTests; n++)
            {
                // Run the camera tracking algorithm with the volume
                // this uses the raycast frame and pose to find a valid camera pose by matching the depth against the volume
                this.SetReferenceFrame(neighbors[n]);

                bool success = this.volume.AlignDepthFloatToReconstruction(
                    this.depthFloatFrame,
                    FusionDepthProcessor.DefaultAlignIterationCount,
                    this.deltaFromReferenceFrame,
                    out this.alignmentEnergy,
                    neighbors[n]);

                // Exclude very tiny alignment energy case which is unlikely to happen in reality - this is more likely a tracking error
                bool relocSuccess = success && this.alignmentEnergy < bestNeighborAlignmentEnergy && this.alignmentEnergy > MinAlignToReconstructionEnergyForSuccess;
        
                if (relocSuccess)
                {
                    bestNeighborAlignmentEnergy = this.alignmentEnergy;
                    bestNeighborIndex = n;

                    // This is after tracking succeeds, so should be a more accurate pose to store...
                    bestNeighborCameraPose = this.volume.GetCurrentWorldToCameraTransform();
                }

                // Find smallest energy neighbor independent of tracking success
                if (this.alignmentEnergy < smallestEnergy)
                {
                    smallestEnergy = this.alignmentEnergy;
                    smallestEnergyNeighborIndex = n;
                }
            }

            matchCandidates.Dispose();

            // Use the neighbor with the smallest residual alignment energy
            // At the cost of additional processing we could also use kNN+Mean camera pose finding here
            // by calculating the mean pose of the best n matched poses and also testing this to see if the 
            // residual alignment energy is less than with kNN.
            if (bestNeighborIndex > -1)
            {
                this.worldToCameraTransform = bestNeighborCameraPose;
                this.SetReferenceFrame(this.worldToCameraTransform);

                // Tracking succeeded!
                this.SetTrackingSucceeded();

                this.ShowStatusMessage("Camera Pose Finder SUCCESS! Residual energy= " + bestNeighborAlignmentEnergy + ", " + poseCount + " frames stored, minimum distance=" + minDistance + ", best match index=" + bestNeighborIndex);

                return true;
            }
            else
            {
                this.worldToCameraTransform = neighbors[smallestEnergyNeighborIndex];
                this.SetReferenceFrame(this.worldToCameraTransform);

                // Camera pose finding failed - return the tracking failed error code
                this.SetTrackingFailed();

                // Tracking Failed will be set again on the next iteration in ProcessDepth
                this.ShowStatusMessage("Camera Pose Finder FAILED! Residual energy=" + smallestEnergy + ", " + poseCount + " frames stored, minimum distance=" + minDistance + ", best match index=" + smallestEnergyNeighborIndex);

                return false;
            }
        }

        /// <summary>
        /// Process the color and depth inputs, converting the color into the depth space
        /// </summary>
        private unsafe void MapColorToDepth()
        {
            this.mapper.MapDepthFrameToColorSpace(this.depthImagePixels, this.colorCoordinates);

            lock (this.rawDataLock)
            {
                // Fill in the visibility depth map.
                Array.Clear(this.depthVisibilityTestMap, 0, this.depthVisibilityTestMap.Length);
                fixed (ushort* ptrDepthVisibilityPixels = this.depthVisibilityTestMap, ptrDepthPixels = this.depthImagePixels)
                {
                    for (int index = 0; index < this.depthImagePixels.Length; ++index)
                    {
                        if (!float.IsInfinity(this.colorCoordinates[index].X) && !float.IsInfinity(this.colorCoordinates[index].Y))
                        {
                            int x = (int)(Math.Floor(this.colorCoordinates[index].X + 0.5f) / ColorDownsampleFactor);
                            int y = (int)(Math.Floor(this.colorCoordinates[index].Y + 0.5f) / ColorDownsampleFactor);

                            if ((x >= 0) && (x < this.depthVisibilityTestMapWidth) && 
                                (y >= 0) && (y < this.depthVisibilityTestMapHeight))
                            {
                                int depthVisibilityTestIndex = (y * this.depthVisibilityTestMapWidth) + x;
                                if ((ptrDepthVisibilityPixels[depthVisibilityTestIndex] == 0) || 
                                    (ptrDepthVisibilityPixels[depthVisibilityTestIndex] > ptrDepthPixels[index]))
                                {
                                    ptrDepthVisibilityPixels[depthVisibilityTestIndex] = ptrDepthPixels[index];
                                }
                            }
                        }
                    }
                }

                if (this.mirrorDepth)
                {
                    // Here we make use of unsafe code to just copy the whole pixel as an int for performance reasons, as we do
                    // not need access to the individual rgba components.
                    fixed (byte* ptrColorPixels = this.colorImagePixels)
                    {
                        int* rawColorPixels = (int*)ptrColorPixels;

                        Parallel.For(
                            0,
                            this.depthHeight,
                            y =>
                            {
                                int destIndex = y * this.depthWidth;

                                for (int x = 0; x < this.depthWidth; ++x, ++destIndex)
                                {
                                    // calculate index into depth array
                                    int colorInDepthX = (int)Math.Floor(colorCoordinates[destIndex].X + 0.5);
                                    int colorInDepthY = (int)Math.Floor(colorCoordinates[destIndex].Y + 0.5);
                                    int depthVisibilityTestX = (int)(colorInDepthX / ColorDownsampleFactor);
                                    int depthVisibilityTestY = (int)(colorInDepthY / ColorDownsampleFactor);
                                    int depthVisibilityTestIndex = (depthVisibilityTestY * this.depthVisibilityTestMapWidth) + depthVisibilityTestX;

                                    // make sure the depth pixel maps to a valid point in color space
                                    if (colorInDepthX >= 0 && colorInDepthX < this.colorWidth && colorInDepthY >= 0
                                        && colorInDepthY < this.colorHeight && this.depthImagePixels[destIndex] != 0)
                                    {
                                        ushort depthTestValue = this.depthVisibilityTestMap[depthVisibilityTestIndex];

                                        if ((this.depthImagePixels[destIndex] - depthTestValue) < DepthVisibilityTestThreshold)
                                        {
                                            // Calculate index into color array
                                            int sourceColorIndex = colorInDepthX + (colorInDepthY * this.colorWidth);

                                            // Copy color pixel
                                            this.resampledColorImagePixelsAlignedToDepth[destIndex] = rawColorPixels[sourceColorIndex];
                                        }
                                        else 
                                        {
                                            this.resampledColorImagePixelsAlignedToDepth[destIndex] = 0;
                                        }
                                    }
                                    else
                                    {
                                        this.resampledColorImagePixelsAlignedToDepth[destIndex] = 0;
                                    }
                                }
                            });
                    }
                }
                else
                {
                    // Here we make use of unsafe code to just copy the whole pixel as an int for performance reasons, as we do
                    // not need access to the individual rgba components.
                    fixed (byte* ptrColorPixels = this.colorImagePixels)
                    {
                        int* rawColorPixels = (int*)ptrColorPixels;

                        // Horizontal flip the color image as the standard depth image is flipped internally in Kinect Fusion
                        // to give a viewpoint as though from behind the Kinect looking forward by default.
                        Parallel.For(
                            0,
                            this.depthHeight,
                            y =>
                            {
                                int destIndex = y * this.depthWidth;
                                int flippedDestIndex = destIndex + (this.depthWidth - 1); // horizontally mirrored

                                for (int x = 0; x < this.depthWidth; ++x, ++destIndex, --flippedDestIndex)
                                {
                                    // calculate index into depth array
                                    int colorInDepthX = (int)Math.Floor(colorCoordinates[destIndex].X + 0.5);
                                    int colorInDepthY = (int)Math.Floor(colorCoordinates[destIndex].Y + 0.5);
                                    int depthVisibilityTestX = (int)(colorInDepthX / ColorDownsampleFactor);
                                    int depthVisibilityTestY = (int)(colorInDepthY / ColorDownsampleFactor);
                                    int depthVisibilityTestIndex = (depthVisibilityTestY * this.depthVisibilityTestMapWidth) + depthVisibilityTestX;

                                    // make sure the depth pixel maps to a valid point in color space
                                    if (colorInDepthX >= 0 && colorInDepthX < this.colorWidth && colorInDepthY >= 0
                                        && colorInDepthY < this.colorHeight && this.depthImagePixels[destIndex] != 0)
                                    {
                                        ushort depthTestValue = this.depthVisibilityTestMap[depthVisibilityTestIndex];

                                        if ((this.depthImagePixels[destIndex] - depthTestValue) < DepthVisibilityTestThreshold)
                                        {
                                            // Calculate index into color array- this will perform a horizontal flip as well
                                            int sourceColorIndex = colorInDepthX + (colorInDepthY * this.colorWidth);

                                            // Copy color pixel
                                            this.resampledColorImagePixelsAlignedToDepth[flippedDestIndex] = rawColorPixels[sourceColorIndex];
                                        }
                                        else
                                        {
                                            this.resampledColorImagePixelsAlignedToDepth[flippedDestIndex] = 0; 
                                        }
                                    }
                                    else
                                    {
                                        this.resampledColorImagePixelsAlignedToDepth[flippedDestIndex] = 0;
                                    }
                                }
                            });
                    }
                }
            }

            this.resampledColorFrameDepthAligned.CopyPixelDataFrom(this.resampledColorImagePixelsAlignedToDepth);
        }

        /// <summary>
        /// Perform volume depth data integration
        /// </summary>
        /// <returns>Returns true if a color frame is available for further processing, false otherwise.</returns>
        private bool IntegrateData()
        {
            // Color may opportunistically be available here - check
            bool colorAvailable = this.colorReadyEvent.WaitOne(0);

            // Don't integrate depth data into the volume if:
            // 1) tracking failed
            // 2) camera pose finder is off and we have paused capture
            // 3) camera pose finder is on and we are still under the m_cMinSuccessfulTrackingFramesForCameraPoseFinderAfterFailure
            //    number of successful frames count.
            bool integrateData = !this.trackingFailed && !this.PauseIntegration &&
                (!this.cameraPoseFinderAvailable || (this.cameraPoseFinderAvailable && !(this.trackingHasFailedPreviously && this.successfulFrameCount < MinSuccessfulTrackingFramesForCameraPoseFinderAfterFailure)));

            // Integrate the frame to volume
            if (integrateData)
            {
                bool integrateColor = this.processedFrameCount % ColorIntegrationInterval == 0 && colorAvailable;

                // Reset this flag as we are now integrating data again
                this.trackingHasFailedPreviously = false;

                if (this.captureColor && integrateColor)
                {
                    // Pre-process color
                    this.MapColorToDepth();

                    // Integrate color and depth
                    this.volume.IntegrateFrame(
                        this.depthFloatFrame, 
                        this.resampledColorFrameDepthAligned, 
                        this.integrationWeight,
                        FusionDepthProcessor.DefaultColorIntegrationOfAllAngles,
                        this.worldToCameraTransform);

                    // Flag that we have captured color
                    this.colorCaptured = true;
                }
                else
                {
                    // Just integrate depth
                    this.volume.IntegrateFrame(
                        this.depthFloatFrame, 
                        this.integrationWeight, 
                        this.worldToCameraTransform);
                }

                // Reset color ready event
                this.colorReadyEvent.Reset();
            }

            return colorAvailable;
        }

        /// <summary>
        /// Render the reconstruction
        /// </summary>
        private void RenderReconstruction()
        {
            if (null == this.volume || this.savingMesh || null == this.raycastPointCloudFrame
                || null == this.shadedSurfaceFrame || null == this.shadedSurfaceNormalsFrame)
            {
                return;
            }

            // If KinectView option has been set, use the worldToCameraTransform, else use the virtualCamera transform
            Matrix4 cameraView = this.KinectView ? this.worldToCameraTransform : this.virtualCameraWorldToCameraMatrix4;

            if (this.captureColor)
            {
                this.volume.CalculatePointCloud(this.raycastPointCloudFrame, this.shadedSurfaceFrame, cameraView);
            }
            else
            {
                this.volume.CalculatePointCloud(this.raycastPointCloudFrame, cameraView);

                // Shade point cloud frame for rendering
                FusionDepthProcessor.ShadePointCloud(
                    this.raycastPointCloudFrame,
                    cameraView,
                    this.worldToBGRTransform,
                    this.displayNormals ? null : this.shadedSurfaceFrame,
                    this.displayNormals ? this.shadedSurfaceNormalsFrame : null);
            }

            // Update the rendered UI image
            Dispatcher.BeginInvoke((Action)(() => this.ReconstructFrameComplete()));

            this.lastRenderTimestamp = DateTime.UtcNow;
        }

        /// <summary>
        /// Update the camera pose finder data.
        /// </summary>
        private void UpdateCameraPoseFinder()
        {
            if (null == this.depthFloatFrame || null == this.resampledColorFrame || null == this.cameraPoseFinder)
            {
                return;
            }

            this.ProcessColorForCameraPoseFinder();

            bool poseHistoryTrimmed = false;
            bool addedPose = false;

            // This function will add the pose to the camera pose finding database when the input frame's minimum
            // distance to the existing database is equal to or above CameraPoseFinderDistanceThresholdAccept 
            // (i.e. indicating that the input has become dis-similar to the existing database and a new frame 
            // should be captured). Note that the color and depth frames must be the same size, however, the 
            // horizontal mirroring setting does not have to be consistent between depth and color. It does have
            // to be consistent between camera pose finder database creation and calling FindCameraPose though,
            // hence we always reset both the reconstruction and database when changing the mirror depth setting.
            this.cameraPoseFinder.ProcessFrame(
                this.depthFloatFrame,
                this.resampledColorFrame,
                this.worldToCameraTransform,
                CameraPoseFinderDistanceThresholdAccept,
                out addedPose,
                out poseHistoryTrimmed);

            if (true == addedPose)
            {
                this.ShowStatusMessageLowPriority("Camera Pose Finder Added Frame! " + this.cameraPoseFinder.GetStoredPoseCount() + " frames stored, minimum distance >= " + CameraPoseFinderDistanceThresholdAccept);
            }

            if (true == poseHistoryTrimmed)
            {
                this.ShowStatusMessage(Properties.Resources.PoseFinderPoseHistoryFull);
            }
        }

        /// <summary>
        /// Called when a ray-casted view of the reconstruction is available for display in the UI 
        /// </summary>
        private void ReconstructFrameComplete()
        {
            // Render shaded surface frame or shaded surface normals frame
            RenderColorImage(
                this.captureColor ? this.shadedSurfaceFrame : (this.displayNormals ? this.shadedSurfaceNormalsFrame : this.shadedSurfaceFrame),
                ref this.shadedSurfaceFramePixelsArgb,
                ref this.shadedSurfaceFrameBitmap,
                this.shadedSurfaceImage);
        }

        /// <summary>
        /// Render Fusion AlignDepthFloatToReconstruction float deltas frame to UI
        /// </summary>
        /// <param name="alignDeltasFloatFrame">Fusion depth float frame</param>
        /// <param name="bitmap">Bitmap contains float frame data for rendering</param>
        /// <param name="image">UI image component to render float frame to</param>
        private void RenderAlignDeltasFloatImage(FusionFloatImageFrame alignDeltasFloatFrame, ref WriteableBitmap bitmap, System.Windows.Controls.Image image)
        {
            if (null == alignDeltasFloatFrame)
            {
                return;
            }

            if (null == bitmap || alignDeltasFloatFrame.Width != bitmap.Width || alignDeltasFloatFrame.Height != bitmap.Height)
            {
                // Create bitmap of correct format
                bitmap = new WriteableBitmap(alignDeltasFloatFrame.Width, alignDeltasFloatFrame.Height, 96.0, 96.0, PixelFormats.Bgr32, null);

                // Set bitmap as source to UI image object
                image.Source = bitmap;
            }

            alignDeltasFloatFrame.CopyPixelDataTo(this.deltaFromReferenceFrameFloatPixels);

            Parallel.For(
            0, 
            alignDeltasFloatFrame.Height, 
            y => 
            {
                int index = y * alignDeltasFloatFrame.Width;
                for (int x = 0; x < alignDeltasFloatFrame.Width; ++x, ++index)
                {
                    float residue = this.deltaFromReferenceFrameFloatPixels[index];

                    if (residue < 1.0f)
                    {
                        this.deltaFromReferenceFramePixelsArgb[index] = (byte)(255.0f * KinectFusionHelper.ClampFloatingPoint(1.0f - residue, 0.0f, 1.0f)); // blue
                        this.deltaFromReferenceFramePixelsArgb[index] |= ((byte)(255.0f * KinectFusionHelper.ClampFloatingPoint(1.0f - Math.Abs(residue), 0.0f, 1.0f))) << 8; // green
                        this.deltaFromReferenceFramePixelsArgb[index] |= ((byte)(255.0f * KinectFusionHelper.ClampFloatingPoint(1.0f + residue, 0.0f, 1.0f))) << 16; // red
                    }
                    else
                    {
                        this.deltaFromReferenceFramePixelsArgb[index] = 0;
                    }
                }
            });

            // Copy colored pixels to bitmap
            bitmap.WritePixels(
                        new Int32Rect(0, 0, alignDeltasFloatFrame.Width, alignDeltasFloatFrame.Height),
                        this.deltaFromReferenceFramePixelsArgb,
                        bitmap.PixelWidth * sizeof(int),
                        0);
        }

        /// <summary>
        /// Render Fusion AlignPointClouds color deltas frame to UI
        /// </summary>
        /// <param name="bitmap">Bitmap contains float frame data for rendering</param>
        /// <param name="image">UI image component to render float frame to</param>
        private void RenderAlignDeltasColorImage(ref WriteableBitmap bitmap, System.Windows.Controls.Image image)
        {
            if (null == bitmap || this.depthWidth != bitmap.Width || this.depthHeight != bitmap.Height)
            {
                // Create bitmap of correct format
                bitmap = new WriteableBitmap(this.depthWidth, this.depthHeight, 96.0, 96.0, PixelFormats.Bgr32, null);

                // Set bitmap as source to UI image object
                image.Source = bitmap;
            }

            // Copy colored pixels to bitmap
            bitmap.WritePixels(
                        new Int32Rect(0, 0, this.depthWidth, this.depthHeight),
                        this.deltaFromReferenceFramePixelsArgb,
                        bitmap.PixelWidth * sizeof(int),
                        0);
        }

        /// <summary>
        /// Render Fusion depth float frame to UI
        /// </summary>
        /// <param name="bitmap">Bitmap contains depth float frame data for rendering</param>
        /// <param name="image">UI image component to render depth float frame to</param>
        private void RenderDepthFloatImage(ref WriteableBitmap bitmap, System.Windows.Controls.Image image)
        {
            if (null == this.depthFloatFrame)
            {
                return;
            }

            if (null == bitmap || this.depthFloatFrame.Width != bitmap.Width || this.depthFloatFrame.Height != bitmap.Height)
            {
                // Create bitmap of correct format
                bitmap = new WriteableBitmap(this.depthFloatFrame.Width, this.depthFloatFrame.Height, 96.0, 96.0, PixelFormats.Bgr32, null);

                // Set bitmap as source to UI image object
                image.Source = bitmap;
            }

            this.depthFloatFrame.CopyPixelDataTo(this.depthFloatFrameDepthPixels);

            // Calculate color of pixels based on depth of each pixel
            float range = 4.0f;
            float oneOverRange = (1.0f / range) * 256.0f;
            float minRange = 0.0f;

            Parallel.For(
            0,
            this.depthFloatFrame.Height,
            y =>
            {
                int index = y * this.depthFloatFrame.Width;
                for (int x = 0; x < this.depthFloatFrame.Width; ++x, ++index)
                {
                    float depth = this.depthFloatFrameDepthPixels[index];
                    int intensity = (depth >= minRange) ? ((byte)((depth - minRange) * oneOverRange)) : 0;

                    this.depthFloatFramePixelsArgb[index] = (255 << 24) | (intensity << 16) | (intensity << 8) | intensity; // set blue, green, red
                }
            });

            // Copy colored pixels to bitmap
            bitmap.WritePixels(
                        new Int32Rect(0, 0, this.depthFloatFrame.Width, this.depthFloatFrame.Height),
                        this.depthFloatFramePixelsArgb,
                        bitmap.PixelWidth * sizeof(int),
                        0);
        }

        /// <summary>
        /// Called on each render of WPF (usually around 60Hz)
        /// </summary>
        /// <param name="sender">Object sending the event</param>
        /// <param name="e">Event arguments</param>
        private void CompositionTargetRendering(object sender, EventArgs e)
        {
            // If the viewChanged flag is used so we only raycast the volume when something changes
            // When reconstructing we call RenderReconstruction manually for every integrated depth frame (see ReconstructDepthData)
            if (this.viewChanged)
            {
                this.viewChanged = false;
                this.RenderReconstruction();
            }
        }

        /// <summary>
        /// Event raised when the mouse updates the graphics camera transformation for the virtual camera
        /// Here we set the viewChanged flag to true, to cause a volume render when the WPF composite update event occurs
        /// </summary>
        /// <param name="sender">Event generator</param>
        /// <param name="e">Event parameter</param>
        private void OnVirtualCameraTransformationChanged(object sender, EventArgs e)
        {
            // Update the stored virtual camera pose
            this.virtualCameraWorldToCameraMatrix4 = this.virtualCamera.WorldToCameraMatrix4;
            this.viewChanged = true;
        }

        /// <summary>
        /// Check if the gap between 2 frames has reached reset time threshold. If yes, reset the reconstruction
        /// </summary>
        /// <param name="frameTimestamp">The frame's timestamp.</param>
        private void CheckResetTimeStamp(TimeSpan frameTimestamp)
        {
            if (!this.lastFrameTimestamp.Equals(TimeSpan.Zero))
            {
                long timeThreshold = (ReconstructionProcessor.Amp == ProcessorType) ? ResetOnTimeStampSkippedMillisecondsGPU : ResetOnTimeStampSkippedMillisecondsCPU;

                // Calculate skipped milliseconds between 2 frames
                long skippedMilliseconds = (long)frameTimestamp.Subtract(this.lastFrameTimestamp).Duration().TotalMilliseconds;

                if (skippedMilliseconds >= timeThreshold)
                {
                    this.ShowStatusMessage(Properties.Resources.ResetVolume);
                    this.resetReconstruction = true;
                }
            }

            // Set timestamp of last frame
            this.lastFrameTimestamp = frameTimestamp;
        }

        /// <summary>
        /// Reset reconstruction object to initial state
        /// </summary>
        private void ResetReconstruction()
        {
            if (null == this.sensor)
            {
                return;
            }

            // Reset tracking error counter
            this.trackingErrorCount = 0;

            // Set the world-view transform to identity, so the world origin is the initial camera location.
            this.worldToCameraTransform = Matrix4.Identity;

            // Reset volume
            if (null != this.volume)
            {
                try
                {
                    // Translate the reconstruction volume location away from the world origin by an amount equal
                    // to the minimum depth threshold. This ensures that some depth signal falls inside the volume.
                    // If set false, the default world origin is set to the center of the front face of the 
                    // volume, which has the effect of locating the volume directly in front of the initial camera
                    // position with the +Z axis into the volume along the initial camera direction of view.
                    if (this.translateResetPoseByMinDepthThreshold)
                    {
                        Matrix4 worldToVolumeTransform = this.defaultWorldToVolumeTransform;

                        // Translate the volume in the Z axis by the minDepthClip distance
                        float minDist = (this.minDepthClip < this.maxDepthClip) ? this.minDepthClip : this.maxDepthClip;
                        worldToVolumeTransform.M43 -= minDist * this.voxelsPerMeter;

                        this.volume.ResetReconstruction(this.worldToCameraTransform, worldToVolumeTransform); 
                    }
                    else
                    {
                        this.volume.ResetReconstruction(this.worldToCameraTransform);
                    }

                    this.ResetTracking();
                    this.ResetColorImage();
                }
                catch (InvalidOperationException)
                {
                    this.ShowStatusMessage(Properties.Resources.ResetFailed);
                }
            }

            // Update manual reset information to status bar
            this.ShowStatusMessage(Properties.Resources.ResetVolume);
        }

        /// <summary>
        /// Re-create the reconstruction object
        /// </summary>
        /// <returns>Indicate success or failure</returns>
        private bool RecreateReconstruction()
        {
            // Check if sensor has been initialized
            if (null == this.sensor)
            {
                return false;
            }

            if (null != this.volume)
            {
                this.volume.Dispose();
                this.volume = null;
            }

            try
            {
                ReconstructionParameters volParam = new ReconstructionParameters(this.voxelsPerMeter, this.voxelsX, this.voxelsY, this.voxelsZ);

                // Set the world-view transform to identity, so the world origin is the initial camera location.
                this.worldToCameraTransform = Matrix4.Identity;

                this.volume = ColorReconstruction.FusionCreateReconstruction(volParam, ProcessorType, DeviceToUse, this.worldToCameraTransform);

                this.defaultWorldToVolumeTransform = this.volume.GetCurrentWorldToVolumeTransform();

                if (this.translateResetPoseByMinDepthThreshold)
                {
                    this.ResetReconstruction();
                }
                else
                {
                    this.ResetTracking();
                    this.ResetColorImage();
                }

                // Map world X axis to blue channel, Y axis to green channel and Z axis to red channel,
                // normalizing each to the range [0, 1]. We also add a shift of 0.5 to both X,Y channels
                // as the world origin starts located at the center of the front face of the volume,
                // hence we need to map negative x,y world vertex locations to positive color values.
                this.worldToBGRTransform = Matrix4.Identity;
                this.worldToBGRTransform.M11 = this.voxelsPerMeter / this.voxelsX;
                this.worldToBGRTransform.M22 = this.voxelsPerMeter / this.voxelsY;
                this.worldToBGRTransform.M33 = this.voxelsPerMeter / this.voxelsZ;
                this.worldToBGRTransform.M41 = 0.5f;
                this.worldToBGRTransform.M42 = 0.5f;
                this.worldToBGRTransform.M44 = 1.0f;

                // Update the graphics volume cube rendering
                if (this.volumeGraphics)
                {
                    Dispatcher.BeginInvoke(
                        (Action)(() =>
                            {
                                // re-create the volume cube display with the new size
                                this.RemoveVolumeCube3DGraphics();
                                this.DisposeVolumeCube3DGraphics();
                                this.CreateCube3DGraphics(volumeCubeLineColor, LineThickness, new Vector3D(0, 0, 0));
                                this.AddVolumeCube3DGraphics();
                            }));
                }

                // Signal that a render is required
                this.viewChanged = true;

                return true;
            }
            catch (ArgumentException)
            {
                this.volume = null;
                this.ShowStatusMessage(Properties.Resources.VolumeResolution);
            }
            catch (InvalidOperationException ex)
            {
                this.volume = null;
                this.ShowStatusMessage(ex.Message);
            }
            catch (DllNotFoundException)
            {
                this.volume = null;
                this.ShowStatusMessage(Properties.Resources.MissingPrerequisite);
            }
            catch (OutOfMemoryException)
            {
                this.volume = null;
                this.ShowStatusMessage(Properties.Resources.OutOfMemory);
            }

            return false;
        }

        /// <summary>
        /// Reset the mapped color image on reset or re-create of volume
        /// </summary>
        private void ResetColorImage()
        {
            if (null != this.resampledColorFrameDepthAligned && null != this.resampledColorImagePixelsAlignedToDepth)
            {
                // Clear the mapped color image
                Array.Clear(this.resampledColorImagePixelsAlignedToDepth, 0, this.resampledColorImagePixelsAlignedToDepth.Length);
                this.resampledColorFrameDepthAligned.CopyPixelDataFrom(this.resampledColorImagePixelsAlignedToDepth);
            }

            this.colorCaptured = false;
        }

        /// <summary>
        /// Handler for click event from "Reset Virtual Camera" button
        /// </summary>
        /// <param name="sender">Event sender</param>
        /// <param name="e">Event arguments</param>
        private void ResetCameraButtonClick(object sender, RoutedEventArgs e)
        {
            if (null != this.virtualCamera)
            {
                this.virtualCamera.Reset();
                this.viewChanged = true;
            }
        }

        /// <summary>
        /// Handler for click event from "Reset Reconstruction" button
        /// </summary>
        /// <param name="sender">Event sender</param>
        /// <param name="e">Event arguments</param>
        private void ResetReconstructionButtonClick(object sender, RoutedEventArgs e)
        {
            if (null == this.sensor)
            {
                return;
            }

            // Signal the worker thread to reset the volume
            this.resetReconstruction = true;

            // Update manual reset information to status bar
            this.ShowStatusMessage(Properties.Resources.ResetVolume);
        }

        /// <summary>
        /// Handler for click event from "Create Mesh" button
        /// </summary>
        /// <param name="sender">Event sender</param>
        /// <param name="e">Event arguments</param>
        private void CreateMeshButtonClick(object sender, RoutedEventArgs e)
        {
            // Mark the start time of saving mesh
            DateTime beginning = DateTime.UtcNow;

            try
            {
                this.ShowStatusMessage(Properties.Resources.SavingMesh);

                ColorMesh mesh = null;

                lock (this.volumeLock)
                {
                    this.savingMesh = true;

                    if (null == this.volume)
                    {
                        this.ShowStatusMessage(Properties.Resources.MeshNullVolume);
                        return;
                    }

                    mesh = this.volume.CalculateMesh(1);
                }

                if (null == mesh)
                {
                    this.ShowStatusMessage(Properties.Resources.ErrorSaveMesh);
                    return;
                }

                Win32.SaveFileDialog dialog = new Win32.SaveFileDialog();

                if (true == this.stlFormat.IsChecked)
                {
                    dialog.FileName = "MeshedReconstruction.stl";
                    dialog.Filter = "STL Mesh Files|*.stl|All Files|*.*";
                }
                else if (true == this.objFormat.IsChecked)
                {
                    dialog.FileName = "MeshedReconstruction.obj";
                    dialog.Filter = "OBJ Mesh Files|*.obj|All Files|*.*";
                }
                else
                {
                    dialog.FileName = "MeshedReconstruction.ply";
                    dialog.Filter = "PLY Mesh Files|*.ply|All Files|*.*";
                }

                if (true == dialog.ShowDialog())
                {
                    if (true == this.stlFormat.IsChecked)
                    {
                        using (BinaryWriter writer = new BinaryWriter(dialog.OpenFile()))
                        {
                            // Default to flip Y,Z coordinates on save
                            KinectFusionHelper.SaveBinaryStlMesh(mesh, writer, true);
                        }
                    }
                    else if (true == this.objFormat.IsChecked)
                    {
                        using (StreamWriter writer = new StreamWriter(dialog.FileName))
                        {
                            // Default to flip Y,Z coordinates on save
                            KinectFusionHelper.SaveAsciiObjMesh(mesh, writer, true);
                        }
                    }
                    else
                    {
                        using (StreamWriter writer = new StreamWriter(dialog.FileName))
                        {
                            // Default to flip Y,Z coordinates on save
                            KinectFusionHelper.SaveAsciiPlyMesh(mesh, writer, true, this.colorCaptured);
                        }
                    }

                    this.ShowStatusMessage(Properties.Resources.MeshSaved);
                }
                else
                {
                    this.ShowStatusMessage(Properties.Resources.MeshSaveCanceled);
                }
            }
            catch (ArgumentException)
            {
                this.ShowStatusMessage(Properties.Resources.ErrorSaveMesh);
            }
            catch (InvalidOperationException)
            {
                this.ShowStatusMessage(Properties.Resources.ErrorSaveMesh);
            }
            catch (IOException)
            {
                this.ShowStatusMessage(Properties.Resources.ErrorSaveMesh);
            }
            catch (OutOfMemoryException)
            {
                this.ShowStatusMessage(Properties.Resources.ErrorSaveMeshOutOfMemory);
            }
            finally
            {
                // Update timestamp of last frame to avoid auto reset reconstruction
                this.lastFrameTimestamp += DateTime.UtcNow - beginning;

                this.savingMesh = false;
            }
        }

        /// <summary>
        /// Handler for volume setting changing event
        /// </summary>
        /// <param name="sender">Event sender</param>
        /// <param name="e">Event argument</param>
        private void VolumeSettingsChanged(object sender, RoutedPropertyChangedEventArgs<double> e)
        {
            // Signal the worker thread to recreate the volume
            this.recreateReconstruction = true;
        }

        /// <summary>
        /// Update the virtual camera transform from this process.
        /// </summary>
        private void UpdateVirtualCameraTransform()
        {
            // Update just the virtual camera pose from the tracked camera
            // We do not update the frustum here, as we do not render it when in Kinect camera view.
            this.virtualCamera.WorldToCameraMatrix4 = this.worldToCameraTransform;
        }

        /// <summary>
        /// Create an axis-aligned coordinate cross for rendering in the WPF3D coordinate system. 
        /// Red is the +X axis, Green is the +Y axis, Blue is the +Z axis
        /// </summary>
        /// <param name="crossOrigin">The origin of the coordinate cross in world space.</param>
        /// <param name="axisSize">The size of the axis in m.</param>
        /// <param name="thickness">The thickness of the lines in screen pixels.</param>
        private void CreateAxisAlignedCoordinateCross3DGraphics(Point3D crossOrigin, float axisSize, int thickness)
        {
            this.axisX = new ScreenSpaceLines3D();

            this.axisX.Points = new Point3DCollection();
            this.axisX.Points.Add(crossOrigin);
            this.axisX.Points.Add(new Point3D(crossOrigin.X + axisSize, crossOrigin.Y, crossOrigin.Z));

            this.axisX.Thickness = 2;
            this.axisX.Color = System.Windows.Media.Color.FromArgb(200, 255, 0, 0);   // Red (X)

            this.axisY = new ScreenSpaceLines3D();

            this.axisY.Points = new Point3DCollection();
            this.axisY.Points.Add(crossOrigin);
            this.axisY.Points.Add(new Point3D(crossOrigin.X, crossOrigin.Y + axisSize, crossOrigin.Z));

            this.axisY.Thickness = 2;
            this.axisY.Color = System.Windows.Media.Color.FromArgb(200, 0, 255, 0);   // Green (Y)

            this.axisZ = new ScreenSpaceLines3D();

            this.axisZ.Points = new Point3DCollection();
            this.axisZ.Points.Add(crossOrigin);
            this.axisZ.Points.Add(new Point3D(crossOrigin.X, crossOrigin.Y, crossOrigin.Z + axisSize));

            this.axisZ.Thickness = thickness;
            this.axisZ.Color = System.Windows.Media.Color.FromArgb(200, 0, 0, 255);   // Blue (Z)
        }

        /// <summary>
        /// Add the coordinate cross axes to the visual tree
        /// </summary>
        private void AddAxisAlignedCoordinateCross3DGraphics()
        {
            if (this.haveAddedCoordinateCross)
            {
                return;
            }

            if (null != this.axisX)
            {
                this.GraphicsViewport.Children.Add(this.axisX);

                this.haveAddedCoordinateCross = true;
            }

            if (null != this.axisY)
            {
                this.GraphicsViewport.Children.Add(this.axisY);
            }

            if (null != this.axisZ)
            {
                this.GraphicsViewport.Children.Add(this.axisZ);
            }
        }

        /// <summary>
        /// Remove the coordinate cross axes from the visual tree
        /// </summary>
        private void RemoveAxisAlignedCoordinateCross3DGraphics()
        {
            if (null != this.axisX)
            {
                this.GraphicsViewport.Children.Remove(this.axisX);
            }

            if (null != this.axisY)
            {
                this.GraphicsViewport.Children.Remove(this.axisY);
            }

            if (null != this.axisZ)
            {
                this.GraphicsViewport.Children.Remove(this.axisZ);
            }

            this.haveAddedCoordinateCross = false;
        }

        /// <summary>
        /// Dispose the coordinate cross axes from the visual tree
        /// </summary>
        private void DisposeAxisAlignedCoordinateCross3DGraphics()
        {
            if (this.haveAddedCoordinateCross)
            {
                this.RemoveAxisAlignedCoordinateCross3DGraphics();
            }

            if (null != this.axisX)
            {
                this.axisX.Dispose();
                this.axisX = null;
            }

            if (null != this.axisY)
            {
                this.axisY.Dispose();
                this.axisY = null;
            }

            if (null != this.axisZ)
            {
                this.axisZ.Dispose();
                this.axisZ = null;
            }
        }

        /// <summary>
        /// Create an axis-aligned volume cube for rendering.
        /// </summary>
        /// <param name="color">The color of the volume cube.</param>
        /// <param name="thickness">The thickness of the lines in screen pixels.</param>
        /// <param name="translation">World to volume translation vector.</param>
        private void CreateCube3DGraphics(System.Windows.Media.Color color, int thickness, Vector3D translation)
        {
            // Scaler for cube size
            float cubeSizeScaler = 1.0f;

            // Before we created a volume which contains the head
            // Here we create a graphical representation of this volume cube
            float oneOverVpm = 1.0f / this.voxelsPerMeter;

            // This cube is world axis aligned
            float cubeSideX = this.voxelsX * oneOverVpm * cubeSizeScaler;
            float halfSideX = cubeSideX * 0.5f;

            float cubeSideY = this.voxelsY * oneOverVpm * cubeSizeScaler;
            float halfSideY = cubeSideY * 0.5f;

            float cubeSideZ = this.voxelsZ * oneOverVpm * cubeSizeScaler;
            float halfSideZ = cubeSideZ * 0.5f;

            // The translation vector is from the origin to the volume front face
            // And here we describe the translation Z as from the origin to the cube center
            // So we continue to translate half volume size align Z
            translation.Z -= halfSideZ / cubeSizeScaler;

            this.volumeCube = new ScreenSpaceLines3D();
            this.volumeCube.Points = new Point3DCollection();

            // Front face
            // TL front - TR front
            this.volumeCube.Points.Add(new Point3D(-halfSideX + translation.X, halfSideY + translation.Y, -halfSideZ + translation.Z));
            this.volumeCube.Points.Add(new Point3D(halfSideX + translation.X, halfSideY + translation.Y, -halfSideZ + translation.Z));

            // TR front - BR front
            this.volumeCube.Points.Add(new Point3D(halfSideX + translation.X, halfSideY + translation.Y, -halfSideZ + translation.Z));
            this.volumeCube.Points.Add(new Point3D(halfSideX + translation.X, -halfSideY + translation.Y, -halfSideZ + translation.Z));

            // BR front - BL front
            this.volumeCube.Points.Add(new Point3D(halfSideX + translation.X, -halfSideY + translation.Y, -halfSideZ + translation.Z));
            this.volumeCube.Points.Add(new Point3D(-halfSideX + translation.X, -halfSideY + translation.Y, -halfSideZ + translation.Z));

            // BL front - TL front
            this.volumeCube.Points.Add(new Point3D(-halfSideX + translation.X, -halfSideY + translation.Y, -halfSideZ + translation.Z));
            this.volumeCube.Points.Add(new Point3D(-halfSideX + translation.X, halfSideY + translation.Y, -halfSideZ + translation.Z));

            // Rear face
            // TL rear - TR rear
            this.volumeCube.Points.Add(new Point3D(-halfSideX + translation.X, halfSideY + translation.Y, halfSideZ + translation.Z));
            this.volumeCube.Points.Add(new Point3D(halfSideX + translation.X, halfSideY + translation.Y, halfSideZ + translation.Z));

            // TR rear - BR rear
            this.volumeCube.Points.Add(new Point3D(halfSideX + translation.X, halfSideY + translation.Y, halfSideZ + translation.Z));
            this.volumeCube.Points.Add(new Point3D(halfSideX + translation.X, -halfSideY + translation.Y, halfSideZ + translation.Z));

            // BR rear - BL rear
            this.volumeCube.Points.Add(new Point3D(halfSideX + translation.X, -halfSideY + translation.Y, halfSideZ + translation.Z));
            this.volumeCube.Points.Add(new Point3D(-halfSideX + translation.X, -halfSideY + translation.Y, halfSideZ + translation.Z));

            // BL rear - TL rear
            this.volumeCube.Points.Add(new Point3D(-halfSideX + translation.X, -halfSideY + translation.Y, halfSideZ + translation.Z));
            this.volumeCube.Points.Add(new Point3D(-halfSideX + translation.X, halfSideY + translation.Y, halfSideZ + translation.Z));

            // Connecting lines
            // TL front - TL rear
            this.volumeCube.Points.Add(new Point3D(-halfSideX + translation.X, halfSideY + translation.Y, -halfSideZ + translation.Z));
            this.volumeCube.Points.Add(new Point3D(-halfSideX + translation.X, halfSideY + translation.Y, halfSideZ + translation.Z));

            // TR front - TR rear
            this.volumeCube.Points.Add(new Point3D(halfSideX + translation.X, halfSideY + translation.Y, -halfSideZ + translation.Z));
            this.volumeCube.Points.Add(new Point3D(halfSideX + translation.X, halfSideY + translation.Y, halfSideZ + translation.Z));

            // BR front - BR rear
            this.volumeCube.Points.Add(new Point3D(halfSideX + translation.X, -halfSideY + translation.Y, -halfSideZ + translation.Z));
            this.volumeCube.Points.Add(new Point3D(halfSideX + translation.X, -halfSideY + translation.Y, halfSideZ + translation.Z));

            // BL front - BL rear
            this.volumeCube.Points.Add(new Point3D(-halfSideX + translation.X, -halfSideY + translation.Y, -halfSideZ + translation.Z));
            this.volumeCube.Points.Add(new Point3D(-halfSideX + translation.X, -halfSideY + translation.Y, halfSideZ + translation.Z));

            this.volumeCube.Thickness = thickness;
            this.volumeCube.Color = color;

            this.volumeCubeAxisX = new ScreenSpaceLines3D();

            this.volumeCubeAxisX.Points = new Point3DCollection();
            this.volumeCubeAxisX.Points.Add(new Point3D(-halfSideX + translation.X, halfSideY + translation.Y, halfSideZ + translation.Z));
            this.volumeCubeAxisX.Points.Add(new Point3D(-halfSideX + 0.1f + translation.X, halfSideY + translation.Y, halfSideZ + translation.Z));

            this.volumeCubeAxisX.Thickness = thickness + 2;
            this.volumeCubeAxisX.Color = System.Windows.Media.Color.FromArgb(200, 255, 0, 0);   // Red (X)

            this.volumeCubeAxisY = new ScreenSpaceLines3D();

            this.volumeCubeAxisY.Points = new Point3DCollection();
            this.volumeCubeAxisY.Points.Add(new Point3D(-halfSideX + translation.X, halfSideY + translation.Y, halfSideZ + translation.Z));
            this.volumeCubeAxisY.Points.Add(new Point3D(-halfSideX + translation.X, halfSideY - 0.1f + translation.Y, halfSideZ + translation.Z));

            this.volumeCubeAxisY.Thickness = thickness + 2;
            this.volumeCubeAxisY.Color = System.Windows.Media.Color.FromArgb(200, 0, 255, 0);   // Green (Y)

            this.volumeCubeAxisZ = new ScreenSpaceLines3D();

            this.volumeCubeAxisZ.Points = new Point3DCollection();
            this.volumeCubeAxisZ.Points.Add(new Point3D(-halfSideX + translation.X, halfSideY + translation.Y, halfSideZ + translation.Z));
            this.volumeCubeAxisZ.Points.Add(new Point3D(-halfSideX + translation.X, halfSideY + translation.Y, halfSideZ - 0.1f + translation.Z));

            this.volumeCubeAxisZ.Thickness = thickness + 2;
            this.volumeCubeAxisZ.Color = System.Windows.Media.Color.FromArgb(200, 0, 0, 255);   // Blue (Z)
        }

        /// <summary>
        /// Add the volume cube and axes to the visual tree
        /// </summary>
        private void AddVolumeCube3DGraphics()
        {
            if (this.haveAddedVolumeCube)
            {
                return;
            }

            if (null != this.volumeCube)
            {
                this.GraphicsViewport.Children.Add(this.volumeCube);

                this.haveAddedVolumeCube = true;
            }

            if (null != this.volumeCubeAxisX)
            {
                this.GraphicsViewport.Children.Add(this.volumeCubeAxisX);
            }

            if (null != this.volumeCubeAxisY)
            {
                this.GraphicsViewport.Children.Add(this.volumeCubeAxisY);
            }

            if (null != this.volumeCubeAxisZ)
            {
                this.GraphicsViewport.Children.Add(this.volumeCubeAxisZ);
            }
        }

        /// <summary>
        /// Remove the volume cube and axes from the visual tree
        /// </summary>
        private void RemoveVolumeCube3DGraphics()
        {
            if (null != this.volumeCube)
            {
                this.GraphicsViewport.Children.Remove(this.volumeCube);
            }

            if (null != this.volumeCubeAxisX)
            {
                this.GraphicsViewport.Children.Remove(this.volumeCubeAxisX);
            }

            if (null != this.volumeCubeAxisY)
            {
                this.GraphicsViewport.Children.Remove(this.volumeCubeAxisY);
            }

            if (null != this.volumeCubeAxisZ)
            {
                this.GraphicsViewport.Children.Remove(this.volumeCubeAxisZ);
            }

            this.haveAddedVolumeCube = false;
        }

        /// <summary>
        /// Dispose the volume cube and axes
        /// </summary>
        private void DisposeVolumeCube3DGraphics()
        {
            if (this.haveAddedVolumeCube)
            {
                this.RemoveVolumeCube3DGraphics();
            }

            if (null != this.volumeCube)
            {
                this.volumeCube.Dispose();
                this.volumeCube = null;
            }

            if (null != this.volumeCubeAxisX)
            {
                this.volumeCubeAxisX.Dispose();
                this.volumeCubeAxisX = null;
            }

            if (null != this.volumeCubeAxisY)
            {
                this.volumeCubeAxisY.Dispose();
                this.volumeCubeAxisY = null;
            }

            if (null != this.volumeCubeAxisZ)
            {
                this.volumeCubeAxisZ.Dispose();
                this.volumeCubeAxisZ = null;
            }
        }

        /// <summary>
        /// Show high priority messages in status bar
        /// </summary>
        /// <param name="message">Message to show on status bar</param>
        private void ShowStatusMessage(string message)
        {
            this.Dispatcher.BeginInvoke(
                (Action)(() =>
                {
                    this.ResetFps();

                    if ((DateTime.Now - this.lastStatusTimestamp).Seconds >= StatusBarInterval)
                    {
                        this.statusBarText.Text = message;
                        this.lastStatusTimestamp = DateTime.Now;
                    }
                    else
                    {
                        this.statusMessageQueue.Enqueue(message);
                    }
                }));
        }

        /// <summary>
        /// Show low priority messages in the status bar. Low priority messages do not reset the fps counter,
        /// and will only be displayed when high priority message has at least StatusBarInterval seconds shown to the user.
        /// Messages that comes in burst or appear extremely frequently should be considered low priority.
        /// </summary>
        /// <param name="message">Message to show on status bar</param>
        private void ShowStatusMessageLowPriority(string message)
        {
            this.Dispatcher.BeginInvoke(
                (Action)(() =>
                {
                    // Make sure the high prioirty messages has at least StatusBarInterval seconds shown to the user.
                    if ((DateTime.Now - this.lastStatusTimestamp).Seconds >= StatusBarInterval)
                    {
                        this.statusBarText.Text = message;
                    }
                }));
        }

        /// <summary>
        /// Allocate the frame buffers and memory used in the process for Kinect Fusion
        /// </summary>
        private void AllocateKinectFusionResources()
        {
            this.SafeDisposeFusionResources();

            // Allocate depth float frame
            this.depthFloatFrame = new FusionFloatImageFrame(this.depthWidth, this.depthHeight);

            // Allocate color frame for color data from Kinect mapped into depth frame
            this.resampledColorFrameDepthAligned = new FusionColorImageFrame(this.depthWidth, this.depthHeight);

            // Allocate delta from reference frame
            this.deltaFromReferenceFrame = new FusionFloatImageFrame(this.depthWidth, this.depthHeight);

            // Allocate point cloud frame
            this.raycastPointCloudFrame = new FusionPointCloudImageFrame(this.depthWidth, this.depthHeight);

            // Allocate shaded surface frame
            this.shadedSurfaceFrame = new FusionColorImageFrame(this.depthWidth, this.depthHeight);

            // Allocate shaded surface normals frame
            this.shadedSurfaceNormalsFrame = new FusionColorImageFrame(this.depthWidth, this.depthHeight);

            // Allocate re-sampled color at depth image size
            this.resampledColorFrame = new FusionColorImageFrame(this.depthWidth, this.depthHeight);

            // Allocate point cloud frame created from input depth
            this.depthPointCloudFrame = new FusionPointCloudImageFrame(this.depthWidth, this.depthHeight);

            // Allocate smoothed depth float frame
            this.smoothDepthFloatFrame = new FusionFloatImageFrame(this.depthWidth, this.depthHeight);

            this.downsampledWidth = this.depthWidth / DownsampleFactor;
            this.downsampledHeight = this.depthHeight / DownsampleFactor;

            // Allocate downsampled image frames
            this.downsampledDepthFloatFrame = new FusionFloatImageFrame(this.downsampledWidth, this.downsampledHeight);

            this.downsampledSmoothDepthFloatFrame = new FusionFloatImageFrame(this.downsampledWidth, this.downsampledHeight);

            this.downsampledRaycastPointCloudFrame = new FusionPointCloudImageFrame(this.downsampledWidth, this.downsampledHeight);

            this.downsampledDepthPointCloudFrame = new FusionPointCloudImageFrame(this.downsampledWidth, this.downsampledHeight);

            this.downsampledDeltaFromReferenceFrameColorFrame = new FusionColorImageFrame(this.downsampledWidth, this.downsampledHeight);

            int depthImageSize = this.depthWidth * this.depthHeight;
            int colorImageByteSize = this.colorWidth * this.colorHeight * sizeof(int);
            int downsampledDepthImageSize = this.downsampledWidth * this.downsampledHeight;

            // Create local depth pixels buffer
            this.depthImagePixels = new ushort[depthImageSize];

            // Create local color pixels buffer
            this.colorImagePixels = new byte[colorImageByteSize];

            // Create local color pixels buffer re-sampled to depth size
            this.resampledColorImagePixels = new int[depthImageSize];

            // Create float pixel array
            this.depthFloatFrameDepthPixels = new float[depthImageSize];

            // Create float pixel array
            this.deltaFromReferenceFrameFloatPixels = new float[depthImageSize];

            // Create colored pixel array of correct format
            this.depthFloatFramePixelsArgb = new int[depthImageSize];

            // Create colored pixel array of correct format
            this.deltaFromReferenceFramePixelsArgb = new int[depthImageSize];

            // Allocate the depth-color mapping points
            this.colorCoordinates = new ColorSpacePoint[depthImageSize];

            // Allocate color points re-sampled to depth size mapped into depth frame of reference
            this.resampledColorImagePixelsAlignedToDepth = new int[depthImageSize];

            // Allocate downsampled image arrays
            this.downsampledDepthImagePixels = new float[downsampledDepthImageSize];

            this.downsampledDeltaFromReferenceColorPixels = new int[downsampledDepthImageSize];

            // Create a camera pose finder with default parameters
            CameraPoseFinderParameters cameraPoseFinderParams = CameraPoseFinderParameters.Defaults;
            this.cameraPoseFinder = CameraPoseFinder.FusionCreateCameraPoseFinder(cameraPoseFinderParams);
       }

        /// <summary>
        /// Dispose fusion resources safely
        /// </summary>
        private void SafeDisposeFusionResources()
        {
            if (null != this.depthFloatFrame)
            {
                this.depthFloatFrame.Dispose();
            }

            if (null != this.resampledColorFrameDepthAligned)
            {
                this.resampledColorFrameDepthAligned.Dispose();
            }

            if (null != this.deltaFromReferenceFrame)
            {
                this.deltaFromReferenceFrame.Dispose();
            }

            if (null != this.raycastPointCloudFrame)
            {
                this.raycastPointCloudFrame.Dispose();
            }

            if (null != this.shadedSurfaceFrame)
            {
                this.shadedSurfaceFrame.Dispose();
            }

            if (null != this.shadedSurfaceNormalsFrame)
            {
                this.shadedSurfaceNormalsFrame.Dispose();
            }

            if (null != this.resampledColorFrame)
            {
                this.resampledColorFrame.Dispose();
            }

            if (null != this.depthPointCloudFrame)
            {
                this.depthPointCloudFrame.Dispose();
            }

            if (null != this.smoothDepthFloatFrame)
            {
                this.smoothDepthFloatFrame.Dispose();
            }

            if (null != this.downsampledDepthFloatFrame)
            {
                this.downsampledDepthFloatFrame.Dispose();
            }

            if (null != this.downsampledSmoothDepthFloatFrame)
            {
                this.downsampledSmoothDepthFloatFrame.Dispose();
            }

            if (null != this.downsampledRaycastPointCloudFrame)
            {
                this.downsampledRaycastPointCloudFrame.Dispose();
            }

            if (null != this.downsampledDepthPointCloudFrame)
            {
                this.downsampledDepthPointCloudFrame.Dispose();
            }

            if (null != this.downsampledDeltaFromReferenceFrameColorFrame)
            {
                this.downsampledDeltaFromReferenceFrameColorFrame.Dispose();
            }

            if (null != this.cameraPoseFinder)
            {
                this.cameraPoseFinder.Dispose();
            }
        }

        /// <summary>
        /// Copy a color frame to a fusion color image 
        /// </summary>
        /// <param name="dest">The destination color image.</param>
        private void CopyColorImageFrame(FusionColorImageFrame dest)
        {
            if (null == dest || null == this.colorImagePixels || null == this.resampledColorImagePixels)
            {
                throw new ArgumentException("dest == null");
            }

            if (dest.Width != this.colorWidth || dest.Height != this.colorHeight)
            {
                throw new ArgumentException("dest != color image size");
            }

            // Copy to intermediate buffer
            System.Buffer.BlockCopy(this.colorImagePixels, 0, this.resampledColorImagePixels, 0, this.colorPixelCount * sizeof(int));

            // Copy into Kinect Fusion image
            dest.CopyPixelDataFrom(this.resampledColorImagePixels);
        }

        /// <summary>
        /// Downsample depth pixels with nearest neighbor
        /// </summary>
        /// <param name="dest">The destination depth image.</param>
        /// <param name="factor">The downsample factor (2=x/2,y/2, 4=x/4,y/4, 8=x/8,y/8, 16=x/16,y/16).</param>
        private unsafe void DownsampleDepthFrameNearestNeighbor(FusionFloatImageFrame dest, int factor)
        {
            if (null == dest || null == this.downsampledDepthImagePixels)
            {
                throw new ArgumentException("inputs null");
            }

            if (false == (2 == factor || 4 == factor || 8 == factor || 16 == factor))
            {
                throw new ArgumentException("factor != 2, 4, 8 or 16");
            }

            int downsampleWidth = this.depthWidth / factor;
            int downsampleHeight = this.depthHeight / factor;

            if (dest.Width != downsampleWidth || dest.Height != downsampleHeight)
            {
                throw new ArgumentException("dest != downsampled image size");
            }

            if (this.mirrorDepth)
            {
                fixed (ushort* rawDepthPixelPtr = this.depthImagePixels)
                {
                    ushort* rawDepthPixels = (ushort*)rawDepthPixelPtr;

                    Parallel.For(
                        0,
                        downsampleHeight,
                        y =>
                        {
                            int destIndex = y * downsampleWidth;
                            int sourceIndex = y * this.depthWidth * factor;

                            for (int x = 0; x < downsampleWidth; ++x, ++destIndex, sourceIndex += factor)
                            {
                                // Copy depth value
                                this.downsampledDepthImagePixels[destIndex] = (float)rawDepthPixels[sourceIndex] * 0.001f;
                            }
                        });
                }
            }
            else
            {
                fixed (ushort* rawDepthPixelPtr = this.depthImagePixels)
                {
                    ushort* rawDepthPixels = (ushort*)rawDepthPixelPtr;

                    // Horizontal flip the color image as the standard depth image is flipped internally in Kinect Fusion
                    // to give a viewpoint as though from behind the Kinect looking forward by default.
                    Parallel.For(
                        0,
                        downsampleHeight,
                        y =>
                        {
                            int flippedDestIndex = (y * downsampleWidth) + (downsampleWidth - 1);
                            int sourceIndex = y * this.depthWidth * factor;

                            for (int x = 0; x < downsampleWidth; ++x, --flippedDestIndex, sourceIndex += factor)
                            {
                                // Copy depth value
                                this.downsampledDepthImagePixels[flippedDestIndex] = (float)rawDepthPixels[sourceIndex] * 0.001f;
                            }
                        });
                }
            }

            dest.CopyPixelDataFrom(this.downsampledDepthImagePixels);
        }

        /// <summary>
        /// Up sample color delta from reference frame with nearest neighbor - replicates pixels
        /// </summary>
        /// <param name="factor">The up sample factor (2=x*2,y*2, 4=x*4,y*4, 8=x*8,y*8, 16=x*16,y*16).</param>
        private unsafe void UpsampleColorDeltasFrameNearestNeighbor(int factor)
        {
            if (null == this.downsampledDeltaFromReferenceFrameColorFrame || null == this.downsampledDeltaFromReferenceColorPixels || null == this.deltaFromReferenceFramePixelsArgb)
            {
                throw new ArgumentException("inputs null");
            }

            if (false == (2 == factor || 4 == factor || 8 == factor || 16 == factor))
            {
                throw new ArgumentException("factor != 2, 4, 8 or 16");
            }

            int upsampleWidth = this.downsampledWidth * factor;
            int upsampleHeight = this.downsampledHeight * factor;

            if (this.depthWidth != upsampleWidth || this.depthHeight != upsampleHeight)
            {
                throw new ArgumentException("upsampled image size != depth size");
            }

            this.downsampledDeltaFromReferenceFrameColorFrame.CopyPixelDataTo(this.downsampledDeltaFromReferenceColorPixels);

            // Here we make use of unsafe code to just copy the whole pixel as an int for performance reasons, as we do
            // not need access to the individual rgba components.
            fixed (int* rawColorPixelPtr = this.downsampledDeltaFromReferenceColorPixels)
            {
                int* rawColorPixels = (int*)rawColorPixelPtr;

                // Note we run this only for the source image height pixels to sparsely fill the destination with rows
                Parallel.For(
                    0,
                    this.downsampledHeight,
                    y =>
                    {
                        int destIndex = y * upsampleWidth * factor;
                        int sourceColorIndex = y * this.downsampledWidth;

                        for (int x = 0; x < this.downsampledWidth; ++x, ++sourceColorIndex)
                        {
                            int color = rawColorPixels[sourceColorIndex];

                            // Replicate pixels horizontally
                            for (int colFactorIndex = 0; colFactorIndex < factor; ++colFactorIndex, ++destIndex)
                            {
                                // Replicate pixels vertically
                                for (int rowFactorIndex = 0; rowFactorIndex < factor; ++rowFactorIndex)
                                {
                                    // Copy color pixel
                                    this.deltaFromReferenceFramePixelsArgb[destIndex + (rowFactorIndex * upsampleWidth)] = color;
                                }
                            }
                        }
                    });
            }

            int sizeOfInt = sizeof(int);
            int rowByteSize = this.downsampledHeight * sizeOfInt;

            // Duplicate the remaining rows with memcpy
            for (int y = 0; y < this.downsampledHeight; ++y)
            {
                // iterate only for the smaller number of rows
                int srcRowIndex = upsampleWidth * factor * y;

                // Duplicate lines
                for (int r = 1; r < factor; ++r)
                {
                    int index = upsampleWidth * ((y * factor) + r);

                    System.Buffer.BlockCopy(
                        this.deltaFromReferenceFramePixelsArgb, srcRowIndex * sizeOfInt, this.deltaFromReferenceFramePixelsArgb, index * sizeOfInt, rowByteSize);
                }
            }
        }

        /// <summary>
        /// Updates the align deltas image given the current mode
        /// </summary>
        private void UpdateAlignDeltas()
        {
            if (this.autoFindCameraPoseWhenLost)
            {
                Dispatcher.BeginInvoke(
                    (Action)
                    (() =>
                        this.RenderAlignDeltasColorImage(
                            ref this.deltaFromReferenceFrameBitmap,
                            this.deltaFromReferenceImage)));
            }
            else
            {
                Dispatcher.BeginInvoke(
                    (Action)
                    (() =>
                        this.RenderAlignDeltasFloatImage(
                            this.deltaFromReferenceFrame,
                            ref this.deltaFromReferenceFrameBitmap,
                            this.deltaFromReferenceImage)));
            }
        }
    }
}
