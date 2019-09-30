//-----------------------------------------------------------------------
// <copyright file="FusionDepthProcessor.cs" company="Microsoft Corporation">
//      Copyright (c) Microsoft Corporation. All rights reserved.
// </copyright>
//-----------------------------------------------------------------------

namespace Microsoft.Kinect.Fusion
{
    using System;
    using System.Runtime.InteropServices;
    using System.Text;

    /// <summary>
    /// The FusionDepthProcessor class encapsulates all operations on depth processing.
    /// </summary>
    public static class FusionDepthProcessor
    {
        /// <summary>
        /// The default minimum depth value.
        /// </summary>
        public const float DefaultMinimumDepth = 0.5f;

        /// <summary>
        /// The default maximum depth value.
        /// </summary>
        public const float DefaultMaximumDepth = 8.0f;

        /// <summary>
        /// The default align iteration count.
        /// </summary>
        public const int DefaultAlignIterationCount = 7;

        /// <summary>
        /// The default integration weight.
        /// </summary>
        public const int DefaultIntegrationWeight = 200;

        /// <summary>
        /// The default color integration: no angle restriction, integrate +/-180 degrees (fastest processing).
        /// </summary>
        public const float DefaultColorIntegrationOfAllAngles = 180.0f;

        /// <summary>
        /// Enumerate the devices capable of running KinectFusion.
        /// This enables a specific device to be chosen when calling NuiFusionCreateReconstruction if desired.
        /// </summary>
        /// <param name="type">The type of processor to enumerate.</param>
        /// <param name="index">
        /// The zero-based index of the device for which the description is returned.
        /// or -1 for the default device for the given processor type.
        /// </param>
        /// <param name="description">On success, the variable is assigned the description string for the device.</param>
        /// <param name="instancePath">On success, the variable is assigned the instance path for the device.</param>
        /// <param name="memoryKB">On success, the variable is assigned the total amount of memory on the device, in kilobytes.</param>
        /// <exception cref="IndexOutOfRangeException">
        /// Thrown when the <paramref name="index"/> parameter is out of range for the specified processor type.
        /// </exception>
        /// <exception cref="ArgumentException">
        /// Thrown when the <paramref name="type"/> parameter is out of range.
        /// </exception>
        /// <exception cref="InvalidOperationException">
        /// Thrown when the call failed for an unknown reason.
        /// </exception>
        public static void GetDeviceInfo(
            ReconstructionProcessor type,
            int index,
            out string description,
            out string instancePath,
            out int memoryKB)
        {
            const int MaxBufferLength = 260;

            StringBuilder descriptionBuffer = new StringBuilder(MaxBufferLength);
            StringBuilder instancePathBuffer = new StringBuilder(MaxBufferLength);
            uint memoryInKB = 0;

            ExceptionHelper.ThrowIfFailed(NativeMethods.NuiFusionGetDeviceInfo(
                type,
                index,
                descriptionBuffer,
                (uint)descriptionBuffer.Capacity,
                instancePathBuffer,
                (uint)instancePathBuffer.Capacity,
                out memoryInKB));

            // Save the results
            description = descriptionBuffer.ToString();
            instancePath = instancePathBuffer.ToString();
            memoryKB = (int)memoryInKB;
        }

