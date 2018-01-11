//------------------------------------------------------------------------------
// <copyright file="MainPage.xaml.cs" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//------------------------------------------------------------------------------

using System;
using System.Collections.Generic;
using System.ComponentModel;
using Windows.ApplicationModel.Resources;
using Windows.Foundation;
using Windows.UI;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Shapes;
using WindowsPreview.Kinect;

namespace Microsoft.Samples.Kinect.BodyBasics
{
    /// <summary>
    /// Main page for sample
    /// </summary>
    public sealed partial class MainPage : Page, INotifyPropertyChanged
    {
        /// <summary>
        /// Resource loader for string resources
        /// </summary>
#if WIN81ORLATER
        private ResourceLoader resourceLoader = ResourceLoader.GetForCurrentView("Resources");
#else
        private ResourceLoader resourceLoader = new ResourceLoader("Resources");
#endif

        /// <summary>
        /// Radius of drawn hand circles
        /// </summary>
        private const double HighConfidenceHandSize = 40;

        /// <summary>
        /// Radius of drawn hand circles
        /// </summary>
        private const double LowConfidenceHandSize = 20;

        /// <summary>
        /// Thickness of drawn joint lines
        /// </summary>
        private const double JointThickness = 8.0;

        /// <summary>
        /// Thickness of seen bone lines
        /// </summary>
        private const double TrackedBoneThickness = 4.0;

        /// <summary>
        /// Thickness of inferred joint lines
        /// </summary>
        private const double InferredBoneThickness = 1.0;

        /// <summary>
        /// Thickness of clip edge rectangles
        /// </summary>
        private const double ClipBoundsThickness = 5;

        /// <summary>
        /// Constant for clamping Z values of camera space points from being negative
        /// </summary>
        private const float InferredZPositionClamp = 0.1f;

        /// <summary>
        /// Active Kinect sensor
        /// </summary>
        private KinectSensor kinectSensor = null;

        /// <summary>
        /// Coordinate mapper to map one type of point to another
        /// </summary>
        private CoordinateMapper coordinateMapper = null;

        /// <summary>
        /// Reader for body frames
        /// </summary>
        private BodyFrameReader bodyFrameReader = null;

        /// <summary>
        /// Array for the bodies
        /// </summary>
        private Body[] bodies = null;

        /// <summary>
        /// Current status text to display
        /// </summary>
        private string statusText = null;

        /// <summary>
        /// Main Canvas that contains all visual objects for all bodies and clipped edges
        /// </summary>
        private Canvas drawingCanvas;

        /// <summary>
        /// List of BodyInfo objects for each potential body
        /// </summary>
        private BodyInfo[] BodyInfos;

        /// <summary>
        /// List of colors for each body tracked
        /// </summary>
        private List<Color> BodyColors;

        /// <summary>
        /// Clipped edges rectangles
        /// </summary>
        private Rectangle LeftClipEdge;
        private Rectangle RightClipEdge;
        private Rectangle TopClipEdge;
        private Rectangle BottomClipEdge;

        private int BodyCount
        {
            set
            {
                if (value == 0)
                {
                    this.BodyInfos = null;
                    return;
                }

                // creates instances of BodyInfo objects for potential number of bodies
                if (this.BodyInfos == null || this.BodyInfos.Length != value)
                {
                    this.BodyInfos = new BodyInfo[value];

                    for (int bodyIndex = 0; bodyIndex < this.bodies.Length; bodyIndex++)
                    {
                        this.BodyInfos[bodyIndex] = new BodyInfo(this.BodyColors[bodyIndex]);
                    }
                }
            }

            get { return this.BodyInfos == null ? 0 : this.BodyInfos.Length; }
        }

        private float JointSpaceWidth { get; set; }

        private float JointSpaceHeight { get; set; }

