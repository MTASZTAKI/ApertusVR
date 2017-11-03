//-----------------------------------------------------------------------
// <copyright file="ExceptionHelper.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// </copyright>
//-----------------------------------------------------------------------

namespace Microsoft.Kinect.Fusion
{
    using System;
    using System.ComponentModel;
    using System.Runtime.InteropServices;

    /// <summary>
    /// The HRESULT defination which is used in Kinect Fusion.
    /// </summary>
    public enum HRESULT
    {
        /// <summary>
        /// The operation succeeded.
        /// </summary>
        S_OK = 0,

        /// <summary>
        /// The GPU is not capable of running Kinect Fusion or there was an error on initialization.
        /// </summary>
        E_NUI_GPU_FAIL = unchecked((int)0x83010BEB),

        /// <summary>
        /// The specified index is out of range.
        /// </summary>
        E_NUI_BADINDEX = unchecked((int)0x83010585),

        /// <summary>
        /// The iterative tracking algorithm encountered a problem aligning the input point clouds and could not calculate
        /// a valid transformation.
        /// </summary>
        E_NUI_FUSION_TRACKING_ERROR = unchecked((int)0x83010BEA),

        /// <summary>
        /// Kinect SDK Runtime cannot be accessed.
        /// </summary>
        E_NUI_FEATURE_NOT_INITIALIZED = unchecked((int)0x83010005),

        /// <summary>
        /// Kinect camera not present.
        /// </summary>
        E_NUI_DEVICE_NOT_CONNECTED = unchecked((int)0x8007048F), // FACILITY_WIN32 (7), 1167L (=0x48f) error code

        /// <summary>
        /// Out of memory when allocating.
        /// </summary>
        E_OUTOFMEMORY = unchecked((int)0x8007000E),

        /// <summary>
        /// Out of memory when allocating on GPU.
        /// </summary>
        E_GPU_OUTOFMEMORY = unchecked((int)0x83010BEC),

        /// <summary>
        /// One or more input arguments were null, invalid or outside valid range.
        /// </summary>
        E_INVALIDARG = unchecked((int)0x80070057),

        /// <summary>
        /// One or more output arguments were null.
        /// </summary>
        E_POINTER = unchecked((int)0x80004003),

        /// <summary>
        /// Generic Error.
        /// </summary>
        E_FAIL = unchecked((int)0x80004005)
    }

    /// <summary>
    /// Translates HRESULTs from native NUI API's and COM interfaces to managed exceptions.
    /// </summary>
    internal static class ExceptionHelper
    {
        /// <summary>
        /// Throws a managed exception for the given native HRESULT from the Fusion API.
        /// </summary>
        /// <param name="hr">Native HRESULT returned from API.</param>
        internal static void ThrowIfFailed(HRESULT hr)
        {
            if (hr < 0)
            {
                var innerException = new Win32Exception((int)hr);
                switch (hr)
                {
                    case HRESULT.E_NUI_GPU_FAIL:
                        throw new InvalidOperationException(Resources.GpuInitializeFailed, innerException);

                    case HRESULT.E_POINTER:
                    case HRESULT.E_INVALIDARG:
                        throw new ArgumentException(Resources.InvalidArguments, innerException);

                    case HRESULT.E_NUI_BADINDEX:
                        throw new IndexOutOfRangeException(Resources.BadIndex, innerException);

                    case HRESULT.E_NUI_DEVICE_NOT_CONNECTED:
                        throw new InvalidOperationException(Resources.DeviceNotConnected, innerException);

                    case HRESULT.E_NUI_FEATURE_NOT_INITIALIZED:
                        throw new InvalidOperationException(Resources.NuiFeatureNotInitialized, innerException);

                    case HRESULT.E_OUTOFMEMORY:
                        throw new OutOfMemoryException(Resources.OutOfCpuMemory, innerException);

                    case HRESULT.E_GPU_OUTOFMEMORY:
                        throw new OutOfMemoryException(Resources.OutOfGpuMemory, innerException);

                    default:
                        // For E_FAIL and other errors occurring during inter-op,
                        // convert to InvalidOperationException and throw.
                        var exception = Marshal.GetExceptionForHR((int)hr);

                        if (exception != null)
                        {
                            // Throw an InvalidOperationException, providing the underlying exception as 
                            // additional data.  While this isn't the purest of APIs, it does ensure that
                            // the HRESULT itself is available (albeit obfuscated), which may allow clients
                            // to differentiate between various exceptions if such a need arises.
                            throw new InvalidOperationException(
                                string.Format(
                                    System.Globalization.CultureInfo.InvariantCulture,
                                    Resources.GenericException,
                                    hr),
                                exception);
                        }

                        break;
                }
            }
        }

        /// <summary>
        /// Cast to ushort and throw a managed exception if the int parameter is out of range.
        /// </summary>
        /// <param name="intValue">Integer value to cast.</param>
        /// <returns>On success, returns the int parameter value cast to a ushort.</returns>
        internal static ushort CastAndThrowIfOutOfUshortRange(int intValue)
        {
            ushort ushortValue = (ushort)intValue;
            if (ushortValue != intValue)
            {
                throw new ArgumentException(Resources.InvalidArguments);
            }

            return ushortValue;
        }

        /// <summary>
        /// Cast to uint and throw a managed exception if the int parameter is out of range.
        /// </summary>
        /// <param name="intValue">Integer value to cast.</param>
        /// <returns>On success, returns the int parameter value cast to a uint.</returns>
        internal static uint CastAndThrowIfOutOfUintRange(int intValue)
        {
            uint uintValue = (uint)intValue;
            if (uintValue != intValue)
            {
                throw new ArgumentException(Resources.InvalidArguments);
            }

            return uintValue;
        }
    }
}
