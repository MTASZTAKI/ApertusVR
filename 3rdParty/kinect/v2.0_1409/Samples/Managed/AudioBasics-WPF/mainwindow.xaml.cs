//------------------------------------------------------------------------------
// <copyright file="MainWindow.xaml.cs" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//------------------------------------------------------------------------------

namespace AudioBasics_WPF
{
    using System;
    using System.Collections.Generic;
    using System.ComponentModel;
    using System.Globalization;
    using System.Windows;
    using System.Windows.Media;
    using System.Windows.Media.Imaging;
    using Microsoft.Kinect;

    /// <summary>
    /// Interaction logic for MainWindow.
    /// </summary>
    public partial class MainWindow : Window
    {
        /// <summary>
        /// Number of samples captured from Kinect audio stream each millisecond.
        /// </summary>
        private const int SamplesPerMillisecond = 16;

        /// <summary>
        /// Number of bytes in each Kinect audio stream sample (32-bit IEEE float).
        /// </summary>
        private const int BytesPerSample = sizeof(float);

        /// <summary>
        /// Number of audio samples represented by each column of pixels in wave bitmap.
        /// </summary>
        private const int SamplesPerColumn = 40;

        /// <summary>
        /// Minimum energy of audio to display (a negative number in dB value, where 0 dB is full scale)
        /// </summary>
        private const int MinEnergy = -90;

        /// <summary>
        /// Width of bitmap that stores audio stream energy data ready for visualization.
        /// </summary>
        private const int EnergyBitmapWidth = 780;

        /// <summary>
        /// Height of bitmap that stores audio stream energy data ready for visualization.
        /// </summary>
        private const int EnergyBitmapHeight = 195;

        /// <summary>
        /// Bitmap that contains constructed visualization for audio stream energy, ready to
        /// be displayed. It is a 2-color bitmap with white as background color and blue as
        /// foreground color.
        /// </summary>
        private readonly WriteableBitmap energyBitmap;

        /// <summary>
        /// Rectangle representing the entire energy bitmap area. Used when drawing background
        /// for energy visualization.
        /// </summary>
        private readonly Int32Rect fullEnergyRect = new Int32Rect(0, 0, EnergyBitmapWidth, EnergyBitmapHeight);

        /// <summary>
        /// Array of background-color pixels corresponding to an area equal to the size of whole energy bitmap.
        /// </summary>
        private readonly byte[] backgroundPixels = new byte[EnergyBitmapWidth * EnergyBitmapHeight];

        /// <summary>
        /// Will be allocated a buffer to hold a single sub frame of audio data read from audio stream.
        /// </summary>
        private readonly byte[] audioBuffer = null;

        /// <summary>
        /// Buffer used to store audio stream energy data as we read audio.
        /// We store 25% more energy values than we strictly need for visualization to allow for a smoother
        /// stream animation effect, since rendering happens on a different schedule with respect to audio
        /// capture.
        /// </summary>
        private readonly float[] energy = new float[(uint)(EnergyBitmapWidth * 1.25)];

        /// <summary>
        /// Object for locking energy buffer to synchronize threads.
        /// </summary>
        private readonly object energyLock = new object();

        /// <summary>
        /// Active Kinect sensor
        /// </summary>
        private KinectSensor kinectSensor = null;

        /// <summary>
        /// Reader for audio frames
        /// </summary>
        private AudioBeamFrameReader reader = null;

        /// <summary>
        /// Last observed audio beam angle in radians, in the range [-pi/2, +pi/2]
        /// </summary>
        private float beamAngle = 0;

        /// <summary>
        /// Last observed audio beam angle confidence, in the range [0, 1]
        /// </summary>
        private float beamAngleConfidence = 0;

        /// <summary>
        /// Array of foreground-color pixels corresponding to a line as long as the energy bitmap is tall.
        /// This gets re-used while constructing the energy visualization.
        /// </summary>
        private byte[] foregroundPixels;

