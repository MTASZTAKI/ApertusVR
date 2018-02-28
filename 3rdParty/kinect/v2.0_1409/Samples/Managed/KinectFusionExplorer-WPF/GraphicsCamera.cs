// -----------------------------------------------------------------------
// <copyright file="GraphicsCamera.cs" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
// -----------------------------------------------------------------------

namespace Microsoft.Samples.Kinect.KinectFusionExplorer
{
    using System;
    using System.Windows;
    using System.Windows.Controls;
    using System.Windows.Input;
    using System.Windows.Media.Media3D;
    using Microsoft.Kinect;
    using Microsoft.Kinect.Fusion;

    using Wpf3DTools;

    /// <summary>
    /// Basic Camera class
    /// </summary>
    public class GraphicsCamera : IDisposable
    {
        #region Fields

        /// <summary>
        /// Graphics camera horizontal field of view, approximately equivalent to default Kinect camera parameters
        /// Note: In WPF the camera's horizontal rather the vertical field-of-view is specified when creating a perspective camera.
        /// </summary>
        public const float DefaultNominalHorizontalFov = 58.5f;

        /// <summary>
        /// Graphics camera vertical field of view (approximately 45degrees), equivalent to default Kinect camera parameters
        /// </summary>
        public const float DefaultNominalVerticalFov = 45.6f;

        /// <summary>
        /// Graphics camera near plane in m
        /// </summary>
        public const float DefaultCameraNear = 0.1f;

        /// <summary>
        /// Graphics camera far plane in m
        /// </summary>
        public const float DefaultCameraFar = 1000.0f;

        /// <summary>
        /// Frustum size in m
        /// </summary>
        public const float DefaultCameraFrustumSize = 1.0f;

        /// <summary>
        /// Camera Frustum 3D graphics line thickness in pixels
        /// </summary>
        public const int DefaultCameraFrustumLineThickness = 2;

        /// <summary>
        /// Translate mouse speed (to convert pixels to m)
        /// </summary>
        private const float TranslateMouse = 0.0002f;

        /// <summary>
        /// Translate mouse wheel speed (to convert wheel units to m)
        /// </summary>
        private const float TranslateWheel = 0.001f;

        /// <summary>
        /// Rotate mouse speed (to convert pixels to radians)
        /// </summary>
        private const float RotateMouse = 0.005f;

        /// <summary>
        /// Default normalized Kinect camera parameters
        /// </summary>
        private static CameraParameters camParams = CameraParameters.Defaults;

        /// <summary>
        /// Camera Frustum 3D graphics line color
        /// </summary>
        private static System.Windows.Media.Color defaultCameraFrustumLineColor = System.Windows.Media.Color.FromArgb(180, 240, 240, 0);   // Yellow, partly transparent

        /// <summary>
        /// The perspective camera cumulative view transform for graphics rendering
        /// </summary>
        private Transform3DGroup cumulativeCameraViewTransform = new Transform3DGroup();

        /// <summary>
        /// The perspective camera partial transform for graphics rendering
        /// </summary>
        private AxisAngleRotation3D rotationTransform = new AxisAngleRotation3D();

        /// <summary>
        /// The perspective camera partial transform for graphics rendering
        /// </summary>
        private TranslateTransform3D translateTransform = new TranslateTransform3D();

        /// <summary>
        /// The perspective camera cumulative transform for graphics rendering (inverse of cumulativeCameraViewTransform)
        /// </summary>
        private MatrixTransform3D cumulativeCameraTransform = new MatrixTransform3D();

        /// <summary>
        /// The transform for frustum graphics rendering
        /// </summary>
        private MatrixTransform3D cameraFrustumTransform3D = new MatrixTransform3D();

        /// <summary>
        /// The cumulative transform for frustum graphics rendering, including a 180 degree rotation to view towards -Z
        /// </summary>
        private Transform3DGroup cumulativeCameraFrustumTransform = new Transform3DGroup();

        /// <summary>
        /// The Viewport3D that the CameraFrustum is added to
        /// </summary>
        private Viewport3D graphicsViewport = null;

        /// <summary>
        /// The UI element to attach to and receive mouse events from
        /// </summary>
        private UIElement attachedElement = null;

