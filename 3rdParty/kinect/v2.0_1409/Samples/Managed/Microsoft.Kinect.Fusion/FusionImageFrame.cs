//-----------------------------------------------------------------------
// <copyright file="FusionImageFrame.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// </copyright>
//-----------------------------------------------------------------------

namespace Microsoft.Kinect.Fusion
{
    using System;
    using System.Runtime.InteropServices;
    using Microsoft.Win32.SafeHandles;

    /// <summary>
    /// The Kinect Fusion image types.
    /// </summary>
    public enum FusionImageType
    {
        /// <summary>
        /// Invalid image type.
        /// </summary>
        Invalid = 0,

        /// <summary>
        /// RGB32 color data.
        /// </summary>
        Color,

        /// <summary>
        /// Depth in meters or AlignDepthFloatToReconstruction deltas, as 32bit float data.
        /// </summary>
        Float,

        /// <summary>
        /// 6 floats per pixel (3D Point x,y,z, Normal x,y,z).
        /// </summary>
        PointCloud
    }

    /// <summary>
    /// The base class for the container of per-frame image data buffers.
    /// </summary>
    public abstract class FusionImageFrame : IDisposable
    {
        /// <summary>
        /// The native image frame structure.
        /// </summary>
        private NUI_FUSION_IMAGE_FRAME? nativeFrame;

        /// <summary>
        /// The camera parameters for this frame.
        /// </summary>
        private CameraParameters cameraParams;

        /// <summary>
        /// The native texture frame buffer.
        /// </summary>
        private NUI_FUSION_FRAME_BUFFER? frameBuffer;

        /// <summary>
        /// Track whether Dispose has been called.
        /// </summary>
        private bool disposed = false;

        /// <summary>
        /// Initializes a new instance of the FusionImageFrame class.
        /// </summary>
        /// <param name="imageType">The type of image frame to create.</param>
        /// <param name="width">The width of the image to create.</param>
        /// <param name="height">The height of the image to create.</param>
        /// <param name="cameraParameters">The Camera Parameters of the image.</param>
        internal FusionImageFrame(FusionImageType imageType, int width, int height, CameraParameters cameraParameters)
        {
            NativeFrameHandle handle;

            ExceptionHelper.ThrowIfFailed(
                NativeMethods.NuiFusionCreateImageFrame(
                imageType,
                (uint)width,
                (uint)height,
                cameraParameters,
                out handle));

            Handle = handle;
        }

        /// <summary>
        /// Finalizes an instance of the FusionImageFrame class.
        /// This destructor will run only if the Dispose method does not get called.
        /// </summary>
        ~FusionImageFrame()
        {
            Dispose(false);
        }

        /// <summary>
        /// Gets this frame's type.
        /// </summary>
        public FusionImageType ImageType
        {
            get
            {
                EnsureMarshalled();
                return (FusionImageType)nativeFrame.Value.ImageType;
            }
        }

        /// <summary>
        /// Gets this frame's width in pixels.
        /// </summary>
        public int Width
        {
            get
            {
                EnsureMarshalled();
                return (int)nativeFrame.Value.Width;
            }
        }

        /// <summary>
        /// Gets this frame's height in pixels.
        /// </summary>
        public int Height
        {
            get
            {
                EnsureMarshalled();
                return (int)nativeFrame.Value.Height;
            }
        }

        /// <summary>
        /// Gets or sets the bytes per pixel of this ImageFrame.
        /// </summary>
        public virtual int BytesPerPixel { get; set; }

        /// <summary>
        /// Gets the total number of pixels in the buffer of this ImageFrame.
        /// </summary>
        public int PixelDataLength
        {
            get { return Width * Height; }
        }

        /// <summary>
        /// Gets the camera parameters used by the frame.
        /// </summary>
        public CameraParameters CameraParameters
        {
            get
            {
                if (null == cameraParams)
                {
                    EnsureMarshalled();
                    cameraParams = (CameraParameters)Marshal.PtrToStructure(
                        nativeFrame.Value.CameraParameters,
                        typeof(CameraParameters));
                }

                return cameraParams;
            }
        }

