// -----------------------------------------------------------------------
// <copyright file="ReconstructionParameters.cs" company="Microsoft Corporation">
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
    public class ReconstructionParameters : IEquatable<ReconstructionParameters>
    {
        /// <summary>
        /// Initializes a new instance of the ReconstructionParameters class.
        /// </summary>
        /// <param name="voxelsPerMeter">Voxels per meter. Must be greater than 0.</param>
        /// <param name="voxelsX">Number of Voxels in X. Must be greater than 0 and a multiple of 32.</param>
        /// <param name="voxelsY">Number ofVoxels in Y. Must be greater than 0 and a multiple of 32.</param>
        /// <param name="voxelsZ">Number ofVoxels in Z. Must be greater than 0 and a multiple of 32.</param>
        public ReconstructionParameters(float voxelsPerMeter, int voxelsX, int voxelsY, int voxelsZ)
        {
            VoxelsPerMeter = voxelsPerMeter;
            VoxelsX = voxelsX;
            VoxelsY = voxelsY;
            VoxelsZ = voxelsZ;
        }

        /// <summary>
        /// Prevents a default instance of the ReconstructionParameters class from being created.
        /// The declaration of the default constructor is used for marshaling operations.
        /// </summary>
        private ReconstructionParameters()
        {
        }

        /// <summary>
        /// Gets the number of Voxels per Meter.
        /// </summary>
        public float VoxelsPerMeter { get; private set; }

        /// <summary>
        /// Gets the size of the reconstruction volume in voxels in the X axis.
        /// </summary>
        public int VoxelsX { get; private set; }

        /// <summary>
        /// Gets the size of the reconstruction volume in voxels in the Y axis.
        /// </summary>
        public int VoxelsY { get; private set; }

        /// <summary>
        /// Gets the size of the reconstruction volume in voxels in the Z axis.
        /// </summary>
        public int VoxelsZ { get; private set; }

        /// <summary>
        /// Calculates the hash code of the ReconstructionParameters.
        /// </summary>
        /// <returns>The hash code.</returns>
        public override int GetHashCode()
        {
            // XNA-like hash generation
            return VoxelsPerMeter.GetHashCode() + VoxelsX.GetHashCode() +
                VoxelsY.GetHashCode() + VoxelsZ.GetHashCode();
        }

        /// <summary>
        /// Determines if the two objects are equal.
        /// </summary>
        /// <param name="other">The object to compare to.</param>
        /// <returns>This method returns true if they are equal and false otherwise.</returns>
        public bool Equals(ReconstructionParameters other)
        {
            return null != other
                && VoxelsPerMeter == other.VoxelsPerMeter
                && VoxelsX == other.VoxelsX
                && VoxelsY == other.VoxelsY
                && VoxelsZ == other.VoxelsZ;
        }
    }
}
