// -----------------------------------------------------------------------
// <copyright file="MathUtils.cs" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
// -----------------------------------------------------------------------
// This source is subject to the Microsoft Limited Permissive License.
// See http://www.microsoft.com/resources/sharedsource/licensingbasics/limitedpermissivelicense.mspx
// This file is based on the 3D Tools for Windows Presentation Foundation
// project.  For more information, see:
// http://CodePlex.com/Wiki/View.aspx?ProjectName=3DTools
//---------------------------------------------------------------------------

namespace Wpf3DTools
{
    using System;
    using System.Diagnostics;
    using System.Globalization;
    using System.Windows;
    using System.Windows.Media;
    using System.Windows.Media.Media3D;

    /// <summary>
    /// A helper class for common math operations.
    /// </summary>
    public static class MathUtils
    {
        /// <summary>
        /// The zero matrix definition
        /// </summary>
        public static readonly Matrix3D ZeroMatrix = new Matrix3D(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);

        /// <summary>
        /// The X axis definition
        /// </summary>
        public static readonly Vector3D XAxis = new Vector3D(1, 0, 0);

        /// <summary>
        /// The Y axis definition
        /// </summary>
        public static readonly Vector3D YAxis = new Vector3D(0, 1, 0);

        /// <summary>
        /// The Z axis definition
        /// </summary>
        public static readonly Vector3D ZAxis = new Vector3D(0, 0, 1);

        /// <summary>
        /// Get the aspect ratio
        /// </summary>
        /// <param name="size">The image or window extent.</param>
        /// <returns>Returns the aspect ratio.</returns>
        public static double GetAspectRatio(Size size)
        {
            return size.Width / size.Height;
        }

        /// <summary>
        /// Convert degrees to radians
        /// </summary>
        /// <param name="degrees">The angle in degrees.</param>
        /// <returns>Returns the angle in radians.</returns>
        public static double DegreesToRadians(double degrees)
        {
            return degrees * (Math.PI / 180.0);
        }

        /// <summary>
        /// Computes the effective view matrix for the given camera.
        /// </summary>
        /// <param name="camera">The perspective, orthogonal or matrix camera.</param>
        /// <returns>A Matrix3D containing the View.</returns>
        public static Matrix3D GetViewMatrix(Camera camera)
        {
            if (camera == null)
            {
                throw new ArgumentNullException("camera");
            }

            ProjectionCamera projectionCamera = camera as ProjectionCamera;

            if (projectionCamera != null)
            {
                return GetViewMatrix(projectionCamera);
            }

            MatrixCamera matrixCamera = camera as MatrixCamera;

            if (matrixCamera != null)
            {
                return matrixCamera.ViewMatrix;
            }

            throw new ArgumentException(string.Format(CultureInfo.CurrentCulture, "Unsupported camera type '{0}'.", camera.GetType().FullName), "camera");
        }

        /// <summary>
        /// Computes the effective projection matrix for the given camera.
        /// </summary>
        /// <param name="camera">The perspective, orthogonal or matrix camera.</param>
        /// <param name="aspectRatio">The aspect ratio of the image or window.</param>
        /// <returns>A Matrix3D containing the Projection matrix.</returns>
        public static Matrix3D GetProjectionMatrix(Camera camera, double aspectRatio)
        {
            if (camera == null)
            {
                throw new ArgumentNullException("camera");
            }

            PerspectiveCamera perspectiveCamera = camera as PerspectiveCamera;

            if (perspectiveCamera != null)
            {
                return GetProjectionMatrix(perspectiveCamera, aspectRatio);
            }

            OrthographicCamera orthographicCamera = camera as OrthographicCamera;

            if (orthographicCamera != null)
            {
                return GetProjectionMatrix(orthographicCamera, aspectRatio);
            }

            MatrixCamera matrixCamera = camera as MatrixCamera;

            if (matrixCamera != null)
            {
                return matrixCamera.ProjectionMatrix;
            }

            throw new ArgumentException(string.Format(CultureInfo.CurrentCulture, "Unsupported camera type '{0}'.", camera.GetType().FullName), "camera");
        }

