//-----------------------------------------------------------------------
// <copyright file="ColorReconstruction.cs" company="Microsoft Corporation">
//      Copyright (c) Microsoft Corporation. All rights reserved.
// </copyright>
//-----------------------------------------------------------------------

namespace Microsoft.Kinect.Fusion
{
    using System;
    using System.Runtime.InteropServices;

    /// <summary>
    /// Reconstruction encapsulates reconstruction volume creation updating and meshing functions with color.
    /// </summary>
    public class ColorReconstruction : IDisposable
    {
        /// <summary>
        /// The native reconstruction interface wrapper.
        /// </summary>
        private INuiFusionColorReconstruction volume;
        private Matrix4 defaultWorldToVolumeTransform;

        /// <summary>
        /// Track whether Dispose has been called.
        /// </summary>
        private bool disposed = false;

        /// <summary>
        /// Initializes a new instance of the ColorReconstruction class.
        /// Default constructor used to initialize with the native color Reconstruction volume object.
        /// </summary>
        /// <param name="volume">
        /// The native Reconstruction volume object to be encapsulated.
        /// </param>
        internal ColorReconstruction(INuiFusionColorReconstruction volume)
        {
            this.volume = volume;
            this.defaultWorldToVolumeTransform = this.GetCurrentWorldToVolumeTransform();
        }

        /// <summary>
        /// Finalizes an instance of the ColorReconstruction class.
        /// This destructor will run only if the Dispose method does not get called.
        /// </summary>
        ~ColorReconstruction()
        {
            Dispose(false);
        }

        /// <summary>
        /// Initialize a Kinect Fusion 3D Reconstruction Volume enabling use with color.
        /// Voxel volume axis sizes must be greater than 0 and a multiple of 32. A Kinect camera 
        /// is also required to be connected.
        /// </summary>
        /// <param name="reconstructionParameters">
        /// The Reconstruction parameters to define the size and shape of the reconstruction volume.
        /// </param>
        /// <param name="reconstructionProcessorType">
        /// the processor type to be used for all calls to the reconstruction volume object returned
        /// from this function.
        /// </param>
        /// <param name="deviceIndex">Set this variable to an explicit zero-based device index to use
        /// a specific GPU as enumerated by FusionDepthProcessor.GetDeviceInfo, or set to -1 to 
        /// automatically select the default device for a given processor type.
        /// </param>
        /// <param name="initialWorldToCameraTransform">
        /// The initial camera pose of the reconstruction volume with respect to the world origin. 
        /// Pass identity as the default camera pose. 
        /// </param>
        /// <returns>The Reconstruction instance.</returns>
        /// <exception cref="ArgumentNullException">
        /// Thrown when the <paramref name="reconstructionParameters"/> parameter is null.
        /// </exception>
        /// <exception cref="ArgumentException">
        /// Thrown when the <paramref name="reconstructionParameters"/> parameter's <c>VoxelX</c>,
        /// <c>VoxelY</c>, or <c>VoxelZ</c> member is not a greater than 0 and multiple of 32.
        /// Thrown when the <paramref name="deviceIndex"/> parameter is less than -1 or greater 
        /// than the number of available devices for the respective processor type.
        /// </exception>
        /// <exception cref="OutOfMemoryException">
        /// Thrown when the memory required for the Reconstruction volume processing could not be
        /// allocated.
        /// </exception>
        /// <exception cref="InvalidOperationException">
        /// Thrown when the Kinect Runtime could not be accessed, the Kinect device is not
        /// connected or the Reconstruction volume is too big so a GPU memory allocation failed, 
        /// or the call failed for an unknown reason.
        /// </exception>
        /// <remarks>
        /// Users can select which device the processing is performed on with
        /// the <paramref name="reconstructionProcessorType"/> parameter. For those with multiple GPUs
        /// the <paramref name="deviceIndex"/> parameter also enables users to explicitly configure
        /// on which device the reconstruction volume is created.
        /// Note that this function creates a default world-volume transform. To set a non-default
        /// transform call ResetReconstruction with an appropriate Matrix4. This default transformation
        /// is a combination of translation in X,Y to locate the world origin at the center of the front
        /// face of the reconstruction volume cube, and scaling by the voxelsPerMeter reconstruction
        /// parameter to convert from the world coordinate system to volume voxel indices.
        /// </remarks>
        public static ColorReconstruction FusionCreateReconstruction(
            ReconstructionParameters reconstructionParameters,
            ReconstructionProcessor reconstructionProcessorType,
            int deviceIndex,
            Matrix4 initialWorldToCameraTransform)
        {
            if (null == reconstructionParameters)
            {
                throw new ArgumentNullException("reconstructionParameters");
            }

            INuiFusionColorReconstruction reconstruction = null;

            ExceptionHelper.ThrowIfFailed(NativeMethods.NuiFusionCreateColorReconstruction(
                reconstructionParameters,
                reconstructionProcessorType,
                deviceIndex,
                ref initialWorldToCameraTransform,
                out reconstruction));

            return new ColorReconstruction(reconstruction);
        }

        /// <summary>
        /// Clear the volume, and set a new world-to-camera transform (camera view pose) or identity. 
        /// This internally sets the default world-to-volume transform. where the Kinect camera is
        /// translated in X,Y to the center of the front face of the volume cube, looking into the cube, 
        /// and the world coordinates are scaled to volume indices according to the voxels per meter 
        /// setting.
        /// </summary>
        /// <param name="initialWorldToCameraTransform">
        /// The initial camera pose with respect to the world origin. 
        /// Pass identity as the default camera pose. 
        /// </param>
        /// <exception cref="InvalidOperationException">
        /// Thrown when the Kinect Runtime could not be accessed, the device is not connected,
        /// or the call failed for an unknown reason.
        /// </exception>
        public void ResetReconstruction(Matrix4 initialWorldToCameraTransform)
        {
            ExceptionHelper.ThrowIfFailed(volume.ResetReconstruction(
                ref initialWorldToCameraTransform,
                ref defaultWorldToVolumeTransform));
        }

        /// <summary>
        /// Clear the reconstruction volume, and set a world-to-camera transform (camera view pose)
        /// and a world-to-volume transform.
        /// The world-volume transform expresses the location and orientation of the world coordinate 
        /// system origin in volume coordinates and the scaling of the world coordinates to
        /// volume indices. In practice, this controls where the reconstruction volume appears in the
        /// real world with respect to the world origin position, or with respect to the camera if 
        /// identity is passed for the initial world-to-camera transform (as the camera and world 
        /// origins then coincide).
        /// To create your own world-volume transformation first get the current transform by calling
        /// GetCurrentWorldToVolumeTransform then either modify the matrix directly or multiply
        /// with your own similarity matrix to alter the volume translation or rotation with respect
        /// to the world coordinate system. Note that other transforms such as skew are not supported.
        /// To reset the volume while keeping the same world-volume transform, first get the current
        /// transform by calling GetCurrentWorldToVolumeTransform and pass this Matrix4 as the
        /// <paramref name="worldToVolumeTransform"/> parameter when calling this reset
        /// function.
        /// </summary>
        /// <param name="initialWorldToCameraTransform">
        /// The initial camera pose with respect to the world origin. 
        /// Pass identity as the default camera pose. 
        /// </param>
        /// <param name="worldToVolumeTransform">A  Matrix4 instance, containing the world to volume
        /// transform.
        /// </param>
        /// <exception cref="InvalidOperationException">
        /// Thrown when the Kinect Runtime could not be accessed, the device is not connected,
        /// or the call failed for an unknown reason.
        /// </exception>
        public void ResetReconstruction(
            Matrix4 initialWorldToCameraTransform, 
            Matrix4 worldToVolumeTransform)
        {
            ExceptionHelper.ThrowIfFailed(volume.ResetReconstruction(
                ref initialWorldToCameraTransform,
                ref worldToVolumeTransform));
        }

