//-----------------------------------------------------------------------
// <copyright file="CameraPoseFinder.cs" company="Microsoft Corporation">
//      Copyright (c) Microsoft Corporation. All rights reserved.
// </copyright>
//-----------------------------------------------------------------------

namespace Microsoft.Kinect.Fusion
{
    using System;
    using System.Runtime.InteropServices;

    /// <summary>
    /// CameraPoseFinder encapsulates camera pose finder creation, updating and pose-finding functions.
    /// </summary>
    public sealed class CameraPoseFinder : IDisposable
    {
        /// <summary>
        /// The default minimum distance threshold for capturing key frame poses for use in ProcessFrame.
        /// </summary>
        public const float DefaultMinimumDistanceThreshold = 0.3f;

        /// <summary>
        /// The native CameraPoseFinder interface wrapper.
        /// </summary>
        private INuiFusionCameraPoseFinder cameraPoseFinder;

        /// <summary>
        /// Track whether Dispose has been called.
        /// </summary>
        private bool disposed = false;

        /// <summary>
        /// Initializes a new instance of the CameraPoseFinder class.
        /// Default constructor used to initialize with the native CameraPoseFinder object.
        /// </summary>
        /// <param name="poseFinder">
        /// The native CameraPoseFinder object to be encapsulated.
        /// </param>
        internal CameraPoseFinder(INuiFusionCameraPoseFinder poseFinder)
        {
            this.cameraPoseFinder = poseFinder;
        }

        /// <summary>
        /// Finalizes an instance of the CameraPoseFinder class.
        /// This destructor will run only if the Dispose method does not get called.
        /// </summary>
        ~CameraPoseFinder()
        {
            Dispose();
        }

        /// <summary>
        /// Initialize a Kinect Fusion cameraPoseFinder.
        /// A Kinect camera is required to be connected.
        /// </summary>
        /// <param name="cameraPoseFinderParameters">
        /// The parameters to define the number of poses and feature sample locations the camera pose finder uses.
        /// </param>
        /// <returns>The CameraPoseFinder instance.</returns>
        /// <exception cref="ArgumentNullException">
        /// Thrown when the <paramref name="cameraPoseFinderParameters"/> parameter is null.
        /// </exception>
        /// <exception cref="ArgumentException">
        /// Thrown when the <paramref name="cameraPoseFinderParameters"/> parameter's 
        /// <c>featureSampleLocationsPerFrameCount</c>, <c>maxPoseHistoryCount</c> member is not a greater than 0, 
        /// or the <c>featureSampleLocationsPerFrameCount</c> member is greater than 1000.
        /// </exception>
        /// <exception cref="OutOfMemoryException">
        /// Thrown when the memory required for the camera pose finder processing could not be
        /// allocated.
        /// </exception>
        /// <exception cref="InvalidOperationException">
        /// Thrown when the Kinect Runtime could not be accessed, the Kinect device is not
        /// connected or the call failed for an unknown reason.
        /// </exception>
        public static CameraPoseFinder FusionCreateCameraPoseFinder(
            CameraPoseFinderParameters cameraPoseFinderParameters)
        {
            if (null == cameraPoseFinderParameters)
            {
                throw new ArgumentNullException("cameraPoseFinderParameters");
            }

            INuiFusionCameraPoseFinder poseFinder = null;

            ExceptionHelper.ThrowIfFailed(NativeMethods.NuiFusionCreateCameraPoseFinder2(
                cameraPoseFinderParameters,
                IntPtr.Zero,
                out poseFinder));

            return new CameraPoseFinder(poseFinder);
        }