        /// <summary>
        /// Computes the transform from world space to the Viewport3DVisual's inner 2D space.
        /// This method can fail if Camera.Transform is non-invertible in which case the camera clip
        /// planes will be coincident and nothing will render. In this case success will be false.
        /// </summary>
        /// <param name="visual">The input visual.</param>
        /// <param name="success">Whether the call succeeded.</param>
        /// <returns>A Matrix3D containing the world-to-viewport transform.</returns>
        public static Matrix3D TryWorldToViewportTransform(Viewport3DVisual visual, out bool success)
        {
            success = false;
            Matrix3D result = TryWorldToCameraTransform(visual, out success);

            if (null == visual)
            {
                return ZeroMatrix;
            }

            if (success)
            {
                result.Append(GetProjectionMatrix(visual.Camera, MathUtils.GetAspectRatio(visual.Viewport.Size)));
                result.Append(GetHomogeneousToViewportTransform(visual.Viewport));
                success = true;
            }

            return result;
        }

        /// <summary>
        /// Computes the transform from world space to camera space
        /// This method can fail if Camera.Transform is non-invertible, in which case the camera clip
        /// planes will be coincident and nothing will render. In this case success will be false.
        /// </summary>
        /// <param name="visual">The input visual.</param>
        /// <param name="success">Whether the call succeeded.</param>
        /// <returns>A Matrix3D containing the world-to-camera transform.</returns>
        public static Matrix3D TryWorldToCameraTransform(Viewport3DVisual visual, out bool success)
        {
            success = false;
            Matrix3D result = Matrix3D.Identity;

            if (null == visual)
            {
                return ZeroMatrix;
            }

            Camera camera = visual.Camera;

            if (null == camera)
            {
                return ZeroMatrix;
            }

            Rect viewport = visual.Viewport;

            if (viewport == Rect.Empty)
            {
                return ZeroMatrix;
            }

            Transform3D cameraTransform = camera.Transform;

            if (cameraTransform != null)
            {
                Matrix3D m = cameraTransform.Value;

                if (!m.HasInverse)
                {
                    return ZeroMatrix;
                }

                m.Invert();
                result.Append(m);
            }

            result.Append(GetViewMatrix(camera));

            success = true;
            return result;
        }

        /// <summary>
        /// Computes the transform from the inner space of the given Visual3D to the 2D space of 
        /// the Viewport3DVisual which contains it. The result will contain the transform of the
        /// given visual. This method can fail if Camera.Transform is non-invertible in which 
        /// case the camera clip planes will be coincident and nothing will render. In this case
        /// success will be false.
        /// </summary>
        /// <param name="visual">The visual.</param>
        /// <param name="viewport">The viewport.</param>
        /// <param name="success">Set true if successful</param>
        /// <returns>A Matrix3D.</returns>
        public static Matrix3D TryTransformTo2DAncestor(DependencyObject visual, out Viewport3DVisual viewport, out bool success)
        {
            Matrix3D to2D = GetWorldTransformationMatrix(visual, out viewport);
            to2D.Append(MathUtils.TryWorldToViewportTransform(viewport, out success));

            if (!success)
            {
                return ZeroMatrix;
            }

            return to2D;
        }

        /// <summary>
        /// Computes the transform from the inner space of the given Visual3D to the camera 
        /// coordinate space. The result will contain the transform of the given visual.
        /// This method can fail if Camera.Transform is non-invertible in which case the 
        /// camera clip planes will be coincident and nothing will render. In this case success
        /// will be false.
        /// </summary>
        /// <param name="visual">The visual.</param>
        /// <param name="viewport">The viewport.</param>
        /// <param name="success">Set true if successful.</param>
        /// <returns>A Matrix3D containing.</returns>
        public static Matrix3D TryTransformToCameraSpace(DependencyObject visual, out Viewport3DVisual viewport, out bool success)
        {
            Matrix3D toViewSpace = GetWorldTransformationMatrix(visual, out viewport);
            toViewSpace.Append(MathUtils.TryWorldToCameraTransform(viewport, out success));

            if (!success)
            {
                return ZeroMatrix;
            }

            return toViewSpace;
        }

