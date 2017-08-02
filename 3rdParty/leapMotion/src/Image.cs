/******************************************************************************\
* Copyright (C) 2012-2016 Leap Motion, Inc. All rights reserved.               *
* Leap Motion proprietary and confidential. Not for distribution.              *
* Use subject to the terms of the Leap Motion SDK Agreement available at       *
* https://developer.leapmotion.com/sdk_agreement, or another agreement         *
* between Leap Motion and you, your company or other organization.             *
\******************************************************************************/
namespace Leap
{
    using System;
    using System.Runtime.InteropServices;
    using LeapInternal;

    /**
     * The Image class represents a single image from one of the Leap Motion cameras.
     *
     * In addition to image data, the Image object provides a distortion map for correcting
     * lens distortion.
     *
     * \include Image_raw.txt
     *
     * Note that Image objects can be invalid, which means that they do not contain
     * valid image data. Get valid Image objects from Frame::frames(). Test for
     * validity with the Image::isValid() function.
     * @since 2.1.0
     */

    public class Image :
      IDisposable
    {
        private ImageData imageData; //The pooled object containing the actual data
        private UInt64 referenceIndex = 0; //Corresponds to the index in the pooled object

        bool _disposed = false;

        public void Dispose(){
            Dispose(true);
            GC.SuppressFinalize(this);
        }

        protected virtual void Dispose(bool disposing){
            if (_disposed)
              return;

            // cleanup
            if (disposing) {
                this.imageData.CheckIn();
            }

            // Free any unmanaged objects here.
            //
            _disposed = true;
        }

        public Image(ImageData data){
            this.imageData = data;
            this.referenceIndex = data.index; //validates that image data hasn't been recycled
        }

        ~Image() {
            Dispose(false);
        }

        public bool IsComplete{
            get{
              if(imageData != null)
                  return imageData.isComplete;

              return false;
            }
        }

        /**
         * The image data.
         *
         * The image data is a set of 8-bit intensity values. The buffer is
         * ``image.Width * image.Height * image.BytesPerPixel`` bytes long.
         *
         * \include Image_data_1.txt
         *
         * @since 2.1.0
         */
        public byte[] Data {
            get {
                if(IsValid && imageData.isComplete)
                    return imageData.pixelBuffer;
                return null;
            }
        }

        public void DataWithArg (byte[] dst)
        {
            if(IsValid && imageData.isComplete)
                Buffer.BlockCopy(Data, 0, dst, 0, Data.Length);
        }

        /**
         * The distortion calibration map for this image.
         *
         * The calibration map is a 64x64 grid of points. Each point is defined by
         * a pair of 32-bit floating point values. Each point in the map
         * represents a ray projected into the camera. The value of
         * a grid point defines the pixel in the image data containing the brightness
         * value produced by the light entering along the corresponding ray. By
         * interpolating between grid data points, you can find the brightness value
         * for any projected ray. Grid values that fall outside the range [0..1] do
         * not correspond to a value in the image data and those points should be ignored.
         *
         * \include Image_distortion_1.txt
         *
         * The calibration map can be used to render an undistorted image as well as to
         * find the true angle from the camera to a feature in the raw image. The
         * distortion map itself is designed to be used with GLSL shader programs.
         * In other contexts, it may be more convenient to use the Image Rectify()
         * and Warp() functions.
         *
         * Distortion is caused by the lens geometry as well as imperfections in the
         * lens and sensor window. The calibration map is created by the calibration
         * process run for each device at the factory (and which can be rerun by the
         * user).
         *
         * Note, in a future release, there will be two distortion maps per image;
         * one containing the horizontal values and the other containing the vertical values.
         *
         * @since 2.1.0
         */
        public float[] Distortion {
            get {
                if(IsValid && imageData.isComplete)
                    return imageData.DistortionData.Data;

                return new float[0];
            }
        }

        public void DistortionWithArg (float[] dst)
        {
            if(IsValid && imageData.isComplete)
                Buffer.BlockCopy(Distortion, 0, dst, 0, Distortion.Length);
        }

        /**
         * Constructs a Image object.
         *
         * An uninitialized image is considered invalid.
         * Get valid Image objects from a ImageList object obtained from the
         * Frame::images() method.
         *
         *
         * @since 2.1.0
         */
        public Image ()
        {
        }

        /**
         * Provides the corrected camera ray intercepting the specified point on the image.
         *
         * Given a point on the image, ``PixelToRectilinear()`` corrects for camera distortion
         * and returns the true direction from the camera to the source of that image point
         * within the Leap Motion field of view.
         *
         * This direction vector has an x and y component [x, y, 1], with the third element
         * always one. Note that this vector uses the 2D camera coordinate system
         * where the x-axis parallels the longer (typically horizontal) dimension and
         * the y-axis parallels the shorter (vertical) dimension. The camera coordinate
         * system does not correlate to the 3D Leap Motion coordinate system.
         *
         * \include Image_rectify_1.txt
         *
         * **Note:** This function should be called immediately after an image is obtained. Incorrect
         * results will be returned if the image orientation has changed or a different device is plugged
         * in between the time the image was received and the time this function is called.
         *
         * Note, this function was formerly named Rectify().
         *
         * @param camera whether the pixel parameter is a pixel in the left or the right stereo image.
         * @param pixel A Vector containing the position of a pixel in the image.
         * @returns A Vector containing the ray direction (the z-component of the vector is always one).
         * @since 2.1.0
         */
        public Vector PixelToRectilinear (PerspectiveType camera, Vector pixel)
        {
            if(this.IsValid && imageData.isComplete){
              Connection connection = Connection.GetConnection();
              return connection.PixelToRectilinear(camera, pixel);
            }
            return Vector.Zero;
        }

        /**
         * Provides the point in the image corresponding to a ray projecting
         * from the camera.
         *
         * Given a ray projected from the camera in the specified direction, ``RectilinearToPixel()``
         * corrects for camera distortion and returns the corresponding pixel
         * coordinates in the image.
         *
         * The ray direction is specified in relationship to the camera. The first
         * vector element corresponds to the "horizontal" view angle; the second
         * corresponds to the "vertical" view angle.
         *
         * \include Image_warp_1.txt
         *
         * The ``RectilinearToPixel()`` function returns pixel coordinates outside of the image bounds
         * if you project a ray toward a point for which there is no recorded data.
         *
         * ``RectilinearToPixel()`` is typically not fast enough for realtime distortion correction.
         * For better performance, use a shader program exectued on a GPU.
         *
         * **Note:** This function should be called immediately after an image is obtained. Incorrect
         * results will be returned if the image orientation has changed or a different device is plugged
         * in between the time the image was received and the time this function is called.
         *
         * Note, this function was formerly named Warp().
         *
         * @param camera whether the ray parameter intercepts the left or the right stereo image.
         * @param ray A Vector containing the ray direction.
         * @returns A Vector containing the pixel coordinates [x, y, 1] (with z always one).
         * @since 2.1.0
         */
        public Vector RectilinearToPixel (PerspectiveType camera, Vector ray)
        {
            if(this.IsValid && imageData.isComplete){
              Connection connection = Connection.GetConnection();
              return connection.RectilinearToPixel(camera, ray);
            }
            return Vector.Zero;
        }

        /**
         * Compare Image object equality.
         *
         * Two Image objects are equal if and only if both Image objects represent the
         * exact same Image and both Images are valid.
         * @since 2.1.0
         */
        public bool Equals (Image other)
        {
            return this.IsValid &&
                other.IsValid &&
                this.SequenceId == other.SequenceId &&
                this.Type == other.Type &&
                this.Timestamp == other.Timestamp;
        }

        /**
         * A string containing a brief, human readable description of the Image object.
         *
         * @returns A description of the Image as a string.
         * @since 2.1.0
         */
        public override string ToString ()
        {
            if(this.IsValid && imageData.isComplete)
                return "Image sequence" + this.SequenceId + ", format: " + this.Format + ", type: " + this.Type;

            if(this.IsValid)
                return "Incomplete image sequence" + this.SequenceId + ", format: " + this.Format + ", type: " + this.Type;

            return "Invalid Image";
        }

        /**
         * The image sequence ID.
         *
         * \include Image_sequenceId.txt
         *
         * @since 2.2.1
         */
        public long SequenceId {
            get {
                if(IsValid)
                    return (long)this.imageData.frame_id;

                return -1;
            }
        }


        /**
         * The image width.
         *
         * \include Image_image_width_1.txt
         *
         * @since 2.1.0
         */
        public int Width {
            get {
                if(IsValid)
                    return (int)imageData.width;

                return 0;
            }
        }

        /**
         * The image height.
         *
         * \include Image_image_height_1.txt
         *
         * @since 2.1.0
         */
        public int Height {
            get {
                if(IsValid)
                    return (int)imageData.height;

                return 0;
            }
        }

        /**
         * The number of bytes per pixel.
         *
         * Use this value along with ``Image::width()`` and ``Image:::height()``
         * to calculate the size of the data buffer.
         *
         * \include Image_bytesPerPixel.txt
         *
         * @since 2.2.0
         */
        public int BytesPerPixel {
            get {
                if(IsValid)
                    return (int)imageData.bpp;

                return 1;
            }
        }

        /**
         * The image format.
         *
         * \include Image_format.txt
         *
         * @since 2.2.0
         */
        public Image.FormatType Format {
            get {
                if(IsValid){
                    switch (imageData.format) {
                    case eLeapImageFormat.eLeapImageType_IR:
                        return Image.FormatType.INFRARED;
                    case eLeapImageFormat.eLeapImageType_RGBIr_Bayer:
                        return Image.FormatType.IBRG;
                    default:
                        return Image.FormatType.INFRARED;
                    }
                }
                return Image.FormatType.INFRARED;
            }
        }

        public Image.ImageType Type{
            get{
                if(IsValid){
                    switch (imageData.type) {
                    case eLeapImageType.eLeapImageType_Default:
                        return Image.ImageType.DEFAULT;
                    case eLeapImageType.eLeapImageType_Raw:
                        return Image.ImageType.RAW;
                    default:
                        return Image.ImageType.DEFAULT;
                    }
                }
                return Image.ImageType.DEFAULT;
            }
        }

        /**
         * The stride of the distortion map.
         *
         * Since each point on the 64x64 element distortion map has two values in the
         * buffer, the stride is 2 times the size of the grid. (Stride is currently fixed
         * at 2 * 64 = 128).
         *
         * \include Image_distortion_width_1.txt
         *
         * @since 2.1.0
         */
        public int DistortionWidth {
            get {
                if(IsValid && imageData.isComplete)
                    return imageData.DistortionSize * 2;

                return 0;
            }
        }

        /**
         * The distortion map height.
         *
         * Currently fixed at 64.
         *
         * \include Image_distortion_height_1.txt
         *
         * @since 2.1.0
         */
        public int DistortionHeight {
            get {
                if(IsValid && imageData.isComplete)
                    return imageData.DistortionSize;

                return 0;
            }
        }

        /**
         * The horizontal ray offset.
         *
         * Used to convert between normalized coordinates in the range [0..1] and the
         * ray slope range [-4..4].
         *
         * \include Image_ray_factors_1.txt
         *
         * @since 2.1.0
         */
        public float RayOffsetX {
            get {
                if(IsValid && imageData.isComplete)
                    return imageData.RayOffsetX;

                return 0;
            }
        }

        /**
         * The vertical ray offset.
         *
         * Used to convert between normalized coordinates in the range [0..1] and the
         * ray slope range [-4..4].
         *
         * \include Image_ray_factors_2.txt
         *
         * @since 2.1.0
         */
        public float RayOffsetY {
            get {
                if(IsValid && imageData.isComplete)
                    return imageData.RayOffsetY;

                return 0;
            }
        }

        /**
         * The horizontal ray scale factor.
         *
         * Used to convert between normalized coordinates in the range [0..1] and the
         * ray slope range [-4..4].
         *
         * \include Image_ray_factors_1.txt
         *
         * @since 2.1.0
         */
        public float RayScaleX {
            get {
                if(IsValid && imageData.isComplete)
                    return imageData.RayScaleX;

                return 0;
            }
        }

        /**
         * The vertical ray scale factor.
         *
         * Used to convert between normalized coordinates in the range [0..1] and the
         * ray slope range [-4..4].
         *
         * \include Image_ray_factors_2.txt
         *
         * @since 2.1.0
         */
        public float RayScaleY {
            get {
                if(IsValid && imageData.isComplete)
                    return imageData.RayScaleY;

                return 0;
            }
        }

        /**
         * Returns a timestamp indicating when this frame began being captured on the device.
         *
         * @since 2.2.7
         */
        public long Timestamp {
            get {
                if(IsValid && imageData.isComplete)
                    return (long)imageData.timestamp;

                return 0;
            }
        }

        /**
         * Reports whether this Image instance contains valid data.
         *
         * @returns true, if and only if the image is valid.
         * @since 2.1.0
         */
        public bool IsValid {
            get {
                //If indexes are different, the ImageData object has been reused and is no longer valid for this image
                return (this.imageData != null) && (this.referenceIndex == this.imageData.index);
            }
        }

        /**
         * Returns an invalid Image object.
         *
         * You can use the instance returned by this function in comparisons testing
         * whether a given Image instance is valid or invalid. (You can also use the
         * Image::isValid() function.)
         *
         * @returns The invalid Image instance.
         * @since 2.1.0
         */
        public static Image Invalid {
            get {
                return new Image ();
            }
        }

        /**
         * Enumerates the possible image formats.
         *
         * The Image::format() function returns an item from the FormatType enumeration.
         * @since 2.2.0
         */
        public enum FormatType
        {
            INFRARED = 0,
            IBRG = 1
        }
        /**
         * Enumerates the image perspectives.
         *
         *
         * @since 3.0
         */
        public enum PerspectiveType
        {
            INVALID = 0,  //!< Invalid or unknown image perspective
            STEREO_LEFT = 1, //!< Left side of a stereo pair
            STEREO_RIGHT = 2, //!< Right side of a stereo pair
            MONO = 3 //!< Reserved for future use
        }

        public enum ImageType
        {
            DEFAULT,
            RAW
        }

        public enum RequestFailureReason{
            Image_Unavailable,
            Images_Disabled,
            Insufficient_Buffer,
            Unknown_Error,
        }
    }

}