        /// <summary>
        /// Converts Kinect depth frames in unsigned short format to depth frames in float format
        /// representing distance from the camera in meters (parallel to the optical center axis).
        /// Note: <paramref name="depthImageData"/> and <paramref name="depthFloatFrame"/> must
        /// be the same pixel resolution and equal to <paramref name="depthImageDataWidth"/> by
        /// <paramref name="depthImageDataHeight"/>.
        /// The min and max depth clip values enable clipping of the input data, for example, to help
        /// isolate particular objects or surfaces to be reconstructed. Note that the thresholds return 
        /// different values when a depth pixel is outside the threshold - pixels inside minDepthClip will
        /// will be returned as 0 and ignored in processing, whereas pixels beyond maxDepthClip will be set
        /// to 1000 to signify a valid depth ray with depth beyond the set threshold. Setting this far-
        /// distance flag is important for reconstruction integration in situations where the camera is
        /// static or does not move significantly, as it enables any voxels closer to the camera
        /// along this ray to be culled instead of persisting (as would happen if the pixels were simply 
        /// set to 0 and ignored in processing). Note that when reconstructing large real-world size volumes,
        /// be sure to set large maxDepthClip distances, as when the camera moves around, any voxels in view
        /// which go beyond this threshold distance from the camera will be removed.
        /// </summary>
        /// <param name="depthImageData">
        /// An array which stores the extended-depth texture of a depth image from the Kinect camera.
        /// </param>
        /// <param name="depthImageDataWidth">Width of the depth image data.</param>
        /// <param name="depthImageDataHeight">Height of the depth image data.</param>
        /// <param name="depthFloatFrame">
        /// A pre-allocated depth float type image frame, to be filled with the floating point depth values.
        /// </param>
        /// <param name="minDepthClip">
        /// Minimum depth distance threshold in meters. Depth pixels below this value will be
        /// returned as invalid (0). Min depth must be positive or 0.
        /// </param>
        /// <param name="maxDepthClip">
        /// Maximum depth distance threshold in meters. Depth pixels above this value will be
        /// returned as invalid (1000). Max depth must be greater than 0.
        /// </param>
        /// <param name="mirrorDepth">
        /// A boolean parameter specifying whether to horizontally mirror the input depth image.
        /// </param>
        /// <exception cref="ArgumentNullException">
        /// Thrown when the <paramref name="depthImageData"/> or the <paramref name="depthFloatFrame"/>
        /// parameter is null.
        /// </exception>
        /// <exception cref="ArgumentException">
        /// Thrown when the <paramref name="depthImageDataWidth"/> parameter and depthFloatFrame's
        /// <c>width</c> is not equal, or the <paramref name="depthImageDataHeight"/> parameter and
        /// depthFloatFrame's <c>height</c> member is not equal.
        /// Thrown when the <paramref name="minDepthClip"/> parameter or the
        /// <paramref name="maxDepthClip"/> is less than zero.
        /// </exception>
        /// <exception cref="OutOfMemoryException">
        /// Thrown if a CPU memory allocation failed.
        /// </exception>
        /// <exception cref="InvalidOperationException">
        /// Thrown when the Kinect Runtime could not be accessed, the device is not connected,
        /// a GPU memory allocation failed or the call failed for an unknown reason.
        /// </exception>
#pragma warning disable 3001
        public static void DepthToDepthFloatFrame(
            ushort[] depthImageData,
            int depthImageDataWidth,
            int depthImageDataHeight,
            FusionFloatImageFrame depthFloatFrame,
            float minDepthClip,
            float maxDepthClip,
            bool mirrorDepth)
        {
            if (null == depthImageData)
            {
                throw new ArgumentNullException("depthImageData");
            }

            if (null == depthFloatFrame)
            {
                throw new ArgumentNullException("depthFloatFrame");
            }

            ExceptionHelper.ThrowIfFailed(NativeMethods.NuiFusionDepthToDepthFloatFrame(
                depthImageData,
                (uint)depthImageDataWidth,
                (uint)depthImageDataHeight,
                FusionImageFrame.ToHandleRef(depthFloatFrame),
                minDepthClip,
                maxDepthClip,
                mirrorDepth));
        }
#pragma warning restore 3001

        /// <summary>
        /// Construct an oriented point cloud in the local camera frame of reference from a depth float
        /// image frame. Here we calculate the 3D position of each depth float pixel with the optical
        /// center of the camera as the origin. We use a right-hand coordinate system, and (in common 
        /// with bitmap images with top left origin) +X is to the right, +Y down, and +Z is now forward
        /// from the Kinect camera into the scene, as though looking into the scene from behind the 
        /// Kinect camera. Both images must be the same size and have the same camera parameters.
        /// </summary>
        /// <param name="depthFloatFrame">The depth float frame to be converted.</param>
        /// <param name="pointCloudFrame">
        /// A pre-allocated point cloud frame, to be filled with 3D points and normals.
        /// </param>
        /// <exception cref="ArgumentNullException">
        /// Thrown when the <paramref name="depthFloatFrame"/> or the <paramref name="pointCloudFrame"/>
        /// parameter is null.
        /// </exception>
        /// <exception cref="ArgumentException">
        /// Thrown when the <paramref name="depthFloatFrame"/> or <paramref name="pointCloudFrame"/>
        /// parameters are different image sizes.
        /// </exception>
        /// <exception cref="OutOfMemoryException">
        /// Thrown if a CPU memory allocation failed.
        /// </exception>
        /// <exception cref="InvalidOperationException">
        /// Thrown when the Kinect Runtime could not be accessed, the device is not connected,
        /// a GPU memory allocation failed or the call failed for an unknown reason.
        /// </exception>
        public static void DepthFloatFrameToPointCloud(
            FusionFloatImageFrame depthFloatFrame,
            FusionPointCloudImageFrame pointCloudFrame)
        {
            if (null == depthFloatFrame)
            {
                throw new ArgumentNullException("depthFloatFrame");
            }

            if (null == pointCloudFrame)
            {
                throw new ArgumentNullException("pointCloudFrame");
            }

            ExceptionHelper.ThrowIfFailed(NativeMethods.NuiFusionDepthFloatFrameToPointCloud(
                FusionImageFrame.ToHandleRef(depthFloatFrame),
                FusionImageFrame.ToHandleRef(pointCloudFrame)));
        }

