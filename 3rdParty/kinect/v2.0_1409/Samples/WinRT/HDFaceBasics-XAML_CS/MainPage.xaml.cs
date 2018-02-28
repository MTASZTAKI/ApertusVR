// --------------------------------------------------------------------------------------------------------------------
// <copyright file="MainPage.xaml.cs" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
// --------------------------------------------------------------------------------------------------------------------

namespace Microsoft.Samples.Kinect.HDFaceBasics
{
    using System;
    using System.Collections.Generic;
    using System.ComponentModel;
    using Microsoft.Kinect.Face;
    using Windows.Foundation;
    using Windows.UI.Xaml;
    using Windows.UI.Xaml.Controls;
    using Windows.UI.Xaml.Navigation;
    using WindowsPreview.Kinect;

    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class MainPage : Page, INotifyPropertyChanged, IDisposable
    {
        /// <summary>
        /// Currently used KinectSensor
        /// </summary>
        private KinectSensor sensor = null;

        /// <summary>
        /// Body frame source to get a BodyFrameReader
        /// </summary>
        private BodyFrameSource bodySource = null;

        /// <summary>
        /// Body frame reader to get body frames
        /// </summary>
        private BodyFrameReader bodyReader = null;

        /// <summary>
        /// HighDefinitionFaceFrameSource to get a reader and a builder from.
        /// Also to set the currently tracked user id to get High Definition Face Frames of
        /// </summary>
        private HighDefinitionFaceFrameSource highDefinitionFaceFrameSource = null;

        /// <summary>
        /// HighDefinitionFaceFrameReader to read HighDefinitionFaceFrame to get FaceAlignment
        /// </summary>
        private HighDefinitionFaceFrameReader highDefinitionFaceFrameReader = null;

        /// <summary>
        /// FaceAlignment is the result of tracking a face, it has face animations location and orientation
        /// </summary>
        private FaceAlignment currentFaceAlignment = null;

        /// <summary>
        /// FaceModel is a result of capturing a face
        /// </summary>
        private FaceModel currentFaceModel = null;

        /// <summary>
        /// Indices don't change, saving them one time is enough
        /// </summary>
        private IReadOnlyList<uint> cachedFaceIndices = null;

        /// <summary>
        /// FaceModelBuilder is used to produce a FaceModel
        /// </summary>
        private FaceModelBuilder faceModelBuilder = null;

        /// <summary>
        /// Face capture operation
        /// </summary>
        private IAsyncOperation<FaceModelData> currentModelCollectionOperation = null;

        /// <summary>
        /// Current face model builder collection status string
        /// </summary>
        private string currentCollectionStatusString = "";

        /// <summary>
        /// Current face model builder capture status string
        /// </summary>
        private string currentCaptureStatusString = "Ready To Start Capture";

        /// <summary>
        /// Array of the bodies acquired from a BodyFrame
        /// </summary>
        Body[] bodies = null;

        /// <summary>
        /// The currently tracked body
        /// </summary>
        private Body currentTrackedBody = null;

        /// <summary>
        /// The currently tracked body Id
        /// </summary>
        private ulong currentTrackingId = 0;

        /// <summary>
        /// Current status text to display
        /// </summary>
        private string statusText = null;

        /// <summary>
        /// Initializes a new instance of the MainPage class.
        /// </summary>
        public MainPage()
        {
            this.InitializeComponent();
            this.DataContext = this;

            this.StatusText = this.MakeStatusText();
        }

        /// <summary>
        /// Property changed event
        /// </summary>
        public event PropertyChangedEventHandler PropertyChanged;

        /// <summary>
        /// Gets or sets the current status text to display
        /// </summary>
        public string StatusText
        {
            get
            {
                return this.statusText;
            }

            set
            {
                if (this.statusText != value)
                {
                    this.statusText = value;

                    // notify any bound elements that the text has changed
                    if (this.PropertyChanged != null)
                    {
                        this.PropertyChanged(this, new PropertyChangedEventArgs("StatusText"));
                    }
                }
            }
        }

        /// <summary>
        /// Called when disposed of
        /// </summary>
        public void Dispose()
        {
            this.Dispose(true);
            GC.SuppressFinalize(this);
        }

        /// <summary>
        /// Dispose based on whether or not managed or native resources should be freed
        /// </summary>
        /// <param name="disposing">Set to true to free both native and managed resources, false otherwise</param>
        public void Dispose(bool disposing)
        {
            if (disposing)
            {
                if (this.currentFaceModel != null)
                {
                    this.currentFaceModel.Dispose();
                    this.currentFaceModel = null;
                }
            }
        }
        
        /// <summary>
        /// When the page is navigated to event handler
        /// </summary>
        /// <param name="e">event arguments</param>
        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            this.HDFaceRenderingPanel.StartRenderLoop();
        }

