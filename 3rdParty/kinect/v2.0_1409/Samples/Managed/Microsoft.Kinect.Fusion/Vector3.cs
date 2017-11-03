//-----------------------------------------------------------------------
// <copyright file="Vector3.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// </copyright>
//-----------------------------------------------------------------------

namespace Microsoft.Kinect.Fusion
{
    using System;
    using System.Runtime.InteropServices;

    /// <summary>
    /// Vector3 is a 3-element vector, typically used for storing 3D points or normals.
    /// </summary>
    [StructLayout(LayoutKind.Sequential)]
    public struct Vector3 : IEquatable<Vector3>
    {
        /// <summary>
        /// Gets or sets the X element.
        /// </summary>
        public float X { get; set; }

        /// <summary>
        /// Gets or sets the Y element.
        /// </summary>
        public float Y { get; set; }

        /// <summary>
        /// Gets or sets the Z element.
        /// </summary>
        public float Z { get; set; }

        /// <summary>
        /// Compares two Vector3 objects for equality.
        /// </summary>
        /// <param name="vector1">The first Vector3 to compare.</param>
        /// <param name="vector2">The second Vector3 to compare.</param>
        /// <returns>Returns true if both Vector3 objects are equal and false otherwise.</returns>
        public static bool operator ==(Vector3 vector1, Vector3 vector2)
        {
            return vector1.Equals(vector2);
        }

        /// <summary>
        /// Compares two Vector3 objects for inequality.
        /// </summary>
        /// <param name="vector1">The first Vector3 to compare.</param>
        /// <param name="vector2">The second Vector3 to compare.</param>
        /// <returns>Returns true if the Vector3 objects are not equal and false otherwise.</returns>
        public static bool operator !=(Vector3 vector1, Vector3 vector2)
        {
            return !vector1.Equals(vector2);
        }

        /// <summary>
        /// Compares two Vector3 objects for equality.
        /// </summary>
        /// <param name="other">The Vector3 to compare with.</param>
        /// <returns>Returns true if both Vector3 objects are equal and false otherwise.</returns>
        public bool Equals(Vector3 other)
        {
            return X == other.X
                && Y == other.Y
                && Z == other.Z;
        }

        /// <summary>
        /// Compares two Vector3 objects for equality.
        /// </summary>
        /// <param name="obj">The object to compare.</param>
        /// <returns>Returns true if they are equal and false otherwise.</returns>
        public override bool Equals(object obj)
        {
            if (!(obj is Vector3))
            {
                return false;
            }

            return this.Equals((Vector3)obj);
        }

        /// <summary>
        /// Gets the hash code for a given Vector3.
        /// </summary>
        /// <returns>The calculated hash code.</returns>
        public override int GetHashCode()
        {
            // Note this method of hash code generation is similar to what the XNA framework does
            return X.GetHashCode() + Y.GetHashCode() + Z.GetHashCode();
        }
    }
}