        /// <summary>
        /// Initializes a new instance of the MainPage class.
        /// </summary>
        public MainPage()
        {
            // one sensor is currently supported
            this.kinectSensor = KinectSensor.GetDefault();

            // get the coordinate mapper
            this.coordinateMapper = this.kinectSensor.CoordinateMapper;

            // get the depth (display) extents
            FrameDescription frameDescription = this.kinectSensor.DepthFrameSource.FrameDescription;

            // get size of joint space
            this.JointSpaceWidth = frameDescription.Width;
            this.JointSpaceHeight = frameDescription.Height;

            // get total number of bodies from BodyFrameSource
            this.bodies = new Body[this.kinectSensor.BodyFrameSource.BodyCount];

            // open the reader for the body frames
            this.bodyFrameReader = this.kinectSensor.BodyFrameSource.OpenReader();

            // wire handler for frame arrival
            this.bodyFrameReader.FrameArrived += this.Reader_BodyFrameArrived;
            
            // set IsAvailableChanged event notifier
            this.kinectSensor.IsAvailableChanged += this.Sensor_IsAvailableChanged;

            // populate body colors, one for each BodyIndex
            this.BodyColors = new List<Color>
            {
                Colors.Red,
                Colors.Orange,
                Colors.Green,
                Colors.Blue,
                Colors.Indigo,
                Colors.Violet
            };

            // sets total number of possible tracked bodies
            // create ellipses and lines for drawing bodies
            this.BodyCount = this.kinectSensor.BodyFrameSource.BodyCount;

            // Instantiate a new Canvas
            this.drawingCanvas = new Canvas();

            // open the sensor
            this.kinectSensor.Open();

            // set the status text
            this.StatusText = this.kinectSensor.IsAvailable ? resourceLoader.GetString("RunningStatusText")
                                                            : resourceLoader.GetString("NoSensorStatusText");

            // use the window object as the view model in this simple example
            this.DataContext = this;

            // initialize the components (controls) of the window
            this.InitializeComponent();

            // set the clip rectangle to prevent rendering outside the canvas
            this.drawingCanvas.Clip = new RectangleGeometry();
            this.drawingCanvas.Clip.Rect = new Rect(0.0, 0.0, this.DisplayGrid.Width, this.DisplayGrid.Height);

            // create visual objects for drawing joints, bone lines, and clipped edges
            this.PopulateVisualObjects();

            // add canvas to DisplayGrid
            this.DisplayGrid.Children.Add(this.drawingCanvas);
        }

        /// <summary>
        /// INotifyPropertyChangedPropertyChanged event to allow window controls to bind to changeable data
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
        /// Execute shutdown tasks
        /// </summary>
        /// <param name="sender">object sending the event</param>
        /// <param name="e">event arguments</param>
        private void MainPage_Unloaded(object sender, RoutedEventArgs e)
        {
            if (this.bodyFrameReader != null)
            {
                // BodyFrameReder is IDisposable
                this.bodyFrameReader.Dispose();
                this.bodyFrameReader = null;
            }

            // Body is IDisposable
            if (this.bodies != null)
            {
                foreach (Body body in this.bodies)
                {
                    if (body != null)
                    {
                        body.Dispose();
                    }
                }
            }

            if (this.kinectSensor != null)
            {
                this.kinectSensor.Close();
                this.kinectSensor = null;
            }
        }

        /// <summary>
        /// Handles the body frame data arriving from the sensor
        /// </summary>
        /// <param name="sender">object sending the event</param>
        /// <param name="e">event arguments</param>
        private void Reader_BodyFrameArrived(object sender, BodyFrameArrivedEventArgs e)
        {
            bool dataReceived = false;
            bool hasTrackedBody = false;

            using (BodyFrame bodyFrame = e.FrameReference.AcquireFrame())
            {
                if (bodyFrame != null)
                {
                    bodyFrame.GetAndRefreshBodyData(this.bodies);
                    dataReceived = true;
                }
            }

            if (dataReceived)
            {
                this.BeginBodiesUpdate();

                // iterate through each body
                for (int bodyIndex = 0; bodyIndex < this.bodies.Length; bodyIndex++)
                {
                    Body body = this.bodies[bodyIndex];

                    if (body.IsTracked)
                    {
                        // check if this body clips an edge
                        this.UpdateClippedEdges(body, hasTrackedBody);

                        this.UpdateBody(body, bodyIndex);

                        hasTrackedBody = true;
                    }
                    else
                    {
                        // collapse this body from canvas as it goes out of view
                        this.ClearBody(bodyIndex);
                    }
                }

                if (!hasTrackedBody)
                {
                    // clear clipped edges if no bodies are tracked
                    this.ClearClippedEdges();
                }
            }
        }

        /// <summary>
        /// Clear update status of all bodies
        /// </summary>
        internal void BeginBodiesUpdate()
        {
            if (this.BodyInfos != null)
            {
                foreach (var bodyInfo in this.BodyInfos)
                {
                    bodyInfo.Updated = false;
                }
            }
        }