        /// <summary>
        /// Initialize a Kinect Fusion cameraPoseFinder, with random number generator seed for feature
        /// locations and feature thresholds.
        /// </summary>
        /// <param name="cameraPoseFinderParameters">
        /// The parameters to define the number of poses and feature sample locations the camera pose finder uses.
        /// </param>
        /// <param name="randomFeatureLocationAndThresholdSeed">
        /// A seed to initialize the random number generator for feature locations and feature thresholds.
        /// </param>
        /// <returns>The CameraPoseFinder instance.</returns>
        /// <exception cref="ArgumentNullException">
        /// Thrown when the <paramref name="cameraPoseFinderParameters"/> parameter is null.
        /// </exception>
        /// <exception cref="ArgumentException">
        /// Thrown when the <paramref name="cameraPoseFinderParameters"/> parameter's 
        /// <c>featureSampleLocationsPerFrameCount</c>, <c>maxPoseHistoryCount</c> member is not a greater than 0, 
        /// or a maximum of 10,000,000, the <c>featureSampleLocationsPerFrameCount</c> member is greater than 1000,
        /// or the <paramref name="randomFeatureLocationAndThresholdSeed"/> parameter is negative.
        /// </exception>
        /// <exception cref="OutOfMemoryException">
        /// Thrown when the memory required for the camera pose finder processing could not be
        /// allocated.
        /// </exception>
        /// <exception cref="InvalidOperationException">
        /// Thrown when the Kinect Runtime could not be accessed, the Kinect device is not
        /// connected or the call failed for an unknown reason.
        /// </exception>
        public static CameraPoseFinder FusionCreateCameraPoseFinder(
            CameraPoseFinderParameters cameraPoseFinderParameters,
            int randomFeatureLocationAndThresholdSeed)
        {
            if (null == cameraPoseFinderParameters)
            {
                throw new ArgumentNullException("cameraPoseFinderParameters");
            }

            uint seed = ExceptionHelper.CastAndThrowIfOutOfUintRange(randomFeatureLocationAndThresholdSeed);

            INuiFusionCameraPoseFinder poseFinder = null;

            ExceptionHelper.ThrowIfFailed(NativeMethods.NuiFusionCreateCameraPoseFinder(
                cameraPoseFinderParameters,
                ref seed,
                out poseFinder));

            return new CameraPoseFinder(poseFinder);
        }

        /// <summary>
        /// Clear the cameraPoseFinder.
        /// </summary>
        /// <exception cref="InvalidOperationException">
        /// Thrown when the Kinect Runtime could not be accessed, the device is not connected,
        /// or the call failed for an unknown reason.
        /// </exception>
        public void ResetCameraPoseFinder()
        {
            ExceptionHelper.ThrowIfFailed(cameraPoseFinder.ResetCameraPoseFinder());
        }