        /// <summary>
        /// Sum of squares of audio samples being accumulated to compute the next energy value.
        /// </summary>
        private float accumulatedSquareSum;

        /// <summary>
        /// Number of audio samples accumulated so far to compute the next energy value.
        /// </summary>
        private int accumulatedSampleCount;

        /// <summary>
        /// Index of next element available in audio energy buffer.
        /// </summary>
        private int energyIndex;

        /// <summary>
        /// Number of newly calculated audio stream energy values that have not yet been
        /// displayed.
        /// </summary>
        private int newEnergyAvailable;

        /// <summary>
        /// Error between time slice we wanted to display and time slice that we ended up
        /// displaying, given that we have to display in integer pixels.
        /// </summary>
        private float energyError;

        /// <summary>
        /// Last time energy visualization was rendered to screen.
        /// </summary>
        private DateTime? lastEnergyRefreshTime;

        /// <summary>
        /// Index of first energy element that has never (yet) been displayed to screen.
        /// </summary>
        private int energyRefreshIndex;

        /// <summary>
        /// Initializes a new instance of the MainWindow class.
        /// </summary>
        public MainWindow()
        {
            // Initialize the components (controls) of the window
            this.InitializeComponent();

            // Only one Kinect Sensor is supported
            this.kinectSensor = KinectSensor.GetDefault();

            // set IsAvailableChanged event notifier
            this.kinectSensor.IsAvailableChanged += this.Sensor_IsAvailableChanged;

            if (this.kinectSensor != null)
            {
                // Open the sensor
                this.kinectSensor.Open();

                // Get its audio source
                AudioSource audioSource = this.kinectSensor.AudioSource;

                // Allocate 1024 bytes to hold a single audio sub frame. Duration sub frame 
                // is 16 msec, the sample rate is 16khz, which means 256 samples per sub frame. 
                // With 4 bytes per sample, that gives us 1024 bytes.
                this.audioBuffer = new byte[audioSource.SubFrameLengthInBytes];

                // Open the reader for the audio frames
                this.reader = audioSource.OpenReader();

                // Uncomment these two lines to overwrite the automatic mode of the audio beam.
                // It will change the beam mode to manual and set the desired beam angle.
                // In this example, point it straight forward.
                // Note that setting beam mode and beam angle will only work if the
                // application window is in the foreground.
                // Furthermore, setting these values is an asynchronous operation --
                // it may take a short period of time for the beam to adjust.
                /*
                audioSource.AudioBeams[0].AudioBeamMode = AudioBeamMode.Manual;
                audioSource.AudioBeams[0].BeamAngle = 0;
                */
            }
            else
            {
                // On failure, set the status text
                this.statusBarText.Text = Properties.Resources.NoSensorStatusText;
                return;
            }

            this.energyBitmap = new WriteableBitmap(EnergyBitmapWidth, EnergyBitmapHeight, 96, 96, PixelFormats.Indexed1, new BitmapPalette(new List<Color> { Colors.White, (Color)this.Resources["KinectPurpleColor"] }));
        }

        /// <summary>
        /// Execute start up tasks
        /// </summary>
        /// <param name="sender">object sending the event</param>
        /// <param name="e">event arguments</param>
        private void MainWindow_Loaded(object sender, RoutedEventArgs e)
        {
            // Initialize foreground pixels
            this.foregroundPixels = new byte[EnergyBitmapHeight];
            for (int i = 0; i < this.foregroundPixels.Length; ++i)
            {
                this.foregroundPixels[i] = 0xff;
            }

            this.waveDisplay.Source = this.energyBitmap;

            CompositionTarget.Rendering += this.UpdateEnergy;

            if (this.reader != null)
            {
                // Subscribe to new audio frame arrived events
                this.reader.FrameArrived += this.Reader_FrameArrived;
            }
        }