        /// <summary>
        /// Update body data for each body that is tracked.
        /// </summary>
        /// <param name="body">body for getting joint info</param>
        /// <param name="bodyIndex">index for body we are currently updating</param>
        internal void UpdateBody(Body body, int bodyIndex)
        {
            IReadOnlyDictionary<JointType, Joint> joints = body.Joints;
            var jointPointsInDepthSpace = new Dictionary<JointType, Point>();

            var bodyInfo = this.BodyInfos[bodyIndex];

            CoordinateMapper coordinateMapper = this.kinectSensor.CoordinateMapper;

            // update all joints
            foreach (var jointType in body.Joints.Keys)
            {
                // sometimes the depth(Z) of an inferred joint may show as negative
                // clamp down to 0.1f to prevent coordinatemapper from returning (-Infinity, -Infinity)
                CameraSpacePoint position = body.Joints[jointType].Position;
                if (position.Z < 0)
                {
                    position.Z = InferredZPositionClamp;
                }

                // map joint position to depth space
                DepthSpacePoint depthSpacePoint = coordinateMapper.MapCameraPointToDepthSpace(position);
                jointPointsInDepthSpace[jointType] = new Point(depthSpacePoint.X, depthSpacePoint.Y);

                // modify the joint's visibility and location
                this.UpdateJoint(bodyInfo.JointPoints[jointType], joints[jointType], jointPointsInDepthSpace[jointType]);

                // modify hand ellipse colors based on hand states
                // modity hand ellipse sizes based on tracking confidences
                if (jointType == JointType.HandRight)
                {
                    this.UpdateHand(bodyInfo.HandRightEllipse, body.HandRightState, body.HandRightConfidence, jointPointsInDepthSpace[jointType]);
                }

                if (jointType == JointType.HandLeft)
                {
                    this.UpdateHand(bodyInfo.HandLeftEllipse, body.HandLeftState, body.HandLeftConfidence, jointPointsInDepthSpace[jointType]);
                }
            }

            // update all bones
            foreach (var bone in bodyInfo.Bones)
            {
                this.UpdateBone(bodyInfo.BoneLines[bone], joints[bone.Item1], joints[bone.Item2],
                                jointPointsInDepthSpace[bone.Item1],
                                jointPointsInDepthSpace[bone.Item2]);
            }
        }

        /// <summary>
        /// Collapse the body from the canvas.
        /// </summary>
        /// <param name="bodyIndex"></param>
        private void ClearBody(int bodyIndex)
        {
            var bodyInfo = this.BodyInfos[bodyIndex];

            // collapse all joint ellipses
            foreach (var joint in bodyInfo.JointPoints)
            {
                joint.Value.Visibility = Windows.UI.Xaml.Visibility.Collapsed;
            }

            // collapse all bone lines
            foreach (var bone in bodyInfo.Bones)
            {
                bodyInfo.BoneLines[bone].Visibility = Windows.UI.Xaml.Visibility.Collapsed;
            }

            // collapse handstate ellipses
            bodyInfo.HandLeftEllipse.Visibility = Windows.UI.Xaml.Visibility.Collapsed;

            bodyInfo.HandRightEllipse.Visibility = Windows.UI.Xaml.Visibility.Collapsed;
        }

        /// <summary>
        /// Updates hand state ellipses depending on tracking state and it's confidence.
        /// </summary>
        /// <param name="ellipse">ellipse representing handstate</param>
        /// <param name="handState">open, closed, or lasso</param>
        /// <param name="trackingConfidence">confidence of handstate</param>
        /// <param name="point">location of handjoint</param>
        private void UpdateHand(Ellipse ellipse, HandState handState, TrackingConfidence trackingConfidence, Point point)
        {
            ellipse.Fill = new SolidColorBrush(this.HandStateToColor(handState));

            // draw handstate ellipse based on tracking confidence
            ellipse.Width = ellipse.Height = (trackingConfidence == TrackingConfidence.Low) ? LowConfidenceHandSize : HighConfidenceHandSize;

            ellipse.Visibility = Windows.UI.Xaml.Visibility.Visible;

            // don't draw handstate if hand joints are not tracked
            if (!Double.IsInfinity(point.X) && !Double.IsInfinity(point.Y))
            {
                Canvas.SetLeft(ellipse, point.X - ellipse.Width / 2);
                Canvas.SetTop(ellipse, point.Y - ellipse.Width / 2);
            }
        }

