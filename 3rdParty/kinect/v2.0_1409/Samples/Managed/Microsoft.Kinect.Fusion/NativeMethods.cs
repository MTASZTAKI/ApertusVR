//-----------------------------------------------------------------------
// <copyright file="NativeMethods.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// </copyright>
//-----------------------------------------------------------------------

namespace Microsoft.Kinect.Fusion
{
    using System;
    using System.Globalization;
    using System.Runtime.InteropServices;
    using System.Text;

    [ComImport, InterfaceType(ComInterfaceType.InterfaceIsIUnknown), Guid("43102C25-82EE-4958-AA24-AC5456BF83C0")]
    internal interface INuiFusionMesh
    {
        /// <summary>
        /// Gets the number of vertices in the mesh (i.e. the number of elements in the array
        /// returned when calling GetVertices).
        /// </summary>
        /// <returns>
        /// Returns the number of vertices in the mesh.
        /// </returns>
        [PreserveSig]
        uint VertexCount();

        /// <summary>
        /// Gets the vertices. Each vertex has a corresponding normal with the same index.
        /// </summary>
        /// <returns>
        /// Returns S_OK if successful; otherwise, returns the error code.
        /// </returns>
        [PreserveSig]
        HRESULT GetVertices([Out] out IntPtr vertices);

        /// <summary>
        /// Gets the number of normals in the mesh (i.e. the number of elements in the array
        /// returned when calling GetNormals).
        /// </summary>
        /// <returns>
        /// Returns the number of normals in the mesh.
        /// </returns>
        [PreserveSig]
        uint NormalCount();

        /// <summary>
        /// Gets the normals. Each normal has a corresponding vertex with the same index.
        /// </summary>
        /// <returns>
        /// Returns S_OK if successful; otherwise, returns the error code.
        /// </returns>
        [PreserveSig]
        HRESULT GetNormals([Out] out IntPtr normals);

        /// <summary>
        /// Gets the number of triangle indices in the mesh (i.e. the number of elements in the array
        /// returned when calling GetTriangleIndices). Each triangle is formed by three consecutive
        /// indices, used to index the vertex and normal buffers.
        /// </summary>
        /// <returns>
        /// Returns the length of the buffer.
        /// </returns>
        [PreserveSig]
        uint TriangleVertexIndexCount();

        /// <summary>
        /// Gets the triangle indices.
        /// </summary>
        /// <returns>
        /// Returns S_OK if successful; otherwise, returns the error code.
        /// </returns>
        [PreserveSig]
        HRESULT GetTriangleIndices([Out] out IntPtr triangleVertexIndices);
    }

    [ComImport, InterfaceType(ComInterfaceType.InterfaceIsIUnknown), Guid("461C71E1-2017-4322-AEA3-303BB8DC545C")]
    internal interface INuiFusionColorMesh
    {
        /// <summary>
        /// Gets the number of vertices in the mesh (i.e. the number of elements in the array
        /// returned when calling GetVertices).
        /// </summary>
        /// <returns>
        /// Returns the number of vertices in the mesh.
        /// </returns>
        [PreserveSig]
        uint VertexCount();

        /// <summary>
        /// Gets the vertices. Each vertex has a corresponding normal with the same index.
        /// </summary>
        /// <returns>
        /// Returns S_OK if successful; otherwise, returns the error code.
        /// </returns>
        [PreserveSig]
        HRESULT GetVertices([Out] out IntPtr vertices);

        /// <summary>
        /// Gets the number of normals in the mesh (i.e. the number of elements in the array
        /// returned when calling GetNormals).
        /// </summary>
        /// <returns>
        /// Returns the number of normals in the mesh.
        /// </returns>
        [PreserveSig]
        uint NormalCount();

        /// <summary>
        /// Gets the normals. Each normal has a corresponding vertex with the same index.
        /// </summary>
        /// <returns>
        /// Returns S_OK if successful; otherwise, returns the error code.
        /// </returns>
        [PreserveSig]
        HRESULT GetNormals([Out] out IntPtr normals);

        /// <summary>
        /// Gets the number of triangle indices in the mesh (i.e. the number of elements in the array
        /// returned when calling GetTriangleIndices). Each triangle is formed by three consecutive
        /// indices, used to index the vertex and normal buffers.
        /// </summary>
        /// <returns>
        /// Returns the length of the buffer.
        /// </returns>
        [PreserveSig]
        uint TriangleVertexIndexCount();

        /// <summary>
        /// Gets the triangle indices.
        /// </summary>
        /// <returns>
        /// Returns S_OK if successful; otherwise, returns the error code.
        /// </returns>
        [PreserveSig]
        HRESULT GetTriangleIndices([Out] out IntPtr triangleVertexIndices);

        /// <summary>
        /// Gets the number of colors in the mesh (i.e. the number of elements in the array
        /// returned when calling GetColors). When capturing color, each vertex will an associated 
        /// color.
        /// </summary>
        /// <returns>
        /// Returns the length of the buffer.
        /// </returns>
        [PreserveSig]
        uint ColorCount();

        /// <summary>
        /// Gets the colors. Each vertex has a corresponding color with the same index.
        /// </summary>
        /// <returns>
        /// Returns S_OK if successful; otherwise, returns the error code.
        /// </returns>
        [PreserveSig]
        HRESULT GetColors([Out] out IntPtr colors);
    }

    [ComImport, InterfaceType(ComInterfaceType.InterfaceIsIUnknown), Guid("AC19AD5F-2218-4C08-A00F-C981C50A09DF")]
    internal interface INuiFusionReconstruction
    {
        /// <summary>
        /// Clear the volume, optionally setting a new initial camera pose and worldToVolumeTransform.
        /// </summary>
        /// <returns>
        /// Returns S_OK if successful; otherwise, returns the error code.
        /// </returns>
        [PreserveSig]
        HRESULT ResetReconstruction(
            [In] ref Matrix4 initialWorldToCameraTransform,
            [In] ref Matrix4 worldToVolumeTransform);

        /// <summary>
        /// Aligns a depth float image to the Reconstruction volume to calculate the new camera pose.
        /// This camera tracking method requires a Reconstruction volume, and updates the internal 
        /// camera pose if successful.
        /// The maximum image resolution supported in this function is 640x480.
        /// </summary>
        /// <returns>
        /// Returns S_OK if successful; otherwise, returns the error code.
        /// </returns>
        [PreserveSig]
        HRESULT AlignDepthFloatToReconstruction(
            [In] HandleRef depthFloatFrame,
            [In] ushort maxAlignIterationCount,
            [In] HandleRef deltaFromReferenceFrame,
            [Out] out float alignmentEnergy,
            [In] ref Matrix4 worldToCameraTransform);

        /// <summary>
        /// Get current internal camera pose.
        /// </summary>
        /// <returns>
        /// Returns S_OK if successful; otherwise, returns the error code.
        /// </returns>
        [PreserveSig]
        HRESULT GetCurrentWorldToCameraTransform([Out] out Matrix4 worldToCameraTransform);

        /// <summary>
        /// Get current internal world to volume transform.
        /// </summary>
        /// <returns>
        /// Returns S_OK if successful; otherwise, returns the error code.
        /// </returns>
        [PreserveSig]
        HRESULT GetCurrentWorldToVolumeTransform([Out] out Matrix4 worldToVolumeTransform);