        /// <summary>
        /// True when mouse left button pressed
        /// </summary>
        private bool mousePressedLeft = false;

        /// <summary>
        /// True when mouse right button pressed
        /// </summary>
        private bool mousePressedRight = false;

        /// <summary>
        /// Last mouse button press position
        /// </summary>
        private Point buttonPressMousePosition = new Point(0, 0);

        /// <summary>
        /// The current position that the mouse is in when a button is clicked or held down inside the active area
        /// </summary>
        private Point currentMousePosition = new Point();

        /// <summary>
        /// Track whether Dispose has been called
        /// </summary>
        private bool disposed = false;

        /// <summary>
        /// Lock for updating pose
        /// </summary>
        private object transformUpdateLock = new object();

        /// <summary>
        /// Set true when the frustum 3D graphics are added to the viewport scene
        /// </summary>
        private bool haveAddedFrustumGraphics = false;

        #endregion

        /// <summary>
        /// Initializes a new instance of the <see cref="GraphicsCamera"/> class.
        /// </summary>
        /// <param name="startTranslation">The start translation.</param>
        /// <param name="startRotation">The start rotation.</param>
        /// <param name="aspectRatio">The aspect ratio of the image or window.</param>
        public GraphicsCamera(Point3D startTranslation, Quaternion startRotation, float aspectRatio)
            : this(startTranslation, startRotation, DefaultNominalHorizontalFov, aspectRatio, DefaultCameraNear, DefaultCameraFar, DefaultCameraFrustumSize)
        {
        }

        /// <summary>
        /// Initializes a new instance of the <see cref="GraphicsCamera"/> class.
        /// </summary>
        /// <param name="startTranslation">The start translation.</param>
        /// <param name="startRotation">The start rotation.</param>
        /// <param name="fovDegrees">The field of view in degrees.</param>
        /// <param name="aspectRatio">The aspect ratio of the image or window.</param>
        /// <param name="near">The near plane distance.</param>
        /// <param name="far">The far plane distance.</param>
        public GraphicsCamera(Point3D startTranslation, Quaternion startRotation, float fovDegrees, float aspectRatio, float near, float far)
            : this(startTranslation, startRotation, fovDegrees, aspectRatio, near, far, DefaultCameraFrustumSize)
        {
        }

        /// <summary>
        /// Initializes a new instance of the <see cref="GraphicsCamera"/> class.
        /// Note: In WPF, the standard Right Hand coordinate system has the +X axis to the right, +Y axis up, and +Z axis out of the screen towards the viewer
        ///       ^ +Y
        ///       |
        ///       +----> +X
        ///      /
        ///     / +Z
        /// </summary>
        /// <param name="startTranslation">The start translation.</param>
        /// <param name="startRotation">The start rotation.</param>
        /// <param name="fovDegrees">The field of view in degrees.</param>
        /// <param name="aspectRatio">The aspect ratio of the image or window.</param>
        /// <param name="near">The near plane distance.</param>
        /// <param name="far">The far plane distance.</param>
        /// <param name="frustumSize">The length of the frustum sides to draw visually.</param>
        public GraphicsCamera(Point3D startTranslation, Quaternion startRotation, float fovDegrees, float aspectRatio, float near, float far, float frustumSize)
        {
            this.FrustumSize = frustumSize;
            this.NearPlane = near;
            this.FarPlane = far;
            this.StartTranslation = startTranslation;
            this.StartRotation = startRotation;
            this.AspectRatio = aspectRatio;

            // Add the transform components into the cumulative transform
            // Normally this would be SRT for world transform of objects, and the inverted form for the camera view (TRS),
            // but here as we are controlling the camera view directly, to orbit around the origin, we can use TR (with scale always 1)
            this.cumulativeCameraViewTransform.Children.Add(this.translateTransform);
            this.cumulativeCameraViewTransform.Children.Add(new RotateTransform3D(this.rotationTransform));

            // We set the initial translation to 0 here as we implement the transformation explicitly.
            // By default the look direction puts a 180 degree transform on the look direction (i.e. we look back along -Z into the screen)
            this.Camera = new PerspectiveCamera(new Point3D(0, 0, 0), new Vector3D(0, 0, -1), new Vector3D(0, 1, 0), fovDegrees);    // look towards the origin
            this.Camera.NearPlaneDistance = near;
            this.Camera.FarPlaneDistance = far;

            // Attach the camera transform to the Perspective camera
            this.Camera.Transform = this.cumulativeCameraViewTransform;

            this.attachedElement = null;

            this.Reset();
        }