        /// <summary>
        /// Helper function to format a status message
        /// </summary>
        /// <returns>Status text</returns>
        private string MakeStatusText()
        {
            return string.Format("Capture: {0} Collection: {1}, Tracking ID = {2}", this.currentCaptureStatusString, this.currentCollectionStatusString, this.currentTrackingId.ToString());
        }

        /// <summary>
        /// Fires when Window is Loaded
        /// </summary>
        /// <param name="sender">object sending the event</param>
        /// <param name="e">event arguments</param>
        private void Page_Loaded(object sender, RoutedEventArgs e)
        {
            this.InitializeHDFace();
        }

        /// <summary>
        /// Initialize Kinect object
        /// </summary>
        private void InitializeHDFace()
        {
            this.sensor = KinectSensor.GetDefault();

            this.bodySource = this.sensor.BodyFrameSource;
            this.bodies = new Body[this.bodySource.BodyCount];

            this.bodyReader = this.bodySource.OpenReader();
            this.bodyReader.FrameArrived += this.BodyReader_FrameArrived;

            this.highDefinitionFaceFrameSource = new HighDefinitionFaceFrameSource(this.sensor);
            this.highDefinitionFaceFrameReader = this.highDefinitionFaceFrameSource.OpenReader();
            this.highDefinitionFaceFrameReader.FrameArrived += this.HDFaceReader_FrameArrived;

            this.currentFaceModel = new FaceModel();
            this.currentFaceAlignment = new FaceAlignment();
            this.cachedFaceIndices = FaceModel.TriangleIndices;

            this.UpdateFaceMesh();

            this.sensor.Open();
        }

        /// <summary>
        /// Sends the new deformed mesh to be drawn
        /// </summary>
        private void UpdateFaceMesh()
        {
            var faceVertices = this.currentFaceModel.CalculateVerticesForAlignment(this.currentFaceAlignment);
            this.HDFaceRenderingPanel.UpdateMesh(faceVertices, this.cachedFaceIndices);
        }

        /// <summary>
        /// This event fires when a BodyFrame is ready for consumption
        /// </summary>
        /// <param name="sender">object sending the event</param>
        /// <param name="e">event arguments</param>
        private void BodyReader_FrameArrived(object sender, BodyFrameArrivedEventArgs e)
        {
            var frameReference = e.FrameReference;

            using (var frame = frameReference.AcquireFrame())
            {
                if (frame == null)
                {
                    return;
                }

                // Get the latest bodies
                frame.GetAndRefreshBodyData(this.bodies);
            }

            // Do we still see the person we're tracking?
            if (this.currentTrackedBody != null)
            {
                this.currentTrackedBody = this.FindBodyWithTrackingId(this.currentTrackingId);

                if (this.currentTrackedBody != null)
                {
                    // We still see the person we're tracking, make no change.
                    return;
                }
            }

            Body selectedBody = this.FindClosestBody();

            if (selectedBody == null)
            {
                this.currentTrackedBody = null;
                this.currentTrackingId = 0;

                return;
            }

            this.currentTrackedBody = selectedBody;
            this.currentTrackingId = selectedBody.TrackingId;

            this.highDefinitionFaceFrameSource.TrackingId = this.currentTrackingId;
        }

