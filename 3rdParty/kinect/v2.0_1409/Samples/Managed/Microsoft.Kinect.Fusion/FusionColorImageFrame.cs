//-----------------------------------------------------------------------
// <copyright file="FusionColorImageFrame.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// </copyright>
//-----------------------------------------------------------------------

namespace Microsoft.Kinect.Fusion
{
    using System;
    using System.Runtime.InteropServices;

    /// <summary>
    /// A frame used specifically for 32bit RGBA-based images.
    /// It provides access to the dimensions, format and pixel data for a frame.
    /// </summary>
    public sealed class FusionColorImageFrame : FusionImageFrame
    {
        /// <summary>
        /// Initializes a new instance of the FusionColorImageFrame class.
        /// </summary>
        /// <param name="width">Image width.</param>
        /// <param name="height">Image height.</param>
        /// <param name="cameraParameters">The camera parameters.</param>
        public FusionColorImageFrame(int width, int height, CameraParameters cameraParameters)
            : base(FusionImageType.Color, width, height, cameraParameters)
        {
        }

        /// <summary>
        /// Initializes a new instance of the FusionColorImageFrame class with default camera parameters.
        /// </summary>
        /// <param name="width">Image width.</param>
        /// <param name="height">Image height.</param>
        public FusionColorImageFrame(int width, int height)
            : base(FusionImageType.Color, width, height, null)
        {
        }

        /// <summary>
        /// Gets the number of bytes per pixel.
        /// </summary>
        public override int BytesPerPixel
        {
            get
            {
                return sizeof(int);
            }
        }

        /// <summary>
        /// This method copies pixel data from a pre-allocated array to this image.
        /// </summary>
        /// <param name="sourcePixelData">
        /// The source int array of pixel data. It must be exactly PixelDataLength pixels in length.
        /// </param>
        public void CopyPixelDataFrom(int[] sourcePixelData)
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
        /// The pixel array to receive the data. The size in pixels must be equal to the frame’s PixelDataLength.
        /// </param>
        public void CopyPixelDataTo(int[] destinationPixelData)
        {
            if (null == destinationPixelData)
            {
                throw new ArgumentNullException("destinationPixelData");
            }

            if (destinationPixelData.Length != this.PixelDataLength)
            {
                throw new ArgumentException(Resources.ImageDataLengthMismatch, "destinationPixelData");
            }

            Marshal.Copy(this.RawBits, destinationPixelData, 0, PixelDataLength);
        }
    }
}
