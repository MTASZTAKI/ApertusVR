//------------------------------------------------------------------------------
// <copyright file="KinectPointerPointSample.xaml.cs" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//------------------------------------------------------------------------------

namespace Microsoft.Samples.Kinect.ControlsBasics
{
    using System;
    using System.Windows;
    using System.Windows.Controls;
    using System.Windows.Media;
    using System.Windows.Shapes;
    using Microsoft.Kinect;
    using Microsoft.Kinect.Input;

    /// <summary>
    /// A sample showing a visualization of PointerPoints from Kinect
    /// </summary>
    public sealed partial class KinectPointerPointSample : UserControl
    {
        /// <summary>
        /// Height of the dot which represents a pointer
        /// </summary>
        private const double DotHeight = 60;

        /// <summary>
        /// Width of the dot which represents a pointer
        /// </summary>
        private const double DotWidth = 60;

        /// <summary>
        /// A black brush
        /// </summary>
        private SolidColorBrush blackBrush = Brushes.Black;

        /// <summary>
        /// A green brush
        /// </summary>
        private SolidColorBrush greenBrush = Brushes.Green;

        /// <summary>
        /// A yellow brush
        /// </summary>
        private SolidColorBrush yellowBrush = Brushes.Yellow;

        /// <summary>
        /// Shows more details about the pointer data
        /// </summary>
        private bool showDetails = false;

        /// <summary>
        /// Keeps track of last time, so we know when we get a new set of pointers. Pointer events fire multiple times per timestamp, based on how
        /// many pointers are present.
        /// </summary>
        private TimeSpan lastTime;

        /// <summary>
        /// Initializes an instance of the <see cref="KinectPointerPointSample"/> class.
        /// </summary>
        public KinectPointerPointSample()
        {
            this.InitializeComponent();
            this.Loaded += KinectPointerPointSample_Loaded;
        }

        /// <summary>
        /// Initialization code 
        /// </summary>
        /// <param name="sender">the page</param>
        /// <param name="e">the loaded event args</param>
        void KinectPointerPointSample_Loaded(object sender, RoutedEventArgs e)
        {
            // Listen to Kinect pointer events
            KinectCoreWindow kinectCoreWindow = KinectCoreWindow.GetForCurrentThread();
            kinectCoreWindow.PointerMoved += kinectCoreWindow_PointerMoved;
        }

        /// <summary>
        /// Handles kinect pointer events
        /// </summary>
        /// <param name="sender">the KinectCoreWindow</param>
        /// <param name="args">Kinect pointer args</param>
        private void kinectCoreWindow_PointerMoved(object sender, KinectPointerEventArgs args)
        {
            KinectPointerPoint kinectPointerPoint = args.CurrentPoint;
            if (lastTime == TimeSpan.Zero || lastTime != kinectPointerPoint.Properties.BodyTimeCounter)
            {
                lastTime = kinectPointerPoint.Properties.BodyTimeCounter;
                mainScreen.Children.Clear();
            }

            RenderPointer(kinectPointerPoint.Properties.IsEngaged,
                kinectPointerPoint.Position,
                kinectPointerPoint.Properties.UnclampedPosition,
                kinectPointerPoint.Properties.HandReachExtent,
                kinectPointerPoint.Properties.BodyTimeCounter,
                kinectPointerPoint.Properties.BodyTrackingId,
                kinectPointerPoint.Properties.HandType);
        }

                /// <summary>
        /// Show pointer information
        /// </summary>
        /// <param name="isEngaged">is the pointer currently engaged (kinect)
        /// <param name="position">location of the pointer, clamped to screen on top, left, bottom</param>
        /// <param name="unclampedPosition">location of the pointer, unclamped</param>
        /// <param name="handReachExtent">z value for the pointer, with Kinect</param>
        /// <param name="timeCounter">timestamp of the pointer</param>
        /// <param name="trackingId">trackingId which helps correlate with TrackingId of a Body</param>
        /// <param name="handType">which handtype (left/right) of the user generated this pointer</param>
        private void RenderPointer(
            bool isEngaged,
            PointF position,
            PointF unclampedPosition,
            float handReachExtent,
            TimeSpan timeCounter,
            ulong trackingId,
            HandType handType)
        {
            StackPanel cursor = null;
            if (cursor == null)
            {
                cursor = new StackPanel();
                mainScreen.Children.Add(cursor);
            }

            cursor.Children.Clear();
            var ellipseColor = isEngaged ? greenBrush : yellowBrush;

            StackPanel sp = new StackPanel()
            {
                Margin = new Thickness(-5, -5, 0, 0),
                Orientation = Orientation.Horizontal
            };
            sp.Children.Add(new Ellipse()
            {
                HorizontalAlignment = HorizontalAlignment.Left,
                Height = DotHeight,
                Width = DotWidth,
                Margin = new Thickness(5),
                Fill = ellipseColor
            });
            cursor.Children.Add(sp);

            if (showDetails)
            {
                cursor.Children.Add(new TextBlock() { Text = "HandReachExtent: " + handReachExtent });
                cursor.Children.Add(new TextBlock() { Text = "BodyTrackingId: " + trackingId });
                cursor.Children.Add(new TextBlock() { Text = "HandType: " + handType });
                cursor.Children.Add(new TextBlock() { Text = "BodyTimeCounter: " + timeCounter });
            }
            Canvas.SetLeft(cursor, position.X * mainScreen.ActualWidth - DotWidth / 2);
            Canvas.SetTop(cursor, position.Y * mainScreen.ActualHeight - DotHeight / 2);

            Ellipse unclampedCursor = new Ellipse()
            {
                HorizontalAlignment = HorizontalAlignment.Left,
                Height = 60,
                Width = 60,
                StrokeThickness = 5,
                Stroke = blackBrush
            };

            mainScreen.Children.Add(unclampedCursor);
            Canvas.SetLeft(unclampedCursor, unclampedPosition.X * mainScreen.ActualWidth - DotWidth / 2);
            Canvas.SetTop(unclampedCursor, unclampedPosition.Y * mainScreen.ActualHeight - DotHeight / 2);
        }

        /// <summary>
        /// User checked/unchecked the show details checkbox
        /// </summary>
        /// <param name="sender">the checkbox</param>
        /// <param name="e">the event args</param>
        private void details_Checked(object sender, RoutedEventArgs e)
        {
            showDetails = details.IsChecked.Value;
        }
    }
}