        /// <summary>
        /// Update a joint.
        /// </summary>
        /// <param name="ellipse"></param>
        /// <param name="joint"></param>
        /// <param name="point"></param>
        private void UpdateJoint(Ellipse ellipse, Joint joint, Point point)
        {
            TrackingState trackingState = joint.TrackingState;

            // only draw if joint is tracked or inferred
            if (trackingState != TrackingState.NotTracked)
            {
                if (trackingState == TrackingState.Tracked)
                {
                    ellipse.Fill = new SolidColorBrush(Colors.Green);
                }
                else
                {
                    // inferred joints are yellow
                    ellipse.Fill = new SolidColorBrush(Colors.Yellow);
                }

                Canvas.SetLeft(ellipse, point.X - JointThickness / 2);
                Canvas.SetTop(ellipse, point.Y - JointThickness / 2);

                ellipse.Visibility = Windows.UI.Xaml.Visibility.Visible;
            }
            else
            {
                ellipse.Visibility = Windows.UI.Xaml.Visibility.Collapsed;
            }
        }

        /// <summary>
        /// Update a bone line.
        /// </summary>
        /// <param name="line">line representing a bone line</param>
        /// <param name="startJoint">start joint of bone line</param>
        /// <param name="endJoint">end joint of bone line</param>
        /// <param name="startPoint">location of start joint</param>
        /// <param name="endPoint">location of end joint</param>
        private void UpdateBone(Line line, Joint startJoint, Joint endJoint, Point startPoint, Point endPoint)
        {
            // don't draw if neither joints are tracked
            if (startJoint.TrackingState == TrackingState.NotTracked || endJoint.TrackingState == TrackingState.NotTracked)
            {
                line.Visibility = Windows.UI.Xaml.Visibility.Collapsed;
                return;
            }

            // all lines are inferred thickness unless both joints are tracked
            line.StrokeThickness = InferredBoneThickness;

            if (startJoint.TrackingState == TrackingState.Tracked &&
                endJoint.TrackingState == TrackingState.Tracked)
            {
                line.StrokeThickness = TrackedBoneThickness;
            }

            line.Visibility = Windows.UI.Xaml.Visibility.Visible;

            line.X1 = startPoint.X;
            line.Y1 = startPoint.Y;
            line.X2 = endPoint.X;
            line.Y2 = endPoint.Y;
        }

        /// <summary>
        /// Draws indicators to show which edges are clipping body data.
        /// </summary>
        /// <param name="body">body to draw clipping information for</param>
        /// <param name="hasTrackedBody">bool to determine if another body is triggering a clipped edge</param>
        private void UpdateClippedEdges(Body body, bool hasTrackedBody)
        {
            // BUG (waiting for confirmation): 
            // Clip dectection works differently for top and right edges compared to left and bottom edges
            // due to the current joint confidence model. This is an ST issue.
            // Joints become inferred immediately as they touch the left/bottom edges and clip detection triggers.
            // Joints squish on the right/top edges and clip detection doesn't trigger until more joints of 
            // the body goes out of view (e.g all hand joints vs only handtip).

            FrameEdges clippedEdges = body.ClippedEdges;

            if (clippedEdges.HasFlag(FrameEdges.Left))
            {
                this.LeftClipEdge.Visibility = Windows.UI.Xaml.Visibility.Visible;
            }
            else if(!hasTrackedBody)
            {
                // don't clear this edge if another body is triggering clipped edge
                this.LeftClipEdge.Visibility = Windows.UI.Xaml.Visibility.Collapsed;
            }

            if (clippedEdges.HasFlag(FrameEdges.Right))
            {
                this.RightClipEdge.Visibility = Windows.UI.Xaml.Visibility.Visible;
            }
            else if (!hasTrackedBody)
            {
                this.RightClipEdge.Visibility = Windows.UI.Xaml.Visibility.Collapsed;
            }

            if (clippedEdges.HasFlag(FrameEdges.Top))
            {
                this.TopClipEdge.Visibility = Windows.UI.Xaml.Visibility.Visible;
            }
            else if (!hasTrackedBody)
            {
                this.TopClipEdge.Visibility = Windows.UI.Xaml.Visibility.Collapsed;
            }

            if (clippedEdges.HasFlag(FrameEdges.Bottom))
            {
                this.BottomClipEdge.Visibility = Windows.UI.Xaml.Visibility.Visible;
            }
            else if (!hasTrackedBody)
            {
                this.BottomClipEdge.Visibility = Windows.UI.Xaml.Visibility.Collapsed;
            }
        }

