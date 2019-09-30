namespace Microsoft.Kinect.Fusion
{
    using System;
    using System.Runtime.InteropServices;

    /// <summary>
    /// Matrix4 is a row-major matrix containing the joint rotation information
    /// in the top left 3x3 and zero for translation.
    /// </summary>
    [StructLayout(LayoutKind.Sequential)]
    public struct Matrix4
    {
        /// <summary>
        /// Gets the identity matrix.
        /// </summary>
        public static Matrix4 Identity
        {
            get
            {
                Matrix4 identityMat = new Matrix4();
                identityMat.M11 = 1.0f;
                identityMat.M22 = 1.0f;
                identityMat.M33 = 1.0f;
                identityMat.M44 = 1.0f;
                return identityMat;
            }
        }

        /// <summary>
        /// Gets or sets Row 1, Column 1.
        /// </summary>
        public float M11 { get; set; }

        /// <summary>
        /// Gets or sets Row 1, Column 2.
        /// </summary>
        public float M12 { get; set; }

        /// <summary>
        /// Gets or sets Row 1, Column 3.
        /// </summary>
        public float M13 { get; set; }

        /// <summary>
        /// Gets or sets Row 1, Column 4.
        /// </summary>
        public float M14 { get; set; }

        /// <summary>
        /// Gets or sets Row 2, Column 1.
        /// </summary>
        public float M21 { get; set; }

        /// <summary>
        /// Gets or sets Row 2, Column 2.
        /// </summary>
        public float M22 { get; set; }

        /// <summary>
        /// Gets or sets Row 2, Column 3.
        /// </summary>
        public float M23 { get; set; }

        /// <summary>
        /// Gets or sets Row 2, Column 4.
        /// </summary>
        public float M24 { get; set; }

        /// <summary>
        /// Gets or sets Row 3, Column 1.
        /// </summary>
        public float M31 { get; set; }

        /// <summary>
        /// Gets or sets Row 3, Column 2.
        /// </summary>
        public float M32 { get; set; }

        /// <summary>
        /// Gets or sets Row 3, Column 3.
        /// </summary>
        public float M33 { get; set; }

        /// <summary>
        /// Gets or sets Row 3, Column 4.
        /// </summary>
        public float M34 { get; set; }

        /// <summary>
        /// Gets or sets Row 4, Column 1.
        /// </summary>
        public float M41 { get; set; }

        /// <summary>
        /// Gets or sets Row 4, Column 2.
        /// </summary>
        public float M42 { get; set; }

        /// <summary>
        /// Gets or sets Row 4, Column 3.
        /// </summary>
        public float M43 { get; set; }

        /// <summary>
        /// Gets or sets Row 4, Column 4.
        /// </summary>
        public float M44 { get; set; }

        /// <summary>
        /// Compares two Matrix4 objects for equality.
        /// </summary>
        /// <param name="mat1">The first Matrix4 to compare.</param>
        /// <param name="mat2">The second Matrix4 to compare.</param>
        /// <returns>Returns true if they are equal and false otherwise.</returns>
        public static bool operator ==(Matrix4 mat1, Matrix4 mat2)
        {
            return mat1.Equals(mat2);
        }

        /// <summary>
        /// Compares two Matrix4 objects for inequality.
        /// </summary>
        /// <param name="mat1">The first Matrix4 to compare.</param>
        /// <param name="mat2">The second Matrix4 to compare.</param>
        /// <returns>Returns true if they are not equal and false otherwise.</returns>
        public static bool operator !=(Matrix4 mat1, Matrix4 mat2)
        {
            return !mat1.Equals(mat2);
        }

        /// <summary>
        /// Compares two Matrix4 objects for equality.
        /// </summary>
        /// <param name="obj">The object to compare.</param>
        /// <returns>Returns true if they are equal and false otherwise.</returns>
        public override bool Equals(object obj)
        {
            if (!(obj is Matrix4))
            {
                return false;
            }

            return this.Equals((Matrix4)obj);
        }

        /// <summary>
        /// Gets the hash code for a given Matrix4.
        /// </summary>
        /// <returns>The calculated hash code.</returns>
        public override int GetHashCode()
        {
            // Note this method of hash code generation is similar to what the XNA framework does
            return M11.GetHashCode() + M12.GetHashCode() + M13.GetHashCode() + M14.GetHashCode()
                + M21.GetHashCode() + M22.GetHashCode() + M23.GetHashCode() + M24.GetHashCode()
                + M31.GetHashCode() + M32.GetHashCode() + M33.GetHashCode() + M34.GetHashCode()
                + M41.GetHashCode() + M42.GetHashCode() + M43.GetHashCode() + M44.GetHashCode();
        }

        /// <summary>
        /// Compares two Matrix4 objects for equality.
        /// </summary>
        /// <param name="mat">The Matrix4 to compare.</param>
        /// <returns>Returns true if they are equal and false otherwise.</returns>
        public bool Equals(Matrix4 mat)
        {
            return M11.Equals(mat.M11) &&
                M12.Equals(mat.M12) &&
                M13.Equals(mat.M13) &&
                M14.Equals(mat.M14) &&
                M21.Equals(mat.M21) &&
                M22.Equals(mat.M22) &&
                M23.Equals(mat.M23) &&
                M24.Equals(mat.M24) &&
                M31.Equals(mat.M31) &&
                M32.Equals(mat.M32) &&
                M33.Equals(mat.M33) &&
                M34.Equals(mat.M34) &&
                M41.Equals(mat.M41) &&
                M42.Equals(mat.M42) &&
                M43.Equals(mat.M43) &&
                M44.Equals(mat.M44);
        }

        /// <summary>
        /// Returns a Matrix4 that is a copy of a native _Matrix4 structure.
        /// </summary>
        /// <param name="mat">The native structure.</param>
        /// <returns>The managed structure.</returns>
        internal static Matrix4 CopyFrom(ref Matrix4 mat)
        {
            return new Matrix4
            {
                M11 = mat.M11,
                M12 = mat.M12,
                M13 = mat.M13,
                M14 = mat.M14,
                M21 = mat.M21,
                M22 = mat.M22,
                M23 = mat.M23,
                M24 = mat.M24,
                M31 = mat.M31,
                M32 = mat.M32,
                M33 = mat.M33,
                M34 = mat.M34,
                M41 = mat.M41,
                M42 = mat.M42,
                M43 = mat.M43,
                M44 = mat.M44
            };
        }
    }
}
