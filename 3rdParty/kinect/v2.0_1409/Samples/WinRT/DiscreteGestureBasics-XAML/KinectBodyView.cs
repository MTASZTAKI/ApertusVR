//------------------------------------------------------------------------------
// <copyright file="KinectBodyView.cs" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//------------------------------------------------------------------------------

namespace Microsoft.Samples.Kinect.DiscreteGestureBasics
{
    using System;
    using System.Collections.Generic;
    using System.ComponentModel;
    using System.Runtime.CompilerServices;
    using Windows.Foundation;
    using Windows.UI;
    using Windows.UI.Xaml;
    using Windows.UI.Xaml.Controls;
    using Windows.UI.Xaml.Media;
    using Windows.UI.Xaml.Shapes;
    using WindowsPreview.Kinect;

    /// <summary>
    /// Visualizes the Kinect Body stream for display in the UI
    /// </summary>
    public sealed class KinectBodyView
    {
        /// <summary> Radius of drawn hand circles </summary>
        private const double HighConfidenceHandSize = 40;

        /// <summary> Radius of drawn hand circles </summary>
        private const double LowConfidenceHandSize = 20;

        /// <summary> Thickness of drawn joint lines </summary>
        private const double JointThickness = 8.0;

        /// <summary> Thickness of seen bone lines </summary>
        private const double TrackedBoneThickness = 4.0;

        /// <summary> Thickness of inferred joint lines </summary>
        private const double InferredBoneThickness = 1.0;

        /// <summary> Thickness of clip edge rectangles </summary>
        private const double ClipBoundsThickness = 5;

        /// <summary> Constant for clamping Z values of camera space points from being negative </summary>
        private const float InferredZPositionClamp = 0.1f;

        /// <summary> Array of brush colors to use for a tracked body; array position corresponds to the body being tracked </summary>
        private readonly Color[] bodyColors = new Color[] { Colors.Red, Colors.Orange, Colors.Green, Colors.Blue, Colors.Indigo, Colors.Violet };

        /// <summary> Array of bodyInfo objects for each potential body </summary>
        private BodyInfo[] bodyInfos = null;

        /// <summary> Coordinate mapper to map one type of point to another </summary>
        private CoordinateMapper coordinateMapper = null;

        /// <summary> Display grid for rendering body data in the UI </summary>
        private Grid displayGrid = null;

        /// <summary> Main Canvas that contains all visual objects for all bodies and clipped edges </summary>
        private Canvas drawingCanvas = null;

        /// <summary> Left clipped edge rectangle </summary>
        private Rectangle leftClipEdge = null;

        /// <summary> Right clipped edge rectangle </summary>
        private Rectangle rightClipEdge = null;

        /// <summary> Top clipped edge rectangle </summary>
        private Rectangle topClipEdge = null;

        /// <summary> Bottom clipped edge rectangle </summary>
        private Rectangle bottomClipEdge;

