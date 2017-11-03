// -----------------------------------------------------------------------
// <copyright file="MatchCandidates.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// </copyright>
// -----------------------------------------------------------------------

namespace Microsoft.Kinect.Fusion
{
    using System;
    using System.Collections.ObjectModel;
    using System.Runtime.InteropServices;

    /// <summary>
    /// The MatchCandidates object is created when finding a camera pose using the camera pose finder. 
    /// This provides access to the matched camera poses and their similarity measurements.
    /// </summary>
    public sealed class MatchCandidates : IDisposable
    {
        /// <summary>
        /// The poses read only collection.
        /// </summary>
        private ReadOnlyCollection<Matrix4> poses;

        /// <summary>
        /// The similarity measurements read only collection.
        /// </summary>
        private ReadOnlyCollection<float> similarityMeasurements;

        /// <summary>
        /// The native INuiFusionMatchCandidates interface wrapper.
        /// </summary>
        private INuiFusionMatchCandidates matchCandidates;

        /// <summary>
        /// Track whether Dispose has been called.
        /// </summary>
        private bool disposed = false;

        /// <summary>
        /// Initializes a new instance of the MatchCandidates class.
        /// </summary>
        /// <param name="matchCandidates">The match candidates interface to be encapsulated.</param>
        internal MatchCandidates(INuiFusionMatchCandidates matchCandidates)
        {
            this.matchCandidates = matchCandidates;
        }

        /// <summary>
        /// Finalizes an instance of the MatchCandidates class.
        /// This destructor will run only if the Dispose method does not get called.
        /// </summary>
        ~MatchCandidates()
        {
            Dispose();
        }

        /// <summary>
        /// Gets the number of matched poses and size of the collection returned 
        /// by calling GetMatchPoses.
        /// </summary>
        /// <returns>Returns the count of smatched poses.</returns>
        /// <exception cref="InvalidOperationException">
        /// Thrown when the call failed for an unknown reason.
        /// </exception>
        public int GetPoseCount()
        {
            return (int)this.matchCandidates.MatchPoseCount();
        }

        /// <summary>
        /// Gets a collection of camera pose candidates matched to the input in the camera pose finder. 
        /// The poses are sorted in terms of descending similarity (i.e. the most similar is first).
        /// Each pose has a corresponding similarity measurement with the same index.
        /// </summary>
        /// <returns>Returns a reference to the read only collection of the poses.</returns>
        /// <exception cref="InvalidOperationException">
        /// Thrown when the call failed for an unknown reason.
        /// </exception>
        public ReadOnlyCollection<Matrix4> GetMatchPoses()
        {
            if (null == poses)
            {
                IntPtr ptr = IntPtr.Zero;
                ExceptionHelper.ThrowIfFailed(this.matchCandidates.GetMatchPoses(out ptr));

                poses = new ReadOnlyCollection<Matrix4>(new NativeArray<Matrix4>(ptr, (int)this.matchCandidates.MatchPoseCount()));
            }

            return poses;
        }

        /// <summary>
        /// Gets the number of similarity measurements and size of the collection returned 
        /// by calling GetSimilarityCount.
        /// </summary>
        /// <returns>Returns the count of similarity measurements.</returns>
        /// <exception cref="InvalidOperationException">
        /// Thrown when the call failed for an unknown reason.
        /// </exception>
        public int GetSimilarityCount()
        {
            return (int)this.matchCandidates.MatchSimilarityCount();
        }

        /// <summary>
        /// Gets the collection of match similarity measurements indicating how similar the match
        /// is to the input to the camera pose finder.  The similarity measure is a count of how many
        /// features were matched, normalized to a value between 0 and 1.0f by dividing the 
        /// featureSampleLocationsPerFrameCount.
        /// Larger values indicate a better match and hence more similarity between the input and the 
        /// respective pose. Each similarity measure has a corresponding pose with the same index and and
        /// are sorted in terms of descending similarity (i.e. the most similar is first).
        /// </summary>
        /// <returns>Returns a reference to the read only collection of the 
        /// similarity measurements.</returns>
        /// <exception cref="InvalidOperationException">
        /// Thrown when the call failed for an unknown reason.
        /// </exception>
        public ReadOnlyCollection<float> GetMatchSimilarities()
        {
            if (null == similarityMeasurements)
            {
                IntPtr ptr = IntPtr.Zero;
                ExceptionHelper.ThrowIfFailed(this.matchCandidates.GetMatchSimilarities(out ptr));

                similarityMeasurements = new ReadOnlyCollection<float>(new NativeArray<float>(ptr, (int)this.matchCandidates.MatchSimilarityCount()));
            }

            return similarityMeasurements;
        }

        /// <summary>
        /// Calculate the minimum distance between the input and the matches.
        /// (i.e. between the input and single most similar pose candidate).
        /// Minimum distance is returned normalized 0-1, with a smaller value indicating higher
        /// similarity and a larger value indicating less similarity. 
        /// You can regulate how close together poses are stored in the camera pose finder database by
        /// only calling ProcessFrame when this value goes above a certain threshold, indicating that 
        /// the input is becoming dissimilar to the existing stored poses. 
        /// </summary>
        /// <returns>Returns a float containing the minimum distance measure.</returns>
        /// <exception cref="InvalidOperationException">
        /// Thrown when the call failed for an unknown reason.
        /// </exception>
        public float CalculateMinimumDistance()
        {
            float minimumDistance = 1.0f;
            ExceptionHelper.ThrowIfFailed(this.matchCandidates.CalculateMinimumDistance(out minimumDistance));

            return minimumDistance;
        }

        /// <summary>
        /// Disposes the match candidates.
        /// </summary>
        public void Dispose()
        {
            if (!this.disposed)
            {
                Marshal.FinalReleaseComObject(this.matchCandidates);
                disposed = true;
            }

            // This object will be cleaned up by the Dispose method.
            GC.SuppressFinalize(this);
        }
    }
}