        /// <summary>
        /// Integrates depth float data into the reconstruction volume using the current internal 
        /// camera pose, or the optional pWorldToCameraTransform camera pose.
        /// </summary>
        /// <returns>
        /// Returns S_OK if successful; otherwise, returns the error code.
        /// </returns>
        [PreserveSig]
        HRESULT IntegrateFrame(
            [In] HandleRef depthFloatFrame, 
            [In] ushort maxIntegrationWeight, 
            [In] ref Matrix4 worldToCameraTransform);

        /// <summary>
        /// A high-level function to process a depth frame through the Kinect Fusion pipeline.
        /// Specifically, this performs on-GPU processing equivalent to the following functions 
        /// for each frame:
        ///
        /// 1) AlignDepthFloatToReconstruction
        /// 2) IntegrateFrame
        ///
        /// Users may also optionally call the low-level functions individually, instead of calling this
        /// function, for more control. However, this function call will be faster due to the integrated 
        /// nature of the calls. After this call completes, if a visible output image of the reconstruction
        /// is required, the user can call CalculatePointCloud and then FusionDepthProcessor.ShadePointCloud.
        /// The maximum image resolution supported in this function is 640x480.
        ///
        /// If there is a tracking error in the AlignDepthFloatToReconstruction stage, no depth data 
        /// integration will be performed, and the camera pose will remain unchanged.
        /// </summary>
        /// <returns>
        /// Returns S_OK if successful; otherwise, returns the error code.
        /// </returns>
        [PreserveSig]
        HRESULT ProcessFrame(
            [In] HandleRef depthFloatFrame,
            [In] ushort maxAlignIterationCount,
            [In] ushort maxIntegrationWeight,
            [Out] out float alignmentEnergy,
            [In] ref Matrix4 worldToCameraTransform);

        /// <summary>
        /// Calculate a point cloud by raycasting into the reconstruction volume, returning the point
        /// cloud containing 3D points and normals of the zero-crossing dense surface at every visible 
        /// pixel in the image from the given camera pose.
        /// This point cloud can be used as a reference frame in the next call to 
        /// FusionDepthProcessor.AlignPointClouds, or passed to FusionDepthProcessor.ShadePointCloud 
        /// to produce a visible image output. 
        /// pointCloudFrame can be an arbitrary image size, for example, enabling you to calculate 
        /// point clouds at the size of your window and then create a visible image by calling 
        /// ShadePointCloud and render this image. However, large images will be expensive to calculate.
        /// </summary>
        /// <returns>
        /// Returns S_OK if successful; otherwise, returns the error code.
        /// </returns>
        [PreserveSig]
        HRESULT CalculatePointCloud(
            [In] HandleRef pointCloudFrame,
            [In] ref Matrix4 worldToCameraTransform);

        /// <summary>
        /// Export a mesh of the zero-crossing dense surfaces in the reconstruction volume.
        /// </summary>
        /// <returns>
        /// Returns S_OK if successful; otherwise, returns the error code.
        /// </returns>
        [PreserveSig]
        HRESULT CalculateMesh(
            [In] uint voxelStep,
            [MarshalAs(UnmanagedType.Interface)] out INuiFusionMesh mesh);

        /// <summary>
        /// Export a part or all of the reconstruction volume as a short array. 
        /// The surface boundary occurs where the tri-linearly interpolated voxel values have a zero crossing.
        /// Note, this means that a 0 in the volume does not necessarily imply a surface.  A surface only 
        /// occurs when an interpolation crosses from positive to negative or vice versa.   
        /// </summary>
        /// <returns>
        /// Returns S_OK if successful; otherwise, returns the error code.
        /// </returns>
        [PreserveSig]
        HRESULT ExportVolumeBlock(
            [In] uint sourceOriginX,
            [In] uint sourceOriginY,
            [In] uint sourceOriginZ,
            [In] uint destinationResolutionX,
            [In] uint destinationResolutionY,
            [In] uint destinationResolutionZ,
            [In] uint voxelStep,
            [In] uint countVolumeBlockBytes,
            [Out, MarshalAs(UnmanagedType.LPArray, ArraySubType = UnmanagedType.I1, SizeParamIndex = 7)] short[] volumeBlock);

        /// <summary>
        /// Import a reconstruction volume as a short array. 
        /// This array must equal the size of the initialized reconstruction volume.
        /// </summary>
        /// <returns>
        /// Returns S_OK if successful; otherwise, returns the error code.
        /// </returns>
        [PreserveSig]
        HRESULT ImportVolumeBlock(
            [In] uint countVolumeBlockBytes,
            [In, MarshalAs(UnmanagedType.LPArray, ArraySubType = UnmanagedType.I1, SizeParamIndex = 0)] short[] volumeBlock);

        /// <summary>
        /// Converts Kinect depth frames in unsigned short format to depth frames in float format 
        /// representing distance from the camera in meters (parallel to the optical center axis).
        /// Note: <paramref name="depthImageData"/> and <paramref name="depthFloatFrame"/> must
        /// be the same pixel resolution. This version of the function runs on the GPU.
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
        /// <returns>
        /// Returns S_OK if successful; otherwise, returns the error code.
        /// </returns>
        [PreserveSig]
        HRESULT DepthToDepthFloatFrame(
            [In, MarshalAs(UnmanagedType.LPArray, ArraySubType = UnmanagedType.I1, SizeParamIndex = 1)] ushort[] depthImageData,
            [In] uint countDepthImageDataBytes,
            [In] HandleRef depthFloatFrame,
            [In] float minDepthClip,
            [In] float maxDepthClip,
            [In, MarshalAs(UnmanagedType.Bool)] bool mirrorDepth);

        /// <summary>
        /// Spatially smooth a depth float image frame using edge-preserving filtering on GPU. 
        /// </summary>
        /// <param name="depthFloatFrame">A pointer to a pre-allocated depth float frame.</param>
        /// <param name="smoothDepthFloatFrame">A pointer to a pre-allocated depth float frame,
        /// to be filled with smoothed depth.</param>
        /// <param name="kernelWidth">Smoothing Kernel Width. Valid values are  1,2,3 
        /// (for 3x3,5x5,7x7 smoothing kernel block size respectively).</param>
        /// <param name="distanceThreshold">A distance difference range that smoothing occurs in.
        /// Pixels with neighboring pixels outside this distance range will not be smoothed 
        /// (larger values indicate discontinuity/edge). Must be greater than 0.</param>
        /// <returns>
        /// Returns S_OK if successful; otherwise, returns the error code.
        /// </returns>
        [PreserveSig]
        HRESULT SmoothDepthFloatFrame(
            [In] HandleRef depthFloatFrame,
            [In] HandleRef smoothDepthFloatFrame,
            [In] uint kernelWidth,
            [In] float distanceThreshold);

        /// <summary>
        /// The AlignPointClouds function uses an on GPU iterative algorithm to align two sets of 
        /// overlapping oriented point clouds and calculate the camera's relative pose.
        /// All images must be the same size and have the same camera parameters. 
        /// </summary>
        /// <returns>
        /// Returns S_OK if successful; otherwise, returns the error code.
        /// </returns>
        [PreserveSig]
        HRESULT AlignPointClouds(
            [In] HandleRef referencePointCloudFrame,
            [In] HandleRef observedPointCloudFrame,
            [In] ushort maxAlignIterationCount,
            [In] HandleRef deltaFromReferenceFrame,
            [Out] out float alignmentEnergy,
            [In, Out] ref Matrix4 referenceToObservedTransform);