        /// <summary>
        /// Returns the length of a vector from origin
        /// </summary>
        /// <param name="point">Point in space to find it's distance from origin</param>
        /// <returns>Distance from origin</returns>
        private double VectorLength(CameraSpacePoint point)
        {
            return Math.Sqrt(Math.Pow(point.X, 2) + Math.Pow(point.Y, 2) + Math.Pow(point.Z, 2));
        }

        /// <summary>
        /// Finds the closest body from the sensor if any
        /// </summary>
        /// <returns>Closest body, null of none</returns>
        private Body FindClosestBody()
        {
            Body result = null;
            double closestBodyDistance = double.MaxValue;

            foreach (var body in this.bodies)
            {
                if (body.IsTracked)
                {
                    var joints = body.Joints;
                    var currentJoint = joints[JointType.SpineBase];

                    var currentLocation = currentJoint.Position;

                    var currentDistance = this.VectorLength(currentLocation);

                    if (result == null || currentDistance < closestBodyDistance)
                    {
                        result = body;
                        closestBodyDistance = currentDistance;
                    }
                }
            }

            return result;
        }

        /// <summary>
        /// Find if there is a body tracked with the given trackingId
        /// </summary>
        /// <param name="trackingId">The tracking Id</param>
        /// <returns>The body object, null of none</returns>
        private Body FindBodyWithTrackingId(ulong trackingId)
        {
            Body result = null;

            foreach (var body in this.bodies)
            {
                if (body.IsTracked)
                {
                    if (body.TrackingId == trackingId)
                    {
                        result = body;
                        break;
                    }
                }
            }

            return result;
        }

        /// <summary>
        /// This event is fired when a new HDFace frame is ready for consumption
        /// </summary>
        /// <param name="sender">object sending the event</param>
        /// <param name="e">event arguments</param>
        private void HDFaceReader_FrameArrived(object sender, HighDefinitionFaceFrameArrivedEventArgs e)
        {
            var frameReference = e.FrameReference;

            // We might miss the chance to acquire the frame; it will be null if it's missed.
            // Also ignore this frame if face tracking failed.
            using (var frame = frameReference.AcquireFrame())
            {
                if (frame == null || !frame.IsFaceTracked)
                {
                    return;
                }

                frame.GetAndRefreshFaceAlignmentResult(this.currentFaceAlignment);

                this.UpdateFaceMesh();
            }
            this.StatusText = this.MakeStatusText();
        }

        /// <summary>
        /// This event is fired when the FaceModelBuilder collection status has changed
        /// </summary>
        /// <param name="sender">object sending the event</param>
        /// <param name="e">event arguments</param>
        private void FaceModelBuilder_CollectionStatusChanged(object sender, CollectionStatusChangedEventArgs e)
        {
            var modelBuilder = sender as FaceModelBuilder;

            FaceModelBuilderCollectionStatus newStatus = modelBuilder.CollectionStatus; //Query the latest status

            this.currentCollectionStatusString = BuildCollectionStatusText(newStatus);
        }

        /// <summary>
        /// This event is fired when the FaceModelBuilder capture status has changed
        /// </summary>
        /// <param name="sender">object sending the event</param>
        /// <param name="e">event arguments</param>
        private void FaceModelBuilder_CaptureStatusChanged(object sender, CaptureStatusChangedEventArgs e)
        {
            var modelBuilder = sender as FaceModelBuilder;

            FaceModelBuilderCaptureStatus newStatus = modelBuilder.CaptureStatus;

            this.currentCaptureStatusString = GetCaptureStatusText(newStatus);
        }

        /// <summary>
        /// Start a face capture on clicking the button
        /// </summary>
        /// <param name="sender">object sending the event</param>
        /// <param name="e">event arguments</param>
        private void StartCaptureButton_Click(object sender, RoutedEventArgs e)
        {
            this.StartCapture();
        }