        /// <summary>
        /// Finalizes an instance of the GraphicsCamera class.
        /// This destructor will run only if the Dispose method does not get called.
        /// </summary>
        ~GraphicsCamera()
        {
            this.Dispose(false);
        }

        #region properties

        /// <summary>
        /// Delegate for Event which signals camera transformation has changed
        /// </summary>
        /// <param name="sender">Event generator</param>
        /// <param name="e">Event parameter</param>
        internal delegate void TransformationChangedEventHandler(object sender, EventArgs e);

        /// <summary>
        /// Event to signal camera transformation has changed
        /// </summary>
        internal event TransformationChangedEventHandler CameraTransformationChanged;

        /// <summary>
        /// Gets the perspective camera for graphics rendering
        /// </summary>
        public PerspectiveCamera Camera { get; private set; }

        /// <summary>
        /// Gets the perspective camera Projection matrix for graphics rendering
        /// </summary>
        public Matrix3D Projection
        {
            get
            {
                return MathUtils.GetProjectionMatrix(this.Camera, this.AspectRatio);
            }
        }

        /// <summary>
        /// Gets the camera start translation in world coordinates
        /// </summary>
        public Point3D StartTranslation { get; private set; }

        /// <summary>
        /// Gets the camera start rotation from identity
        /// </summary>
        public Quaternion StartRotation { get; private set; }

        /// <summary>
        /// Gets the camera transform matrix (i.e. CameraToWorld Transform, or SE3)
        /// </summary>
        public Matrix3D CameraToWorldMatrix3D
        {
            get
            {
                lock (this.transformUpdateLock)
                {
                    return this.Camera.Transform.Value;
                }
            }
        }

        /// <summary>
        /// Gets or sets the camera View matrix (i.e. WorldToCamera Transform) as Matrix3D
        /// </summary>
        public Matrix3D WorldToCameraMatrix3D
        {
            get
            {
                lock (this.transformUpdateLock)
                {
                    return this.cumulativeCameraViewTransform.Value;
                }
            }

            set
            {
                lock (this.transformUpdateLock)
                {
                    Matrix3D transform = value;
                    this.UpdateTransform(KinectFusionHelper.Matrix3DToQuaternion(transform), new Vector3D(transform.OffsetX, transform.OffsetY, transform.OffsetZ));
                }
            }
        }

        /// <summary>
        /// Gets or sets the camera View matrix (i.e. WorldToCamera Transform) as Matrix4
        /// </summary>
        public Matrix4 WorldToCameraMatrix4
        {
            get
            {
                lock (this.transformUpdateLock)
                {
                    // Assumes T,R
                    Matrix3D trans = this.cumulativeCameraViewTransform.Children[0].Value;
                    Matrix3D rot = this.cumulativeCameraViewTransform.Children[1].Value;

                    Quaternion q = KinectFusionHelper.Matrix3DToQuaternion(rot);

                    double angle = q.Angle;
                    Vector3D axis = q.Axis;
                    axis.X = -axis.X; // negate x rotation

                    Matrix3D newMat = Matrix3D.Identity;
                    newMat.Rotate(new Quaternion(axis, angle));

                    newMat.OffsetX = -trans.OffsetX; // negate x translation
                    newMat.OffsetY = trans.OffsetY;
                    newMat.OffsetZ = trans.OffsetZ;

                    return KinectFusionHelper.ConvertMatrix3DToMatrix4(newMat);
                }
            }

            set
            {
                Matrix3D transform = KinectFusionHelper.ConvertMatrix4ToMatrix3D(value);

                Quaternion q = KinectFusionHelper.Matrix3DToQuaternion(transform);

                double angle = q.Angle;
                Vector3D axis = q.Axis;
                axis.X = -axis.X; // negate x rotation

                Quaternion newq = new Quaternion(axis, angle);

                // negate x translation
                this.UpdateTransform(newq, new Vector3D(-transform.OffsetX, transform.OffsetY, transform.OffsetZ));
            }
        }