        /// <summary>
        /// Set a reference depth frame to be used internally to help with tracking when calling 
        /// AlignDepthFloatToReconstruction to calculate a new camera pose. This function should
        /// only be called when not using the default tracking behavior of Kinect Fusion.
        /// </summary>
        /// <returns>
        /// Returns S_OK if successful; otherwise, returns the error code.
        /// </returns>
        [PreserveSig]
        HRESULT SetAlignDepthFloatToReconstructionReferenceFrame(
            [In] HandleRef referenceDepthFloatFrame);

        /// <summary>
        /// Calculate a point cloud and depth image by raycasting into the reconstruction volume. 
        /// This returns the point cloud containing 3D points and normals of the zero-crossing dense 
        /// surface at every visible pixel in the image from the given camera pose. This point cloud
        /// can then be used as a reference frame in the next call to 
        /// FusionDepthProcessor.AlignPointClouds, or passed to FusionDepthProcessor.ShadePointCloud
        /// to produce a visible image output.
        /// pointCloudFrame and depthFloatFrame can be an arbitrary image size, for example, enabling 
        /// you to calculate point clouds at the size of your window and then create a visible image 
        /// by calling FusionDepthProcessor.ShadePointCloud and render this image. However, large 
        /// images will be expensive to calculate.
        /// </summary>
        /// <returns>
        /// Returns S_OK if successful; otherwise, returns the error code.
        /// </returns>
        [PreserveSig]
        HRESULT CalculatePointCloudAndDepth(
            [In] HandleRef pointCloudFrame,
            [In] HandleRef depthFloatFrame,
            [In] ref Matrix4 worldToCameraTransform);
    }

    [ComImport, InterfaceType(ComInterfaceType.InterfaceIsIUnknown), Guid("A833A9A1-3608-49FA-982A-E050BFFD8C96")]
    internal interface INuiFusionColorReconstruction
    {
        /// <summary>
        /// Clear the volume, optionally setting a new initial camera pose and worldToVolumeTransform.
        /// </summary>
        /// <returns>
        /// Returns S_OK if successful; otherwise, returns the error code.
        /// </returns>
        [PreserveSig]
        HRESULT ResetReconstruction(
            [In] ref Matrix4 initialWorldToCameraTransform,
            [In] ref Matrix4 worldToVolumeTransform);

        /// <summary>
        /// Aligns a depth float image to the Reconstruction volume to calculate the new camera pose.
        /// This camera tracking method requires a Reconstruction volume, and updates the internal 
        /// camera pose if successful.
        /// The maximum image resolution supported in this function is 640x480.
        /// </summary>
        /// <returns>
        /// Returns S_OK if successful; otherwise, returns the error code.
        /// </returns>
        [PreserveSig]
        HRESULT AlignDepthFloatToReconstruction(
            [In] HandleRef depthFloatFrame,
            [In] ushort maxAlignIterationCount,
            [In] HandleRef deltaFromReferenceFrame,
            [Out] out float alignmentEnergy,
            [In] ref Matrix4 worldToCameraTransform);

        /// <summary>
        /// Get current internal camera pose.
        /// </summary>
        /// <returns>
        /// Returns S_OK if successful; otherwise, returns the error code.
        /// </returns>
        [PreserveSig]
        HRESULT GetCurrentWorldToCameraTransform([Out] out Matrix4 worldToCameraTransform);

        /// <summary>
        /// Get current internal world to volume transform.
        /// </summary>
        /// <returns>
        /// Returns S_OK if successful; otherwise, returns the error code.
        /// </returns>
        [PreserveSig]
        HRESULT GetCurrentWorldToVolumeTransform([Out] out Matrix4 worldToVolumeTransform);

        /// <summary>
        /// Integrates depth float data into the reconstruction volume using the current internal 
        /// camera pose, or the optional worldToCameraTransform camera pose. Also constrains 
        /// integration to integrate color over a given angle in degrees relative to the surface normal 
        /// (recommended use is for thin structure scanning).
        /// Note: Pass FusionDepthProcessor.DefaultColorIntegrationOfAllAngles to ignore the angle parameter
        /// and accept color from all angles (default, fastest processing).
        /// </summary>
        /// <returns>
        /// Returns S_OK if successful; otherwise, returns the error code.
        /// </returns>
        [PreserveSig]
        HRESULT IntegrateFrame(
            [In] HandleRef depthFloatFrame,
            [In] HandleRef colorFrame,
            [In] ushort maxIntegrationWeight,
            [In] float maxColorIntegrationAngle,
            [In] ref Matrix4 worldToCameraTransform);

        /// <summary>
        /// A high-level function to process a depth frame through the Kinect Fusion pipeline.
        /// Optionally also integrates color, further also constrains integration to integrate 
        /// color over a given angle in degrees relative to the surface normal (recommended use is for
        /// thin structure scanning).
        /// Note: Pass FusionDepthProcessor.DefaultColorIntegrationOfAllAngles to ignore the angle parameter
        /// and accept color from all angles (default, fastest processing).
        /// Specifically, this performs on-GPU processing equivalent to the following functions 
        /// for each frame:
        ///
        /// 1) AlignDepthFloatToReconstruction
        /// 2) IntegrateFrame
        ///
        /// Users may also optionally call the low-level functions individually, instead of calling this
        /// function, for more control. However, this function call will be faster due to the integrated 
        /// nature of the calls. After this call completes, if a visible output image of the reconstruction
        /// is required, the user can call CalculatePointCloud and then FusionDepthProcessor.ShadePointCloud.
        /// The maximum image resolution supported in this function is 640x480.
        ///
        /// If there is a tracking error in the AlignDepthFloatToReconstruction stage, no depth data 
        /// integration will be performed, and the camera pose will remain unchanged.
        /// </summary>
        /// <returns>
        /// Returns S_OK if successful; otherwise, returns the error code.
        /// </returns>
        [PreserveSig]
        HRESULT ProcessFrame(
            [In] HandleRef depthFloatFrame,
            [In] HandleRef colorFrame,
            [In] ushort maxAlignIterationCount,
            [In] ushort maxIntegrationWeight,
            [In] float maxColorIntegrationAngle,
            [Out] out float alignmentEnergy,
            [In] ref Matrix4 worldToCameraTransform);

        /// <summary>
        /// Calculate a point cloud by raycasting into the reconstruction volume, returning the point
        /// cloud containing 3D points and normals of the zero-crossing dense surface at every visible 
        /// pixel in the image from the given camera pose.
        /// This point cloud can be used as a reference frame in the next call to 
        /// FusionDepthProcessor.AlignPointClouds, or passed to FusionDepthProcessor.ShadePointCloud to
        /// produce a visible image output. 
        /// pPointCloudFrame can be an arbitrary image size, for example, enabling you to calculate 
        /// point clouds at the size of your window and then create a visible image by calling 
        /// FusionDepthProcessor.ShadePointCloud and render this image. However, large images will be 
        /// expensive to calculate.
        /// </summary>
        /// <returns>
        /// Returns S_OK if successful; otherwise, returns the error code.
        /// </returns>
        [PreserveSig]
        HRESULT CalculatePointCloud(
            [In] HandleRef pointCloudFrame,
            [In] HandleRef colorFrame,
            [In] ref Matrix4 worldToCameraTransform);

