// -----------------------------------------------------------------------
// <copyright file="CameraParameters.cs"  company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// </copyright>
// -----------------------------------------------------------------------

namespace Microsoft.Kinect.Fusion
{
    using System;
    using System.Runtime.InteropServices;

    /// <summary>
    /// This class is used to store the intrinsic camera parameters.
    /// These parameters describe the optical system of the camera lens and sensor.
    /// </summary>
    [StructLayout(LayoutKind.Sequential)]
    public class CameraParameters : IEquatable<CameraParameters>
    {
        /// <summary>
        /// The Kinect For Windows parameters.
        /// </summary>
        private const float CameraDepthNominalFocalLengthInPixels = 367.0094f;

        /// <summary>
        /// The x value of the default normalized focal length.
        /// </summary>
        private const float DepthNormFocalLengthX = CameraDepthNominalFocalLengthInPixels / 512.0f;

        /// <summary>
        /// The y value of the default normalized focal length.
        /// </summary>
        private const float DepthNormFocalLengthY = CameraDepthNominalFocalLengthInPixels / 424.0f;

        /// <summary>
        /// The x value of the depth normalized principal point.
        /// </summary>
        private const float DepthNormPrincipalPointX = 0.511245f;

        /// <summary>
        /// The y value of the depth normalized principal point.
        /// </summary>
        private const float DepthNormPrincipalPointY = 0.489791f;

        /// <summary>
        /// The private member variable to cache the default camera parameters.
        /// </summary>
        private static CameraParameters defaultCameraParameters;

        /// <summary>
        /// Initializes a new instance of the CameraParameters class.
        /// </summary>
        /// <param name="focalLengthX">The focal length for X normalized by the camera width.</param>
        /// <param name="focalLengthY">The focal length for Y normalized by the camera height.</param>
        /// <param name="principalPointX">The principal point for X normalized by the camera width.</param>
        /// <param name="principalPointY">The principal point for Y normalized by the camera height.</param>
        public CameraParameters(float focalLengthX, float focalLengthY, float principalPointX, float principalPointY)
        {
            FocalLengthX = focalLengthX;
            FocalLengthY = focalLengthY;
            PrincipalPointX = principalPointX;
            PrincipalPointY = principalPointY;
        }

        /// <summary>
        /// Initializes a new instance of the CameraParameters class.
        /// </summary>
        /// <param name="sensor">The Kinect sensor to use to initialize the camera parameters.</param>
        internal CameraParameters(KinectSensor sensor)
        {
            if (null == sensor)
            {
                throw new ArgumentNullException("sensor");
            }

            this.FocalLengthX = DepthNormFocalLengthX;
            this.FocalLengthY = DepthNormFocalLengthY;
            this.PrincipalPointX = DepthNormPrincipalPointX;
            this.PrincipalPointY = DepthNormPrincipalPointY;
        }

        /// <summary>
        /// Prevents a default instance of the CameraParameters class from being created.
        /// The declaration of the default constructor is used for marshaling operations.
        /// </summary>
        private CameraParameters()
        {
        }

        /// <summary>
        /// Gets the default parameters.
        /// </summary>
        public static CameraParameters Defaults
        {
            get
            {
                if (null == defaultCameraParameters)
                {
                    defaultCameraParameters = new CameraParameters(
                        DepthNormFocalLengthX,
                        DepthNormFocalLengthY,
                        DepthNormPrincipalPointX,
                        DepthNormPrincipalPointY);
                }

                return defaultCameraParameters;
            }
        }

        /// <summary>
        /// Gets the focal length for X normalized by the camera width.
        /// </summary>
        public float FocalLengthX { get; private set; }

        /// <summary>
        /// Gets the focal length for Y normalized by the camera height.
        /// </summary>
        public float FocalLengthY { get; private set; }

        /// <summary>
        /// Gets the principal point for X normalized by the camera width.
        /// </summary>
        public float PrincipalPointX { get; private set; }

        /// <summary>
        /// Gets the principal point for Y normalized by the camera height.
        /// </summary>
        public float PrincipalPointY { get; private set; }

        /// <summary>
        /// Calculates the hash code of the CameraParameters.
        /// </summary>
        /// <returns>The hash code.</returns>
        public override int GetHashCode()
        {
            // Note this method of hash code generation is similar to what the XNA framework does
            return FocalLengthX.GetHashCode() + FocalLengthY.GetHashCode()
                 + PrincipalPointX.GetHashCode() + PrincipalPointY.GetHashCode();
        }

        /// <summary>
        /// Determines if the two objects are equal.
        /// </summary>
        /// <param name="other">The object to compare to.</param>
        /// <returns>This method returns true if they are equal and false otherwise.</returns>
        public bool Equals(CameraParameters other)
        {
            return null != other
                && FocalLengthX == other.FocalLengthX
                && FocalLengthY == other.FocalLengthY
                && PrincipalPointX == other.PrincipalPointX
                && PrincipalPointY == other.PrincipalPointY;
        }
    }
}