        /// <summary>
        /// Transforms the axis-aligned bounding box 'bounds' by 'transform'
        /// </summary>
        /// <param name="bounds">The AABB to transform</param>
        /// <param name="transform">The transform to apply</param>
        /// <returns>Transformed AABB</returns>
        public static Rect3D TransformBounds(Rect3D bounds, Matrix3D transform)
        {
            double x1 = bounds.X;
            double y1 = bounds.Y;
            double z1 = bounds.Z;
            double x2 = bounds.X + bounds.SizeX;
            double y2 = bounds.Y + bounds.SizeY;
            double z2 = bounds.Z + bounds.SizeZ;

            Point3D[] points = new Point3D[] 
            {
                new Point3D(x1, y1, z1),
                new Point3D(x1, y1, z2),
                new Point3D(x1, y2, z1),
                new Point3D(x1, y2, z2),
                new Point3D(x2, y1, z1),
                new Point3D(x2, y1, z2),
                new Point3D(x2, y2, z1),
                new Point3D(x2, y2, z2),
            };

            transform.Transform(points);

            // reuse the 1 and 2 variables to stand for smallest and largest
            Point3D p = points[0];
            x1 = x2 = p.X; 
            y1 = y2 = p.Y; 
            z1 = z2 = p.Z;

            for (int i = 1; i < points.Length; i++)
            {
                p = points[i];

                x1 = Math.Min(x1, p.X); 
                y1 = Math.Min(y1, p.Y); 
                z1 = Math.Min(z1, p.Z);
                x2 = Math.Max(x2, p.X); 
                y2 = Math.Max(y2, p.Y); 
                z2 = Math.Max(z2, p.Z);
            }

            return new Rect3D(x1, y1, z1, x2 - x1, y2 - y1, z2 - z1);
        }

        /// <summary>
        /// Normalizes vector if |vector| > 0.
        /// This normalization is slightly different from Vector3D.Normalize. Here we just divide
        /// by the length but Vector3D.Normalize tries to avoid overflow when finding the length.
        /// </summary>
        /// <param name="vector">The vector to normalize</param>
        /// <returns>'true' if vector was normalized</returns>
        public static bool TryNormalize(ref Vector3D vector)
        {
            double length = vector.Length;

            if (length != 0)
            {
                vector /= length;
                return true;
            }

            return false;
        }

        /// <summary>
        /// Computes the center of 'box'
        /// </summary>
        /// <param name="box">The rectangle we want the center of.</param>
        /// <returns>The center point</returns>
        public static Point3D GetCenter(Rect3D box)
        {
            return new Point3D(box.X + (box.SizeX / 2), box.Y + (box.SizeY / 2), box.Z + (box.SizeZ / 2));
        }

        /// <summary>
        /// Get the view matrix from a camera.
        /// </summary>
        /// <param name="camera">The camera we want the view matrix of.</param>
        /// <returns>Returns the view matrix.</returns>
        private static Matrix3D GetViewMatrix(ProjectionCamera camera)
        {
            Debug.Assert(camera != null, "Caller needs to ensure camera is non-null.");

            // This math is identical to what you find documented for
            // D3DXMatrixLookAtRH with the exception that WPF uses a
            // LookDirection vector rather than a LookAt point.
            Vector3D axisZ = -camera.LookDirection;
            axisZ.Normalize();

            Vector3D axisX = Vector3D.CrossProduct(camera.UpDirection, axisZ);
            axisX.Normalize();

            Vector3D axisY = Vector3D.CrossProduct(axisZ, axisX);

            Vector3D position = (Vector3D)camera.Position;
            double offsetX = -Vector3D.DotProduct(axisX, position);
            double offsetY = -Vector3D.DotProduct(axisY, position);
            double offsetZ = -Vector3D.DotProduct(axisZ, position);

            return new Matrix3D(
                axisX.X, 
                axisY.X, 
                axisZ.X, 
                0,
                axisX.Y, 
                axisY.Y, 
                axisZ.Y, 
                0,
                axisX.Z, 
                axisY.Z, 
                axisZ.Z, 
                0,
                offsetX, 
                offsetY, 
                offsetZ, 
                1);
        }

