//------------------------------------------------------------------------------
// <copyright file="MainPage.xaml.cs" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//------------------------------------------------------------------------------

using System.ComponentModel;
using System.IO;
using System.Runtime.InteropServices.WindowsRuntime;
using Windows.ApplicationModel.Resources;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Media.Imaging;
using WindowsPreview.Kinect;

namespace Microsoft.Samples.Kinect.DepthBasics
{
    /// <summary>
    /// Main page for sample
    /// </summary>
    public sealed partial class MainPage : Page, INotifyPropertyChanged
    {
        /// <summary>
        /// Map depth range to byte range
        /// </summary>
        private const int MapDepthToByte = 8000 / 256;

        /// <summary>
        /// Resource loader for string resources
        /// </summary>
#if WIN81ORLATER
        private ResourceLoader resourceLoader = ResourceLoader.GetForCurrentView("Resources");
#else
        private ResourceLoader resourceLoader = new ResourceLoader("Resources");
#endif


        /// <summary>
        /// Size of the RGB pixel in the bitmap
        /// </summary>
        private readonly int cbytesPerPixel = 4;

        /// <summary>
        /// Active Kinect sensor
        /// </summary>
        private KinectSensor kinectSensor = null;

        /// <summary>
        /// Reader for depth frames
        /// </summary>
        private DepthFrameReader depthFrameReader = null;

        /// <summary>
        /// Bitmap to display
        /// </summary>
        private WriteableBitmap bitmap = null;

        /// <summary>
        /// Intermediate storage for receiving frame data from the sensor
        /// </summary>
        private ushort[] depthFrameData = null;

        /// <summary>
        /// Intermediate storage for frame data converted to color
        /// </summary>
        private byte[] depthPixels = null;

        /// <summary>
        /// Current status text to display
        /// </summary>
        private string statusText = null;

        /// <summary>
        /// Initializes a new instance of the MainPage class.
        /// </summary>
        public MainPage()
        {
            // get the kinectSensor object
            this.kinectSensor = KinectSensor.GetDefault();

            // get the depthFrameDescription from the DepthFrameSource
            FrameDescription depthFrameDescription = this.kinectSensor.DepthFrameSource.FrameDescription;

            // open the reader for the depth frames
            this.depthFrameReader = this.kinectSensor.DepthFrameSource.OpenReader();

            // wire handler for frame arrival
            this.depthFrameReader.FrameArrived += this.Reader_DepthFrameArrived;

            // allocate space to put the pixels being received and converted
            this.depthFrameData = new ushort[depthFrameDescription.Width * depthFrameDescription.Height];
            this.depthPixels = new byte[depthFrameDescription.Width * depthFrameDescription.Height * this.cbytesPerPixel];

            // create the bitmap to display
            this.bitmap = new WriteableBitmap(depthFrameDescription.Width, depthFrameDescription.Height);//, 96.0, 96.0, PixelFormats.Bgr32, null);

            // set IsAvailableChanged event notifier
            this.kinectSensor.IsAvailableChanged += this.Sensor_IsAvailableChanged;

            // open the sensor
            this.kinectSensor.Open();

            // set the status text
            this.StatusText = this.kinectSensor.IsAvailable ? resourceLoader.GetString("RunningStatusText")
                                                            : resourceLoader.GetString("NoSensorStatusText");

            // use the window object as the view model in this simple example
            this.DataContext = this;

            // initialize the components (controls) of the window
            this.InitializeComponent();
        }

        /// <summary>
        /// INotifyPropertyChangedPropertyChanged event to allow window controls to bind to changeable data.
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
            if (this.depthFrameReader != null)
            {
                // DepthFrameReder is IDisposable
                this.depthFrameReader.Dispose();
                this.depthFrameReader = null;
            }

            if (this.kinectSensor != null)
            {
                this.kinectSensor.Close();
                this.kinectSensor = null;
            }
        }

        /// <summary>
        /// Handles the depth frame data arriving from the sensor.
        /// </summary>
        /// <param name="sender">object sending the event</param>
        /// <param name="e">event arguments</param>
        private void Reader_DepthFrameArrived(object sender, DepthFrameArrivedEventArgs e)
        {
            ushort minDepth = 0;
            ushort maxDepth = 0;

            bool depthFrameProcessed = false;
            
            // DepthFrame is IDisposable
            using (DepthFrame depthFrame = e.FrameReference.AcquireFrame())
            {
                if (depthFrame != null)
                {
                    FrameDescription depthFrameDescription = depthFrame.FrameDescription;

                    // verify data and write the new depth frame data to the display bitmap
                    if (((depthFrameDescription.Width * depthFrameDescription.Height) == this.depthFrameData.Length) &&
                        (depthFrameDescription.Width == this.bitmap.PixelWidth) && (depthFrameDescription.Height == this.bitmap.PixelHeight))
                    {
                        // Copy the pixel data from the image to a temporary array
                        depthFrame.CopyFrameDataToArray(this.depthFrameData);

                        minDepth = depthFrame.DepthMinReliableDistance;

                        // Note: In order to see the full range of depth (including the less reliable far field depth)
                        // we are setting maxDepth to the extreme potential depth threshold
                        maxDepth = ushort.MaxValue;

                        // If you wish to filter by reliable depth distance, uncomment the following line:
                        //// maxDepth = depthFrame.DepthMaxReliableDistance
                        
                        depthFrameProcessed = true;
                    }
                }
            }

            // we got a frame, convert and render
            if (depthFrameProcessed)
            {
                ConvertDepthData(minDepth, maxDepth);

                RenderDepthPixels(this.depthPixels);
            }
        }

        /// <summary>
        /// Converts depth to RGB.
        /// </summary>
        /// <param name="frame"></param>
        private void ConvertDepthData(ushort minDepth, ushort maxDepth)
        {
            int colorPixelIndex = 0;
            for (int i = 0; i < this.depthFrameData.Length; ++i)
            {
                // Get the depth for this pixel
                ushort depth = this.depthFrameData[i];

                // To convert to a byte, we're mapping the depth value to the byte range.
                // Values outside the reliable depth range are mapped to 0 (black).
                byte intensity = (byte)(depth >= minDepth && depth <= maxDepth ? (depth / MapDepthToByte) : 0);

                // Write out blue byte
                this.depthPixels[colorPixelIndex++] = intensity;

                // Write out green byte
                this.depthPixels[colorPixelIndex++] = intensity;

                // Write out red byte                        
                this.depthPixels[colorPixelIndex++] = intensity;

                // Write out alpha byte                        
                this.depthPixels[colorPixelIndex++] = 255;
            }
        }

        /// <summary>
        /// Renders color pixels into the writeableBitmap.
        /// </summary>
        /// <param name="pixels">pixel data</param>
        private void RenderDepthPixels(byte[] pixels)
        {
            pixels.CopyTo(this.bitmap.PixelBuffer);
            this.bitmap.Invalidate();
            theImage.Source = this.bitmap;
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
}