        /// <summary>
        /// Export a mesh of the zero-crossing dense surfaces in the reconstruction volume,
        /// with per-vertex color.
        /// </summary>
        /// <returns>
        /// Returns S_OK if successful; otherwise, returns the error code.
        /// </returns>
        [PreserveSig]
        HRESULT CalculateMesh(
            [In] uint voxelStep,
            [MarshalAs(UnmanagedType.Interface)] out INuiFusionColorMesh mesh);

        /// <summary>
        /// Export a part or all of the reconstruction volume as a short array and color as int array.
        /// The surface boundary occurs where the tri-linearly interpolated voxel values have a zero crossing.
        /// Note, this means that a 0 in the volume does not necessarily imply a surface.  A surface only 
        /// occurs when an interpolation crosses from positive to negative or vice versa.   
        /// </summary>
        /// <returns>
        /// Returns S_OK if successful; otherwise, returns the error code.
        /// </returns>
        [PreserveSig]
        HRESULT ExportVolumeBlock(
            [In] uint sourceOriginX,
            [In] uint sourceOriginY,
            [In] uint sourceOriginZ,
            [In] uint destinationResolutionX,
            [In] uint destinationResolutionY,
            [In] uint destinationResolutionZ,
            [In] uint voxelStep,
            [In] uint countVolumeBlockBytes,
            [In] uint countColorVolumeBlockBytes,
            [Out, MarshalAs(UnmanagedType.LPArray, ArraySubType = UnmanagedType.I1, SizeParamIndex = 7)] short[] volumeBlock,
            [Out, MarshalAs(UnmanagedType.LPArray, ArraySubType = UnmanagedType.I1, SizeParamIndex = 8)] int[] colorVolumeBlock);

        /// <summary>
        /// Import a reconstruction volume as a short array and color as int array.
        /// This array must equal the size of the initialized reconstruction volume.
        /// </summary>
        /// <returns>
        /// Returns S_OK if successful; otherwise, returns the error code.
        /// </returns>
        [PreserveSig]
        HRESULT ImportVolumeBlock(
            [In] uint countVolumeBlockBytes,
            [In] uint countColorVolumeBlockBytes,
            [In, MarshalAs(UnmanagedType.LPArray, ArraySubType = UnmanagedType.I1, SizeParamIndex = 0)] short[] volumeBlock,
            [In, MarshalAs(UnmanagedType.LPArray, ArraySubType = UnmanagedType.I1, SizeParamIndex = 1)] int[] colorVolumeBlock);

        /// <summary>
        /// Converts Kinect depth frames in unsigned short format to depth frames in float format 
        /// representing distance from the camera in meters (parallel to the optical center axis).
        /// Note: <paramref name="depthImageData"/> and <paramref name="depthFloatFrame"/> must
        /// be the same pixel resolution. This version of the function runs on the GPU.
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
        /// <returns>
        /// Returns S_OK if successful; otherwise, returns the error code.
        /// </returns>
        [PreserveSig]
        HRESULT DepthToDepthFloatFrame(
            [In, MarshalAs(UnmanagedType.LPArray, ArraySubType = UnmanagedType.I1, SizeParamIndex = 1)] ushort[] depthImageData,
            [In] uint countDepthImageDataBytes,
            [In] HandleRef depthFloatFrame,
            [In] float minDepthClip,
            [In] float maxDepthClip,
            [In, MarshalAs(UnmanagedType.Bool)] bool mirrorDepth);

        /// <summary>
        /// Spatially smooth a depth float image frame using edge-preserving filtering on GPU. 
        /// </summary>
        /// <param name="depthFloatFrame">A pointer to a pre-allocated depth float frame.</param>
        /// <param name="smoothDepthFloatFrame">A pointer to a pre-allocated depth float frame,
        /// to be filled with smoothed depth.</param>
        /// <param name="kernelWidth">Smoothing Kernel Width. Valid values are  1,2,3 
        /// (for 3x3,5x5,7x7 smoothing kernel block size respectively).</param>
        /// <param name="distanceThreshold">A distance difference range that smoothing occurs in.
        /// Pixels with neighboring pixels outside this distance range will not be smoothed 
        /// (larger values indicate discontinuity/edge). Must be greater than 0.</param>
        /// <returns>
        /// Returns S_OK if successful; otherwise, returns the error code.
        /// </returns>
        [PreserveSig]
        HRESULT SmoothDepthFloatFrame(
            [In] HandleRef depthFloatFrame,
            [In] HandleRef smoothDepthFloatFrame,
            [In] uint kernelWidth,
            [In] float distanceThreshold);

        /// <summary>
        /// The AlignPointClouds function uses an on GPU iterative algorithm to align two sets of 
        /// overlapping oriented point clouds and calculate the camera's relative pose.
        /// All images must be the same size and have the same camera parameters. 
        /// </summary>
        /// <returns>
        /// Returns S_OK if successful; otherwise, returns the error code.
        /// </returns>
        [PreserveSig]
        HRESULT AlignPointClouds(
            [In] HandleRef referencePointCloudFrame,
            [In] HandleRef observedPointCloudFrame,
            [In] ushort maxAlignIterationCount,
            [In] HandleRef deltaFromReferenceFrame,
            [Out] out float alignmentEnergy,
            [In, Out] ref Matrix4 referenceToObservedTransform);

        /// <summary>
        /// Set a reference depth frame to be used internally to help with tracking when calling 
        /// AlignDepthFloatToReconstruction to calculate a new camera pose. This function should
        /// only be called when not using the default tracking behavior of Kinect Fusion.
        /// </summary>
        /// <returns>
        /// Returns S_OK if successful; otherwise, returns the error code.
        /// </returns>
        [PreserveSig]
        HRESULT SetAlignDepthFloatToReconstructionReferenceFrame(
            [In] HandleRef referenceDepthFloatFrame);

        /// <summary>
        /// Calculate a point cloud and depth image by raycasting into the reconstruction volume. 
        /// This returns the point cloud containing 3D points and normals of the zero-crossing dense 
        /// surface at every visible pixel in the image from the given camera pose. This point cloud
        /// can then be used as a reference frame in the next call to 
        /// FusionDepthProcessor.AlignPointClouds, or passed to FusionDepthProcessor.ShadePointCloud
        /// to produce a visible image output.
        /// pointCloudFrame and depthFloatFrame can be an arbitrary image size, for example, enabling 
        /// you to calculate point clouds at the size of your window and then create a visible image 
        /// by calling FusionDepthProcessor.ShadePointCloud and render this image. However, large 
        /// images will be expensive to calculate.
        /// </summary>
        /// <returns>
        /// Returns S_OK if successful; otherwise, returns the error code.
        /// </returns>
        [PreserveSig]
        HRESULT CalculatePointCloudAndDepth(
            [In] HandleRef pointCloudFrame,
            [In] HandleRef depthFloatFrame,
            [In] HandleRef colorFrame,
            [In] ref Matrix4 worldToCameraTransform);
    }

    [ComImport, InterfaceType(ComInterfaceType.InterfaceIsIUnknown), Guid("42C1992C-043A-492D-A486-E9275197E7E6")]
    internal interface INuiFusionMatchCandidates
    {
        /// <summary>
        /// Gets the number of camera poses matched to the input in the camera pose finder.
        /// </summary>
        /// <returns>
        /// Returns the number of Matrix4 poses.
        /// </returns>
        [PreserveSig]
        uint MatchPoseCount();