        /// <summary>
        /// Execute shutdown tasks
        /// </summary>
        /// <param name="sender">object sending the event</param>
        /// <param name="e">event arguments</param>
        private void MainWindow_Closing(object sender, CancelEventArgs e)
        {
            CompositionTarget.Rendering -= this.UpdateEnergy;

            if (this.reader != null)
            {
                // AudioBeamFrameReader is IDisposable
                this.reader.Dispose();
                this.reader = null;
            }

            if (this.kinectSensor != null)
            {
                this.kinectSensor.Close();
                this.kinectSensor = null;
            }
        }

        /// <summary>
        /// Handles the audio frame data arriving from the sensor
        /// </summary>
        /// <param name="sender">object sending the event</param>
        /// <param name="e">event arguments</param>
        private void Reader_FrameArrived(object sender, AudioBeamFrameArrivedEventArgs e)
        {
            AudioBeamFrameReference frameReference = e.FrameReference;
            AudioBeamFrameList frameList = frameReference.AcquireBeamFrames();

            if (frameList != null)
            {
                // AudioBeamFrameList is IDisposable
                using (frameList)
                {
                    // Only one audio beam is supported. Get the sub frame list for this beam
                    IReadOnlyList<AudioBeamSubFrame> subFrameList = frameList[0].SubFrames;

                    // Loop over all sub frames, extract audio buffer and beam information
                    foreach (AudioBeamSubFrame subFrame in subFrameList)
                    {
                        // Check if beam angle and/or confidence have changed
                        bool updateBeam = false;

                        if (subFrame.BeamAngle != this.beamAngle)
                        {
                            this.beamAngle = subFrame.BeamAngle;
                            updateBeam = true;
                        }

                        if (subFrame.BeamAngleConfidence != this.beamAngleConfidence)
                        {
                            this.beamAngleConfidence = subFrame.BeamAngleConfidence;
                            updateBeam = true;
                        }

                        if (updateBeam)
                        {
                            // Refresh display of audio beam
                            this.AudioBeamChanged();
                        }

                        // Process audio buffer
                        subFrame.CopyFrameDataToArray(this.audioBuffer);

                        for (int i = 0; i < this.audioBuffer.Length; i += BytesPerSample)
                        {
                            // Extract the 32-bit IEEE float sample from the byte array
                            float audioSample = BitConverter.ToSingle(this.audioBuffer, i);

                            this.accumulatedSquareSum += audioSample * audioSample;
                            ++this.accumulatedSampleCount;

                            if (this.accumulatedSampleCount < SamplesPerColumn)
                            {
                                continue;
                            }

                            float meanSquare = this.accumulatedSquareSum / SamplesPerColumn;

                            if (meanSquare > 1.0f)
                            {   
                                // A loud audio source right next to the sensor may result in mean square values
                                // greater than 1.0. Cap it at 1.0f for display purposes.
                                meanSquare = 1.0f;
                            } 

                            // Calculate energy in dB, in the range [MinEnergy, 0], where MinEnergy < 0
                            float energy = MinEnergy;

                            if (meanSquare > 0)
                            {
                                energy = (float)(10.0 * Math.Log10(meanSquare));
                            }

                            lock (this.energyLock)
                            {
                                // Normalize values to the range [0, 1] for display
                                this.energy[this.energyIndex] = (MinEnergy - energy) / MinEnergy;
                                this.energyIndex = (this.energyIndex + 1) % this.energy.Length;
                                ++this.newEnergyAvailable;
                            }

                            this.accumulatedSquareSum = 0;
                            this.accumulatedSampleCount = 0;
                        }
                    }
                }
            }
        }
        