        /// <summary>
        /// Create a visible color shaded image of a point cloud and its normals. All image
        /// frames must have the same width and height.
        /// </summary>
        /// <param name="pointCloudFrame">The point cloud frame to be shaded.</param>
        /// <param name="worldToCameraTransform">
        /// The world to camera transform (camera pose) where the raycast was performed from.
        /// Pass identity if the point cloud did not originate from a raycast and is in the
        /// camera local coordinate system.
        /// </param>
        /// <param name="worldToBGRTransform">
        /// A transform mapping the XYZ co-ordinates of world space to a BGR color space,
        /// used to color surfaces according to their world positions. RGB values are clamped
        /// to the range [0, 1].
        /// </param>
        /// <param name="shadedSurfaceFrame">
        /// Optionally, a pre-allocated color image frame, to be filled with the shaded
        /// surface image. Pass null to skip this image.
        /// </param>
        /// <param name="shadedSurfaceNormalsFrame">
        /// Optionally, a pre-allocated color image frame, to be filled with the color shaded
        /// normals image with color indicating orientation. Pass null to skip this image.
        /// </param>
        /// <exception cref="ArgumentNullException">
        /// Thrown when the <paramref name="pointCloudFrame"/> parameter is null.
        /// </exception>
        /// <exception cref="ArgumentException">
        /// Thrown when the <paramref name="pointCloudFrame"/> or <paramref name="shadedSurfaceFrame"/>
        /// or <paramref name="shadedSurfaceNormalsFrame"/> parameters are different image sizes.
        /// Thrown when the <paramref name="pointCloudFrame"/> or <paramref name="shadedSurfaceFrame"/>
        /// or <paramref name="shadedSurfaceNormalsFrame"/> parameters have different camera parameters.
        /// </exception>
        /// <exception cref="OutOfMemoryException">
        /// Thrown if a CPU memory allocation failed.
        /// </exception>
        /// <exception cref="InvalidOperationException">
        /// Thrown when the Kinect Runtime could not be accessed, the device is not connected,
        /// a GPU memory allocation failed or the call failed for an unknown reason.
        /// </exception>
        public static void ShadePointCloud(
            FusionPointCloudImageFrame pointCloudFrame,
            Matrix4 worldToCameraTransform,
            Matrix4 worldToBGRTransform,
            FusionColorImageFrame shadedSurfaceFrame,
            FusionColorImageFrame shadedSurfaceNormalsFrame)
        {
            if (null == pointCloudFrame)
            {
                throw new ArgumentNullException("pointCloudFrame");
            }

            ExceptionHelper.ThrowIfFailed(NativeMethods.NuiFusionShadePointCloud(
                FusionImageFrame.ToHandleRef(pointCloudFrame),
                ref worldToCameraTransform,
                ref worldToBGRTransform,
                FusionImageFrame.ToHandleRef(shadedSurfaceFrame),
                FusionImageFrame.ToHandleRef(shadedSurfaceNormalsFrame)));
        }