        /// <summary>
        /// Gets the camera poses matched to the input in the camera pose finder. The poses are sorted in
        /// terms of descending similarity (i.e. the most similar is first).
        /// </summary>
        /// <returns>
        /// Returns S_OK if successful; otherwise, returns the error code.
        /// </returns>
        [PreserveSig]
        HRESULT GetMatchPoses([Out] out IntPtr poses);

        /// <summary>
        /// Gets the number of camera pose similarity measurements to the input in the camera pose finder.
        /// </summary>
        /// <returns>
        /// Returns the number of similarity measurements (equal to number of poses).
        /// </returns>
        [PreserveSig]
        uint MatchSimilarityCount();

        /// <summary>
        /// Gets the matched frame similarity measurements, indicating how similar the match is to the input
        /// in the camera pose finder. These measurements correspond to the respective poses returned in 
        /// GetMatchPoses and are sorted in terms of descending similarity (i.e. the most similar is first).
        /// </summary>
        /// <returns>
        /// Returns S_OK if successful; otherwise, returns the error code.
        /// </returns>
        [PreserveSig]
        HRESULT GetMatchSimilarities([Out] out IntPtr similarityMeasurements);

        /// <summary>
        /// Calculate the minimum distance between the input and the single most similar pose frame.
        /// </summary>
        /// <returns>
        /// Returns S_OK if successful; otherwise, returns the error code.
        /// </returns>
        [PreserveSig]
        HRESULT CalculateMinimumDistance([Out] out float minimumDistance);
    }

    [ComImport, InterfaceType(ComInterfaceType.InterfaceIsIUnknown), Guid("4C98136C-F201-4C00-9911-4FCB9918FFD5")]
    internal interface INuiFusionCameraPoseFinder
    {
        /// <summary>
        /// Clear the camera pose finder database information.
        /// </summary>
        /// <returns>
        /// Returns S_OK if successful; otherwise, returns the error code.
        /// </returns>
        [PreserveSig]
        HRESULT ResetCameraPoseFinder();

        /// <summary>
        /// Test input camera frames against the camera pose finder database, adding frames to the
        /// database if dis-similar enough to existing frames. Both input depth and color frames must 
        /// be identical sizes, with valid camera parameters, and captured at the same time.
        /// </summary>
        /// <returns>
        /// Returns S_OK if successful; otherwise, returns the error code.
        /// </returns>
        [PreserveSig]
        HRESULT ProcessFrame(
            [In] HandleRef depthFloatFrame,
            [In] HandleRef colorFrame,
            [In] ref Matrix4 worldToCameraTransform,
            [In] float minimumDistanceThreshold,
            [Out, MarshalAs(UnmanagedType.Bool)] out bool addedFrame,
            [Out, MarshalAs(UnmanagedType.Bool)] out bool historyTrimmed);

        /// <summary>
        /// Test input camera frames against the camera pose finder database, returning a set of similar
        /// camera poses in <paramref name="matchCandidates"/>. These poses and similarity measurements
        /// are ordered in terms of descending similarity (i.e. the most similar is first).
        /// Both input depth and color frames must be identical sizes, with valid camera parameters
        /// and captured at the same time.
        /// </summary>
        /// <returns>
        /// Returns S_OK if successful; otherwise, returns the error code.
        /// </returns>
        [PreserveSig]
        HRESULT FindCameraPose(
            [In] HandleRef depthFloatFrame,
            [In] HandleRef colorFrame,
            [Out, MarshalAs(UnmanagedType.Interface)] out INuiFusionMatchCandidates matchCandidates);

        /// <summary>
        /// Load a previously saved camera pose finder database from disk. 
        /// Note: All existing camera pose finder data is replaced on a successful load of the database.
        /// If the database is saved to disk alongside the reconstruction volume, when both are
        /// re-loaded, this potentially enables reconstruction and tracking to be re-started and
        /// the reconstruction updated by running the camera pose finder.
        /// </summary>
        /// <returns>
        /// Returns S_OK if successful; otherwise, returns the error code.
        /// </returns>
        [PreserveSig]
        HRESULT LoadCameraPoseFinderDatabase(
            [In, MarshalAs(UnmanagedType.LPWStr)]string fileName);

        /// <summary>
        /// Save the camera pose finder database to disk. 
        /// If the database is saved to disk alongside the reconstruction volume, when both are
        /// re-loaded, this potentially enables reconstruction and tracking to be re-started and
        /// the reconstruction updated by running the camera pose finder.
        /// </summary>
        /// <returns>
        /// Returns S_OK if successful; otherwise, returns the error code.
        /// </returns>
        [PreserveSig]
        HRESULT SaveCameraPoseFinderDatabase(
            [In, MarshalAs(UnmanagedType.LPWStr)]string fileName);

        /// <summary>
        /// Get the parameters used to create the camera pose finder database.
        /// </summary>
        /// <returns>
        /// Returns S_OK if successful; otherwise, returns the error code.
        /// </returns>
        [PreserveSig]
        HRESULT GetCameraPoseFinderParameters(
            [Out] CameraPoseFinderParameters cameraPoseFinderParameters);

        /// <summary>
        /// Get the number of poses stored in the camera pose finder database.
        /// </summary>
        /// <returns>
        /// Returns the number of stored poses.
        /// </returns>
        [PreserveSig]
        uint GetStoredPoseCount();
    }

    /// <summary>
    /// The native NUI_FUSION_IMAGE_FRAME structure.
    /// </summary>
    [StructLayout(LayoutKind.Sequential)]
    internal struct NUI_FUSION_IMAGE_FRAME
    {
        /// <summary>
        /// The width of the image frame.
        /// </summary>
        public uint Width;

        /// <summary>
        /// The height of the image frame.
        /// </summary>
        public uint Height;

        /// <summary>
        /// The image frame type.
        /// </summary>
        public FusionImageType ImageType;

        /// <summary>
        /// The pointer to the camera parameters.
        /// </summary>
        public IntPtr CameraParameters;

        /// <summary>
        /// The pointer to the image frame buffer.
        /// </summary>
        public IntPtr FrameBuffer;
    }

    /// <summary>
    /// The native NUI_FUSION_FRAME_BUFFER structure.
    /// </summary>
    [StructLayout(LayoutKind.Sequential)]
    internal struct NUI_FUSION_FRAME_BUFFER
    {
        /// <summary>
        /// The pitch of the frame buffer.
        /// </summary>
        public uint Pitch;

        /// <summary>
        /// The pointer to the buffer data.
        /// </summary>
        public IntPtr RawBits;
    }

    /// <summary>
    /// Native API declarations from KinectFusion dll.
    /// </summary>
    internal static class NativeMethods
    {
        /// <summary>
        /// The delegate for NuiFusionCreateImageFrame function.
        /// </summary>
        private static NuiFusionCreateImageFrameDelegate CreateImageFrame;

        /// <summary>
        /// The delegate for NuiFusionReleaseImageFrame function.
        /// </summary>
        private static NuiFusionReleaseImageFrameDelegate ReleaseImageFrame;

        /// <summary>
        /// The delegate for NuiFusionDepthToDepthFloatFrame function.
        /// </summary>
        private static NuiFusionDepthToDepthFloatFrameDelegate DepthToDepthFloatFrame;

        /// <summary>
        /// The delegate for NuiFusionDepthFloatFrameToPointCloud function.
        /// </summary>
        private static NuiFusionDepthFloatFrameToPointCloudDelegate DepthFloatFrameToPointCloud;