        /// <summary>
        /// Get the projection matrix from an orthographic camera.
        /// </summary>
        /// <param name="camera">The camera we want the projection matrix of.</param>
        /// <param name="aspectRatio">The aspect ratio of the image or window.</param>
        /// <returns>Returns the projection matrix.</returns>
        private static Matrix3D GetProjectionMatrix(OrthographicCamera camera, double aspectRatio)
        {
            Debug.Assert(camera != null, "Caller needs to ensure camera is non-null.");

            // This math is identical to what you find documented for
            // D3DXMatrixOrthoRH with the exception that in WPF only
            // the camera's width is specified.  Height is calculated
            // from width and the aspect ratio.
            double w = camera.Width;
            double h = w / aspectRatio;
            double zn = camera.NearPlaneDistance;
            double zf = camera.FarPlaneDistance;

            double m33 = 1 / (zn - zf);
            double m43 = zn * m33;

            return new Matrix3D(
                2 / w,
                0, 
                0, 
                0,
                0, 
                2 / h,
                0, 
                0,
                0, 
                0, 
                m33, 
                0,
                0,
                0,
                m43,
                1);
        }

        /// <summary>
        /// Get the projection matrix from a perspective camera.
        /// </summary>
        /// <param name="camera">The camera we want the projection matrix of.</param>
        /// <param name="aspectRatio">The aspect ratio of the image or window.</param>
        /// <returns>Returns the projection matrix.</returns>
        private static Matrix3D GetProjectionMatrix(PerspectiveCamera camera, double aspectRatio)
        {
            Debug.Assert(camera != null, "Caller needs to ensure camera is non-null.");

            // This math is identical to what you find documented for
            // D3DXMatrixPerspectiveFovRH with the exception that in
            // WPF the camera's horizontal rather the vertical
            // field-of-view is specified.
            double horizFoV = MathUtils.DegreesToRadians(camera.FieldOfView);
            double zn = camera.NearPlaneDistance;
            double zf = camera.FarPlaneDistance;

            double scaleX = 1 / Math.Tan(horizFoV / 2);
            double scaleY = aspectRatio * scaleX;
            double m33 = (zf == double.PositiveInfinity) ? -1 : (zf / (zn - zf));
            double m43 = zn * m33;

            return new Matrix3D(
                scaleX,
                0, 
                0, 
                0,
                0,
                scaleY,
                0,
                0,
                0,
                0,
                m33,
                -1,
                0,
                0,
                m43,
                0);
        }

        /// <summary>
        /// Get the 3D to viewport transformation.
        /// </summary>
        /// <param name="viewport">The viewport rectangle.</param>
        /// <returns>Returns the 3D to viewport transform matrix.</returns>
        private static Matrix3D GetHomogeneousToViewportTransform(Rect viewport)
        {
            double scaleX = viewport.Width / 2;
            double scaleY = viewport.Height / 2;
            double offsetX = viewport.X + scaleX;
            double offsetY = viewport.Y + scaleY;

            return new Matrix3D(
                scaleX,
                0,
                0,
                0,
                0, 
                -scaleY, 
                0, 
                0,
                0, 
                0, 
                1, 
                0,
                offsetX, 
                offsetY, 
                0, 
                1);
        }

        /// <summary>
        /// Gets the object space to world space transformation for the given DependencyObject
        /// </summary>
        /// <param name="visual">The visual whose world space transform should be found</param>
        /// <param name="viewport">The Viewport3DVisual the Visual is contained within</param>
        /// <returns>The world space transformation</returns>
        private static Matrix3D GetWorldTransformationMatrix(DependencyObject visual, out Viewport3DVisual viewport)
        {
            Matrix3D worldTransform = Matrix3D.Identity;
            viewport = null;

            if (!(visual is Visual3D))
            {
                throw new ArgumentException("Must be of type Visual3D.", "visual");
            }

            while (visual != null)
            {
                if (!(visual is ModelVisual3D))
                {
                    break;
                }

                Transform3D transform = (Transform3D)visual.GetValue(ModelVisual3D.TransformProperty);

                if (transform != null)
                {
                    worldTransform.Append(transform.Value);
                }

                visual = VisualTreeHelper.GetParent(visual);
            }

            viewport = visual as Viewport3DVisual;

            if (viewport == null)
            {
                if (visual != null)
                {
                    // In WPF 3D v1 the only possible configuration is a chain of
                    // ModelVisual3Ds leading up to a Viewport3DVisual.
                    throw new InvalidOperationException(
                        string.Format(CultureInfo.CurrentCulture, "Unsupported type: '{0}'.  Expected tree of ModelVisual3Ds leading up to a Viewport3DVisual.", visual.GetType().FullName));
                }

                return ZeroMatrix;
            }

            return worldTransform;
        }
    }
}
