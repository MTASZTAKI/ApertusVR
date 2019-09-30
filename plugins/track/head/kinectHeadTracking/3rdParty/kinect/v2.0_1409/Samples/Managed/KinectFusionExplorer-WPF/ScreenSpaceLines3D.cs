// -----------------------------------------------------------------------
// <copyright file="ScreenSpaceLines3D.cs" company="Microsoft">
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
    using System.Windows;
    using System.Windows.Media;
    using System.Windows.Media.Media3D;

    /// <summary>
    ///     ScreenSpaceLines3D are a 3D line primitive whose thickness
    ///     is constant in 2D space post projection.
    ///     This means that the lines do not become foreshortened as
    ///     they recede from the camera as other 3D primitives do under
    ///     a typical perspective projection.
    ///     Example Usage:
    ///     &lt;tools:ScreenSpaceLines3D
    ///         Points="0,0,0 0,1,0 0,1,0 1,1,0 1,1,0 0,0,1"
    ///         Thickness="5" Color="Red"&gt;
    ///     "Screen space" is a bit of a misnomer as the line thickness
    ///     is specified in the 2D coordinate system of the container
    ///     Viewport3D, not the screen.
    /// </summary>
    public class ScreenSpaceLines3D : ModelVisual3D, IDisposable
    {
        /// <summary>
        /// Color property dependency property.
        /// </summary>
        public static readonly DependencyProperty ColorProperty =
            DependencyProperty.Register(
                "Color",
                typeof(Color),
                typeof(ScreenSpaceLines3D),
                new PropertyMetadata(
                    Colors.White,
                    OnColorChanged));

        /// <summary>
        /// Points property dependency property.
        /// </summary>
        public static readonly DependencyProperty PointsProperty =
            DependencyProperty.Register(
                "Points",
                typeof(Point3DCollection),
                typeof(ScreenSpaceLines3D),
                new PropertyMetadata(
                    null,
                    OnPointsChanged));

        /// <summary>
        /// Thickness property dependency property.
        /// </summary>
        public static readonly DependencyProperty ThicknessProperty =
            DependencyProperty.Register(
                "Thickness",
                typeof(double),
                typeof(ScreenSpaceLines3D),
                new PropertyMetadata(
                    1.0,
                    OnThicknessChanged));

        /// <summary>
        /// The geometry model.
        /// </summary>
        private readonly GeometryModel3D model;

        /// <summary>
        /// The mesh.
        /// </summary>
        private readonly MeshGeometry3D mesh;

        /// <summary>
        /// The visual to screen transform.
        /// </summary>
        private Matrix3D visualToScreen;

        /// <summary>
        /// The screen to visual transform.
        /// </summary>
        private Matrix3D screenToVisual;

        /// <summary>
        /// Track whether Dispose has been called
        /// </summary>
        private bool disposed = false;

        /// <summary>
        /// Whether the CompositionTarget is hooked for WPF rendering
        /// </summary>
        private bool hookedCompositionTarget = false;

        /// <summary>
        /// Initializes a new instance of the ScreenSpaceLines3D class
        /// </summary>
        public ScreenSpaceLines3D()
        {
            this.mesh = new MeshGeometry3D();
            this.model = new GeometryModel3D();
            this.model.Geometry = this.mesh;
            this.SetColor(this.Color);

            this.Content = this.model;
            this.Points = new Point3DCollection();

            CompositionTarget.Rendering += this.OnRender;
            this.hookedCompositionTarget = true;
        }

        /// <summary>
        /// Finalizes an instance of the ScreenSpaceLines3D class.
        /// This destructor will run only if the Dispose method does not get called.
        /// </summary>
        ~ScreenSpaceLines3D()
        {
            this.Dispose(false);
        }

        /// <summary>
        /// Gets or sets the line color
        /// </summary>
        public Color Color
        {
            get { return (Color)this.GetValue(ColorProperty); }
            set { this.SetValue(ColorProperty, value); }
        }

        /// <summary>
        /// Gets or sets the line thickness
        /// </summary>
        public double Thickness
        {
            get { return (double)this.GetValue(ThicknessProperty); }
            set { this.SetValue(ThicknessProperty, value); }
        }

        /// <summary>
        /// Gets or sets the line points
        /// </summary>
        public Point3DCollection Points
        {
            get { return (Point3DCollection)this.GetValue(PointsProperty); }
            set { this.SetValue(PointsProperty, value); }
        }

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
        /// Unhooks the Composition target
        /// </summary>
        /// <param name="disposing">Whether the function was called from Dispose.</param>
        protected virtual void Dispose(bool disposing)
        {
            if (!this.disposed)
            {
                if (disposing)
                {
                    this.UnhookCompositionTarget();
                }
            }

            this.disposed = true;
        }

        /// <summary>
        /// Called when the color changes
        /// </summary>
        /// <param name="sender">The sender object.</param>
        /// <param name="args">The parameters.</param>
        private static void OnColorChanged(DependencyObject sender, DependencyPropertyChangedEventArgs args)
        {
            ((ScreenSpaceLines3D)sender).SetColor((Color)args.NewValue);
        }

        /// <summary>
        /// Called when the thickness changes.
        /// </summary>
        /// <param name="sender">The sender object.</param>
        /// <param name="args">The parameters.</param>
        private static void OnThicknessChanged(DependencyObject sender, DependencyPropertyChangedEventArgs args)
        {
            ((ScreenSpaceLines3D)sender).GeometryDirty();
        }

        /// <summary>
        /// Called when the points change.
        /// </summary>
        /// <param name="sender">The sender object.</param>
        /// <param name="args">The parameters.</param>
        private static void OnPointsChanged(DependencyObject sender, DependencyPropertyChangedEventArgs args)
        {
            ((ScreenSpaceLines3D)sender).GeometryDirty();
        }

        /// <summary>
        /// Unhooks the Composition target if hooked
        /// </summary>
        private void UnhookCompositionTarget()
        {
            if (this.hookedCompositionTarget)
            {
                CompositionTarget.Rendering -= this.OnRender;
            }
        }

        /// <summary>
        /// Set the color for the lines
        /// </summary>
        /// <param name="color">The line color.</param>
        private void SetColor(Color color)
        {
            MaterialGroup unlitMaterial = new MaterialGroup();
            unlitMaterial.Children.Add(new DiffuseMaterial(new SolidColorBrush(Colors.Black)));
            unlitMaterial.Children.Add(new EmissiveMaterial(new SolidColorBrush(color)));
            unlitMaterial.Freeze();

            this.model.Material = unlitMaterial;
            this.model.BackMaterial = unlitMaterial;
        }

        /// <summary>
        /// Called when rendering.
        /// </summary>
        /// <param name="sender">The sender object.</param>
        /// <param name="e">The event args.</param>
        private void OnRender(object sender, EventArgs e)
        {
            if (this.Points.Count == 0 && this.mesh.Positions.Count == 0)
            {
                return;
            }

            if (this.UpdateTransforms())
            {
                this.RebuildGeometry();
            }
        }

        /// <summary>
        /// Called to force update.
        /// </summary>
        private void GeometryDirty()
        {
            // Force next call to UpdateTransforms() to return true.
            this.visualToScreen = MathUtils.ZeroMatrix;
        }

        /// <summary>
        /// Called to rebuild the visual lines.
        /// </summary>
        private void RebuildGeometry()
        {
            double halfThickness = this.Thickness / 2.0;
            int numLines = this.Points.Count / 2;

            Point3DCollection positions = new Point3DCollection(numLines * 4);

            for (int i = 0; i < numLines; i++)
            {
                int startIndex = i * 2;

                Point3D startPoint = this.Points[startIndex];
                Point3D endPoint = this.Points[startIndex + 1];

                this.AddSegment(positions, startPoint, endPoint, halfThickness);
            }

            positions.Freeze();
            this.mesh.Positions = positions;

            Int32Collection indices = new Int32Collection(this.Points.Count * 3);

            for (int i = 0; i < this.Points.Count / 2; i++)
            {
                indices.Add((i * 4) + 2);
                indices.Add((i * 4) + 1);
                indices.Add((i * 4) + 0);

                indices.Add((i * 4) + 2);
                indices.Add((i * 4) + 3);
                indices.Add((i * 4) + 1);
            }

            indices.Freeze();
            this.mesh.TriangleIndices = indices;
        }

        /// <summary>
        /// Called to add a line segment.
        /// </summary>
        /// <param name="positions">The point positions to link with lines.</param>
        /// <param name="startPoint">The start point.</param>
        /// <param name="endPoint">The end point.</param>
        /// <param name="halfThickness">The half thickness.</param>
        private void AddSegment(Point3DCollection positions, Point3D startPoint, Point3D endPoint, double halfThickness)
        {
            // NOTE: We want the vector below to be perpendicular post projection so
            //       we need to compute the line direction in post-projective space.
            Vector3D lineDirection = (endPoint * this.visualToScreen) - (startPoint * this.visualToScreen);
            lineDirection.Z = 0;
            lineDirection.Normalize();

            // NOTE: Implicit Rot(90) during construction to get a perpendicular vector.
            Vector delta = new Vector(-lineDirection.Y, lineDirection.X);
            delta *= halfThickness;

            Point3D pointOut1, pointOut2;

            this.Widen(startPoint, delta, out pointOut1, out pointOut2);

            positions.Add(pointOut1);
            positions.Add(pointOut2);

            this.Widen(endPoint, delta, out pointOut1, out pointOut2);

            positions.Add(pointOut1);
            positions.Add(pointOut2);
        }

        /// <summary>
        /// Called to widen line segments.
        /// </summary>
        /// <param name="pointIn">The input point positions.</param>
        /// <param name="delta">The difference to the initial line widths.</param>
        /// <param name="pointOut1">The output points with the delta added to X,Y position.</param>
        /// <param name="pointOut2">The output points with the delta subtracted from the X,Y position.</param>
        private void Widen(Point3D pointIn, Vector delta, out Point3D pointOut1, out Point3D pointOut2)
        {
            Point4D pointIn4 = (Point4D)pointIn;
            Point4D pointOut41 = pointIn4 * this.visualToScreen;
            Point4D pointOut42 = pointOut41;

            pointOut41.X += delta.X * pointOut41.W;
            pointOut41.Y += delta.Y * pointOut41.W;

            pointOut42.X -= delta.X * pointOut42.W;
            pointOut42.Y -= delta.Y * pointOut42.W;

            pointOut41 *= this.screenToVisual;
            pointOut42 *= this.screenToVisual;

            // NOTE: Z is not modified above, so we use the original Z below.
            pointOut1 = new Point3D(
                pointOut41.X / pointOut41.W,
                pointOut41.Y / pointOut41.W,
                pointOut41.Z / pointOut41.W);

            pointOut2 = new Point3D(
                pointOut42.X / pointOut42.W,
                pointOut42.Y / pointOut42.W,
                pointOut42.Z / pointOut42.W);
        }

        /// <summary>
        /// Called to Update Transforms.
        /// </summary>
        /// <returns>Returns true if successful.</returns>
        private bool UpdateTransforms()
        {
            Viewport3DVisual viewport;
            bool success;

            Matrix3D newVisualToScreen = MathUtils.TryTransformTo2DAncestor(this, out viewport, out success);

            if (!success || !newVisualToScreen.HasInverse)
            {
                this.mesh.Positions = null;
                return false;
            }

            if (this.visualToScreen == newVisualToScreen)
            {
                return false;
            }

            this.visualToScreen = this.screenToVisual = newVisualToScreen;
            this.screenToVisual.Invert();

            return true;
        }
    }
}