        /// <summary>
        /// Clear all clipped edges.
        /// </summary>
        private void ClearClippedEdges()
        {
            this.LeftClipEdge.Visibility = Windows.UI.Xaml.Visibility.Collapsed;

            this.RightClipEdge.Visibility = Windows.UI.Xaml.Visibility.Collapsed;

            this.TopClipEdge.Visibility = Windows.UI.Xaml.Visibility.Collapsed;

            this.BottomClipEdge.Visibility = Windows.UI.Xaml.Visibility.Collapsed;
        }

        /// <summary>
        /// Select color of hand state
        /// </summary>
        /// <param name="handState"></param>
        /// <returns></returns>
        private Color HandStateToColor(HandState handState)
        {
            switch (handState)
            {
                case HandState.Open:
                    return Colors.Green;

                case HandState.Closed:
                    return Colors.Red;

                case HandState.Lasso:
                    return Colors.Blue;
            }

            return Colors.Transparent;
        }

        /// <summary>
        /// Instantiate new objects for joints, bone lines, and clipped edge rectangles
        /// </summary>
        private void PopulateVisualObjects()
        {
            // create clipped edges and set to collapsed initially
            this.LeftClipEdge = new Rectangle()
            {
                Fill = new SolidColorBrush(Colors.Red),
                Width = ClipBoundsThickness,
                Height = this.DisplayGrid.Height,
                Visibility = Windows.UI.Xaml.Visibility.Collapsed
            };

            this.RightClipEdge = new Rectangle()
            {
                Fill = new SolidColorBrush(Colors.Red),
                Width = ClipBoundsThickness,
                Height = this.DisplayGrid.Height,
                Visibility = Windows.UI.Xaml.Visibility.Collapsed
            };

            this.TopClipEdge = new Rectangle()
            {
                Fill = new SolidColorBrush(Colors.Red),
                Width = this.DisplayGrid.Width,
                Height = ClipBoundsThickness,
                Visibility = Windows.UI.Xaml.Visibility.Collapsed
            };

            this.BottomClipEdge = new Rectangle()
            {
                Fill = new SolidColorBrush(Colors.Red),
                Width = this.DisplayGrid.Width,
                Height = ClipBoundsThickness,
                Visibility = Windows.UI.Xaml.Visibility.Collapsed
            };

            foreach (var bodyInfo in this.BodyInfos)
            {
                // add left and right hand ellipses of all bodies to canvas
                this.drawingCanvas.Children.Add(bodyInfo.HandLeftEllipse);
                this.drawingCanvas.Children.Add(bodyInfo.HandRightEllipse);

                // add joint ellipses of all bodies to canvas
                foreach (var joint in bodyInfo.JointPoints)
                {
                    this.drawingCanvas.Children.Add(joint.Value);
                }

                // add bone lines of all bodies to canvas
                foreach (var bone in bodyInfo.Bones)
                {
                    this.drawingCanvas.Children.Add(bodyInfo.BoneLines[bone]);
                }
            }

            // add clipped edges rectanges to main canvas
            this.drawingCanvas.Children.Add(this.LeftClipEdge);
            this.drawingCanvas.Children.Add(this.RightClipEdge);
            this.drawingCanvas.Children.Add(this.TopClipEdge);
            this.drawingCanvas.Children.Add(this.BottomClipEdge);

            // position the clipped edges
            Canvas.SetLeft(this.LeftClipEdge, 0);
            Canvas.SetTop(this.LeftClipEdge, 0);
            Canvas.SetLeft(this.RightClipEdge, this.DisplayGrid.Width - ClipBoundsThickness);
            Canvas.SetTop(this.RightClipEdge, 0);
            Canvas.SetLeft(this.TopClipEdge, 0);
            Canvas.SetTop(this.TopClipEdge, 0);
            Canvas.SetLeft(this.BottomClipEdge, 0);
            Canvas.SetTop(this.BottomClipEdge, this.DisplayGrid.Height - ClipBoundsThickness);
        }

        /// <summary>
        /// BodyInfo class that contains joint ellipses, handstate ellipses, lines for bones between two joints.
        /// </summary>
        private class BodyInfo
        {
            public bool Updated { get; set; }

            public Color BodyColor { get; set; }

            // ellipse representing left handstate
            public Ellipse HandLeftEllipse { get; set; }