        /// <summary>
        /// Create a visible color shaded image of a point cloud and its normals with simple
        /// grayscale L.N surface shading. All image frames must have the same width and height.
        /// </summary>
        /// <param name="pointCloudFrame">The point cloud frame to be shaded.</param>
        /// <param name="worldToCameraTransform">
        /// The world to camera transform (camera pose) where the raycast was performed from.
        /// Pass identity if the point cloud did not originate from a raycast and is in the
        /// camera local coordinate system.
        /// </param>
        /// <param name="shadedSurfaceFrame">
        /// Optionally, a pre-allocated color image frame, to be filled with the grayscale L.N 
        /// shaded surface image. Pass null to skip this image.
        /// </param>
        /// <param name="shadedSurfaceNormalsFrame">
        /// Optionally, a pre-allocated color image frame, to be filled with the color shaded
        /// normals image with color indicating orientation. Pass null to skip this image.
        /// </param>
        /// <exception cref="ArgumentNullException">
        /// Thrown when the <paramref name="pointCloudFrame"/> parameter is null.
        /// </exception>
        /// <exception cref="ArgumentException">
        /// Thrown when the <paramref name="pointCloudFrame"/> or <paramref name="shadedSurfaceFrame"/>
        /// or <paramref name="shadedSurfaceNormalsFrame"/> parameters are different image sizes.
        /// Thrown when the <paramref name="pointCloudFrame"/> or <paramref name="shadedSurfaceFrame"/>
        /// or <paramref name="shadedSurfaceNormalsFrame"/> parameters have different camera parameters.
        /// </exception>
        /// <exception cref="OutOfMemoryException">
        /// Thrown if a CPU memory allocation failed.
        /// </exception>
        /// <exception cref="InvalidOperationException">
        /// Thrown when the Kinect Runtime could not be accessed, the device is not connected,
        /// a GPU memory allocation failed or the call failed for an unknown reason.
        /// </exception>
        public static void ShadePointCloud(
            FusionPointCloudImageFrame pointCloudFrame,
            Matrix4 worldToCameraTransform,
            FusionColorImageFrame shadedSurfaceFrame,
            FusionColorImageFrame shadedSurfaceNormalsFrame)
        {
            if (null == pointCloudFrame)
            {
                throw new ArgumentNullException("pointCloudFrame");
            }

            ExceptionHelper.ThrowIfFailed(NativeMethods.NuiFusionShadePointCloud2(
                FusionImageFrame.ToHandleRef(pointCloudFrame),
                ref worldToCameraTransform,
                IntPtr.Zero,
                FusionImageFrame.ToHandleRef(shadedSurfaceFrame),
                FusionImageFrame.ToHandleRef(shadedSurfaceNormalsFrame)));
        }

