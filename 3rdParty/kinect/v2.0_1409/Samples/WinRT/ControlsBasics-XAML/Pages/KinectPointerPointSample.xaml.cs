//------------------------------------------------------------------------------
// <copyright file="KinectPointerPointSample.xaml.cs" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//------------------------------------------------------------------------------

namespace Microsoft.Samples.Kinect.ControlsBasics
{
    using System;
    using System.Collections.Generic;
    using Windows.Foundation;
    using Windows.UI;
    using Windows.UI.Core;
    using Windows.UI.Input;
    using Windows.UI.Xaml;
    using Windows.UI.Xaml.Controls;
    using Windows.UI.Xaml.Media;
    using Windows.UI.Xaml.Shapes;
    using WindowsPreview.Kinect.Input;
    
    /// <summary>
    /// A basic page that provides characteristics common to most applications.
    /// </summary>
    public sealed partial class KinectPointerPointSample : Microsoft.Samples.Kinect.ControlsBasics.Common.LayoutAwarePage
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
        private SolidColorBrush blackBrush;

        /// <summary>
        /// A green brush
        /// </summary>
        private SolidColorBrush greenBrush;

        /// <summary>
        /// A yellow brush
        /// </summary>
        private SolidColorBrush yellowBrush;

        /// <summary>
        /// Shows more details about the pointer data
        /// </summary>
        private bool showDetails = false;

        /// <summary>
        /// Shows kinect pointer data when true, otherwise mouse/pen/touch
        /// </summary>
        private bool showKinect = false;

        /// <summary>
        /// Keeps track of last time, so we know when we get a new set of pointers. Pointer events fire multiple times per timestamp, based on how
        /// many pointers are present.
        /// </summary>
        private ulong lastTime;
        
        /// <summary>
        /// Initializes an instance of the <see cref="KinectPointerPointSample"/> class.
        /// </summary>
        public KinectPointerPointSample()
        {
            this.InitializeComponent();
            this.Loaded += KinectPointerPointSample_Loaded;
            this.Unloaded += KinectPointerPointSample_Unloaded;
        }

        /// <summary>
        /// Uninitialization code
        /// </summary>
        /// <param name="sender">the page</param>
        /// <param name="e">the unloaded event args</param>
        private void KinectPointerPointSample_Unloaded(object sender, RoutedEventArgs e)
        {
            // Unsubscribe from mouse/pen/touch pointer events
            var coreWindow = CoreWindow.GetForCurrentThread();
            coreWindow.PointerMoved -= coreWindow_PointerMoved;

            // Unsubscribe from Kinect pointer events
            var kinectCoreWindow = KinectCoreWindow.GetForCurrentThread();
            kinectCoreWindow.PointerMoved -= kinectCoreWindow_PointerMoved;
        }

        /// <summary>
        /// Initialization code 
        /// </summary>
        /// <param name="sender">the page</param>
        /// <param name="e">the loaded event args</param>
        private void KinectPointerPointSample_Loaded(object sender, RoutedEventArgs e)
        {
            // Listen to mouse/pen/touch pointer events
            var coreWindow = CoreWindow.GetForCurrentThread();
            coreWindow.PointerMoved += coreWindow_PointerMoved;

            // Listen to Kinect pointer events
            KinectCoreWindow kinectCoreWindow = KinectCoreWindow.GetForCurrentThread();
            kinectCoreWindow.PointerMoved += kinectCoreWindow_PointerMoved;

            // Initialize brushes
            blackBrush = new SolidColorBrush(ColorHelper.FromArgb(128, 0, 0, 0));
            yellowBrush = new SolidColorBrush(ColorHelper.FromArgb(255, 245, 255, 000));
            greenBrush = new SolidColorBrush(ColorHelper.FromArgb(255, 0, 255, 0));

            // Default to showing Kinect pointer inof
            this.kinect.IsChecked = true; // show Kinect data initially, not mouse/touch.
        }