        /// <summary>
        /// Gets or Sets the native image frame handle.
        /// </summary>
        internal NativeFrameHandle Handle { get; private set; }

        /// <summary>
        /// Gets the texture data pitch inside current frame.
        /// </summary>
        internal uint Pitch
        {
            get
            {
                if (null == frameBuffer)
                {
                    EnsureMarshalled();
                    frameBuffer = (NUI_FUSION_FRAME_BUFFER)Marshal.PtrToStructure(
                        nativeFrame.Value.FrameBuffer,
                        typeof(NUI_FUSION_FRAME_BUFFER));
                }

                return frameBuffer.Value.Pitch;
            }
        }

        /// <summary>
        /// Gets the texture data inside current frame.
        /// </summary>
        internal IntPtr RawBits
        {
            get
            {
                if (null == frameBuffer)
                {
                    EnsureMarshalled();
                    frameBuffer = (NUI_FUSION_FRAME_BUFFER)Marshal.PtrToStructure(
                        nativeFrame.Value.FrameBuffer,
                        typeof(NUI_FUSION_FRAME_BUFFER));
                }

                return frameBuffer.Value.RawBits;
            }
        }

        /// <summary>
        /// Convert a FusionImageFrame to HandleRef structure.
        /// </summary>
        /// <param name="imageFrame">The FusionImageFrame to be converted.</param>
        /// <returns>
        /// Returns null if the input <para>imageFrame</para> is null or a HandleRef structure.
        /// </returns>
        public static HandleRef ToHandleRef(FusionImageFrame imageFrame)
        {
            return null != imageFrame ?
                new HandleRef(imageFrame, NativeFrameHandle.ToIntPtr(imageFrame.Handle)) : new HandleRef();
        }

        /// <summary>
        /// Disposes the FusionImageFrame.
        /// </summary>
        public void Dispose()
        {
            Dispose(true);

            // This object will be cleaned up by the Dispose method.
            GC.SuppressFinalize(this);
        }

        /// <summary>
        /// Frees all memory associated with the FusionImageFrame.
        /// </summary>
        /// <param name="disposing">Whether the function was called from Dispose.</param>
        protected virtual void Dispose(bool disposing)
        {
            if (!this.disposed)
            {
                if (null != Handle)
                {
                    Handle.Close();
                }

                disposed = true;
            }
        }

        /// <summary>
        /// Marshal the image from native if not already marshaled.
        /// </summary>
        private void EnsureMarshalled()
        {
            if (!nativeFrame.HasValue)
            {
                nativeFrame = (NUI_FUSION_IMAGE_FRAME)Marshal.PtrToStructure(
                    NativeFrameHandle.ToIntPtr(Handle),
                    typeof(NUI_FUSION_IMAGE_FRAME));
            }
        }
    }

    /// <summary>
    /// The native frame pointer wrapper.
    /// </summary>
    internal class NativeFrameHandle : CriticalHandleZeroOrMinusOneIsInvalid
    {
        /// <summary>
        /// Prevents a default instance of the NativeFrameHandle class from being created.
        /// It is only used for marshalling.
        /// </summary>
        private NativeFrameHandle()
        {
        }

        /// <summary>
        /// Convert a NativeFrameHandle to IntPtr.
        /// </summary>
        /// <param name="frameHandle">The NativeFrameHandle to be converted.</param>
        /// <returns>
        /// Returns IntPtr.Zero if the input NativeFrameHandle is null or the underlying IntPtr value.
        /// </returns>
        public static IntPtr ToIntPtr(NativeFrameHandle frameHandle)
        {
            return null != frameHandle ? frameHandle.handle : IntPtr.Zero;
        }

        /// <summary>
        /// Releases the specified frame of image data.
        /// </summary>
        /// <returns>
        /// true if the handle is released successfully; otherwise,
        /// in the event of a catastrophic failure, false. In this case,
        /// it generates a ReleaseHandleFailed Managed Debugging Assistant.
        /// </returns>
        protected override bool ReleaseHandle()
        {
            try
            {
                ExceptionHelper.ThrowIfFailed(NativeMethods.NuiFusionReleaseImageFrame(this));
            }
            catch (InvalidOperationException)
            {
                return false;
            }

            return true;
        }
    }
}
