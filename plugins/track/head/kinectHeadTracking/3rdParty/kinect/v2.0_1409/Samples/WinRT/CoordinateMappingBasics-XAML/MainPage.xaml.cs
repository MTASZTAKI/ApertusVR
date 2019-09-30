//------------------------------------------------------------------------------
// <copyright file="MainPage.xaml.cs" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//------------------------------------------------------------------------------

using System;
using System.Collections.Concurrent;
using System.ComponentModel;
using System.IO;
using System.Runtime.InteropServices;
using System.Runtime.InteropServices.WindowsRuntime;
using System.Threading.Tasks;
using Windows.ApplicationModel.Resources;
using Windows.Storage.Streams;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Media.Imaging;
using WindowsPreview.Kinect;

namespace Microsoft.Samples.Kinect.CoordinateMappingBasics
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
        /// Active Kinect sensor
        /// </summary>
        private KinectSensor kinectSensor = null;

        /// <summary>
        /// Coordinate mapper to map one type of point to another
        /// </summary>
        private CoordinateMapper coordinateMapper = null;

        /// <summary>
        /// Reader for depth/color/body index frames
        /// </summary>
        private MultiSourceFrameReader multiSourceFrameReader = null;

        /// <summary>
        /// Bitmap to display
        /// </summary>
        private WriteableBitmap bitmap = null;

        /// <summary>
        /// Intermediate storage for the color to depth mapping
        /// </summary>
        private DepthSpacePoint[] colorMappedToDepthPoints = null;

        /// <summary>
        /// Current status text to display
        /// </summary>
        private string statusText = null;

        /// <summary>
        /// Initializes a new instance of the MainPage class.
        /// </summary>
        public MainPage()
        {
            // initialize the components (controls) of the window
            this.InitializeComponent();

            this.kinectSensor = KinectSensor.GetDefault();

            this.coordinateMapper = this.kinectSensor.CoordinateMapper;

            FrameDescription depthFrameDescription = this.kinectSensor.DepthFrameSource.FrameDescription;

            int depthWidth = depthFrameDescription.Width;
            int depthHeight = depthFrameDescription.Height;

            FrameDescription colorFrameDescription = this.kinectSensor.ColorFrameSource.FrameDescription;

            int colorWidth = colorFrameDescription.Width;
            int colorHeight = colorFrameDescription.Height;

            this.colorMappedToDepthPoints = new DepthSpacePoint[colorWidth * colorHeight];

            this.bitmap = new WriteableBitmap(colorWidth, colorHeight);

            theImage.Source = this.bitmap;

            this.multiSourceFrameReader = this.kinectSensor.OpenMultiSourceFrameReader(FrameSourceTypes.Depth | FrameSourceTypes.Color | FrameSourceTypes.BodyIndex);

            this.multiSourceFrameReader.MultiSourceFrameArrived += this.Reader_MultiSourceFrameArrived;

            this.kinectSensor.IsAvailableChanged += this.Sensor_IsAvailableChanged;

            this.kinectSensor.Open();

            this.StatusText = this.kinectSensor.IsAvailable ? resourceLoader.GetString("RunningStatusText")
                                                            : resourceLoader.GetString("NoSensorStatusText");

            this.DataContext = this;
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
        /// Execute shutdown tasks.
        /// </summary>
        /// <param name="sender">object sending the event</param>
        /// <param name="e">event arguments</param>
        private void MainPage_Unloaded(object sender, RoutedEventArgs e)
        {
            if (this.multiSourceFrameReader != null)
            {
                // MultiSourceFrameReader is IDisposable
                this.multiSourceFrameReader.Dispose();
                this.multiSourceFrameReader = null;
            }

            if (this.kinectSensor != null)
            {
                this.kinectSensor.Close();
                this.kinectSensor = null;
            }
        }

        /// <summary>
        /// Handles the depth/color/body index frame data arriving from the sensor.
        /// </summary>
        /// <param name="sender">object sending the event</param>
        /// <param name="e">event arguments</param>
        private void Reader_MultiSourceFrameArrived(object sender, MultiSourceFrameArrivedEventArgs e)
        {
            int depthWidth = 0;
            int depthHeight = 0;

            DepthFrame depthFrame = null;
            IBuffer depthFrameData = null;

            ColorFrame colorFrame = null;
            
            BodyIndexFrame bodyIndexFrame = null;
            IBuffer bodyIndexFrameData = null;
            IBufferByteAccess bodyIndexByteAccess = null;
 
            MultiSourceFrame multiSourceFrame = e.FrameReference.AcquireFrame();           

            // If the Frame has expired by the time we process this event, return.
            if (multiSourceFrame == null)
            {
                return;
            }

            // We use a try/finally to ensure that we clean up before we exit the function.  
            // This includes calling Dispose on any Frame objects that we may have and unlocking the bitmap back buffer.
            try
            {
                depthFrame = multiSourceFrame.DepthFrameReference.AcquireFrame();
                colorFrame = multiSourceFrame.ColorFrameReference.AcquireFrame();
                bodyIndexFrame = multiSourceFrame.BodyIndexFrameReference.AcquireFrame();

                // If any frame has expired by the time we process this event, return.
                // The "finally" statement will Dispose any that are not null.
                if ((depthFrame == null) || (colorFrame == null) || (bodyIndexFrame == null))
                {
                    return;
                }

                // Process Depth
                FrameDescription depthFrameDescription = depthFrame.FrameDescription;

                depthWidth = depthFrameDescription.Width;
                depthHeight = depthFrameDescription.Height;

                // Access the depth frame data directly via LockImageBuffer to avoid making a copy
                depthFrameData = depthFrame.LockImageBuffer();

                this.coordinateMapper.MapColorFrameToDepthSpaceUsingIBuffer(depthFrameData, this.colorMappedToDepthPoints);

                // We're done with the DepthFrame 
                depthFrame.Dispose();
                depthFrame = null;

                // We must force a release of the IBuffer in order to ensure that we have dropped all references to it.
                System.Runtime.InteropServices.Marshal.ReleaseComObject(depthFrameData);
                depthFrameData = null;
        
                // Process Color
                colorFrame.CopyConvertedFrameDataToBuffer(this.bitmap.PixelBuffer, ColorImageFormat.Bgra);

                // We're done with the ColorFrame 
                colorFrame.Dispose();
                colorFrame = null;

                FrameDescription bodyIndexFrameDescription = bodyIndexFrame.FrameDescription;

                // Access the body index frame data directly via LockImageBuffer to avoid making a copy
                bodyIndexFrameData = bodyIndexFrame.LockImageBuffer();

                int colorMappedToDepthPointCount = this.colorMappedToDepthPoints.Length;

                unsafe
                {
                    bodyIndexByteAccess = (IBufferByteAccess)bodyIndexFrameData;
                    byte* bodyIndexBytes = null;
                    bodyIndexByteAccess.Buffer(out bodyIndexBytes);

                    fixed (DepthSpacePoint* colorMappedToDepthPointsPointer = this.colorMappedToDepthPoints)
                    {
                        IBufferByteAccess bitmapBackBufferByteAccess = (IBufferByteAccess)this.bitmap.PixelBuffer;

                        byte* bitmapBackBufferBytes = null;
                        bitmapBackBufferByteAccess.Buffer(out bitmapBackBufferBytes);

                        // Treat the color data as 4-byte pixels
                        uint* bitmapPixelsPointer = (uint*)bitmapBackBufferBytes;

                        // Loop over each row and column of the color image
                        // Zero out any pixels that don't correspond to a body index
                        for (int colorIndex = 0; colorIndex < colorMappedToDepthPointCount; ++colorIndex)
                        {
                            float colorMappedToDepthX = colorMappedToDepthPointsPointer[colorIndex].X;
                            float colorMappedToDepthY = colorMappedToDepthPointsPointer[colorIndex].Y;

                            // The sentinel value is -inf, -inf, meaning that no depth pixel corresponds to this color pixel.
                            if (!float.IsNegativeInfinity(colorMappedToDepthX) &&
                                !float.IsNegativeInfinity(colorMappedToDepthY))
                            {
                                // Make sure the depth pixel maps to a valid point in color space
                                int depthX = (int)(colorMappedToDepthX + 0.5f);
                                int depthY = (int)(colorMappedToDepthY + 0.5f);

                                // If the point is not valid, there is no body index there.
                                if ((depthX >= 0) && (depthX < depthWidth) && (depthY >= 0) && (depthY < depthHeight))
                                {
                                    int depthIndex = (depthY * depthWidth) + depthX;

                                    // If we are tracking a body for the current pixel, do not zero out the pixel
                                    if (bodyIndexBytes[depthIndex] != 0xff)
                                    {
                                        continue;
                                    }
                                }
                            }

                            bitmapPixelsPointer[colorIndex] = 0;
                        }
                    }

                    this.bitmap.Invalidate();
                }
            }
            finally
            {
                if (depthFrame != null)
                {
                    depthFrame.Dispose();
                }

                if (depthFrameData != null)
                {
                    // We must force a release of the IBuffer in order to ensure that we have dropped all references to it.
                    System.Runtime.InteropServices.Marshal.ReleaseComObject(depthFrameData);
                }

                if (colorFrame != null)
                {
                    colorFrame.Dispose();
                }

                if (bodyIndexFrame != null)
                {
                    bodyIndexFrame.Dispose();
                }

                if (bodyIndexFrameData != null)
                {
                    // We must force a release of the IBuffer in order to ensure that we have dropped all references to it.
                    System.Runtime.InteropServices.Marshal.ReleaseComObject(bodyIndexFrameData);
                }

                if (bodyIndexByteAccess != null)
                {
                    // We must force a release of the IBuffer in order to ensure that we have dropped all references to it.
                    System.Runtime.InteropServices.Marshal.ReleaseComObject(bodyIndexByteAccess);
                }
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
            this.StatusText = this.kinectSensor.IsAvailable ? resourceLoader.GetString("RunningStatusText")
                                                            : resourceLoader.GetString("SensorNotAvailableStatusText");
        }
    }

    [Guid("905a0fef-bc53-11df-8c49-001e4fc686da"), InterfaceType(ComInterfaceType.InterfaceIsIUnknown)]
    interface IBufferByteAccess
    {
        unsafe void Buffer(out byte* pByte);
    }
}