        /// <summary>
        /// Cancel the current face capture operation
        /// </summary>
        private void StopFaceCapture()
        {
            if (this.currentModelCollectionOperation != null)
            {
                this.currentModelCollectionOperation.Cancel();
                this.currentModelCollectionOperation = null;
            }

            if (this.faceModelBuilder != null)
            {
                this.faceModelBuilder.CollectionStatusChanged -= this.FaceModelBuilder_CollectionStatusChanged;
                this.faceModelBuilder.CaptureStatusChanged -= this.FaceModelBuilder_CaptureStatusChanged;
                this.faceModelBuilder.Dispose();
                this.faceModelBuilder = null;
            }
        }

        /// <summary>
        /// Start a face capture operation
        /// </summary>
        private void StartCapture()
        {
            if (this.currentModelCollectionOperation != null)
            {
                this.StopFaceCapture();
            }

            this.faceModelBuilder = this.highDefinitionFaceFrameSource.OpenModelBuilder(FaceModelBuilderAttributes.None);
            this.faceModelBuilder.CollectionStatusChanged += this.FaceModelBuilder_CollectionStatusChanged;
            this.faceModelBuilder.CaptureStatusChanged += this.FaceModelBuilder_CaptureStatusChanged;

            this.currentModelCollectionOperation = this.faceModelBuilder.CollectFaceDataAsync();
            this.currentModelCollectionOperation.Completed = new AsyncOperationCompletedHandler<FaceModelData>((IAsyncOperation<FaceModelData> asyncOp, AsyncStatus status) =>
            {
                if (status == AsyncStatus.Completed)
                {
                    using (var modelData = asyncOp.GetResults())
                    {
                        this.currentFaceModel = modelData.ProduceFaceModel();
                    }

                    this.StopFaceCapture();
                    currentCaptureStatusString = "Capture Complete";
                    currentCollectionStatusString = "";
                }
                else if (status == AsyncStatus.Error)
                {
                    this.StopFaceCapture();
                    currentCaptureStatusString = "Error collecting face data!";
                    currentCollectionStatusString = "";
                }
                else
                {
                    this.StopFaceCapture();
                    currentCaptureStatusString = "Collecting face data incomplete!";
                    currentCollectionStatusString = "";
                }
            });
        }

        /// <summary>
        /// Build a string from the collection status bit flag combinations (simplified)
        /// </summary>
        /// <param name="status">Status value</param>
        /// <returns>Status value as text</returns>
        private string BuildCollectionStatusText(FaceModelBuilderCollectionStatus status)
        {
            string res = String.Empty;

            if (FaceModelBuilderCollectionStatus.Complete == status)
            {
                return " Done ";
            }

            if ((status & FaceModelBuilderCollectionStatus.MoreFramesNeeded) != 0)
            {
                res += " More ";
            }

            if ((status & FaceModelBuilderCollectionStatus.FrontViewFramesNeeded) != 0)
            {
                res += " Front";
            }

            if ((status & FaceModelBuilderCollectionStatus.LeftViewsNeeded) != 0)
            {
                res += " Left ";
            }

            if ((status & FaceModelBuilderCollectionStatus.RightViewsNeeded) != 0)
            {
                res += " Right";
            }

            if ((status & FaceModelBuilderCollectionStatus.TiltedUpViewsNeeded) != 0)
            {
                res += "  Up  ";
            }

            return res;
        }

        /// <summary>
        /// Gets the current capture status
        /// </summary>
        /// <param name="status">Status value</param>
        /// <returns>Status value as text</returns>
        private string GetCaptureStatusText(FaceModelBuilderCaptureStatus status)
        {
            switch (status)
            {
                case FaceModelBuilderCaptureStatus.FaceTooFar:
                    return "  Too Far ";
                case FaceModelBuilderCaptureStatus.FaceTooNear:
                    return " Too Near ";
                case FaceModelBuilderCaptureStatus.GoodFrameCapture:
                    return "Good Frame";
                case FaceModelBuilderCaptureStatus.LostFaceTrack:
                    return "Lost Track";
                case FaceModelBuilderCaptureStatus.MovingTooFast:
                    return " Too Fast ";
                case FaceModelBuilderCaptureStatus.OtherViewsNeeded:
                    return "More Views";
                case FaceModelBuilderCaptureStatus.SystemError:
                    return "  Error   ";
            }

            return string.Empty;
        }
    }
}