        /// <summary>
        /// Gets the camera frustum 3D graphical representation
        /// </summary>
        public ScreenSpaceLines3D CameraFrustum { get; private set; }

        /// <summary>
        /// Gets the near plane
        /// </summary>
        public float NearPlane { get; private set; }

        /// <summary>
        /// Gets the far plane
        /// </summary>
        public float FarPlane { get; private set; }

        /// <summary>
        /// Gets the camera graphics frustum size in m
        /// </summary>
        public float FrustumSize { get; private set; }

        /// <summary>
        /// Gets the camera aspect ratio
        /// </summary>
        public float AspectRatio { get; private set; }

        #endregion

        /// <summary>
        /// Dispose resources
        /// </summary>
        public void Dispose()
        {
            this.Dispose(true);

            // This object will be cleaned up by the Dispose method.
            GC.SuppressFinalize(this);
        }

        /// <summary>
        /// Attach to a UI element to receive mouse events
        /// </summary>
        /// <param name="element">The framework element to attach to.</param>
        public void Attach(UIElement element)
        {
            if (null == element)
            {
                return;
            }

            element.MouseMove += this.OnMouseMove;
            element.MouseLeftButtonDown += this.OnMouseLeftDown;
            element.MouseLeftButtonUp += this.OnMouseLeftUp;
            element.MouseRightButtonDown += this.OnMouseRightDown;
            element.MouseRightButtonUp += this.OnMouseRightUp;
            element.MouseWheel += this.OnMouseWheel;
            element.MouseLeave += this.OnMouseLeave;

            this.attachedElement = element;
        }

        /// <summary>
        /// Detach from a UI element to stop receiving mouse events
        /// </summary>
        /// <param name="element">The framework element to detach from.</param>
        public void Detach(UIElement element)
        {
            if (null == element)
            {
                return;
            }

            element.MouseMove -= this.OnMouseMove;
            element.MouseLeftButtonDown -= this.OnMouseLeftDown;
            element.MouseLeftButtonUp -= this.OnMouseLeftUp;
            element.MouseRightButtonDown -= this.OnMouseRightDown;
            element.MouseRightButtonUp -= this.OnMouseRightUp;
            element.MouseWheel -= this.OnMouseWheel;
            element.MouseLeave -= this.OnMouseLeave;

            this.attachedElement = null;
        }

        /// <summary>
        /// Camera frustum 3D graphics
        /// </summary>
        /// <param name="viewport">The viewport.</param>
        /// <param name="depthWidth">The width of the depth image.</param>
        /// <param name="depthHeight">The height of the depth image.</param>
        public void CreateFrustum3DGraphics(Viewport3D viewport, float depthWidth, float depthHeight)
        {
            if (null == viewport)
            {
                return;
            }

            this.CreateFrustum3DGraphics(viewport, DefaultCameraNear, DefaultCameraFrustumSize, depthWidth, depthHeight, defaultCameraFrustumLineColor, DefaultCameraFrustumLineThickness);
        }