        /// <summary>
        /// The AlignPointClouds function uses an iterative algorithm to align two sets of oriented
        /// point clouds and calculate the camera's relative pose. This is a generic function which
        /// can be used independently of a Reconstruction Volume with sets of overlapping point clouds.
        /// All images must be the same size and have the same camera parameters.
        /// To find the frame-to-frame relative transformation between two sets of point clouds in
        /// the camera local frame of reference (created by DepthFloatFrameToPointCloud),
        /// set the <paramref name="observedToReferenceTransform"/> to the identity.
        /// To calculate the frame-to-model pose transformation between point clouds calculated from 
        /// new depth frames with DepthFloatFrameToPointCloud and point clouds calculated from an 
        /// existing Reconstruction volume with CalculatePointCloud (e.g. from the previous frame),
        /// pass the CalculatePointCloud image as the reference frame, and the current depth frame 
        /// point cloud from DepthFloatFrameToPointCloud as the observed frame. Set the 
        /// <paramref name="observedToReferenceTransform"/> to the previous frames calculated camera
        /// pose that was used in the CalculatePointCloud call.
        /// Note that here the current frame point cloud will be in the camera local frame of
        /// reference, whereas the raycast points and normals will be in the global/world coordinate
        /// system. By passing the <paramref name="observedToReferenceTransform"/> you make the 
        /// algorithm aware of the transformation between the two coordinate systems.
        /// The <paramref name="observedToReferenceTransform"/> pose supplied can also take into
        /// account information you may have from other sensors or sensing mechanisms to aid the
        /// tracking. To do this multiply the relative frame to frame delta transformation from
        /// the other sensing system with the previous frame's pose before passing to this function.
        /// Note that any delta transform used should be in the same coordinate system as that
        /// returned by the DepthFloatFrameToPointCloud calculation.
        /// </summary>
        /// <param name="referencePointCloudFrame">
        /// The point cloud frame of the reference camera, or the previous Kinect point cloud frame.
        /// </param>
        /// <param name="observedPointCloudFrame">
        /// The point cloud frame of the observed camera, or the current Kinect frame.
        /// </param>
        /// <param name="maxAlignIterationCount">
        /// The maximum number of iterations of the algorithm to run. The minimum value is 1.
        /// Using only a small number of iterations will have a faster runtime, however, the
        /// algorithm may not converge to the correct transformation.
        /// </param>
        /// <param name="deltaFromReferenceFrame">
        /// Optionally, a pre-allocated color image frame, to be filled with color-coded data
        /// from the camera tracking. This may be used as input to additional vision algorithms such as
        /// object segmentation. Values vary depending on whether the pixel was a valid pixel used in
        /// tracking (inlier) or failed in different tests (outlier). 0xff000000 indicates an invalid 
        /// input vertex (e.g. from 0 input depth), or one where no correspondences occur between point
        /// cloud images. Outlier vertices rejected due to too large a distance between vertices are 
        /// coded as 0xff008000. Outlier vertices rejected due to to large a difference in normal angle
        /// between point clouds are coded as 0xff800000. Inliers are color shaded depending on the 
        /// residual energy at that point, with more saturated colors indicating more discrepancy
        /// between vertices and less saturated colors (i.e. more white) representing less discrepancy,
        /// or less information at that pixel. Pass null if this image is not required.
        /// </param>
        /// <param name="observedToReferenceTransform">
        /// A pre-allocated transformation matrix. At entry to the function this should be filled
        /// with the best guess for the observed to reference transform (usually the last frame's
        /// calculated pose). At exit this is filled with he calculated pose or identity if the
        /// calculation failed.
        /// </param>
        /// <returns>
        /// Returns true if successful; returns false if the algorithm encountered a problem aligning
        /// the input point clouds and could not calculate a valid transformation, and
        /// the <paramref name="observedToReferenceTransform"/> parameter is set to identity.
        /// </returns>
        /// <exception cref="ArgumentNullException">
        /// Thrown when the <paramref name="referencePointCloudFrame"/> or the
        /// <paramref name="observedPointCloudFrame"/> parameter is null.
        /// </exception>
        /// <exception cref="ArgumentException">
        /// Thrown when the <paramref name="referencePointCloudFrame"/> or <paramref name="observedPointCloudFrame"/>
        /// or <paramref name="deltaFromReferenceFrame"/> parameters are different image sizes.
        /// Thrown when the <paramref name="referencePointCloudFrame"/> or <paramref name="observedPointCloudFrame"/>
        /// or <paramref name="deltaFromReferenceFrame"/> parameters have different camera parameters.
        /// Thrown when the <paramref name="maxAlignIterationCount"/> parameter is less than 1.
        /// </exception>
        /// <exception cref="OutOfMemoryException">
        /// Thrown if a CPU memory allocation failed.
        /// </exception>
        /// <exception cref="InvalidOperationException">
        /// Thrown when the Kinect Runtime could not be accessed, the device is not connected,
        /// a GPU memory allocation failed or the call failed for an unknown reason.
        /// </exception>
        public static bool AlignPointClouds(
            FusionPointCloudImageFrame referencePointCloudFrame,
            FusionPointCloudImageFrame observedPointCloudFrame,
            int maxAlignIterationCount,
            FusionColorImageFrame deltaFromReferenceFrame,
            ref Matrix4 observedToReferenceTransform)
        {
            if (null == referencePointCloudFrame)
            {
                throw new ArgumentNullException("referencePointCloudFrame");
            }

            if (null == observedPointCloudFrame)
            {
                throw new ArgumentNullException("observedPointCloudFrame");
            }

            ushort maxIterations = ExceptionHelper.CastAndThrowIfOutOfUshortRange(maxAlignIterationCount);

            HRESULT hr = NativeMethods.NuiFusionAlignPointClouds(
                FusionImageFrame.ToHandleRef(referencePointCloudFrame),
                FusionImageFrame.ToHandleRef(observedPointCloudFrame),
                maxIterations,
                FusionImageFrame.ToHandleRef(deltaFromReferenceFrame),
                ref observedToReferenceTransform);

            if (hr == HRESULT.E_NUI_FUSION_TRACKING_ERROR)
            {
                return false;
            }
            else
            {
                ExceptionHelper.ThrowIfFailed(hr);
            }

            return true;
        }
    }
}