        /// <summary>
        /// Test input camera frames against the camera pose finder database, adding frames to the 
        /// database if dis-similar enough to existing frames. Both input depth and color frames 
        /// must be identical sizes, a minimum size of 80x60, with valid camera parameters, and 
        /// captured at the same time.
        /// Note that once the database reaches its maximum initialized size, it will overwrite old
        /// pose information. Check the <pararmref name="pHistoryTrimmed"/> flag or the number of 
        /// poses in the database to determine whether the old poses are being overwritten.
        /// </summary>
        /// <param name="depthFloatFrame">The depth float frame to be processed.</param>
        /// <param name="colorFrame">The color frame to be processed.</param>
        /// <param name="worldToCameraTransform"> The current camera pose (usually the camera pose 
        /// result from the last AlignPointClouds or AlignDepthFloatToReconstruction).</param>
        /// <param name="minimumDistanceThreshold">A float distance threshold between 0 and 1.0f which
        /// regulates how close together poses are stored in the database. Input frames
        /// which have a minimum distance equal to or above this threshold when compared against the 
        /// database will be stored, as it indicates the input has become dis-similar to the existing 
        /// stored poses. Set to 0.0f to ignore and always add a pose when this function is called, 
        /// however in this case, unless there is an external test of distance, there is a risk this
        /// can lead to many duplicated poses.
        /// </param>
        /// <param name="addedPose">
        /// Set true when the input frame was added to the camera pose finder database.
        /// </param>
        /// <param name="trimmedHistory">
        /// Set true if the maxPoseHistoryCount was reached when the input frame is stored, so the
        /// oldest pose was overwritten in the camera pose finder database to store the latest pose.
        /// </param>
        /// <exception cref="ArgumentNullException">
        /// Thrown when the <paramref name="depthFloatFrame"/> or <paramref name="colorFrame"/> 
        /// parameter is null. </exception>
        /// <exception cref="ArgumentException">
        /// Thrown when the <paramref name="depthFloatFrame"/> and <paramref name="colorFrame"/> 
        /// parameter is an incorrect or different image size, or their <c>CameraParameters</c>
        /// member is null or has incorrectly sized focal lengths, or the 
        /// <paramref name="minimumDistanceThreshold"/> parameter is less than 0 or greater 
        /// than 1.0f.</exception>
        /// <exception cref="InvalidOperationException">
        /// Thrown when the Kinect Runtime could not be accessed, the device is not connected,
        /// or the call failed for an unknown reason.
        /// </exception>
        /// <remarks>
        /// The camera pose finder works by accumulating whether the values at each sample location pixel
        /// in a saved pose frame are less than or greater than a threshold which is randomly chosen 
        /// between minimum and maximum boundaries (e.g. for color this is 0-255). Given enough samples
        /// this represents a unique key frame signature that we can match against, as different poses
        /// will have different values for surfaces which are closer or further away, or different
        /// colors.
        /// Note that unlike depth, the robustness of finding a valid camera pose can have issues with
        /// ambient illumination levels in the color image. For best matching results, both the Kinect 
        /// camera and also the environment should have exactly the same configuration as when the 
        /// database key frame images were captured i.e. if you had a fixed exposure and custom white
        /// balance, this should again be set when testing the database later, otherwise the matching
        /// accuracy will be reduced. 
        /// To improve accuracy, it is possible to not just provide a red, green, blue input in the
        /// color image, but instead provide a different 3 channels of match data scaled 0-255. For
        /// example, to be more illumination independent, you could calculate hue and saturation, or
        /// convert RGB to to LAB and use the AB channels. Other measures such as texture response
        /// or corner response could additionally be computed and used in one or more of the channels.
        /// </remarks>
         public void ProcessFrame(
            FusionFloatImageFrame depthFloatFrame,
            FusionColorImageFrame colorFrame,
            Matrix4 worldToCameraTransform, 
            float minimumDistanceThreshold,
            out bool addedPose,
            out bool trimmedHistory)
        {
            if (null == depthFloatFrame)
            {
                throw new ArgumentNullException("depthFloatFrame");
            }

            if (null == colorFrame)
            {
                throw new ArgumentNullException("colorFrame");
            }

            HRESULT hr = cameraPoseFinder.ProcessFrame(
                FusionImageFrame.ToHandleRef(depthFloatFrame),
                FusionImageFrame.ToHandleRef(colorFrame),
                ref worldToCameraTransform,
                minimumDistanceThreshold,
                out addedPose,
                out trimmedHistory);

            ExceptionHelper.ThrowIfFailed(hr);
        }

        /// <summary>
        /// Find the most similar camera poses to the current camera input by comparing against the
        /// camera pose finder database, and returning a set of similar camera poses. These poses 
        /// and similarity measurements are ordered in terms of decreasing similarity (i.e. the most 
        /// similar is first). Both input depth and color frames must be identical sizes, with valid 
        /// camera parameters and captured at the same time.
        /// </summary>
        /// <param name="depthFloatFrame">The depth float frame to be processed.</param>
        /// <param name="colorFrame">The color frame to be processed.</param>
        /// <returns>Returns the matched frames object created by the camera pose finder.</returns>
        /// <exception cref="ArgumentNullException">
        /// Thrown when the <paramref name="depthFloatFrame"/> or <paramref name="colorFrame"/> 
        /// parameter is null. </exception>
        /// <exception cref="ArgumentException">
        /// Thrown when the <paramref name="depthFloatFrame"/> and  <paramref name="colorFrame"/> 
        /// parameter is an incorrect or different image size, or their <c>CameraParameters</c>
        /// member is null or has incorrectly sized focal lengths.</exception>
        /// <exception cref="InvalidOperationException">
        /// Thrown when the Kinect Runtime could not be accessed, 
        /// or the call failed for an unknown reason.
        /// </exception>
        /// <returns>Returns a set of matched frames/poses.</returns>
        public MatchCandidates FindCameraPose(
            FusionFloatImageFrame depthFloatFrame,
            FusionColorImageFrame colorFrame)
        {
            if (null == depthFloatFrame)
            {
                throw new ArgumentNullException("depthFloatFrame");
            }

            if (null == colorFrame)
            {
                throw new ArgumentNullException("colorFrame");
            }

            INuiFusionMatchCandidates matchCandidates = null;

            ExceptionHelper.ThrowIfFailed(cameraPoseFinder.FindCameraPose(
                FusionImageFrame.ToHandleRef(depthFloatFrame),
                FusionImageFrame.ToHandleRef(colorFrame),
                out matchCandidates));

            return new MatchCandidates(matchCandidates);
        }