        /// <summary>
        /// Camera frustum 3D graphics
        /// </summary>
        /// <param name="viewport">The viewport.</param>
        /// <param name="near">The near plane of the frustum.</param>
        /// <param name="far">The far plane of the frustum.</param>
        /// <param name="depthWidth">The width of the depth image.</param>
        /// <param name="depthHeight">The height of the depth image.</param>
        /// <param name="color">The color to draw the frustum.</param>
        /// <param name="thickness">The line thickness to use when drawing the frustum.</param>
        public void CreateFrustum3DGraphics(Viewport3D viewport, float near, float far, float depthWidth, float depthHeight, System.Windows.Media.Color color, int thickness)
        {
            if (null == viewport)
            {
                return;
            }

            this.graphicsViewport = viewport;

            // De-normalize default camera params
            float px = camParams.PrincipalPointX * depthWidth;
            float py = camParams.PrincipalPointY * depthHeight;

            float fx = camParams.FocalLengthX * depthWidth;
            float fy = camParams.FocalLengthY * depthHeight;

            float iflx = 1.0f / fx;
            float ifly = 1.0f / fy;

            this.CameraFrustum = new ScreenSpaceLines3D();

            this.CameraFrustum.Points = new Point3DCollection();
            Point3DCollection pts = this.CameraFrustum.Points;

            // Near plane rectangle
            pts.Add(KinectFusionHelper.BackProject(0, 0, near, px, py, iflx, ifly));
            pts.Add(KinectFusionHelper.BackProject(depthWidth, 0, near, px, py, iflx, ifly));

            pts.Add(KinectFusionHelper.BackProject(depthWidth, 0, near, px, py, iflx, ifly));
            pts.Add(KinectFusionHelper.BackProject(depthWidth, depthHeight, near, px, py, iflx, ifly));

            pts.Add(KinectFusionHelper.BackProject(depthWidth, depthHeight, near, px, py, iflx, ifly));
            pts.Add(KinectFusionHelper.BackProject(0, depthHeight, near, px, py, iflx, ifly));

            pts.Add(KinectFusionHelper.BackProject(0, depthHeight, near, px, py, iflx, ifly));
            pts.Add(KinectFusionHelper.BackProject(0, 0, near, px, py, iflx, ifly));

            // Far plane rectangle
            pts.Add(KinectFusionHelper.BackProject(0, 0, far, px, py, iflx, ifly));
            pts.Add(KinectFusionHelper.BackProject(depthWidth, 0, far, px, py, iflx, ifly));

            pts.Add(KinectFusionHelper.BackProject(depthWidth, 0, far, px, py, iflx, ifly));
            pts.Add(KinectFusionHelper.BackProject(depthWidth, depthHeight, far, px, py, iflx, ifly));

            pts.Add(KinectFusionHelper.BackProject(depthWidth, depthHeight, far, px, py, iflx, ifly));
            pts.Add(KinectFusionHelper.BackProject(0, depthHeight, far, px, py, iflx, ifly));

            pts.Add(KinectFusionHelper.BackProject(0, depthHeight, far, px, py, iflx, ifly));
            pts.Add(KinectFusionHelper.BackProject(0, 0, far, px, py, iflx, ifly));

            // Connecting lines
            pts.Add(KinectFusionHelper.BackProject(0, 0, near, px, py, iflx, ifly));
            pts.Add(KinectFusionHelper.BackProject(0, 0, far, px, py, iflx, ifly));

            pts.Add(KinectFusionHelper.BackProject(depthWidth, 0, near, px, py, iflx, ifly));
            pts.Add(KinectFusionHelper.BackProject(depthWidth, 0, far, px, py, iflx, ifly));

            pts.Add(KinectFusionHelper.BackProject(depthWidth, depthHeight, near, px, py, iflx, ifly));
            pts.Add(KinectFusionHelper.BackProject(depthWidth, depthHeight, far, px, py, iflx, ifly));

            pts.Add(KinectFusionHelper.BackProject(0, depthHeight, near, px, py, iflx, ifly));
            pts.Add(KinectFusionHelper.BackProject(0, depthHeight, far, px, py, iflx, ifly));

            this.CameraFrustum.Thickness = thickness;
            this.CameraFrustum.Color = color;

            // Add a fixed rotation around the Y axis to look back down +Z towards origin
            Matrix3D fixedRotY180 = new Matrix3D();
            fixedRotY180.Rotate(new Quaternion(new Vector3D(0, 1, 0), 180));
            this.cumulativeCameraFrustumTransform.Children.Add(new MatrixTransform3D(fixedRotY180));
            this.cumulativeCameraFrustumTransform.Children.Add(this.cameraFrustumTransform3D);

            this.CameraFrustum.Transform = this.cumulativeCameraFrustumTransform;
        }

        /// <summary>
        /// Add the frustum graphics to the visual tree
        /// </summary>
        public void AddFrustum3DGraphics()
        {
            if (!this.haveAddedFrustumGraphics && null != this.graphicsViewport && null != this.CameraFrustum)
            {
                this.graphicsViewport.Children.Add(this.CameraFrustum);

                this.haveAddedFrustumGraphics = true;
            }
        }