        /// <summary>
        /// A delegate for NuiFusionShadePointCloud function.
        /// </summary>
        private static NuiFusionShadePointCloudDelegate ShadePointCloud;

        /// <summary>
        /// A delegate for NuiFusionShadePointCloud function.
        /// </summary>
        private static NuiFusionShadePointCloud2Delegate ShadePointCloud2;

        /// <summary>
        /// The delegate for NuiFusionAlignPointClouds function.
        /// </summary>
        private static NuiFusionAlignPointCloudsDelegate AlignPointClouds;

        /// <summary>
        /// The delegate for NuiFusionCreateReconstruction function.
        /// </summary>
        private static NuiFusionCreateReconstructionDelegate CreateReconstruction;

        /// <summary>
        /// The delegate for NuiFusionCreateColorReconstruction function.
        /// </summary>
        private static NuiFusionCreateColorReconstructionDelegate CreateColorReconstruction;

        /// <summary>
        /// The delegate for NuiFusionGetDeviceInfo function.
        /// </summary>
        private static NuiFusionGetDeviceInfoDelegate GetDeviceInfo;

        /// <summary>
        /// The delegate for NuiFusionCreateCameraPoseFinder function.
        /// </summary>
        private static NuiFusionCreateCameraPoseFinderDelegate CreateCameraPoseFinder;

        /// <summary>
        /// The delegate for NuiFusionCreateCameraPoseFinder function to handle null.
        /// </summary>
        private static NuiFusionCreateCameraPoseFinder2Delegate CreateCameraPoseFinder2;

        /// <summary>
        /// The native KinectFusion dll handle.
        /// </summary>
        private static IntPtr fusionModule;

        /// <summary>
        /// Create an image frame for use with Kinect Fusion with a specified data type and resolution.
        /// Note that image width must be a minimum of 32 pixels in both width and height and for camera 
        /// tracking Align functions and volume Integration, use of the default camera parameters is only 
        /// supported with 4:3 pixel aspect ratio images such as uniformly scaled versions of the source Kinect 
        /// image (e.g. 160x120,320x240,640x480 etc.). To crop to smaller non 4:3 ratios and still use the 
        /// default camera parameters set unwanted pixels to 0 depth, which will be ignored in processing, or 
        /// alternately, the user can supply their own calibration with an arbitrary sized image. For example,
        /// a user supplied set of parameters can be used when calling CalculatePointCloud to calculate a 
        /// large image of the reconstruction at the UI window resolution (perhaps with a virtual viewpoint 
        /// different to the Kinect camera or a non 4:3 aspect image ratio) by then subsequently calling 
        /// ShadePointCloud and rendering the resulting images on screen.
        /// </summary>
        /// <returns>
        /// Returns S_OK if successful; otherwise, returns the error code.
        /// </returns>
        internal delegate HRESULT NuiFusionCreateImageFrameDelegate(
                                                                 [In] FusionImageType frameType,
                                                                 [In] uint width,
                                                                 [In] uint height,
                                                                 [In, Optional] CameraParameters cameraParameters,
                                                                 [Out] out NativeFrameHandle imageFrame);

        /// <summary>
        /// Releases the specified frame of data.
        /// </summary>
        /// <returns>
        /// Returns S_OK if successful; otherwise, returns the error code.
        /// </returns>
        internal delegate HRESULT NuiFusionReleaseImageFrameDelegate([In] NativeFrameHandle imageFrame);

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
        /// <returns>
        /// Returns S_OK if successful; otherwise, returns the error code.
        /// </returns>
        internal delegate HRESULT NuiFusionDepthToDepthFloatFrameDelegate(
                                                                       [In] ushort[] depthImageData,
                                                                       [In] uint depthImageDataWidth,
                                                                       [In] uint depthImageDataHeight,
                                                                       [In] HandleRef depthFloatFrame,
                                                                       [In] float minDepthClip,
                                                                       [In] float maxDepthClip,
                                                                       [In, MarshalAs(UnmanagedType.Bool)] bool mirrorDepth);

        /// <summary>
        /// Construct an oriented point cloud in the local camera frame of reference from a depth float
        /// image frame. Here we calculate the 3D position of each depth float pixel with the optical
        /// center of the camera as the origin. Both images must be the same size and have the same camera
        /// parameters. We use a right-hand coordinate system, and (in common with bitmap images with top left
        /// origin) +X is to the right, +Y down, and +Z is now forward from the Kinect camera into the scene,
        /// as though looking into the scene from behind the kinect.
        /// </summary>
        /// <returns>
        /// Returns S_OK if successful; otherwise, returns the error code.
        /// </returns>
        internal delegate HRESULT NuiFusionDepthFloatFrameToPointCloudDelegate(
                                                                            [In] HandleRef depthFloatFrame,
                                                                            [In] HandleRef pointCloudFrame);

        /// <summary>
        /// Create visible color shaded images of a point cloud and its normals. All image frames must be
        /// the same size and have the same camera parameters.
        /// </summary>
        /// <returns>
        /// Returns S_OK if successful; otherwise, returns the error code.
        /// </returns>
        internal delegate HRESULT NuiFusionShadePointCloudDelegate(
                                                                [In] HandleRef pointCloudFrame,
                                                                [In] ref Matrix4 worldToCameraTransform,
                                                                [In] ref Matrix4 worldToBGRTransform,
                                                                [In] HandleRef shadedSurfaceFrame,
                                                                [In] HandleRef shadedSurfaceNormalsFrame);

        /// <summary>
        /// Create visible color shaded images of a point cloud and its normals. All image frames must be
        /// the same size and have the same camera parameters.
        /// </summary>
        /// <returns>
        /// Returns S_OK if successful; otherwise, returns the error code.
        /// </returns>
        internal delegate HRESULT NuiFusionShadePointCloud2Delegate(
                                                                [In] HandleRef pointCloudFrame,
                                                                [In] ref Matrix4 worldToCameraTransform,
                                                                [In] IntPtr worldToBGRTransform,
                                                                [In] HandleRef shadedSurfaceFrame,
                                                                [In] HandleRef shadedSurfaceNormalsFrame);

        /// <summary>
        /// The AlignPointClouds function uses an iterative algorithm to align two sets of oriented point 
        /// clouds and calculate the camera's relative pose. This is a generic function which can be used
        /// independently of a Reconstruction Volume with sets of overlapping point clouds.
        /// All images must be the same size and have the same camera parameters.
        /// To find the frame to frame relative transformation between two sets of point clouds in the 
        /// camera local frame of reference (created by NuiFusionDepthFloatFrameToPointCloud), set the
        /// referenceToObservedTransform to NULL or the identity.
        /// To calculate the pose transformation between new depth frames and an existing Reconstruction
        /// volume, pass in previous frame's point cloud from CalculatePointCloud as the reference frame,
        /// and the current frame point cloud (from NuiFusionDepthFloatFrameToPointCloud) as the observed
        /// frame. Set the referenceToObservedTransform to the previous frames calculated camera pose.
        /// Note that here the current frame point cloud will be in the camera local frame of reference,
        /// whereas the synthetic points and normals will be in the global/world volume coordinate system.
        /// By passing the referenceToObservedTransform you make the algorithm aware of the transformation
        /// between them.
        /// The referenceToObservedTransform pose supplied can also take into account information you may
        /// have from other sensors or sensing mechanisms to aid the tracking. 
        /// To do this multiply the relative frame to frame delta transformation from the other sensing
        /// system with the previous frame's pose before passing to this function.
        /// Note that any delta transform used should be in the same coordinate system as that returned 
        /// by the NuiFusionDepthFloatFrameToPointCloud calculation.
        /// </summary>
        /// <returns>
        /// Returns S_OK if successful; otherwise, returns the error code.
        /// </returns>
        internal delegate HRESULT NuiFusionAlignPointCloudsDelegate(
                                                                 [In] HandleRef referencePointCloudFrame,
                                                                 [In] HandleRef observedPointCloudFrame,
                                                                 [In] ushort maxAlignIterationCount,
                                                                 [In, Optional] HandleRef deltaFromReferenceFrame,
                                                                 [In, Out] ref Matrix4 referenceToObservedTransform);