        /// <summary>
        /// Shows PointerPoints (from mouse, pen, touch) visualization
        /// </summary>
        /// <param name="sender">the core window</param>
        /// <param name="args">the pointer args</param>
        void coreWindow_PointerMoved(CoreWindow sender, PointerEventArgs args)
        {
            if (!showKinect)
            {
                PointerPoint pointerPoint = args.CurrentPoint;
                if (lastTime == 0 || lastTime != pointerPoint.Timestamp)
                {
                    lastTime = pointerPoint.Timestamp;
                    mainScreen.Children.Clear();
                }

                RenderPointer(pointerPoint.Properties.IsPrimary,
                    new Point(pointerPoint.Position.X / this.ActualWidth,
                        pointerPoint.Position.Y / this.ActualHeight),
                    new Point(pointerPoint.Position.X / this.ActualWidth,
                        pointerPoint.Position.Y / this.ActualHeight),
                    0.0f,
                    TimeSpan.Zero,
                    pointerPoint.PointerId,
                    HandType.NONE);
            }
        }

        /// <summary>
        /// Handles kinect pointer events
        /// </summary>
        /// <param name="sender">the KinectCoreWindow</param>
        /// <param name="args">Kinect pointer args</param>
        private void kinectCoreWindow_PointerMoved(KinectCoreWindow sender, KinectPointerEventArgs args)
        {
            if (showKinect)
            {
                KinectPointerPoint kinectPointerPoint = args.CurrentPoint;
                if (lastTime == 0 || lastTime != kinectPointerPoint.Timestamp)
                {
                    lastTime = kinectPointerPoint.Timestamp;
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
        }

        /// <summary>
        /// Show pointer information
        /// </summary>
        /// <param name="isEngaged">is the pointer currently engaged (kinect) or primary (mouse/touch/pen)</param>
        /// <param name="position">location of the pointer, clamped to screen on top, left, bottom</param>
        /// <param name="unclampedPosition">location of the pointer, unclamped</param>
        /// <param name="handReachExtent">z value for the pointer, with Kinect</param>
        /// <param name="timeCounter">timestamp of the pointer</param>
        /// <param name="trackingId">trackingId which helps correlate with TrackingId of a Body</param>
        /// <param name="handType">which handtype (left/right) of the user generated this pointer</param>
        private void RenderPointer(
            bool isEngaged,
            Windows.Foundation.Point position,
            Windows.Foundation.Point unclampedPosition,
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
                Height = DotHeight,
                Width = DotWidth,
                StrokeThickness = 5,
                Stroke = blackBrush
            };

            mainScreen.Children.Add(unclampedCursor);
            Canvas.SetLeft(unclampedCursor, unclampedPosition.X * mainScreen.ActualWidth - DotWidth / 2);
            Canvas.SetTop(unclampedCursor, unclampedPosition.Y * mainScreen.ActualHeight - DotHeight / 2);
        }

        /// <summary>
        /// Populates the page with content passed during navigation.  Any saved state is also
        /// provided when recreating a page from a prior session.
        /// </summary>
        /// <param name="navigationParameter">The parameter value passed to
        /// <see cref="Frame.Navigate(Type, Object)"/> when this page was initially requested.
        /// </param>
        /// <param name="pageState">A dictionary of state preserved by this page during an earlier
        /// session.  This will be null the first time a page is visited.</param>
        protected override void LoadState(Object navigationParameter, Dictionary<String, Object> pageState)
        {
        }

        /// <summary>
        /// Preserves state associated with this page in case the application is suspended or the
        /// page is discarded from the navigation cache.  Values must conform to the serialization
        /// requirements of <see cref="SuspensionManager.SessionState"/>.
        /// </summary>
        /// <param name="pageState">An empty dictionary to be populated with serializable state.</param>
        protected override void SaveState(Dictionary<String, Object> pageState)
        {
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

        /// <summary>
        /// User checked/unchecked the show Kinect checkbox
        /// </summary>
        /// <param name="sender">the checkbox</param>
        /// <param name="e">the event args</param>
        private void kinect_Checked(object sender, RoutedEventArgs e)
        {
            showKinect = kinect.IsChecked.Value;
        }
    }
}