        /// <summary>
        /// Remove the frustum graphics from the visual tree
        /// </summary>
        public void RemoveFrustum3DGraphics()
        {
            if (this.haveAddedFrustumGraphics && null != this.graphicsViewport && null != this.CameraFrustum)
            {
                this.graphicsViewport.Children.Remove(this.CameraFrustum);

                this.haveAddedFrustumGraphics = false;
            }
        }

        /// <summary>
        /// Dispose the frustum graphics
        /// </summary>
        public void DisposeFrustum3DGraphics()
        {
            if (this.haveAddedFrustumGraphics)
            {
                this.RemoveFrustum3DGraphics();
            }

            if (null != this.CameraFrustum)
            {
                this.CameraFrustum.Dispose();
                this.CameraFrustum = null;
            }
        }

        /// <summary>
        /// Reset the camera to starting translation and rotation
        /// </summary>
        public void Reset()
        {
            this.UpdateTransform(this.StartRotation, new Vector3D(this.StartTranslation.X, this.StartTranslation.Y, this.StartTranslation.Z));
        }

        /// <summary>
        /// Update virtual camera transform
        /// Note: To update the frustum graphics transform, call the UpdateFrustumTransform functions separately
        /// </summary>
        /// <param name="rotationQuaternion">The rotation.</param>
        /// <param name="translation">The translation.</param>
        public void UpdateTransform(Quaternion rotationQuaternion, Vector3D translation)
        {
            if (null == this.Camera)
            {
                return;
            }

            this.rotationTransform.Axis = rotationQuaternion.Axis;
            this.rotationTransform.Angle = rotationQuaternion.Angle;

            this.translateTransform.OffsetX = translation.X;
            this.translateTransform.OffsetY = translation.Y;
            this.translateTransform.OffsetZ = translation.Z;

            // Update the combined transforms
            Matrix3D view = this.cumulativeCameraViewTransform.Value; // Invert the camera view transform to get camera matrix
            view.Invert();
            this.cumulativeCameraTransform.Matrix = view;

            // Raise Transformation Changed Event
            if (null != this.CameraTransformationChanged)
            {
                this.CameraTransformationChanged(this, null);
            }
        }

        /// <summary>
        /// Update frustum graphics transform
        /// </summary>
        /// <param name="worldToCameraMatrix3D">The transformation.</param>
        public void UpdateFrustumTransformMatrix3D(Matrix3D worldToCameraMatrix3D)
        {
            // Update the frustum graphics if we have created them
            if (null != this.CameraFrustum)
            {
                this.cameraFrustumTransform3D.Matrix = worldToCameraMatrix3D;   // Set the camera transform to the frustum graphics
            }
        }

        /// <summary>
        /// Update frustum graphics transform
        /// </summary>
        /// <param name="worldToCameraMatrix4">The transformation.</param>
        public void UpdateFrustumTransformMatrix4(Matrix4 worldToCameraMatrix4)
        {
            // Update the frustum graphics if we have created them
            if (null != this.CameraFrustum)
            {
                Matrix3D transform = KinectFusionHelper.ConvertMatrix4ToMatrix3D(worldToCameraMatrix4);

                Quaternion q = KinectFusionHelper.Matrix3DToQuaternion(transform);

                double angle = q.Angle;
                Vector3D axis = q.Axis;
                axis.X = -axis.X; // negate x rotation

                Quaternion newq = new Quaternion(axis, angle);

                Transform3DGroup cumulativeFrustumViewTransform = new Transform3DGroup();

                cumulativeFrustumViewTransform.Children.Add(new TranslateTransform3D(new Vector3D(-transform.OffsetX, transform.OffsetY, transform.OffsetZ)));
                cumulativeFrustumViewTransform.Children.Add(new RotateTransform3D(new AxisAngleRotation3D(axis, angle)));

                this.cameraFrustumTransform3D.Matrix = cumulativeFrustumViewTransform.Value;   // Set the camera transform to the frustum graphics
            }
        }