        /// <summary>
        /// Create Kinect Fusion Reconstruction Volume instance.
        /// Voxel volume axis sizes must be greater than 0 and a multiple of 32.
        /// Users can select which device the processing is performed on with the reconstructionProcessorType parameter.
        /// For those with multiple devices the deviceIndex parameter also enables users to explicitly configure on 
        /// which device the reconstruction volume is created.
        /// Note that this function creates a default world-volume transform. To set a non-default
        /// transform call ResetReconstruction with an appropriate Matrix4. This default transform is a
        /// translation and scale to locate the world origin at the center of the front face of the volume
        /// and set the relationship between the the world coordinates and volume indices.
        /// </summary>
        /// <returns>
        /// Returns S_OK if successful; otherwise, returns the error code.
        /// </returns>
        internal delegate HRESULT NuiFusionCreateReconstructionDelegate(
                                                                     [In] ReconstructionParameters reconstructionParameters,
                                                                     [In] ReconstructionProcessor reconstructionProcessorType,
                                                                     [In] int deviceIndex,
                                                                     [In] ref Matrix4 initialWorldToCameraTransform,
                                                                     [Out] out INuiFusionReconstruction ppVolume);

        /// <summary>
        /// Create Kinect Fusion Reconstruction Volume instance enabling use of in-volume color.
        /// Voxel volume axis sizes must be greater than 0 and a multiple of 32.
        /// Users can select which device the processing is performed on with the reconstructionProcessorType parameter.
        /// For those with multiple devices the deviceIndex parameter also enables users to explicitly configure on 
        /// which device the reconstruction volume is created.
        /// Note that this function creates a default world-volume transform. To set a non-default
        /// transform call ResetReconstruction with an appropriate Matrix4. This default transform is a
        /// translation and scale to locate the world origin at the center of the front face of the volume
        /// and set the relationship between the the world coordinates and volume indices.
        /// </summary>
        /// <returns>
        /// Returns S_OK if successful; otherwise, returns the error code.
        /// </returns>
        internal delegate HRESULT NuiFusionCreateColorReconstructionDelegate(
                                                                     [In] ReconstructionParameters reconstructionParameters,
                                                                     [In] ReconstructionProcessor reconstructionProcessorType,
                                                                     [In] int deviceIndex,
                                                                     [In] ref Matrix4 initialWorldToCameraTransform,
                                                                     [Out] out INuiFusionColorReconstruction ppVolume);

        /// <summary>
        /// Enumerate the devices capable of running KinectFusion.
        /// This enables a specific device to be chosen when calling NuiFusionCreateReconstruction if desired.
        /// </summary>
        /// <param name="type">The type of processor to enumerate.</param>
        /// <param name="index">The zero-based index of the device for which the description is returned.</param>
        /// <param name="description">A buffer that receives a description string for the device.</param>
        /// <param name="descriptionSizeInChar">The size of the buffer referenced by <paramref name="description"/>, in characters.</param>
        /// <param name="instancePath">A buffer that receives the device instance string.</param>
        /// <param name="instancePathSizeInChar">The size of the buffer referenced by <paramref name="instancePath"/>, in characters.</param>
        /// <param name="memoryKB">On success, the variable is assigned the total amount of memory on the device, in kilobytes.</param>
        /// <returns>
        /// Returns S_OK if successful; otherwise, returns the error code.
        /// </returns>
        internal delegate HRESULT NuiFusionGetDeviceInfoDelegate(
                                                                 [In] ReconstructionProcessor type,
                                                                 [In] int index,
                                                                 [Out, MarshalAs(UnmanagedType.LPWStr)] StringBuilder description,
                                                                 [In] uint descriptionSizeInChar,
                                                                 [Out, MarshalAs(UnmanagedType.LPWStr)] StringBuilder instancePath,
                                                                 [In] uint instancePathSizeInChar,
                                                                 [Out] out uint memoryKB);

        /// <summary>
        /// Create Kinect Fusion camera pose finder instance.
        /// </summary>
        /// <returns>
        /// Returns S_OK if successful; otherwise, returns the error code.
        /// </returns>
        internal delegate HRESULT NuiFusionCreateCameraPoseFinderDelegate(
                                                                     [In] CameraPoseFinderParameters cameraPoseFinderParameters,
                                                                     [In] ref uint randomFeatureLocationAndThresholdSeed,
                                                                     [Out] out INuiFusionCameraPoseFinder ppNuiFusionCameraPoseFinder);

        /// <summary>
        /// Create Kinect Fusion camera pose finder instance 2 to handle null.
        /// </summary>
        /// <returns>
        /// Returns S_OK if successful; otherwise, returns the error code.
        /// </returns>
        internal delegate HRESULT NuiFusionCreateCameraPoseFinder2Delegate(
                                                                     [In] CameraPoseFinderParameters cameraPoseFinderParameters,
                                                                     [In] IntPtr randomFeatureLocationAndThresholdSeed,
                                                                     [Out] out INuiFusionCameraPoseFinder ppNuiFusionCameraPoseFinder);


        /// <summary>
        /// Get the delegate for the native NuiFusionCreateImageFrame function.
        /// </summary>
        internal static NuiFusionCreateImageFrameDelegate NuiFusionCreateImageFrame
        {
            get
            {
                if (CreateImageFrame == null)
                {
                    CreateImageFrame = GetFunctionDelegate<NuiFusionCreateImageFrameDelegate>("NuiFusionCreateImageFrame");
                }

                return CreateImageFrame;
            }
        }

        /// <summary>
        /// Get the delegate for the native NuiFusionReleaseImageFrame function.
        /// </summary>
        internal static NuiFusionReleaseImageFrameDelegate NuiFusionReleaseImageFrame
        {
            get
            {
                if (ReleaseImageFrame == null)
                {
                    ReleaseImageFrame = GetFunctionDelegate<NuiFusionReleaseImageFrameDelegate>("NuiFusionReleaseImageFrame");
                }

                return ReleaseImageFrame;
            }
        }

        /// <summary>
        /// Get the delegate for the native NuiFusionDepthToDepthFloatFrame function.
        /// </summary>
        internal static NuiFusionDepthToDepthFloatFrameDelegate NuiFusionDepthToDepthFloatFrame
        {
            get
            {
                if (DepthToDepthFloatFrame == null)
                {
                    DepthToDepthFloatFrame = GetFunctionDelegate<NuiFusionDepthToDepthFloatFrameDelegate>("NuiFusionDepthToDepthFloatFrame");
                }

                return DepthToDepthFloatFrame;
            }
        }