            // ellipse representing right handstate
            public Ellipse HandRightEllipse { get; set; }

            // dictionary of all joints in a body
            public Dictionary<JointType, Ellipse> JointPoints { get; private set; }

            // definition of bones
            public TupleList<JointType, JointType> Bones { get; private set; }

            // collection of bones associated with the line object
            public Dictionary<Tuple<JointType, JointType>, Line> BoneLines { get; private set; }

            public BodyInfo(Color bodyColor)
            {
                this.BodyColor = bodyColor;
               
                // create hand state ellipses
                this.HandLeftEllipse = new Ellipse()
                {
                    Visibility = Windows.UI.Xaml.Visibility.Collapsed 
                };

                this.HandRightEllipse = new Ellipse()
                {
                    Visibility = Windows.UI.Xaml.Visibility.Collapsed
                };

                // a joint defined as a jointType with a point location in XY space represented by an ellipse
                this.JointPoints = new Dictionary<JointType, Ellipse>();

                // pre-populate list of joints and set to non-visible initially
                foreach (JointType jointType in Enum.GetValues(typeof(JointType)))
                {
                    this.JointPoints.Add(jointType, new Ellipse()
                    {
                        Visibility = Windows.UI.Xaml.Visibility.Collapsed,
                        Fill = new SolidColorBrush(BodyColor),
                        Width = JointThickness,
                        Height = JointThickness
                    });
                }

                // collection of bones
                this.BoneLines = new Dictionary<Tuple<JointType, JointType>, Line>();

                // a bone defined as a line between two joints
                this.Bones = new TupleList<JointType, JointType>
                {
                    // Torso
                    { JointType.Head, JointType.Neck },
                    { JointType.Neck, JointType.SpineShoulder },
                    { JointType.SpineShoulder, JointType.SpineMid },
                    { JointType.SpineMid, JointType.SpineBase },
                    { JointType.SpineShoulder, JointType.ShoulderRight },
                    { JointType.SpineShoulder, JointType.ShoulderLeft },
                    { JointType.SpineBase, JointType.HipRight },
                    { JointType.SpineBase, JointType.HipLeft },

                    // Right Arm
                    { JointType.ShoulderRight, JointType.ElbowRight },
                    { JointType.ElbowRight, JointType.WristRight },
                    { JointType.WristRight, JointType.HandRight },
                    { JointType.HandRight, JointType.HandTipRight },
                    { JointType.WristRight, JointType.ThumbRight },

                    // Left Arm
                    { JointType.ShoulderLeft, JointType.ElbowLeft },
                    { JointType.ElbowLeft, JointType.WristLeft },
                    { JointType.WristLeft, JointType.HandLeft },
                    { JointType.HandLeft, JointType.HandTipLeft },
                    { JointType.WristLeft, JointType.ThumbLeft },

                    // Right Leg
                    { JointType.HipRight, JointType.KneeRight },
                    { JointType.KneeRight, JointType.AnkleRight },
                    { JointType.AnkleRight, JointType.FootRight },
                
                    // Left Leg
                    { JointType.HipLeft, JointType.KneeLeft },
                    { JointType.KneeLeft, JointType.AnkleLeft },
                    { JointType.AnkleLeft, JointType.FootLeft },
                };

                // pre-populate list of bones that are non-visible initially
                foreach (var bone in this.Bones)
                {
                    this.BoneLines.Add(bone, new Line() 
                    { 
                        Stroke = new SolidColorBrush(BodyColor),
                        Visibility = Visibility.Collapsed
                    });
                }
            }
        }

        private class TupleList<T1, T2> : List<Tuple<T1, T2>>
        {
            public void Add(T1 item, T2 item2)
            {
                this.Add(new Tuple<T1, T2>(item, item2));
            }
        }

        /// <summary>
        /// Handles the event which the sensor becomes unavailable (E.g. paused, closed, unplugged).
        /// </summary>
        /// <param name="sender">object sending the event</param>
        /// <param name="e">event arguments</param>
        private void Sensor_IsAvailableChanged(object sender, IsAvailableChangedEventArgs e)
        {
            // on failure, set the status text
            if (!this.kinectSensor.IsAvailable)
            {
                this.StatusText = resourceLoader.GetString("SensorNotAvailableStatusText");
            }
            else
            {
                this.StatusText = resourceLoader.GetString("RunningStatusText");
            }
        }
    }
}