        /// <summary>
        /// Update the transform from the mouse deltas
        /// </summary>
        /// <param name="deltaX">The delta in X.</param>
        /// <param name="deltaY">The delta in Y.</param>
        /// <param name="deltaZ">The delta in Z.</param>
        /// <param name="rotate">Whether to rotate.</param>
        /// <param name="translate">Whether to translate in X,Y.</param>
        public void UpdateTransformFromMouseDeltas(int deltaX, int deltaY, int deltaZ, bool rotate, bool translate)
        {
            if (rotate)
            {
                this.RotateCamera(deltaX, deltaY);
            }
            else if (translate)
            {
                this.TranslateCamera(deltaX, deltaY);
            }

            // Mouse wheel
            if (deltaZ != 0) 
            {
                this.TranslateCameraMouseWheel(deltaZ);
            }
        }

        /// <summary>
        /// Frees associated memory and tidies up
        /// </summary>
        /// <param name="disposing">Whether the function was called from Dispose.</param>
        protected virtual void Dispose(bool disposing)
        {
            if (!this.disposed)
            {
                if (disposing)
                {
                    if (null != this.attachedElement)
                    {
                        this.Detach(this.attachedElement); // stop getting mouse events
                    }

                    this.RemoveFrustum3DGraphics();
                    this.DisposeFrustum3DGraphics();
                }
            }

            this.disposed = true;
        }

        ////////////////////////////////////////////////////////////////
        // Mouse Handling

        /// <summary>
        /// Called when mouse left button pressed inside the reconstruction image
        /// </summary>
        /// <param name="sender">Event generator</param>
        /// <param name="e">Event parameter</param>
        private void OnMouseLeftDown(object sender, MouseButtonEventArgs e)
        {
            if (!this.mousePressedRight)
            {
                this.mousePressedLeft = true;
                Point pos = e.GetPosition(this.attachedElement);
                this.buttonPressMousePosition = new Point(pos.X, pos.Y);
            }
        }

        /// <summary>
        /// Called when mouse right button pressed inside the reconstruction image
        /// </summary>
        /// <param name="sender">Event generator</param>
        /// <param name="e">Event parameter</param>
        private void OnMouseRightDown(object sender, MouseButtonEventArgs e)
        {
            if (!this.mousePressedLeft)
            {
                this.mousePressedRight = true;
                Point pos = e.GetPosition(this.attachedElement);
                this.buttonPressMousePosition = new Point(pos.X, pos.Y);
            }
        }

        /// <summary>
        /// Called when mouse left button released
        /// </summary>
        /// <param name="sender">Event generator</param>
        /// <param name="e">Event parameter</param>
        private void OnMouseLeftUp(object sender, MouseButtonEventArgs e)
        {
            if (this.mousePressedLeft)
            {
                float dx, dy;
                if (this.CalculateMouseDeltas(e, out dx, out dy))
                {
                    this.UpdateTransformFromMouseDeltas((int)dx, (int)dy, 0, this.mousePressedLeft, this.mousePressedRight);
                }

                this.mousePressedLeft = false;
            }
        }

        /// <summary>
        /// Called when mouse right button released
        /// </summary>
        /// <param name="sender">Event generator</param>
        /// <param name="e">Event parameter</param>
        private void OnMouseRightUp(object sender, MouseButtonEventArgs e)
        {
            if (this.mousePressedRight)
            {
                float dx, dy;
                if (this.CalculateMouseDeltas(e, out dx, out dy))
                {
                    this.UpdateTransformFromMouseDeltas((int)dx, (int)dy, 0, this.mousePressedLeft, this.mousePressedRight);
                }

                this.mousePressedRight = false;
            }
        }

        /// <summary>
        /// Called when mouse moves when pointer inside image
        /// </summary>
        /// <param name="sender">Event generator</param>
        /// <param name="e">Event parameter</param>
        private void OnMouseMove(object sender, MouseEventArgs e)
        {
            if (this.mousePressedLeft || this.mousePressedRight)
            {
                float dx, dy;
                if (this.CalculateMouseDeltas(e, out dx, out dy))
                {
                    this.UpdateTransformFromMouseDeltas((int)dx, (int)dy, 0, this.mousePressedLeft, this.mousePressedRight);
                }
            }
        }