        /// <summary>
        /// Aligns a depth float image to the Reconstruction volume to calculate the new camera pose.
        /// This camera tracking method requires a Reconstruction volume, and updates the internal 
        /// camera pose if successful. The maximum image resolution supported in this function is 640x480.
        /// </summary>
        /// <param name="depthFloatFrame">The depth float frame to be processed.</param>
        /// <param name="maxAlignIterationCount">
        /// The maximum number of iterations of the algorithm to run. 
        /// The minimum value is 1. Using only a small number of iterations will have a faster runtime,
        /// however, the algorithm may not converge to the correct transformation.
        /// </param>
        /// <param name="deltaFromReferenceFrame">
        /// Optionally, a pre-allocated float image frame, to be filled with information about how
        /// well each observed pixel aligns with the passed in reference frame. This may be processed
        /// to create a color rendering, or may be used as input to additional vision algorithms such 
        /// as object segmentation. These residual values are normalized -1 to 1 and represent the 
        /// alignment cost/energy for each pixel. Larger magnitude values (either positive or negative)
        /// represent more discrepancy, and lower values represent less discrepancy or less information
        /// at that pixel.
        /// Note that if valid depth exists, but no reconstruction model exists behind the depth   
        /// pixels, 0 values indicating perfect alignment will be returned for that area. In contrast,
        /// where no valid depth occurs 1 values will always be returned. Pass null if not required.
        /// </param>
        /// <param name="alignmentEnergy">
        /// A float to receive a value describing how well the observed frame aligns to the model with
        /// the calculated pose. A larger magnitude value represent more discrepancy, and a lower value
        /// represent less discrepancy. Note that it is unlikely an exact 0 (perfect alignment) value 
        /// will ever be returned as every frame from the sensor will contain some sensor noise.
        /// </param>
        /// <param name="worldToCameraTransform">
        /// The best guess of the camera pose (usually the camera pose result from the last
        /// AlignPointClouds or AlignDepthFloatToReconstruction).
        /// </param>
        /// <returns>
        /// Returns true if successful; return false if the algorithm encountered a problem aligning
        /// the input depth image and could not calculate a valid transformation.
        /// </returns>
        /// <exception cref="ArgumentNullException">
        /// Thrown when the <paramref name="depthFloatFrame"/> parameter is null.
        /// </exception>
        /// <exception cref="ArgumentException">
        /// Thrown when the <paramref name="depthFloatFrame"/> parameter is an incorrect image size.
        /// Thrown when the <paramref name="maxAlignIterationCount"/> parameter is less than 1 or
        /// an incorrect value.
        /// </exception>
        /// <exception cref="InvalidOperationException">
        /// Thrown when the Kinect Runtime could not be accessed, the device is not connected
        /// or the call failed for an unknown reason.
        /// </exception>
        /// <remarks>
        /// Note that this function is designed primarily for tracking either with static scenes when
        /// performing environment reconstruction, or objects which move rigidly when performing object
        /// reconstruction from a static camera. Consider using the function AlignPointClouds instead 
        /// if tracking failures occur due to parts of a scene which move non-rigidly or should be 
        /// considered as outliers, although in practice, such issues are best avoided by carefully 
        /// designing or constraining usage scenarios wherever possible.
        /// </remarks> 
        public bool AlignDepthFloatToReconstruction(
            FusionFloatImageFrame depthFloatFrame,
            int maxAlignIterationCount,
            FusionFloatImageFrame deltaFromReferenceFrame,
            out float alignmentEnergy,
            Matrix4 worldToCameraTransform)
        {
            if (null == depthFloatFrame)
            {
                throw new ArgumentNullException("depthFloatFrame");
            }

            ushort maxIterations = ExceptionHelper.CastAndThrowIfOutOfUshortRange(maxAlignIterationCount);

            HRESULT hr = volume.AlignDepthFloatToReconstruction(
                FusionImageFrame.ToHandleRef(depthFloatFrame),
                maxIterations,
                FusionImageFrame.ToHandleRef(deltaFromReferenceFrame),
                out alignmentEnergy,
                ref worldToCameraTransform);

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

        /// <summary>
        /// Get current internal world-to-camera transform (camera view pose).
        /// </summary>
        /// <returns>The current world to camera pose.</returns>
        /// <exception cref="InvalidOperationException">
        /// Thrown when the call failed for an unknown reason.
        /// </exception>
        public Matrix4 GetCurrentWorldToCameraTransform()
        {
            Matrix4 cameraPose;
            ExceptionHelper.ThrowIfFailed(volume.GetCurrentWorldToCameraTransform(out cameraPose));

            return cameraPose;
        }

        /// <summary>
        /// Get current internal world-to-volume transform.
        /// Note: A right handed coordinate system is used, with the origin of the volume (i.e. voxel 0,0,0)
        /// at the top left of the front plane of the cube. Similar to bitmap images with top left origin, 
        /// +X is to the right, +Y down, and +Z is now forward from origin into the reconstruction volume.
        /// The default transform is a combination of translation in X,Y to locate the world origin at the
        /// center of the front face of the reconstruction volume cube (with the camera looking onto the
        /// volume along +Z), and scaling by the voxelsPerMeter reconstruction parameter to convert from
        /// world coordinate system to volume voxel indices.
        /// </summary>
        /// <returns>The current world to volume transform. This is a similarity transformation
        /// that converts world coordinates to volume coordinates.</returns>
        /// <exception cref="InvalidOperationException">
        /// Thrown when the call failed for an unknown reason.
        /// </exception>
        public Matrix4 GetCurrentWorldToVolumeTransform()
        {
            Matrix4 transform;
            ExceptionHelper.ThrowIfFailed(volume.GetCurrentWorldToVolumeTransform(out transform));

            return transform;
        }

        /// <summary>
        /// Integrates depth float data into the reconstruction volume from the passed
        /// camera pose.
        /// Note: this function will also set the internal camera pose.
        /// </summary>
        /// <param name="depthFloatFrame">The depth float frame to be integrated.</param>
        /// <param name="maxIntegrationWeight">
        /// A parameter to control the temporal smoothing of depth integration. Minimum value is 1.
        /// Lower values have more noisy representations, but objects that move integrate and 
        /// disintegrate faster, so are suitable for more dynamic environments. Higher values
        /// integrate objects more slowly, but provides finer detail with less noise.</param>
        /// <param name="worldToCameraTransform">
        /// The camera pose (usually the camera pose result from the last
        /// FusionDepthProcessor.AlignPointClouds or AlignDepthFloatToReconstruction).
        /// </param>
        /// <exception cref="ArgumentNullException">
        /// Thrown when the <paramref name="depthFloatFrame"/> parameter is null.
        /// </exception>
        /// <exception cref="ArgumentException">
        /// Thrown when the <paramref name="maxIntegrationWeight"/> parameter is less than 1 or
        /// greater than the maximum unsigned short value.
        /// </exception>
        /// <exception cref="InvalidOperationException">
        /// Thrown when the Kinect Runtime could not be accessed, the device is not connected
        /// or the call failed for an unknown reason.
        /// </exception>
        public void IntegrateFrame(
            FusionFloatImageFrame depthFloatFrame,
            int maxIntegrationWeight,
            Matrix4 worldToCameraTransform)
        {
            if (null == depthFloatFrame)
            {
                throw new ArgumentNullException("depthFloatFrame");
            }

            ushort integrationWeight = ExceptionHelper.CastAndThrowIfOutOfUshortRange(maxIntegrationWeight);

            ExceptionHelper.ThrowIfFailed(volume.IntegrateFrame(
                FusionImageFrame.ToHandleRef(depthFloatFrame),
                FusionImageFrame.ToHandleRef(null),
                integrationWeight,
                FusionDepthProcessor.DefaultColorIntegrationOfAllAngles,
                ref worldToCameraTransform));
        }

        /// <summary>
        /// Integrates depth float data and color data into the reconstruction volume from the
        /// passed camera pose. Here the angle parameter constrains the integration to
        /// integrate color over a given angle relative to the surface normal (recommended use 
        /// is for thin structure scanning).
        /// </summary>
        /// <param name="depthFloatFrame">The depth float frame to be integrated.</param>
        /// <param name="colorFrame">The color frame to be integrated.</param>
        /// <param name="maxIntegrationWeight">
        /// A parameter to control the temporal smoothing of depth integration. Minimum value is 1.
        /// Lower values have more noisy representations, but objects that move integrate and 
        /// disintegrate faster, so are suitable for more dynamic environments. Higher values
        /// integrate objects more slowly, but provides finer detail with less noise.</param>
        /// <param name="maxColorIntegrationAngle">An angle parameter in degrees to specify the angle
        /// with respect to the surface normal over which color will be integrated. This can be used so
        /// only when the camera sensor is near parallel with the surface (i.e. the camera direction of
        /// view is perpendicular to the surface), or  +/- an angle from the surface normal direction that
        /// color is integrated. 
        /// Pass FusionDepthProcessor.DefaultColorIntegrationOfAllAngles to ignore and accept color from
        /// all angles (default, fastest processing).
        /// This angle relative to this normal direction vector describe the acceptance half angle, for 
        /// example, a +/- 90 degree acceptance angle in all directions (i.e. a 180 degree hemisphere) 
        /// relative to the normal would integrate color in any orientation of the sensor towards the 
        /// front of the surface, even when parallel to the surface, whereas a 0 acceptance angle would
        /// only integrate color directly along a single ray exactly perpendicular to the surface.
        /// In reality, the useful range of values is actually between 0 and 90 exclusively 
        /// (e.g. setting +/- 60 degrees = 120 degrees total acceptance angle).
        /// Note that there is a trade-off here, as setting this has a runtime cost, however, conversely,
        /// ignoring this will integrate color from any angle over all voxels along camera rays around the
        /// zero crossing surface region in the volume, which can cause thin structures to have the same 
        /// color on both sides</param>
        /// <param name="worldToCameraTransform">
        /// The camera pose (usually the camera pose result from the last AlignPointClouds or 
        /// AlignDepthFloatToReconstruction).
        /// </param>
        /// <exception cref="ArgumentNullException">
        /// Thrown when the <paramref name="depthFloatFrame"/> or <paramref name="colorFrame"/> 
        /// parameter is null.</exception>
        /// <exception cref="ArgumentException">
        /// Thrown when the <paramref name="maxIntegrationWeight"/> parameter is less than 1 or
        /// greater than the maximum unsigned short value, or the 
        /// Thrown when the <paramref name="maxColorIntegrationAngle"/> parameter value is not
        /// FusionDepthProcessor.DefaultColorIntegrationOfAllAngles or between 0 and 90 degrees,
        /// exclusively.
        /// </exception>
        /// <exception cref="InvalidOperationException">
        /// Thrown when the Kinect Runtime could not be accessed, the device is not connected
        /// or the call failed for an unknown reason.
        /// </exception>
        public void IntegrateFrame(
            FusionFloatImageFrame depthFloatFrame,
            FusionColorImageFrame colorFrame,
            int maxIntegrationWeight,
            float maxColorIntegrationAngle,
            Matrix4 worldToCameraTransform)
        {
            if (null == depthFloatFrame)
            {
                throw new ArgumentNullException("depthFloatFrame");
            }

            if (null == colorFrame)
            {
                throw new ArgumentNullException("colorFrame");
            }

            ushort integrationWeight = ExceptionHelper.CastAndThrowIfOutOfUshortRange(maxIntegrationWeight);

            ExceptionHelper.ThrowIfFailed(volume.IntegrateFrame(
                FusionImageFrame.ToHandleRef(depthFloatFrame),
                FusionImageFrame.ToHandleRef(colorFrame),
                integrationWeight,
                maxColorIntegrationAngle,
                ref worldToCameraTransform));
        }

        /// <summary>
        /// A high-level function to process a depth frame through the Kinect Fusion pipeline.
        /// Specifically, this performs processing equivalent to the following functions for each frame:
        /// <para>
        /// 1) AlignDepthFloatToReconstruction
        /// 2) IntegrateFrame
        /// </para>
        /// If there is a tracking error in the AlignDepthFloatToReconstruction stage, no depth data 
        /// integration will be performed, and the camera pose will remain unchanged.
        /// The maximum image resolution supported in this function is 640x480.
        /// </summary>
        /// <param name="depthFloatFrame">The depth float frame to be processed.</param>
        /// <param name="maxAlignIterationCount">
        /// The maximum number of iterations of the align camera tracking algorithm to run.
        /// The minimum value is 1. Using only a small number of iterations will have a faster
        /// runtime, however, the algorithm may not converge to the correct transformation.
        /// </param>
        /// <param name="maxIntegrationWeight">
        /// A parameter to control the temporal smoothing of depth integration. Lower values have
        /// more noisy representations, but objects that move appear and disappear faster, so are
        /// suitable for more dynamic environments. Higher values integrate objects more slowly,
        /// but provides finer detail with less noise.
        /// </param>
        /// <param name="alignmentEnergy">
        /// A float to receive a value describing how well the observed frame aligns to the model with
        /// the calculated pose. A larger magnitude value represent more discrepancy, and a lower value
        /// represent less discrepancy. Note that it is unlikely an exact 0 (perfect alignment) value 
        /// will ever be returned as every frame from the sensor will contain some sensor noise.
        /// </param>
        /// <param name="worldToCameraTransform">
        /// The best guess of the latest camera pose (usually the camera pose result from the last
        /// process call).
        /// </param>
        /// <returns>
        /// Returns true if successful; return false if the algorithm encountered a problem aligning
        /// the input depth image and could not calculate a valid transformation.
        /// </returns>
        /// <exception cref="ArgumentNullException">
        /// Thrown when the <paramref name="depthFloatFrame"/> parameter is null.
        /// </exception>
        /// <exception cref="ArgumentException">
        /// Thrown when the <paramref name="depthFloatFrame"/> parameter is an incorrect image size.
        /// Thrown when the <paramref name="maxAlignIterationCount"/> parameter is less than 1 or
        /// greater than the maximum unsigned short value.
        /// Thrown when the <paramref name="maxIntegrationWeight"/> parameter is less than 1 or 
        /// greater than the maximum unsigned short value.
        /// </exception>
        /// <exception cref="InvalidOperationException">
        /// Thrown when the Kinect Runtime could not be accessed, the device is not connected,
        /// or the call failed for an unknown reason.
        /// </exception>
        /// <remarks>
        /// Users may also optionally call the low-level functions individually, instead of calling this
        /// function, for more control. However, this function call will be faster due to the integrated 
        /// nature of the calls. After this call completes, if a visible output image of the reconstruction
        /// is required, the user can call CalculatePointCloud and then FusionDepthProcessor.ShadePointCloud.
        /// </remarks>
        public bool ProcessFrame(
            FusionFloatImageFrame depthFloatFrame,
            int maxAlignIterationCount,
            int maxIntegrationWeight,
            out float alignmentEnergy,
            Matrix4 worldToCameraTransform)
        {
            if (null == depthFloatFrame)
            {
                throw new ArgumentNullException("depthFloatFrame");
            }

            ushort maxIterations = ExceptionHelper.CastAndThrowIfOutOfUshortRange(maxAlignIterationCount);
            ushort maxWeight = ExceptionHelper.CastAndThrowIfOutOfUshortRange(maxIntegrationWeight);

            HRESULT hr = volume.ProcessFrame(
                FusionImageFrame.ToHandleRef(depthFloatFrame),
                FusionImageFrame.ToHandleRef(null),
                maxIterations,
                maxWeight,
                FusionDepthProcessor.DefaultColorIntegrationOfAllAngles,
                out alignmentEnergy,
                ref worldToCameraTransform);

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

        /// <summary>
        /// A high-level function to process a depth frame through the Kinect Fusion pipeline.
        /// Also integrates color, further using a parameter to constrain the integration to
        /// integrate color over a given angle relative to the surface normal (recommended use 
        /// is for thin structure scanning).
        /// Specifically, this performs processing equivalent to the following functions for each frame:
        /// <para>
        /// 1) AlignDepthFloatToReconstruction
        /// 2) IntegrateFrame
        /// </para>
        /// If there is a tracking error in the AlignDepthFloatToReconstruction stage, no depth data 
        /// integration will be performed, and the camera pose will remain unchanged.
        /// The maximum image resolution supported in this function is 640x480.
        /// </summary>
        /// <param name="depthFloatFrame">The depth float frame to be processed.</param>
        /// <param name="colorFrame">The color frame to be processed.</param>
        /// <param name="maxAlignIterationCount">
        /// The maximum number of iterations of the align camera tracking algorithm to run.
        /// The minimum value is 1. Using only a small number of iterations will have a faster
        /// runtime, however, the algorithm may not converge to the correct transformation.
        /// </param>
        /// <param name="maxIntegrationWeight">
        /// A parameter to control the temporal smoothing of depth integration. Lower values have
        /// more noisy representations, but objects that move appear and disappear faster, so are
        /// suitable for more dynamic environments. Higher values integrate objects more slowly,
        /// but provides finer detail with less noise.
        /// </param>
        /// <param name="maxColorIntegrationAngle">An angle parameter in degrees to specify the angle
        /// with respect to the surface normal over which color will be integrated.This can be used so
        /// only when the camera sensor is near parallel with the surface (i.e. the camera direction of
        /// view is perpendicular to the surface), or  +/- an angle from the surface normal direction that
        /// color is integrated. 
        /// Pass FusionDepthProcessor.DefaultColorIntegrationOfAllAngles to ignore and accept color from
        /// all angles (default, fastest processing).
        /// This angle relative to this normal direction vector describe the acceptance half angle, for 
        /// example, a +/- 90 degree acceptance angle in all directions (i.e. a 180 degree hemisphere) 
        /// relative to the normal would integrate color in any orientation of the sensor towards the 
        /// front of the surface, even when parallel to the surface, whereas a 0 acceptance angle would
        /// only integrate color directly along a single ray exactly perpendicular to the surface.
        /// In reality, the useful range of values is actually between 0 and 90 exclusively 
        /// (e.g. setting +/- 60 degrees = 120 degrees total acceptance angle).
        /// Note that there is a trade-off here, as setting this has a runtime cost, however, conversely,
        /// ignoring this will integrate color from any angle over all voxels along camera rays around the
        /// zero crossing surface region in the volume, which can cause thin structures to have the same 
        /// color on both sides.</param>
        /// <param name="alignmentEnergy">
        /// A float to receive a value describing how well the observed frame aligns to the model with
        /// the calculated pose. A larger magnitude value represent more discrepancy, and a lower value
        /// represent less discrepancy. Note that it is unlikely an exact 0 (perfect alignment) value 
        /// will ever be returned as every frame from the sensor will contain some sensor noise.
        /// </param>
        /// <param name="worldToCameraTransform">
        /// The best guess of the latest camera pose (usually the camera pose result from the last
        /// process call).
        /// </param>
        /// <returns>
        /// Returns true if successful; return false if the algorithm encountered a problem aligning
        /// the input depth image and could not calculate a valid transformation.
        /// </returns>
        /// <exception cref="ArgumentNullException">
        /// Thrown when the <paramref name="depthFloatFrame"/> or <paramref name="colorFrame"/>  
        /// parameter is null. </exception>
        /// <exception cref="ArgumentException">
        /// Thrown when the <paramref name="depthFloatFrame"/> or <paramref name="colorFrame"/>  
        /// parameter is an incorrect image size.
        /// Thrown when the <paramref name="maxAlignIterationCount"/> parameter is less than 1 or
        /// greater than the maximum unsigned short value.
        /// Thrown when the <paramref name="maxIntegrationWeight"/> parameter is less than 1 or 
        /// greater than the maximum unsigned short value.
        /// Thrown when the <paramref name="maxColorIntegrationAngle"/> parameter value is not
        /// FusionDepthProcessor.DefaultColorIntegrationOfAllAngles or between 0 and 90 degrees, 
        /// exclusively.
        /// </exception>
        /// <exception cref="InvalidOperationException">
        /// Thrown when the Kinect Runtime could not be accessed, the device is not connected,
        /// or the call failed for an unknown reason.
        /// </exception>
        /// <remarks>
        /// Users may also optionally call the low-level functions individually, instead of calling this
        /// function, for more control. However, this function call will be faster due to the integrated 
        /// nature of the calls. After this call completes, if a visible output image of the reconstruction
        /// is required, the user can call CalculatePointCloud and then FusionDepthProcessor.ShadePointCloud.
        /// </remarks>
        public bool ProcessFrame(
            FusionFloatImageFrame depthFloatFrame,
            FusionColorImageFrame colorFrame,
            int maxAlignIterationCount,
            int maxIntegrationWeight,
            float maxColorIntegrationAngle,
            out float alignmentEnergy,
            Matrix4 worldToCameraTransform)
        {
            if (null == depthFloatFrame)
            {
                throw new ArgumentNullException("depthFloatFrame");
            }

            if (null == colorFrame)
            {
                throw new ArgumentNullException("colorFrame");
            }

            ushort maxIterations = ExceptionHelper.CastAndThrowIfOutOfUshortRange(maxAlignIterationCount);
            ushort maxWeight = ExceptionHelper.CastAndThrowIfOutOfUshortRange(maxIntegrationWeight);

            HRESULT hr = volume.ProcessFrame(
                FusionImageFrame.ToHandleRef(depthFloatFrame),
                FusionImageFrame.ToHandleRef(colorFrame),
                maxIterations,
                maxWeight,
                maxColorIntegrationAngle,
                out alignmentEnergy,
                ref worldToCameraTransform);

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

        /// <summary>
        /// Calculate a point cloud by raycasting into the reconstruction volume, returning the point
        /// cloud containing 3D points and normals of the zero-crossing dense surface at every visible
        /// pixel in the image from the given camera pose.
        /// This point cloud can be used as a reference frame in the next call to
        /// FusionDepthProcessor.AlignPointClouds, or passed to FusionDepthProcessor.ShadePointCloud
        /// to produce a visible image output.
        /// The <paramref name="pointCloudFrame"/> can be an arbitrary image size, for example, enabling
        /// you to calculate point clouds at the size of your window and then create a visible image by
        /// calling FusionDepthProcessor.ShadePointCloud and render this image, however, be aware that 
        /// large images will be expensive to calculate.
        /// </summary>
        /// <param name="pointCloudFrame">
        /// The pre-allocated point cloud frame, to be filled by raycasting into the reconstruction volume.
        /// Typically used as the reference frame with the FusionDepthProcessor.AlignPointClouds function
        /// or for visualization by calling FusionDepthProcessor.ShadePointCloud.
        /// </param>
        /// <param name="worldToCameraTransform">
        /// The world to camera transform (camera pose) to raycast from.
        /// </param>
        /// <exception cref="ArgumentNullException">
        /// Thrown when the <paramref name="pointCloudFrame"/> parameter is null.
        /// </exception>
        /// <exception cref="InvalidOperationException">
        /// Thrown when the call failed for an unknown reason.
        /// </exception>
        public void CalculatePointCloud(
            FusionPointCloudImageFrame pointCloudFrame,
            Matrix4 worldToCameraTransform)
        {
            if (null == pointCloudFrame)
            {
                throw new ArgumentNullException("pointCloudFrame");
            }

            ExceptionHelper.ThrowIfFailed(volume.CalculatePointCloud(
                FusionImageFrame.ToHandleRef(pointCloudFrame),
                FusionImageFrame.ToHandleRef(null),
                ref worldToCameraTransform));
        }

        /// <summary>
        /// Calculate a point cloud by raycasting into the reconstruction volume, returning the point
        /// cloud containing 3D points and normals of the zero-crossing dense surface at every visible
        /// pixel in the image from the given camera pose, and optionally the color visualization image.
        /// This point cloud can be used as a reference frame in the next call to
        /// FusionDepthProcessor.AlignPointClouds, or passed to FusionDepthProcessor.ShadePointCloud
        /// to produce a visible image output.
        /// The <paramref name="pointCloudFrame"/> can be an arbitrary image size, for example, enabling
        /// you to calculate point clouds at the size of your window and then create a visible image by
        /// calling FusionDepthProcessor.ShadePointCloud and render this image, however, be aware that 
        /// large images will be expensive to calculate.
        /// </summary>
        /// <param name="pointCloudFrame">
        /// The pre-allocated point cloud frame, to be filled by raycasting into the reconstruction volume.
        /// Typically used as the reference frame with the FusionDepthProcessor.AlignPointClouds function
        /// or for visualization by calling FusionDepthProcessor.ShadePointCloud.
        /// </param>
        /// <param name="colorFrame">Optionally, the color frame to fill. Pass null to ignore.</param>
        /// <param name="worldToCameraTransform">
        /// The world to camera transform (camera pose) to raycast from.
        /// </param>
        /// <exception cref="ArgumentNullException">
        /// Thrown when the <paramref name="pointCloudFrame"/> parameter is null. </exception>
        /// <exception cref="InvalidOperationException">
        /// Thrown when the call failed for an unknown reason.
        /// </exception>
        public void CalculatePointCloud(
            FusionPointCloudImageFrame pointCloudFrame,
            FusionColorImageFrame colorFrame,
            Matrix4 worldToCameraTransform)
        {
            if (null == pointCloudFrame)
            {
                throw new ArgumentNullException("pointCloudFrame");
            }

            if (null == colorFrame)
            {
                throw new ArgumentNullException("colorFrame");
            }

            ExceptionHelper.ThrowIfFailed(volume.CalculatePointCloud(
                FusionImageFrame.ToHandleRef(pointCloudFrame),
                FusionImageFrame.ToHandleRef(colorFrame),
                ref worldToCameraTransform));
        }

        /// <summary>
        /// Export a polygon mesh of the zero-crossing dense surfaces from the reconstruction volume
        /// with per-vertex color.
        /// </summary>
        /// <param name="voxelStep">
        /// The step value in voxels for sampling points to use in the volume when exporting a mesh, which
        /// determines the final resolution of the mesh. Use higher values for lower resolution meshes. 
        /// voxelStep must be greater than 0 and smaller than the smallest volume axis voxel resolution. 
        /// To mesh the volume at its full resolution, use a step value of 1.
        /// Note: Any value higher than 1 for this parameter runs the risk of missing zero crossings, and
        /// hence missing surfaces or surface details.
        /// </param>
        /// <returns>Returns the mesh object created by Kinect Fusion.</returns>
        /// <exception cref="ArgumentException">
        /// Thrown when the <paramref name="voxelStep"/> parameter is less than 1 or 
        /// greater than the maximum unsigned int value or the smallest volume axis resolution.
        /// </exception>
        /// <exception cref="OutOfMemoryException">
        /// Thrown if the CPU memory required for mesh calculation could not be allocated.
        /// </exception>
        /// <exception cref="InvalidOperationException">
        /// Thrown when the Kinect Runtime could not be accessed, the device is not connected,
        /// a GPU memory allocation failed or the call failed for an unknown reason.
        /// </exception>
        public ColorMesh CalculateMesh(int voxelStep)
        {
            INuiFusionColorMesh mesh = null;

            uint step = ExceptionHelper.CastAndThrowIfOutOfUintRange(voxelStep);

            ExceptionHelper.ThrowIfFailed(volume.CalculateMesh(step, out mesh));

            return new ColorMesh(mesh);
        }

        /// <summary>
        /// Export a part or all of the reconstruction volume as a short array. 
        /// The surface boundary occurs where the tri-linearly interpolated voxel values have a zero crossing
        /// (i.e. when an interpolation crosses from positive to negative or vice versa). A voxel value of 
        /// 0x8000 indicates that a voxel is uninitialized and has no valid data associated with it.
        /// </summary>
        /// <param name="sourceOriginX">The reconstruction volume voxel index in the X axis from which the 
        /// extraction should begin. This value must be greater than or equal to 0 and less than the
        /// reconstruction volume X axis voxel resolution.</param>
        /// <param name="sourceOriginY">The reconstruction volume voxel index in the Y axis from which the 
        /// extraction should begin. This value must be greater than or equal to 0 and less than the
        /// reconstruction volume Y axis voxel resolution.</param>
        /// <param name="sourceOriginZ">The reconstruction volume voxel index in the Z axis from which the 
        /// extraction should begin. This value must be greater than or equal to 0 and less than the
        /// reconstruction volume Z axis voxel resolution.</param>
        /// <param name="destinationResolutionX">The X axis resolution/width of the new voxel volume to return
        /// in the array. This value must be greater than 0 and less than or equal to the current volume X 
        /// axis voxel resolution. The final count of (sourceOriginX+(destinationResolutionX*voxelStep) must 
        /// not be greater than the current reconstruction volume X axis voxel resolution.</param>
        /// <param name="destinationResolutionY">The Y axis resolution/height of the new voxel volume to return
        /// in the array. This value must be greater than 0 and less than or equal to the current volume Y 
        /// axis voxel resolution. The final count of (sourceOriginY+(destinationResolutionY*voxelStep) must 
        /// not be greater than the current reconstruction volume Y axis voxel resolution.</param>
        /// <param name="destinationResolutionZ">The Z axis resolution/depth of the new voxel volume to return
        /// in the array. This value must be greater than 0 and less than or equal to the current volume Z 
        /// axis voxel resolution. The final count of (sourceOriginZ+(destinationResolutionZ*voxelStep) must 
        /// not be greater than the current reconstruction volume Z axis voxel resolution.</param>
        /// <param name="voxelStep">The step value in integer voxels for sampling points to use in the
        /// volume when exporting. The value must be greater than 0 and less than the smallest 
        /// volume axis voxel resolution. To export the volume at its full resolution, use a step value of 1. 
        /// Use higher step values to skip voxels and return the new volume as if there were a lower effective 
        /// resolution volume. For example, when exporting with a destination resolution of 320^3, setting 
        /// voxelStep to 2 would actually cover a 640^3 voxel are a(destinationResolution*voxelStep) in the 
        /// source reconstruction, but the data returned would skip every other voxel in the original volume.
        /// NOTE:  Any value higher than 1 for this value runs the risk of missing zero crossings, and hence
        /// missing surfaces or surface details.</param>
        /// <param name="volumeBlock">A pre-allocated short array to be filled with 
        /// volume data. The number of elements in this user array should be allocated as:
        /// (destinationResolutionX * destinationResolutionY * destinationResolutionZ) 
        /// To access the voxel located at x,y,z use pVolume[z][y][x], or index as 1D array for a particular
        /// voxel(x,y,z) as follows: with pitch = x resolution, slice = (y resolution * pitch)
        /// unsigned int index = (z * slice)  + (y * pitch) + x;
        /// Note: A right handed coordinate system is used, with the origin of the volume (i.e. voxel 0,0,0) 
        /// at the top left of the front plane of the cube. Similar to bitmap images with top left origin, 
        /// +X is to the right, +Y down, and +Z is forward from origin into the reconstruction volume.
        /// </param>
        /// <exception cref="ArgumentNullException">
        /// Thrown when the <paramref name="volumeBlock"/> parameter is null.
        /// </exception>
        /// <exception cref="ArgumentException">
        /// Thrown when the <paramref name="volumeBlock"/> parameter length is not equal to
        /// (<paramref name="destinationResolutionX"/> * <paramref name="destinationResolutionY"/> *
        /// <paramref name="destinationResolutionZ"/>).
        /// Thrown when a sourceOrigin or destinationResolution parameter less than 1 or
        /// greater than the maximum unsigned short value.
        /// Thrown when the (sourceOrigin+(destinationResolution*voxelStep) calculation was
        /// greater than the current reconstruction volume voxel resolution along an axis.
        /// </exception>
        /// <exception cref="OutOfMemoryException">
        /// Thrown if the CPU memory required for volume export could not be allocated.
        /// </exception>
        /// <exception cref="InvalidOperationException">
        /// Thrown when the Kinect Runtime could not be accessed, the device is not connected,
        /// a GPU memory allocation failed or the call failed for an unknown reason.
        /// </exception>
        public void ExportVolumeBlock(
            int sourceOriginX,
            int sourceOriginY,
            int sourceOriginZ,
            int destinationResolutionX,
            int destinationResolutionY,
            int destinationResolutionZ,
            int voxelStep,
            short[] volumeBlock)
        {
            if (null == volumeBlock)
            {
                throw new ArgumentNullException("volumeBlock");
            }
            
            uint srcX = ExceptionHelper.CastAndThrowIfOutOfUintRange(sourceOriginX);
            uint srcY = ExceptionHelper.CastAndThrowIfOutOfUintRange(sourceOriginY);
            uint srcZ = ExceptionHelper.CastAndThrowIfOutOfUintRange(sourceOriginZ);

            uint destX = ExceptionHelper.CastAndThrowIfOutOfUintRange(destinationResolutionX);
            uint destY = ExceptionHelper.CastAndThrowIfOutOfUintRange(destinationResolutionY);
            uint destZ = ExceptionHelper.CastAndThrowIfOutOfUintRange(destinationResolutionZ);

            uint step = ExceptionHelper.CastAndThrowIfOutOfUintRange(voxelStep);

            if (volumeBlock.Length != (destX * destY * destZ))
            {
                throw new ArgumentException("volumeBlock");
            }

            ExceptionHelper.ThrowIfFailed(volume.ExportVolumeBlock(srcX, srcY, srcZ, destX, destY, destZ, step, (uint)volumeBlock.Length * sizeof(short), 0, volumeBlock, null));
        }

        /// <summary>
        /// Export a part or all of the reconstruction volume as a short array with color as int array. 
        /// The surface boundary occurs where the tri-linearly interpolated voxel values have a zero crossing
        /// (i.e. when an interpolation crosses from positive to negative or vice versa). A voxel value of 
        /// 0x8000 indicates that a voxel is uninitialized and has no valid data associated with it.
        /// </summary>
        /// <param name="sourceOriginX">The reconstruction volume voxel index in the X axis from which the 
        /// extraction should begin. This value must be greater than or equal to 0 and less than the
        /// reconstruction volume X axis voxel resolution.</param>
        /// <param name="sourceOriginY">The reconstruction volume voxel index in the Y axis from which the 
        /// extraction should begin. This value must be greater than or equal to 0 and less than the
        /// reconstruction volume Y axis voxel resolution.</param>
        /// <param name="sourceOriginZ">The reconstruction volume voxel index in the Z axis from which the 
        /// extraction should begin. This value must be greater than or equal to 0 and less than the
        /// reconstruction volume Z axis voxel resolution.</param>
        /// <param name="destinationResolutionX">The X axis resolution/width of the new voxel volume to return
        /// in the array. This value must be greater than 0 and less than or equal to the current volume X 
        /// axis voxel resolution. The final count of (sourceOriginX+(destinationResolutionX*voxelStep) must 
        /// not be greater than the current reconstruction volume X axis voxel resolution.</param>
        /// <param name="destinationResolutionY">The Y axis resolution/height of the new voxel volume to return
        /// in the array. This value must be greater than 0 and less than or equal to the current volume Y 
        /// axis voxel resolution. The final count of (sourceOriginY+(destinationResolutionY*voxelStep) must 
        /// not be greater than the current reconstruction volume Y axis voxel resolution.</param>
        /// <param name="destinationResolutionZ">The Z axis resolution/depth of the new voxel volume to return
        /// in the array. This value must be greater than 0 and less than or equal to the current volume Z 
        /// axis voxel resolution. The final count of (sourceOriginZ+(destinationResolutionZ*voxelStep) must 
        /// not be greater than the current reconstruction volume Z axis voxel resolution.</param>
        /// <param name="voxelStep">The step value in integer voxels for sampling points to use in the
        /// volume when exporting. The value must be greater than 0 and less than the smallest 
        /// volume axis voxel resolution. To export the volume at its full resolution, use a step value of 1. 
        /// Use higher step values to skip voxels and return the new volume as if there were a lower effective 
        /// resolution volume. For example, when exporting with a destination resolution of 320^3, setting 
        /// voxelStep to 2 would actually cover a 640^3 voxel are a(destinationResolution*voxelStep) in the 
        /// source reconstruction, but the data returned would skip every other voxel in the original volume.
        /// NOTE:  Any value higher than 1 for this value runs the risk of missing zero crossings, and hence
        /// missing surfaces or surface details.</param>
        /// <param name="volumeBlock">A pre-allocated short array to be filled with 
        /// volume data. The number of elements in this user array should be allocated as:
        /// (destinationResolutionX * destinationResolutionY * destinationResolutionZ) 
        /// To access the voxel located at x,y,z use pVolume[z][y][x], or index as 1D array for a particular
        /// voxel(x,y,z) as follows: with pitch = x resolution, slice = (y resolution * pitch)
        /// unsigned int index = (z * slice)  + (y * pitch) + x;
        /// Note: A right handed coordinate system is used, with the origin of the volume (i.e. voxel 0,0,0) 
        /// at the top left of the front plane of the cube. Similar to bitmap images with top left origin, 
        /// +X is to the right, +Y down, and +Z is forward from origin into the reconstruction volume.
        /// </param>
        /// <param name="colorVolumeBlock">A pre-allocated int array filled with color volume data.
        /// The number of elements must be identical to those in <paramref name="volumeBlock"/>. 
        /// </param>
        /// <exception cref="ArgumentNullException">
        /// Thrown when the <paramref name="volumeBlock"/> or <paramref name="colorVolumeBlock"/> parameter 
        /// is null.
        /// </exception>
        /// <exception cref="ArgumentException">
        /// Thrown when the <paramref name="volumeBlock"/> or <paramref name="colorVolumeBlock"/> parameter
        /// length is not equal to:
        /// (<paramref name="destinationResolutionX"/> * <paramref name="destinationResolutionY"/> *
        /// <paramref name="destinationResolutionZ"/>).
        /// Thrown when a sourceOrigin or destinationResolution parameter less than 1 or
        /// greater than the maximum unsigned short value.
        /// Thrown when the (sourceOrigin+(destinationResolution*voxelStep) calculation was
        /// greater than the current reconstruction volume voxel resolution along an axis.
        /// </exception>
        /// <exception cref="OutOfMemoryException">
        /// Thrown if the CPU memory required for volume export could not be allocated.
        /// </exception>
        /// <exception cref="InvalidOperationException">
        /// Thrown when the Kinect Runtime could not be accessed, the device is not connected,
        /// a GPU memory allocation failed or the call failed for an unknown reason.
        /// </exception>
        public void ExportVolumeBlock(
            int sourceOriginX,
            int sourceOriginY,
            int sourceOriginZ,
            int destinationResolutionX,
            int destinationResolutionY,
            int destinationResolutionZ,
            int voxelStep,
            short[] volumeBlock,
            int[] colorVolumeBlock)
        {
            if (null == volumeBlock)
            {
                throw new ArgumentNullException("volumeBlock");
            }

            if (null == colorVolumeBlock)
            {
                throw new ArgumentNullException("colorVolumeBlock");
            }

            uint srcX = ExceptionHelper.CastAndThrowIfOutOfUintRange(sourceOriginX);
            uint srcY = ExceptionHelper.CastAndThrowIfOutOfUintRange(sourceOriginY);
            uint srcZ = ExceptionHelper.CastAndThrowIfOutOfUintRange(sourceOriginZ);

            uint destX = ExceptionHelper.CastAndThrowIfOutOfUintRange(destinationResolutionX);
            uint destY = ExceptionHelper.CastAndThrowIfOutOfUintRange(destinationResolutionY);
            uint destZ = ExceptionHelper.CastAndThrowIfOutOfUintRange(destinationResolutionZ);

            uint step = ExceptionHelper.CastAndThrowIfOutOfUintRange(voxelStep);

            if (volumeBlock.Length != (destX * destY * destZ))
            {
                throw new ArgumentException("volumeBlock");
            }

            if (colorVolumeBlock.Length != (destX * destY * destZ))
            {
                throw new ArgumentException("colorVolumeBlock");
            }

            ExceptionHelper.ThrowIfFailed(volume.ExportVolumeBlock(srcX, srcY, srcZ, destX, destY, destZ, step, (uint)volumeBlock.Length * sizeof(short), (uint)colorVolumeBlock.Length * sizeof(int), volumeBlock, colorVolumeBlock));
        }

        /// <summary>
        /// Import a reconstruction volume as a short array. 
        /// This array must equal the size of the current initialized reconstruction volume.
        /// </summary>
        /// <param name="volumeBlock">A pre-allocated short array filled with volume data.
        /// The number of elements in this user array should be allocated as:
        /// (sourceResolutionX * sourceResolutionY * sourceResolutionZ) 
        /// To access the voxel located at x,y,z use pVolume[z][y][x], or index as 1D array for a particular
        /// voxel(x,y,z) as follows: with pitch = x resolution, slice = (y resolution * pitch)
        /// unsigned int index = (z * slice)  + (y * pitch) + x;
        /// Note: A right handed coordinate system is used, with the origin of the volume (i.e. voxel 0,0,0) 
        /// at the top left of the front plane of the cube. Similar to bitmap images with top left origin, 
        /// +X is to the right, +Y down, and +Z is forward from origin into the reconstruction volume.
        /// </param>
        /// <exception cref="ArgumentNullException">
        /// Thrown when the <paramref name="volumeBlock"/> parameter is null.
        /// </exception>
        /// <exception cref="ArgumentException">
        /// Thrown when the <paramref name="volumeBlock"/> parameter length is not equal to
        /// the existing initialized volume.
        /// </exception>
        /// <exception cref="OutOfMemoryException">
        /// Thrown if the CPU memory required for volume export could not be allocated.
        /// </exception>
        /// <exception cref="InvalidOperationException">
        /// Thrown when the Kinect Runtime could not be accessed, the device is not connected,
        /// a GPU memory allocation failed or the call failed for an unknown reason.
        /// </exception>
        public void ImportVolumeBlock(
            short[] volumeBlock)
        {
            if (null == volumeBlock)
            {
                throw new ArgumentNullException("volumeBlock");
            }

            ExceptionHelper.ThrowIfFailed(volume.ImportVolumeBlock((uint)volumeBlock.Length * sizeof(short), 0, volumeBlock, null));
        }

        /// <summary>
        /// Import a reconstruction volume as a short array with color as int array. 
        /// This arrays must both have the same number of elements and this must be equal to the size of the
        /// current initialized reconstruction volume.
        /// </summary>
        /// <param name="volumeBlock">A pre-allocated short array filled with volume data.
        /// The number of elements in this user array should be allocated as:
        /// (sourceResolutionX * sourceResolutionY * sourceResolutionZ) 
        /// To access the voxel located at x,y,z use pVolume[z][y][x], or index as 1D array for a particular
        /// voxel(x,y,z) as follows: with pitch = x resolution, slice = (y resolution * pitch)
        /// unsigned int index = (z * slice)  + (y * pitch) + x;
        /// Note: A right handed coordinate system is used, with the origin of the volume (i.e. voxel 0,0,0) 
        /// at the top left of the front plane of the cube. Similar to bitmap images with top left origin, 
        /// +X is to the right, +Y down, and +Z is forward from origin into the reconstruction volume.
        /// </param>
        /// <param name="colorVolumeBlock">A pre-allocated int array filled with color volume data.
        /// The number of elements must be identical to those in <paramref name="volumeBlock"/>. 
        /// </param>
        /// <exception cref="ArgumentNullException">
        /// Thrown when the <paramref name="volumeBlock"/> or <paramref name="colorVolumeBlock"/> parameter
        ///  is null.
        /// </exception>
        /// <exception cref="ArgumentException">
        /// Thrown when the <paramref name="volumeBlock"/> or <paramref name="colorVolumeBlock"/> parameter 
        /// length is not equal to the existing initialized volume or the two array lengths are different.
        /// </exception>
        /// <exception cref="OutOfMemoryException">
        /// Thrown if the CPU memory required for volume export could not be allocated.
        /// </exception>
        /// <exception cref="InvalidOperationException">
        /// Thrown when the Kinect Runtime could not be accessed, the device is not connected,
        /// a GPU memory allocation failed or the call failed for an unknown reason.
        /// </exception>
        public void ImportVolumeBlock(
            short[] volumeBlock,
            int[] colorVolumeBlock)
        {
            if (null == volumeBlock)
            {
                throw new ArgumentNullException("volumeBlock");
            }

            if (null == colorVolumeBlock)
            {
                throw new ArgumentNullException("colorVolumeBlock");
            }

            if (volumeBlock.Length != colorVolumeBlock.Length)
            {
                throw new ArgumentException("volumeBlock.Length != colorVolumeBlock.Length");
            }

            ExceptionHelper.ThrowIfFailed(volume.ImportVolumeBlock((uint)volumeBlock.Length * sizeof(short), (uint)colorVolumeBlock.Length * sizeof(int), volumeBlock, colorVolumeBlock));
        }

        /// <summary>
        /// Converts Kinect depth frames in unsigned short format to depth frames in float format 
        /// representing distance from the camera in meters (parallel to the optical center axis).
        /// Note: <paramref name="depthImageData"/> and <paramref name="depthFloatFrame"/> must
        /// be the same pixel resolution. This version of the function runs on the GPU.
        /// </summary>
        /// <param name="depthImageData">The source depth data.</param>
        /// <param name="depthFloatFrame">A depth float frame, to be filled with depth.</param>
        /// <param name="minDepthClip">The minimum depth threshold. Values below this will be set to 0.</param>
        /// <param name="maxDepthClip">The maximum depth threshold. Values above this will be set to 1000.</param>
        /// <param name="mirrorDepth">Set true to mirror depth, false so the image appears correct if viewing
        /// the Kinect camera from behind.</param>
        /// <exception cref="ArgumentNullException">
        /// Thrown when the <paramref name="depthImageData"/> or 
        /// <paramref name="depthFloatFrame"/> parameter is null.
        /// </exception>
        /// <exception cref="ArgumentException">
        /// Thrown when the <paramref name="depthImageData"/> or
        /// <paramref name="depthFloatFrame"/> parameter is an incorrect image size, or the 
        /// kernelWidth is an incorrect size.
        /// </exception>
        /// <exception cref="InvalidOperationException">
        /// Thrown when the Kinect Runtime could not be accessed, the device is not connected
        /// or the call failed for an unknown reason.
        /// </exception>
        /// <remarks>
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
        /// </remarks>
#pragma warning disable 3001
        public void DepthToDepthFloatFrame(
            ushort[] depthImageData,
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

            ExceptionHelper.ThrowIfFailed(volume.DepthToDepthFloatFrame(
                depthImageData,
                (uint)depthImageData.Length * sizeof(short),
                FusionImageFrame.ToHandleRef(depthFloatFrame),
                minDepthClip,
                maxDepthClip,
                mirrorDepth));
        }
#pragma warning restore 3001

        /// <summary>
        /// Spatially smooth a depth float image frame using edge-preserving filtering on GPU. 
        /// </summary>
        /// <param name="depthFloatFrame">A source depth float frame.</param>
        /// <param name="smoothDepthFloatFrame">A depth float frame, to be filled with smoothed 
        /// depth.</param>
        /// <param name="kernelWidth">Smoothing Kernel Width. Valid values are  1,2,3 
        /// (for 3x3,5x5,7x7 smoothing kernel block size respectively).</param>
        /// <param name="distanceThreshold">A distance difference range that smoothing occurs in.
        /// Pixels with neighboring pixels outside this distance range will not be smoothed 
        /// (larger values indicate discontinuity/edge). Must be greater than 0.</param>
        /// <exception cref="ArgumentNullException">
        /// Thrown when the <paramref name="smoothDepthFloatFrame"/> or 
        /// <paramref name="depthFloatFrame"/> parameter is null.
        /// </exception>
        /// <exception cref="ArgumentException">
        /// Thrown when the <paramref name="smoothDepthFloatFrame"/> or
        /// <paramref name="depthFloatFrame"/> parameter is an incorrect image size, or the 
        /// kernelWidth is an incorrect size.
        /// </exception>
        /// <exception cref="InvalidOperationException">
        /// Thrown when the Kinect Runtime could not be accessed, the device is not connected
        /// or the call failed for an unknown reason.
        /// </exception>
        public void SmoothDepthFloatFrame(
            FusionFloatImageFrame depthFloatFrame,
            FusionFloatImageFrame smoothDepthFloatFrame,
            int kernelWidth,
            float distanceThreshold)
        {
            if (null == smoothDepthFloatFrame)
            {
                throw new ArgumentNullException("smoothDepthFloatFrame");
            }

            if (null == depthFloatFrame)
            {
                throw new ArgumentNullException("depthFloatFrame");
            }

            uint kW = ExceptionHelper.CastAndThrowIfOutOfUintRange(kernelWidth);

            ExceptionHelper.ThrowIfFailed(volume.SmoothDepthFloatFrame(
                FusionImageFrame.ToHandleRef(depthFloatFrame),
                FusionImageFrame.ToHandleRef(smoothDepthFloatFrame),
                kW,
                distanceThreshold));
        }

        /// <summary>
        /// The AlignPointClouds function uses an on GPU iterative algorithm to align two sets of  
        /// overlapping oriented point clouds and calculate the camera's relative pose.
        /// All images must be the same size and have the same camera parameters. 
        /// </summary>
        /// <param name="referencePointCloudFrame">A reference point cloud frame.</param>
        /// <param name="observedPointCloudFrame">An observerd point cloud frame.</param>
        /// <param name="maxAlignIterationCount">The number of iterations to run.</param>
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
        /// <param name="alignmentEnergy">A value describing
        /// how well the observed frame aligns to the model with the calculated pose (mean distance between
        /// matching points in the point clouds). A larger magnitude value represent more discrepancy, and 
        /// a lower value represent less discrepancy. Note that it is unlikely an exact 0 (perfect alignment) 
        /// value will ever/ be returned as every frame from the sensor will contain some sensor noise. 
        /// Pass NULL to ignore this parameter.</param>
        /// <param name="referenceToObservedTransform">The initial guess at the transform. This is 
        /// updated on tracking success, or returned as identity on failure.</param>
        /// <returns>
        /// Returns true if successful; return false if the algorithm encountered a problem aligning
        /// the input depth image and could not calculate a valid transformation.
        /// </returns>
        /// <exception cref="ArgumentNullException">
        /// Thrown when the <paramref name="referencePointCloudFrame"/> or 
        /// <paramref name="observedPointCloudFrame"/> parameter is null.
        /// </exception>
        /// <exception cref="ArgumentException">
        /// Thrown when the <paramref name="referencePointCloudFrame"/> or
        /// <paramref name="observedPointCloudFrame"/> or <paramref name="deltaFromReferenceFrame"/>
        /// parameter is an incorrect image size, or the iterations parameter is not greater than 0.
        /// </exception>
        /// <exception cref="InvalidOperationException">
        /// Thrown when the Kinect Runtime could not be accessed, the device is not connected
        /// or the call failed for an unknown reason.
        /// </exception>
        public bool AlignPointClouds(
            FusionPointCloudImageFrame referencePointCloudFrame,
            FusionPointCloudImageFrame observedPointCloudFrame,
            int maxAlignIterationCount,
            FusionColorImageFrame deltaFromReferenceFrame,
            out float alignmentEnergy,
            ref Matrix4 referenceToObservedTransform)
        {
            if (null == referencePointCloudFrame)
            {
                throw new ArgumentNullException("referencePointCloudFrame");
            }

            if (null == observedPointCloudFrame)
            {
                throw new ArgumentNullException("observedPointCloudFrame");
            }

            ushort iterations = ExceptionHelper.CastAndThrowIfOutOfUshortRange(maxAlignIterationCount);

            HRESULT hr = volume.AlignPointClouds(
                FusionImageFrame.ToHandleRef(referencePointCloudFrame),
                FusionImageFrame.ToHandleRef(observedPointCloudFrame),
                iterations,
                FusionImageFrame.ToHandleRef(deltaFromReferenceFrame),
                out alignmentEnergy,
                ref referenceToObservedTransform);

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

        /// <summary>
        /// Set a reference depth frame to be used internally to help with tracking when calling 
        /// AlignDepthFloatToReconstruction to calculate a new camera pose. This function should
        /// only be called when not using the default tracking behavior of Kinect Fusion.
        /// </summary>
        /// <remarks>
        /// AlignDepthFloatToReconstruction internally saves the last depth frame it was passed and
        /// uses this image to help it track when called the next time. For example, this can be used
        /// if you are reconstructing and lose track, then want to re-start tracking from a different
        /// (known) location without resetting the volume. To enable the tracking to succeed you
        /// could perform a raycast from the new location to get a depth image (by calling 
        /// CalculatePointCloudAndDepth) then call this set function with the depth image, before 
        /// calling AlignDepthFloatToReconstruction.
        /// </remarks>
        /// <param name="referenceDepthFloatFrame">A previous depth float frame where align was
        /// successful (and hence same functionality as AlignDepthFloatToReconstruction), 
        /// or a ray-casted model depth.</param>
        /// <exception cref="ArgumentNullException">
        /// Thrown when the <paramref name="referenceDepthFloatFrame"/> parameter is null.
        /// </exception>
        /// <exception cref="ArgumentException">
        /// Thrown when the <paramref name="referenceDepthFloatFrame"/> parameter is an incorrect 
        /// image size.</exception>
        /// <exception cref="InvalidOperationException">
        /// Thrown when the call failed for an unknown reason.
        /// </exception>
        public void SetAlignDepthFloatToReconstructionReferenceFrame(
            FusionFloatImageFrame referenceDepthFloatFrame)
        {
            if (null == referenceDepthFloatFrame)
            {
                throw new ArgumentNullException("referenceDepthFloatFrame");
            }

            ExceptionHelper.ThrowIfFailed(volume.SetAlignDepthFloatToReconstructionReferenceFrame(
                FusionImageFrame.ToHandleRef(referenceDepthFloatFrame)));
        }

        /// <summary>
        /// Calculate a point cloud, depth and optionally color image by raycasting into the 
        /// reconstruction volume. This returns the point cloud containing 3D points and normals of the 
        /// zero-crossing dense surface at every visible pixel in the image from the given camera pose,
        /// the depth to the surface, and optionally the color visualization image.
        /// </summary>
        /// <param name="pointCloudFrame">A point cloud frame, to be filled by raycasting into the 
        /// reconstruction volume. Typically used as the reference frame with the 
        /// FusionDepthProcessor.AlignPointClouds function or for visualization by calling 
        /// FusionDepthProcessor.ShadePointCloud.</param>
        /// <param name="depthFloatFrame">A floating point depth frame, to be filled with floating point
        /// depth in meters to the raycast surface. This image must be identical in size, and camera 
        /// parameters to the <paramref name="pointCloudFrame"/> parameter.</param>
        /// <param name="colorFrame">Optionally, the color frame to fill. Pass null to ignore.</param>
        /// <param name="worldToCameraTransform">The world-to-camera transform (camera pose) to 
        /// raycast from.</param>
        /// <exception cref="ArgumentNullException">
        /// Thrown when the <paramref name="pointCloudFrame"/> or 
        /// <paramref name="depthFloatFrame"/> parameter is null.
        /// </exception>
        /// <exception cref="ArgumentException">
        /// Thrown when the <paramref name="pointCloudFrame"/> or <paramref name="depthFloatFrame"/> or
        /// <paramref name="colorFrame"/> parameter is an incorrect image size.
        /// </exception>
        /// <exception cref="InvalidOperationException">
        /// Thrown when the Kinect Runtime could not be accessed, the device is not connected
        /// or the call failed for an unknown reason.
        /// </exception>
        /// <remarks>
        /// This point cloud can then be used as a reference frame in the next call to 
        /// FusionDepthProcessor.AlignPointClouds, or passed to FusionDepthProcessor.ShadePointCloud
        /// to produce a visible image output.The depth image can be used as a reference frame for
        /// AlignDepthFloatToReconstruction by calling SetAlignDepthFloatToReconstructionReferenceFrame 
        /// to enable a greater range of tracking. The <paramref name="pointCloudFrame"/> and 
        /// <paramref name="depthFloatFrame"/> parameters can be an arbitrary image size, for example, 
        /// enabling you to calculate point clouds at the size of your UI window and then create a visible
        /// image by calling FusionDepthProcessor.ShadePointCloud and render this image, however, be aware
        /// that the calculation of high resolution images will be expensive in terms of runtime.
        /// </remarks>
        public void CalculatePointCloudAndDepth(
            FusionPointCloudImageFrame pointCloudFrame,
            FusionFloatImageFrame depthFloatFrame,
            FusionColorImageFrame colorFrame,
            Matrix4 worldToCameraTransform)
        {
            if (null == pointCloudFrame)
            {
                throw new ArgumentNullException("pointCloudFrame");
            }

            if (null == depthFloatFrame)
            {
                throw new ArgumentNullException("depthFloatFrame");
            }

            ExceptionHelper.ThrowIfFailed(volume.CalculatePointCloudAndDepth(
                FusionImageFrame.ToHandleRef(pointCloudFrame),
                FusionImageFrame.ToHandleRef(depthFloatFrame),
                FusionImageFrame.ToHandleRef(colorFrame),
                ref worldToCameraTransform));
        }

        /// <summary>
        /// Disposes the Reconstruction.
        /// </summary>
        public void Dispose()
        {
            Dispose(true);

            // This object will be cleaned up by the Dispose method.
            GC.SuppressFinalize(this);
        }

        /// <summary>
        /// Frees all memory associated with the Reconstruction.
        /// </summary>
        /// <param name="disposing">Whether the function was called from Dispose.</param>
        protected virtual void Dispose(bool disposing)
        {
            if (!this.disposed)
            {
                Marshal.FinalReleaseComObject(volume);
                disposed = true;
            }
        }
    }
}