        /// <summary>
        /// Load a previously saved camera pose finder database from disk. 
        /// Note: All existing data is replaced on a successful load of the database.
        /// If the database is saved to disk alongside the reconstruction volume, when both are
        /// re-loaded, this potentially enables reconstruction and tracking to be re-started 
        /// and the reconstruction updated by running the camera pose finder.
        /// </summary>
        /// <param name="fileName">The filename of the database file to load.</param>
        /// <exception cref="ArgumentException">
        /// Thrown when <paramref name="fileName"/> is incorrect or the file was not found.</exception>
        /// <exception cref="InvalidOperationException">
        /// Thrown when the Kinect Runtime could not be accessed, the device is not connected,
        /// or the call failed for an unknown reason.
        /// </exception>
        public void LoadCameraPoseFinderDatabase(
            string fileName)
        {
            ExceptionHelper.ThrowIfFailed(cameraPoseFinder.LoadCameraPoseFinderDatabase(fileName));
        }

        /// <summary>
        /// Save the camera pose finder database to disk. 
        /// If the camera pose finder database is saved to disk alongside the reconstruction volume, 
        /// when both are re-loaded, this potentially enables reconstruction and tracking to be 
        /// re-started and the reconstruction updated by running the camera pose finder.
        /// </summary>
        /// <param name="fileName">The filename of the database file to save.</param>
        /// <exception cref="ArgumentException">
        /// Thrown when <paramref name="fileName"/> is incorrect or the path was not found.</exception>
        /// <exception cref="InvalidOperationException">
        /// Thrown when the Kinect Runtime could not be accessed, the device is not connected, 
        /// the device does not have enough space, or the call failed for an unknown reason.
        /// </exception>
        public void SaveCameraPoseFinderDatabase(
            string fileName)
        {
            ExceptionHelper.ThrowIfFailed(cameraPoseFinder.SaveCameraPoseFinderDatabase(fileName));
        }

        /// <summary>
        /// Get the parameters used in the camera pose finder.
        /// </summary>
        /// <returns>Returns the parameters used in the camera pose finder.</returns>
        /// <exception cref="InvalidOperationException">
        /// Thrown when the Kinect Runtime could not be accessed, or
        /// the call failed for an unknown reason.
        /// </exception>
        public CameraPoseFinderParameters GetCameraPoseFinderParameters()
        {
            CameraPoseFinderParameters parameters = new CameraPoseFinderParameters(
                CameraPoseFinderParameters.Defaults.FeatureSampleLocationsPerFrame,
                CameraPoseFinderParameters.Defaults.MaxPoseHistory,
                CameraPoseFinderParameters.Defaults.MaxDepthThreshold);

            HRESULT hr = cameraPoseFinder.GetCameraPoseFinderParameters(parameters);

            ExceptionHelper.ThrowIfFailed(hr);

            return parameters;
        }

        /// <summary>
        /// Get the count of stored frames in the camera pose finder database.
        /// </summary>
        /// <returns>Returns the count of stored poses in the database.</returns>
        /// <exception cref="InvalidOperationException">
        /// Thrown when the Kinect Runtime could not be accessed, 
        /// or the call failed for an unknown reason.
        /// </exception>
        public int GetStoredPoseCount()
        {
            return (int)cameraPoseFinder.GetStoredPoseCount();
        }

        /// <summary>
        /// Disposes the CameraPoseFinder.
        /// </summary>
        public void Dispose()
        {
            if (!this.disposed)
            {
                Marshal.FinalReleaseComObject(cameraPoseFinder);
                disposed = true;
            }

            // This object will be cleaned up by the Dispose method.
            GC.SuppressFinalize(this);
        }
    }
}