        /// <summary>
        /// Called when mouse wheel moves when pointer inside image
        /// </summary>
        /// <param name="sender">Event generator</param>
        /// <param name="e">Event parameter</param>
        private void OnMouseWheel(object sender, MouseWheelEventArgs e)
        {
            if (e.Delta != 0)
            {
                this.UpdateTransformFromMouseDeltas(0, 0, (int)e.Delta, this.mousePressedLeft, this.mousePressedRight);
            }
        }

        /// <summary>
        /// Called when the mouse leaves the interactive area
        /// </summary>
        /// <param name="sender">Event generator</param>
        /// <param name="e">Event parameter</param>
        private void OnMouseLeave(object sender, MouseEventArgs e)
        {
            if (this.mousePressedLeft || this.mousePressedRight)
            {
                float dx, dy;
                if (this.CalculateMouseDeltas(e, out dx, out dy))
                {
                    this.UpdateTransformFromMouseDeltas((int)dx, (int)dy, 0, this.mousePressedLeft, this.mousePressedRight);
                }

                this.mousePressedLeft = false;
                this.mousePressedRight = false;
            }
        }

        /// <summary>
        /// Calculate the difference between mouse positions
        /// </summary>
        /// <param name="e">The event args.</param>
        /// <param name="deltaX">The delta in the x axis.</param>
        /// <param name="deltaY">The delta in the y axis.</param>
        /// <returns>Returns true if deltaX or deltaY are non-0.</returns>
        private bool CalculateMouseDeltas(MouseEventArgs e, out float deltaX, out float deltaY)
        {
            this.currentMousePosition = e.GetPosition(this.attachedElement);

            deltaX = (float)this.currentMousePosition.X - (float)this.buttonPressMousePosition.X;
            deltaY = (float)this.currentMousePosition.Y - (float)this.buttonPressMousePosition.Y;

            if (!(deltaX == 0 && deltaY == 0))
            {
                return true;
            }
            else
            {
                return false;
            }
        }

        /// <summary>
        /// Rotate and update the camera
        /// </summary>
        /// <param name="deltaX">The delta in X.</param>
        /// <param name="deltaY">The delta in Y.</param>
        private void RotateCamera(float deltaX, float deltaY)
        {
            if (null == this.attachedElement)
            {
                return;
            }

            Quaternion qx = new Quaternion(new Vector3D(1, 0, 0), -deltaY * RotateMouse);
            Quaternion qy = new Quaternion(new Vector3D(0, 1, 0), -deltaX * RotateMouse);

            var delta = qx * qy;

            // Compose the delta with the previous orientation
            var q = new Quaternion(this.rotationTransform.Axis, this.rotationTransform.Angle);
            q *= delta;

            this.UpdateTransform(q, new Vector3D(this.translateTransform.OffsetX, this.translateTransform.OffsetY, this.translateTransform.OffsetZ));
        }

        /// <summary>
        /// Translate and update the camera
        /// </summary>
        /// <param name="deltaX">The delta in X.</param>
        /// <param name="deltaY">The delta in Y.</param>
        private void TranslateCamera(float deltaX, float deltaY)
        {
            // Get the current orientation from the RotateTransform3D
            var q = new Quaternion(this.rotationTransform.Axis, this.rotationTransform.Angle);

            // Update translation
            double x = this.translateTransform.OffsetX + (-deltaX * TranslateMouse);
            double y = this.translateTransform.OffsetY + (deltaY * TranslateMouse);
            double z = this.translateTransform.OffsetZ;

            this.UpdateTransform(q, new Vector3D(x, y, z));
        }

        /// <summary>
        /// Translate and update the camera (zoom)
        /// </summary>
        /// <param name="deltaZ">The delta in Z.</param>
        private void TranslateCameraMouseWheel(float deltaZ)
        {
            // Get the current orientation from the RotateTransform3D
            var q = new Quaternion(this.rotationTransform.Axis, this.rotationTransform.Angle);

            // We are orbiting, so we just need to adjust the Z
            double x = this.translateTransform.OffsetX;
            double y = this.translateTransform.OffsetY;
            double z = this.translateTransform.OffsetZ + (deltaZ * TranslateWheel);

            this.UpdateTransform(q, new Vector3D(x, y, z));
        }
    }
}
