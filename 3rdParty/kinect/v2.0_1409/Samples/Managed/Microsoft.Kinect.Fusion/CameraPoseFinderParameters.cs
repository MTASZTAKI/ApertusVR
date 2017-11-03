 // -----------------------------------------------------------------------
// <copyright file="CameraPoseFinderParameters.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// </copyright>
// -----------------------------------------------------------------------

namespace Microsoft.Kinect.Fusion
{
    using System;
    using System.Runtime.InteropServices;

    /// <summary>
    /// This class is used to setup the volume parameters.
    /// </summary>
    [StructLayout(LayoutKind.Sequential)]
    public sealed class CameraPoseFinderParameters : IEquatable<CameraPoseFinderParameters>
    {
        /// <summary>
        /// The default number of feature sample locations per frame.
        /// </summary>
        private const int DefaultFeatureSampleLocationsPerFrame = 500;

        /// <summary>
        /// The default maximum pose history.
        /// </summary>
        private const int DefaultMaxPoseHistoryCount = 10000;

        /// <summary>
        /// The default maximum depth threshold.
        /// </summary>
        private const float DefaultMaxDepthThreshold = 4.0f;

        /// <summary>
        /// The private member variable to cache the default camera parameters.
        /// </summary>
        private static CameraPoseFinderParameters defaultRelocalizationParameters;

        /// <summary>
        /// A lock object protecting the default parameter creation.
        /// </summary>
        private static object lockObject = new object();

        /// <summary>
        /// Initializes a new instance of the CameraPoseFinderParameters class.
        /// </summary>
        /// <param name="featureSampleLocationsPerFrameCount">Number of features to extract per frame. This must be greater
        /// than 0 and  a maximum of 1000.</param>
        /// <param name="maxPoseHistoryCount">Maximum size of the camera pose finder pose history database. 
        /// This must be greater than 0 and less than 10,000,000.</param>
        /// <param name="maxDepthThreshold">Maximum depth to be used when choosing a threshold value for each
        /// sample features. This should be greater than 0.4f and a maximum of the closest working distance
        /// you expect in your scenario (i.e. if all your reconstruction is at short range 0-2m, set 2.0f here).
        /// Note that with the there is a trade-off, as setting large distances may make the system less 
        /// discriminative, hence more features may be required to maintain matching performance.</param>
        public CameraPoseFinderParameters(int featureSampleLocationsPerFrameCount, int maxPoseHistoryCount, float maxDepthThreshold)
        {
            FeatureSampleLocationsPerFrame = featureSampleLocationsPerFrameCount;
            MaxPoseHistory = maxPoseHistoryCount;
            MaxDepthThreshold = maxDepthThreshold;
        }

        /// <summary>
        /// Prevents a default instance of the CameraPoseFinderParameters class from being created.
        /// The declaration of the default constructor is used for marshaling operations.
        /// </summary>
        private CameraPoseFinderParameters()
        {
        }

        /// <summary>
        /// Gets the default parameters.
        /// </summary>
        public static CameraPoseFinderParameters Defaults
        {
            get
            {
                lock (lockObject)
                {
                    if (null == defaultRelocalizationParameters)
                    {
                        defaultRelocalizationParameters = new CameraPoseFinderParameters(DefaultFeatureSampleLocationsPerFrame, DefaultMaxPoseHistoryCount, DefaultMaxDepthThreshold);
                    }
                }

                return defaultRelocalizationParameters;
            }
        }

        /// <summary>
        /// Gets the number of locations to sample features per frame.
        /// </summary>
        public int FeatureSampleLocationsPerFrame { get; private set; }

        /// <summary>
        /// Gets the maximum size of the camera pose finder pose history database.
        /// </summary>
        public int MaxPoseHistory { get; private set; }

        /// <summary>
        /// Gets the maximum depth when choosing a threshold value for each sample feature in a key frame.
        /// </summary>
        public float MaxDepthThreshold { get; private set; }

        /// <summary>
        /// Calculates the hash code of the CameraPoseFinderParameters.
        /// </summary>
        /// <returns>The hash code.</returns>
        public override int GetHashCode()
        {
            // XNA-like hash generation
            return FeatureSampleLocationsPerFrame.GetHashCode() + MaxPoseHistory.GetHashCode() + MaxDepthThreshold.GetHashCode();
        }

        /// <summary>
        /// Determines if the two objects are equal.
        /// </summary>
        /// <param name="other">The object to compare to.</param>
        /// <returns>This method returns true if they are equal and false otherwise.</returns>
        public bool Equals(CameraPoseFinderParameters other)
        {
            return null != other && FeatureSampleLocationsPerFrame == other.FeatureSampleLocationsPerFrame && MaxPoseHistory == other.MaxPoseHistory
                   && MaxDepthThreshold == other.MaxDepthThreshold;
        }
    }
}
