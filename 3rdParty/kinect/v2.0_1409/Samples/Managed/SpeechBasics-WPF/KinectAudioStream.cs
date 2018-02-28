//------------------------------------------------------------------------------
// <copyright file="KinectAudioStream.cs" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//------------------------------------------------------------------------------

namespace Microsoft.Samples.Kinect.SpeechBasics
{
    using System;
    using System.IO;
 
    /// <summary>
    /// Wrapper Stream Class to Support 32->16bit conversion and support Speech call to Seek
    /// </summary>
    internal class KinectAudioStream : Stream
    {
        /// <summary>
        /// Holds the kinect audio stream, in 32-bit IEEE float format
        /// </summary>
        private Stream kinect32BitStream;

        /// <summary>
        /// Initializes a new instance of the <see cref="KinectAudioStream" /> class.
        /// </summary>
        /// <param name="input">Kinect audio stream</param>
        public KinectAudioStream(Stream input)
        {
            this.kinect32BitStream = input;
        }

        /// <summary>
        /// Gets or sets a value indicating whether speech recognition is active
        /// </summary>
        public bool SpeechActive { get; set; }

        /// <summary>
        /// CanRead property
        /// </summary>
        public override bool CanRead
        {
            get { return true; }
        }

        /// <summary>
        /// CanWrite property
        /// </summary>
        public override bool CanWrite
        {
            get { return false; }
        }

        /// <summary>
        /// CanSeek property
        /// </summary>
        public override bool CanSeek
        {
            // Speech does not call - but set value correctly
            get { return false; }
        }

        /// <summary>
        /// Position Property
        /// </summary>
        public override long Position
        {
            // Speech gets the position
            get { return 0; }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets the length of the stream. Not implemented.
        /// </summary>
        public override long Length
        {
            get { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Flush the stream. Not implemented.
        /// </summary>
        public override void Flush()
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Stream Seek. Not implemented and always returns 0.
        /// </summary>
        /// <param name="offset">A byte offset relative to the origin parameter</param>
        /// <param name="origin">A value of type SeekOrigin indicating the reference point used to obtain the new position</param>
        /// <returns>Always returns 0</returns>
        public override long Seek(long offset, SeekOrigin origin)
        {
            // Even though CanSeek == false, Speech still calls seek. Return 0 to make Speech happy instead of NotImplementedException()
            return 0;
        }

        /// <summary>
        /// Set the length of the stream. Not implemented.
        /// </summary>
        /// <param name="value">Length of the stream</param>
        public override void SetLength(long value)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Write into the stream. Not implemented.
        /// </summary>
        /// <param name="buffer">Buffer to write</param>
        /// <param name="offset">Offset into the buffer</param>
        /// <param name="count">Number of bytes to write</param>
        public override void Write(byte[] buffer, int offset, int count)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Read from the stream and convert from 32 bit IEEE float to 16 bit signed integer
        /// </summary>
        /// <param name="buffer">Input buffer</param>
        /// <param name="offset">Offset into buffer</param>
        /// <param name="count">Number of bytes to read</param>
        /// <returns>bytes read</returns>
        public override int Read(byte[] buffer, int offset, int count)
        {
            // Kinect gives 32-bit float samples. Speech asks for 16-bit integer samples.
            const int SampleSizeRatio = sizeof(float) / sizeof(short); // = 2. 

            // Speech reads at high frequency - allow some wait period between reads (in msec)
            const int SleepDuration = 50;

            // Allocate buffer for receiving 32-bit float from Kinect
            int readcount = count * SampleSizeRatio;
            byte[] kinectBuffer = new byte[readcount];

            int bytesremaining = readcount;

            // Speech expects all requested bytes to be returned
            while (bytesremaining > 0)
            {
                // If we are no longer processing speech commands, exit
                if (!this.SpeechActive)
                {
                    return 0;
                }

                int result = this.kinect32BitStream.Read(kinectBuffer, readcount - bytesremaining, bytesremaining);
                bytesremaining -= result;

                // Speech will read faster than realtime - wait for more data to arrive
                if (bytesremaining > 0)
                {
                    System.Threading.Thread.Sleep(SleepDuration);
                }
            }

            // Convert each float audio sample to short
            for (int i = 0; i < count / sizeof(short); i++)
            {
                // Extract a single 32-bit IEEE value from the byte array
                float sample = BitConverter.ToSingle(kinectBuffer, i * sizeof(float));

                // Make sure it is in the range [-1, +1]
                if (sample > 1.0f)
                {
                    sample = 1.0f;
                }
                else if (sample < -1.0f)
                {
                    sample = -1.0f;
                }

                // Scale float to the range (short.MinValue, short.MaxValue] and then 
                // convert to 16-bit signed with proper rounding
                short convertedSample = Convert.ToInt16(sample * short.MaxValue);

                // Place the resulting 16-bit sample in the output byte array
                byte[] local = BitConverter.GetBytes(convertedSample);
                System.Buffer.BlockCopy(local, 0, buffer, offset + (i * sizeof(short)), sizeof(short));
            }

            return count;
        }
    }
}