        /// <summary>
        /// Initializes a new instance of the KinectBodyView class
        /// </summary>
        /// <param name="kinectSensor">Active Kinect sensor</param>
        /// <param name="displayGrid">Grid used to display Kinect Body data in the UI</param>
        public KinectBodyView(KinectSensor kinectSensor, Grid displayGrid)
        {
            if (kinectSensor == null)
            {
                throw new ArgumentNullException("kinectSensor");
            }

            if (displayGrid == null)
            {
                throw new ArgumentNullException("displayGrid");
            }

            this.DisplayGrid = displayGrid;

            // get the coordinate mapper
            this.coordinateMapper = kinectSensor.CoordinateMapper;

            // get the depth (display) extents
            FrameDescription frameDescription = kinectSensor.DepthFrameSource.FrameDescription;

            // get size of joint space
            this.JointSpaceWidth = frameDescription.Width;
            this.JointSpaceHeight = frameDescription.Height;

            // sets total number of possible tracked bodies
            // create ellipses and lines for drawing bodies
            int bodyCount = kinectSensor.BodyFrameSource.BodyCount;
            this.bodyInfos = new BodyInfo[bodyCount];
            for (int bodyIndex = 0; bodyIndex < bodyCount; bodyIndex++)
            {
                this.bodyInfos[bodyIndex] = new BodyInfo(this.bodyColors[bodyIndex]);
            }
            
            // Instantiate a new Canvas
            this.drawingCanvas = new Canvas();

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
        /// Gets the DisplayGrid used to visualize the Kinect body objects in the UI
        /// </summary>
        public Grid DisplayGrid
        {
            get
            {
                return this.displayGrid;
            }

            private set
            {
                if (this.displayGrid != value)
                {
                    this.displayGrid = value;
                    this.NotifyPropertyChanged();
                }
            }
        }

        /// <summary> Gets or sets Joint width </summary>
        private float JointSpaceWidth { get; set; }

        /// <summary> Gets or sets Joint height </summary>
        private float JointSpaceHeight { get; set; }

        /// <summary>
        /// Updates body information with latest data from the sensor
        /// Called by the Reader_BodyFrameArrived event handler in MainPage
        /// </summary>
        /// <param name="kinectSensor">Active kinect sensor</param>
        /// <param name="bodies">Array of body objects to update</param>
        public void UpdateBodyFrame(KinectSensor kinectSensor, Body[] bodies)
        {
            if (kinectSensor == null)
            {
                throw new ArgumentNullException("kinectSensor");
            }

            if (bodies != null)
            {
                bool hasTrackedBody = false;
                this.BeginBodiesUpdate();

                // iterate through each body
                for (int bodyIndex = 0; bodyIndex < bodies.Length; bodyIndex++)
                {
                    Body body = bodies[bodyIndex];

                    if (body.IsTracked)
                    {
                        // check if this body clips an edge
                        this.UpdateClippedEdges(body, hasTrackedBody);

                        this.UpdateBody(kinectSensor, body, bodyIndex);

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
            if (this.bodyInfos != null)
            {
                foreach (var bodyInfo in this.bodyInfos)
                {
                    bodyInfo.Updated = false;
                }
            }
        }

        /// <summary>
        /// Updates bones, joints and hand state for a single body.
        /// </summary>
        /// <param name="kinectSensor">active kinect sensor</param>
        /// <param name="body">body for getting joint info</param>
        /// <param name="bodyIndex">index for body we are currently updating</param>
        internal void UpdateBody(KinectSensor kinectSensor, Body body, int bodyIndex)
        {
            if (kinectSensor == null)
            {
                throw new ArgumentNullException("kinectSensor");
            }

            if (body != null)
            {
                IReadOnlyDictionary<JointType, Joint> joints = body.Joints;
                var jointPointsInDepthSpace = new Dictionary<JointType, Point>();
                var bodyInfo = this.bodyInfos[bodyIndex];
                CoordinateMapper coordinateMapper = kinectSensor.CoordinateMapper;

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
                    this.UpdateBone(
                        bodyInfo.BoneLines[bone],
                        joints[bone.Item1],
                        joints[bone.Item2],
                        jointPointsInDepthSpace[bone.Item1],
                        jointPointsInDepthSpace[bone.Item2]);
                }
            }
        }

        /// <summary>
        /// Collapse the body from the canvas.
        /// </summary>
        /// <param name="bodyIndex">index of the item to clear in the body information array</param>
        private void ClearBody(int bodyIndex)
        {
            var bodyInfo = this.bodyInfos[bodyIndex];

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
        /// <param name="ellipse">ellipse representing hand state</param>
        /// <param name="handState">open, closed, or lasso</param>
        /// <param name="trackingConfidence">confidence of hand state</param>
        /// <param name="point">location of hand joint</param>
        private void UpdateHand(Ellipse ellipse, HandState handState, TrackingConfidence trackingConfidence, Point point)
        {
            ellipse.Fill = new SolidColorBrush(this.HandStateToColor(handState));

            // draw handstate ellipse based on tracking confidence
            ellipse.Width = ellipse.Height = (trackingConfidence == TrackingConfidence.Low) ? LowConfidenceHandSize : HighConfidenceHandSize;
            ellipse.Visibility = Windows.UI.Xaml.Visibility.Visible;

            // don't draw handstate if hand joints are not tracked
            if (!double.IsInfinity(point.X) && !double.IsInfinity(point.Y))
            {
                Canvas.SetLeft(ellipse, point.X - (ellipse.Width / 2));
                Canvas.SetTop(ellipse, point.Y - (ellipse.Width / 2));
            }
        }

        /// <summary>
        /// Update a joint.
        /// </summary>
        /// <param name="ellipse">ellipse which represents a body joint</param>
        /// <param name="joint">body joint to update</param>
        /// <param name="point">location of body joint</param>
        private void UpdateJoint(Ellipse ellipse, Joint joint, Point point)
        {
            TrackingState trackingState = joint.TrackingState;

            // only draw joint if it is tracked/inferred
            if (trackingState != TrackingState.NotTracked && !double.IsInfinity(point.X) && !double.IsInfinity(point.Y))
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

                Canvas.SetLeft(ellipse, point.X - (JointThickness / 2));
                Canvas.SetTop(ellipse, point.Y - (JointThickness / 2));
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
        /// <param name="hasTrackedBody">True, if a body is triggering a clipped edge</param>
        private void UpdateClippedEdges(Body body, bool hasTrackedBody)
        {
            // Clip dectection works differently for top and right edges compared to left and bottom edges
            // due to the current joint confidence model. This is an ST issue.
            // Joints become inferred immediately as they touch the left/bottom edges and clip detection triggers.
            // Joints squish on the right/top edges and clip detection doesn't trigger until more joints of 
            // the body go out of view (e.g all hand joints vs only handtip).
            FrameEdges clippedEdges = body.ClippedEdges;

            if (clippedEdges.HasFlag(FrameEdges.Left))
            {
                this.leftClipEdge.Visibility = Windows.UI.Xaml.Visibility.Visible;
            }
            else if (!hasTrackedBody)
            {
                // don't clear this edge if another body is triggering clipped edge
                this.leftClipEdge.Visibility = Windows.UI.Xaml.Visibility.Collapsed;
            }

            if (clippedEdges.HasFlag(FrameEdges.Right))
            {
                this.rightClipEdge.Visibility = Windows.UI.Xaml.Visibility.Visible;
            }
            else if (!hasTrackedBody)
            {
                this.rightClipEdge.Visibility = Windows.UI.Xaml.Visibility.Collapsed;
            }

            if (clippedEdges.HasFlag(FrameEdges.Top))
            {
                this.topClipEdge.Visibility = Windows.UI.Xaml.Visibility.Visible;
            }
            else if (!hasTrackedBody)
            {
                this.topClipEdge.Visibility = Windows.UI.Xaml.Visibility.Collapsed;
            }

            if (clippedEdges.HasFlag(FrameEdges.Bottom))
            {
                this.bottomClipEdge.Visibility = Windows.UI.Xaml.Visibility.Visible;
            }
            else if (!hasTrackedBody)
            {
                this.bottomClipEdge.Visibility = Windows.UI.Xaml.Visibility.Collapsed;
            }
        }

        /// <summary>
        /// Clear all clipped edges.
        /// </summary>
        private void ClearClippedEdges()
        {
            this.leftClipEdge.Visibility = Windows.UI.Xaml.Visibility.Collapsed;
            this.rightClipEdge.Visibility = Windows.UI.Xaml.Visibility.Collapsed;
            this.topClipEdge.Visibility = Windows.UI.Xaml.Visibility.Collapsed;
            this.bottomClipEdge.Visibility = Windows.UI.Xaml.Visibility.Collapsed;
        }

        /// <summary>
        /// Select color of hand state
        /// </summary>
        /// <param name="handState">Current state of the hand (open, closed, lasso)</param>
        /// <returns>Color which maps to the current hand state</returns>
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
            this.leftClipEdge = new Rectangle()
            {
                Fill = new SolidColorBrush(Colors.Red),
                Width = ClipBoundsThickness,
                Height = this.DisplayGrid.Height,
                Visibility = Windows.UI.Xaml.Visibility.Collapsed
            };

            this.rightClipEdge = new Rectangle()
            {
                Fill = new SolidColorBrush(Colors.Red),
                Width = ClipBoundsThickness,
                Height = this.DisplayGrid.Height,
                Visibility = Windows.UI.Xaml.Visibility.Collapsed
            };

            this.topClipEdge = new Rectangle()
            {
                Fill = new SolidColorBrush(Colors.Red),
                Width = this.DisplayGrid.Width,
                Height = ClipBoundsThickness,
                Visibility = Windows.UI.Xaml.Visibility.Collapsed
            };

            this.bottomClipEdge = new Rectangle()
            {
                Fill = new SolidColorBrush(Colors.Red),
                Width = this.DisplayGrid.Width,
                Height = ClipBoundsThickness,
                Visibility = Windows.UI.Xaml.Visibility.Collapsed
            };

            foreach (var bodyInfo in this.bodyInfos)
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
            this.drawingCanvas.Children.Add(this.leftClipEdge);
            this.drawingCanvas.Children.Add(this.rightClipEdge);
            this.drawingCanvas.Children.Add(this.topClipEdge);
            this.drawingCanvas.Children.Add(this.bottomClipEdge);

            // position the clipped edges
            Canvas.SetLeft(this.leftClipEdge, 0);
            Canvas.SetTop(this.leftClipEdge, 0);
            Canvas.SetLeft(this.rightClipEdge, this.DisplayGrid.Width - ClipBoundsThickness);
            Canvas.SetTop(this.rightClipEdge, 0);
            Canvas.SetLeft(this.topClipEdge, 0);
            Canvas.SetTop(this.topClipEdge, 0);
            Canvas.SetLeft(this.bottomClipEdge, 0);
            Canvas.SetTop(this.bottomClipEdge, this.DisplayGrid.Height - ClipBoundsThickness);
        }

        /// <summary>
        /// Notifies UI that a property has changed
        /// </summary>
        /// <param name="propertyName">Name of property that has changed</param> 
        private void NotifyPropertyChanged([CallerMemberName] string propertyName = "")
        {
            if (this.PropertyChanged != null)
            {
                this.PropertyChanged(this, new PropertyChangedEventArgs(propertyName));
            }
        }

        /// <summary>
        /// BodyInfo class that contains joint ellipses, hand state ellipses, lines for bones between two joints.
        /// </summary>
        private class BodyInfo
        {
            /// <summary>
            /// Initializes a new instance of the BodyInfo class
            /// </summary>
            /// <param name="bodyColor">Color to use when displaying this body in the UI</param>
            public BodyInfo(Color bodyColor)
            {
                if (bodyColor == null)
                {
                    throw new ArgumentNullException("bodyColor");
                }

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
                    this.JointPoints.Add(
                        jointType, 
                        new Ellipse()
                        {
                            Visibility = Windows.UI.Xaml.Visibility.Collapsed,
                            Fill = new SolidColorBrush(this.BodyColor),
                            Width = JointThickness,
                            Height = JointThickness
                        });
                }

                // collection of bones
                this.BoneLines = new Dictionary<Tuple<JointType, JointType>, Line>();

                // a bone defined as a line between two joints
                this.Bones = new List<Tuple<JointType, JointType>>();
                
                // Torso
                this.Bones.Add(new Tuple<JointType, JointType>(JointType.Head, JointType.Neck));
                this.Bones.Add(new Tuple<JointType, JointType>(JointType.Neck, JointType.SpineShoulder));
                this.Bones.Add(new Tuple<JointType, JointType>(JointType.SpineShoulder, JointType.SpineMid));
                this.Bones.Add(new Tuple<JointType, JointType>(JointType.SpineMid, JointType.SpineBase));
                this.Bones.Add(new Tuple<JointType, JointType>(JointType.SpineShoulder, JointType.ShoulderRight));
                this.Bones.Add(new Tuple<JointType, JointType>(JointType.SpineShoulder, JointType.ShoulderLeft));
                this.Bones.Add(new Tuple<JointType, JointType>(JointType.SpineBase, JointType.HipRight));
                this.Bones.Add(new Tuple<JointType, JointType>(JointType.SpineBase, JointType.HipLeft));

                // Right Arm
                this.Bones.Add(new Tuple<JointType, JointType>(JointType.ShoulderRight, JointType.ElbowRight));
                this.Bones.Add(new Tuple<JointType, JointType>(JointType.ElbowRight, JointType.WristRight));
                this.Bones.Add(new Tuple<JointType, JointType>(JointType.WristRight, JointType.HandRight));
                this.Bones.Add(new Tuple<JointType, JointType>(JointType.HandRight, JointType.HandTipRight));
                this.Bones.Add(new Tuple<JointType, JointType>(JointType.WristRight, JointType.ThumbRight));

                // Left Arm
                this.Bones.Add(new Tuple<JointType, JointType>(JointType.ShoulderLeft, JointType.ElbowLeft));
                this.Bones.Add(new Tuple<JointType, JointType>(JointType.ElbowLeft, JointType.WristLeft));
                this.Bones.Add(new Tuple<JointType, JointType>(JointType.WristLeft, JointType.HandLeft));
                this.Bones.Add(new Tuple<JointType, JointType>(JointType.HandLeft, JointType.HandTipLeft));
                this.Bones.Add(new Tuple<JointType, JointType>(JointType.WristLeft, JointType.ThumbLeft));

                // Right Leg
                this.Bones.Add(new Tuple<JointType, JointType>(JointType.HipRight, JointType.KneeRight));
                this.Bones.Add(new Tuple<JointType, JointType>(JointType.KneeRight, JointType.AnkleRight));
                this.Bones.Add(new Tuple<JointType, JointType>(JointType.AnkleRight, JointType.FootRight));

                // Left Leg
                this.Bones.Add(new Tuple<JointType, JointType>(JointType.HipLeft, JointType.KneeLeft));
                this.Bones.Add(new Tuple<JointType, JointType>(JointType.KneeLeft, JointType.AnkleLeft));
                this.Bones.Add(new Tuple<JointType, JointType>(JointType.AnkleLeft, JointType.FootLeft));

                // pre-populate list of bones that are non-visible initially
                foreach (var bone in this.Bones)
                {
                    this.BoneLines.Add(
                        bone, 
                        new Line()
                        {
                            Stroke = new SolidColorBrush(this.BodyColor),
                            Visibility = Visibility.Collapsed
                        });
                }
            }

            /// <summary> Gets or sets a value indicating whether the BodyInfo object has been updated </summary>
            public bool Updated { get; set; }

            /// <summary> Gets or sets the body color associated with the BodyInfo object </summary>
            public Color BodyColor { get; set; }

            /// <summary> Gets or sets the ellipse representing left hand state </summary>
            public Ellipse HandLeftEllipse { get; set; }

            /// <summary> Gets or sets the ellipse representing right hand state </summary>
            public Ellipse HandRightEllipse { get; set; }

            /// <summary> Gets the dictionary of all joints in a body </summary>
            public Dictionary<JointType, Ellipse> JointPoints { get; private set; }

            /// <summary> Gets the bone definitions for a body </summary>
            public List<Tuple<JointType, JointType>> Bones { get; private set; }

            /// <summary> Gets a collection of bones associated with the line object </summary>
            public Dictionary<Tuple<JointType, JointType>, Line> BoneLines { get; private set; }
        }
    }
}