        /// <summary>
        /// Get the delegate for the native NuiFusionDepthFloatFrameToPointCloud function.
        /// </summary>
        internal static NuiFusionDepthFloatFrameToPointCloudDelegate NuiFusionDepthFloatFrameToPointCloud
        {
            get
            {
                if (DepthFloatFrameToPointCloud == null)
                {
                    DepthFloatFrameToPointCloud = GetFunctionDelegate<NuiFusionDepthFloatFrameToPointCloudDelegate>("NuiFusionDepthFloatFrameToPointCloud");
                }

                return DepthFloatFrameToPointCloud;
            }
        }

        /// <summary>
        /// Get the delegate for the native NuiFusionShadePointCloud function.
        /// </summary>
        internal static NuiFusionShadePointCloudDelegate NuiFusionShadePointCloud
        {
            get
            {
                if (ShadePointCloud == null)
                {
                    ShadePointCloud = GetFunctionDelegate<NuiFusionShadePointCloudDelegate>("NuiFusionShadePointCloud");
                }

                return ShadePointCloud;
            }
        }

        /// <summary>
        /// Get the delegate for the native NuiFusionShadePointCloud2 function.
        /// </summary>
        internal static NuiFusionShadePointCloud2Delegate NuiFusionShadePointCloud2
        {
            get
            {
                if (ShadePointCloud2 == null)
                {
                    ShadePointCloud2 = GetFunctionDelegate<NuiFusionShadePointCloud2Delegate>("NuiFusionShadePointCloud");
                }

                return ShadePointCloud2;
            }
        }

        /// <summary>
        /// Get the delegate for the native NuiFusionAlignPointClouds function.
        /// </summary>
        internal static NuiFusionAlignPointCloudsDelegate NuiFusionAlignPointClouds
        {
            get
            {
                if (AlignPointClouds == null)
                {
                    AlignPointClouds = GetFunctionDelegate<NuiFusionAlignPointCloudsDelegate>("NuiFusionAlignPointClouds");
                }

                return AlignPointClouds;
            }
        }

        /// <summary>
        /// Get the delegate for the native NuiFusionCreateReconstruction function.
        /// </summary>
        internal static NuiFusionCreateReconstructionDelegate NuiFusionCreateReconstruction
        {
            get
            {
                if (CreateReconstruction == null)
                {
                    CreateReconstruction = GetFunctionDelegate<NuiFusionCreateReconstructionDelegate>("NuiFusionCreateReconstruction");
                }

                return CreateReconstruction;
            }
        }

        /// <summary>
        /// Get the delegate for the native NuiFusionCreateColorReconstruction function.
        /// </summary>
        internal static NuiFusionCreateColorReconstructionDelegate NuiFusionCreateColorReconstruction
        {
            get
            {
                if (CreateColorReconstruction == null)
                {
                    CreateColorReconstruction = GetFunctionDelegate<NuiFusionCreateColorReconstructionDelegate>("NuiFusionCreateColorReconstruction");
                }

                return CreateColorReconstruction;
            }
        }

        /// <summary>
        /// Get the delegate for the native NuiFusionGetDeviceInfo function.
        /// </summary>
        internal static NuiFusionGetDeviceInfoDelegate NuiFusionGetDeviceInfo
        {
            get
            {
                if (GetDeviceInfo == null)
                {
                    GetDeviceInfo = GetFunctionDelegate<NuiFusionGetDeviceInfoDelegate>("NuiFusionGetDeviceInfo");
                }

                return GetDeviceInfo;
            }
        }

        /// <summary>
        /// Get the delegate for the native NuiFusionCreateCameraPoseFinder function.
        /// </summary>
        internal static NuiFusionCreateCameraPoseFinderDelegate NuiFusionCreateCameraPoseFinder
        {
            get
            {
                if (CreateCameraPoseFinder == null)
                {
                    CreateCameraPoseFinder = GetFunctionDelegate<NuiFusionCreateCameraPoseFinderDelegate>("NuiFusionCreateCameraPoseFinder");
                }

                return CreateCameraPoseFinder;
            }
        }

        /// <summary>
        /// Get the delegate for the native NuiFusionCreateCameraPoseFinder function.
        /// </summary>
        internal static NuiFusionCreateCameraPoseFinder2Delegate NuiFusionCreateCameraPoseFinder2
        {
            get
            {
                if (CreateCameraPoseFinder2 == null)
                {
                    CreateCameraPoseFinder2 = GetFunctionDelegate<NuiFusionCreateCameraPoseFinder2Delegate>("NuiFusionCreateCameraPoseFinder");
                }

                return CreateCameraPoseFinder2;
            }
        }

        /// <summary>
        /// Get the library name for current execution architecture.
        /// </summary>
        private static string LibraryName
        {
            get { return "Kinect20.Fusion.dll"; }
        }

        // Get the native handle to the KinectFusion dll.
        private static IntPtr FusionModule
        {
            get
            {
                if (fusionModule == IntPtr.Zero)
                {
                    System.Reflection.Assembly assembly = System.Reflection.Assembly.GetExecutingAssembly();
                    string location = System.IO.Path.GetDirectoryName(assembly.Location);
                    fusionModule = LoadLibrary(location + '\\' + LibraryName);

                    if (fusionModule == IntPtr.Zero)
                    {
                        string platformPath = (sizeof(int) == IntPtr.Size ? @"\x86\" : @"\x64\");
                        fusionModule = LoadLibrary(location + platformPath + LibraryName);

                        if (fusionModule == IntPtr.Zero)
                        {
                            throw new InvalidOperationException(
                                string.Format(CultureInfo.InvariantCulture, Resources.LibraryInvocationFailed, LibraryName));
                        }
                    }
                }

                return fusionModule;
            }
        }

        /// <summary>
        /// Loads the specified dll into the address space of the calling process.
        /// </summary>
        /// <returns>
        /// If the function succeeds, the return value is a handle to the module.
        /// If the function fails, the return value is NULL. To get extended error information, call GetLastError
        /// </returns>
        [DllImport("kernel32", SetLastError = true, CharSet = CharSet.Unicode)]
        internal static extern IntPtr LoadLibrary(string fileName);

        /// <summary>
        /// Retrieves the address of an exported function or variable from the specified
        /// dynamic-link library (DLL).
        /// </summary>
        /// <returns>
        /// If the function succeeds, the return value is a handle to the module.
        /// If the function fails, the return value is NULL. To get extended error information, call GetLastError
        /// </returns>
        [DllImport("kernel32", SetLastError = true, BestFitMapping = false)]
        internal static extern IntPtr GetProcAddress(IntPtr module, [MarshalAs(UnmanagedType.LPStr)] string procName);

        /// <summary>
        /// Get the delegate of a native function.
        /// </summary>
        /// <typeparam name="FunctionType">The function delegate type.</typeparam>
        /// <param name="functionName">The native function name to be loaded.</param>
        /// <returns>The delegate of the function.</returns>
        private static FunctionType GetFunctionDelegate<FunctionType>(string functionName)
            where FunctionType : class
        {
            // Find the entry point
            IntPtr nativeFunction = GetProcAddress(FusionModule, functionName);

            if (nativeFunction == IntPtr.Zero)
            {
                throw new InvalidOperationException(
                    string.Format(CultureInfo.InvariantCulture, Resources.LoadFunctionFailed, functionName));
            }

            // Get a delegate from it
            return Marshal.GetDelegateForFunctionPointer(
                    nativeFunction,
                    typeof(FunctionType)) as FunctionType;
        }
    }
}