        /// <summary>
        /// Method called when audio beam angle and/or confidence have changed.
        /// </summary>
        private void AudioBeamChanged()
        {
            // Maximum possible confidence corresponds to this gradient width
            const float MinGradientWidth = 0.04f;

            // Set width of mark based on confidence.
            // A confidence of 0 would give us a gradient that fills whole area diffusely.
            // A confidence of 1 would give us the narrowest allowed gradient width.
            float halfWidth = Math.Max(1 - this.beamAngleConfidence, MinGradientWidth) / 2;

            // Update the gradient representing sound source position to reflect confidence
            this.beamBarGsPre.Offset = Math.Max(this.beamBarGsMain.Offset - halfWidth, 0);
            this.beamBarGsPost.Offset = Math.Min(this.beamBarGsMain.Offset + halfWidth, 1);

            // Convert from radians to degrees for display purposes
            float beamAngleInDeg = this.beamAngle * 180.0f / (float)Math.PI;

            // Rotate gradient to match angle
            beamBarRotation.Angle = -beamAngleInDeg;
            beamNeedleRotation.Angle = -beamAngleInDeg;
            
            // Display new numerical values
            beamAngleText.Text = string.Format(CultureInfo.CurrentCulture, Properties.Resources.BeamAngle, beamAngleInDeg.ToString("0", CultureInfo.CurrentCulture));
            beamConfidenceText.Text = string.Format(CultureInfo.CurrentCulture, Properties.Resources.BeamAngleConfidence, this.beamAngleConfidence.ToString("0.00", CultureInfo.CurrentCulture));
        }

        /// <summary>
        /// Handles rendering energy visualization into a bitmap.
        /// </summary>
        /// <param name="sender">object sending the event.</param>
        /// <param name="e">event arguments.</param>
        private void UpdateEnergy(object sender, EventArgs e)
        {
            lock (this.energyLock)
            {
                // Calculate how many energy samples we need to advance since the last update in order to
                // have a smooth animation effect
                DateTime now = DateTime.UtcNow;
                DateTime? previousRefreshTime = this.lastEnergyRefreshTime;
                this.lastEnergyRefreshTime = now;

                // No need to refresh if there is no new energy available to render
                if (this.newEnergyAvailable <= 0)
                {
                    return;
                }

                if (previousRefreshTime != null)
                {
                    float energyToAdvance = this.energyError + (((float)(now - previousRefreshTime.Value).TotalMilliseconds * SamplesPerMillisecond) / SamplesPerColumn);
                    int energySamplesToAdvance = Math.Min(this.newEnergyAvailable, (int)Math.Round(energyToAdvance));
                    this.energyError = energyToAdvance - energySamplesToAdvance;
                    this.energyRefreshIndex = (this.energyRefreshIndex + energySamplesToAdvance) % this.energy.Length;
                    this.newEnergyAvailable -= energySamplesToAdvance;
                }

                // clear background of energy visualization area
                this.energyBitmap.WritePixels(this.fullEnergyRect, this.backgroundPixels, EnergyBitmapWidth, 0);

                // Draw each energy sample as a centered vertical bar, where the length of each bar is
                // proportional to the amount of energy it represents.
                // Time advances from left to right, with current time represented by the rightmost bar.
                int baseIndex = (this.energyRefreshIndex + this.energy.Length - EnergyBitmapWidth) % this.energy.Length;
                for (int i = 0; i < EnergyBitmapWidth; ++i)
                {
                    const int HalfImageHeight = EnergyBitmapHeight / 2;

                    // Each bar has a minimum height of 1 (to get a steady signal down the middle) and a maximum height
                    // equal to the bitmap height.
                    int barHeight = (int)Math.Max(1.0, this.energy[(baseIndex + i) % this.energy.Length] * EnergyBitmapHeight);

                    // Center bar vertically on image
                    var barRect = new Int32Rect(i, HalfImageHeight - (barHeight / 2), 1, barHeight);

                    // Draw bar in foreground color
                    this.energyBitmap.WritePixels(barRect, this.foregroundPixels, 1, 0);
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
            this.statusBarText.Text = this.kinectSensor.IsAvailable ? Properties.Resources.RunningStatusText
                                                            : Properties.Resources.SensorNotAvailableStatusText;
        }
    }
}