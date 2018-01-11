//-----------------------------------------------------------------------
// <copyright file="FusionFloatImageFrame.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// </copyright>
//-----------------------------------------------------------------------

namespace Microsoft.Kinect.Fusion
{
    using System;
    using System.Runtime.InteropServices;

    /// <summary>
    /// A frame used specifically for float-based images.
    /// It provides access to the dimensions, format and pixel data for a depth frame.
    /// </summary>
    public sealed class FusionFloatImageFrame : FusionImageFrame
    {
        /// <summary>
        /// Initializes a new instance of the FusionFloatImageFrame class.
        /// </summary>
        /// <param name="width">Image width.</param>
        /// <param name="height">Image height.</param>
        /// <param name="cameraParameters">The camera parameters.</param>
        public FusionFloatImageFrame(int width, int height, CameraParameters cameraParameters)
            : base(FusionImageType.Float, width, height, cameraParameters)
        {
        }

        /// <summary>
        /// Initializes a new instance of the FusionFloatImageFrame class with default camera parameters.
        /// </summary>
        /// <param name="width">Image width.</param>
        /// <param name="height">Image height.</param>
        public FusionFloatImageFrame(int width, int height)
            : this(width, height, null)
        {
        }

        /// <summary>
        /// Gets the bytes per pixel of this image frame.
        /// </summary>
        public override int BytesPerPixel
        {
            get { return sizeof(float); }
        }

        /// <summary>
        ///  This method copies pixel data from a pre-allocated array to this image.
        /// </summary>
        /// <param name="sourcePixelData">
        /// The source float array of pixel data. It must be exactly PixelDataLength pixels in length.
        /// </param>
        public void CopyPixelDataFrom(float[] sourcePixelData)
        {
            if (null == sourcePixelData)
            {
                throw new ArgumentNullException("sourcePixelData");
            }

            if (sourcePixelData.Length != this.PixelDataLength)
            {
                throw new ArgumentException(Resources.ImageDataLengthMismatch, "sourcePixelData");
            }

            Marshal.Copy(sourcePixelData, 0, this.RawBits, PixelDataLength);
        }

        /// <summary>
        /// This method copies pixel data from this frame to a pre-allocated array.
        /// </summary>
        /// <param name="destinationPixelData">
        /// The destination float array to receive the data. It must be exactly PixelDataLength pixels in length.
        /// </param>
        public void CopyPixelDataTo(float[] destinationPixelData)
        {
            if (null == destinationPixelData)
            {
                throw new ArgumentNullException("destinationPixelData");
            }

            if (destinationPixelData.Length != this.PixelDataLength)
            {
                throw new ArgumentException(Resources.ImageDataLengthMismatch, "destinationPixelData");
            }

            Marshal.Copy(this.RawBits, destinationPixelData, 0, this.PixelDataLength);
        }
    }
}
