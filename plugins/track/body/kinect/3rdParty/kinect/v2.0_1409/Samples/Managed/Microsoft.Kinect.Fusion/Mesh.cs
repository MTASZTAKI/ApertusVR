// -----------------------------------------------------------------------
// <copyright file="Mesh.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// </copyright>
// -----------------------------------------------------------------------

namespace Microsoft.Kinect.Fusion
{
    using System;
    using System.Collections.ObjectModel;
    using System.Runtime.InteropServices;

    /// <summary>
    /// The Mesh object is created when meshing a Reconstruction volume. This provides access to the vertices,
    /// normals and triangle indexes of the mesh.
    /// </summary>
    public class Mesh : IDisposable
    {
        /// <summary>
        /// The vertices read only collection.
        /// </summary>
        private ReadOnlyCollection<Vector3> vertices;

        /// <summary>
        /// The normals read only collection.
        /// </summary>
        private ReadOnlyCollection<Vector3> normals;

        /// <summary>
        /// The triangle indexes read only collection.
        /// </summary>
        private ReadOnlyCollection<int> triangleIndexes;

        /// <summary>
        /// The native INuiFusionMesh interface wrapper.
        /// </summary>
        private INuiFusionMesh mesh;

        /// <summary>
        /// Track whether Dispose has been called.
        /// </summary>
        private bool disposed = false;

        /// <summary>
        /// Initializes a new instance of the Mesh class.
        /// </summary>
        /// <param name="mesh">The mesh interface to be encapsulated.</param>
        internal Mesh(INuiFusionMesh mesh)
        {
            this.mesh = mesh;
        }

        /// <summary>
        /// Finalizes an instance of the Mesh class.
        /// This destructor will run only if the Dispose method does not get called.
        /// </summary>
        ~Mesh()
        {
            Dispose(false);
        }

        /// <summary>
        /// Gets the collection of vertices. Each vertex has a corresponding normal with the same index.
        /// </summary>
        /// <returns>Returns a reference to the read only collection of the vertices.</returns>
        /// <exception cref="InvalidOperationException">
        /// Thrown when the call failed for an unknown reason.
        /// </exception>
        public ReadOnlyCollection<Vector3> GetVertices()
        {
            if (null == vertices)
            {
                IntPtr ptr = IntPtr.Zero;
                ExceptionHelper.ThrowIfFailed(mesh.GetVertices(out ptr));

                vertices = new ReadOnlyCollection<Vector3>(new NativeArray<Vector3>(ptr, (int)mesh.VertexCount()));
            }

            return vertices;
        }

        /// <summary>
        /// Gets the collection of normals. Each normal has a corresponding vertex with the same index.
        /// </summary>
        /// <returns>Returns a reference to the read only collection of the normals.</returns>
        /// <exception cref="InvalidOperationException">
        /// Thrown when the call failed for an unknown reason.
        /// </exception>
        public ReadOnlyCollection<Vector3> GetNormals()
        {
            if (null == normals)
            {
                IntPtr ptr = IntPtr.Zero;
                ExceptionHelper.ThrowIfFailed(mesh.GetNormals(out ptr));

                normals = new ReadOnlyCollection<Vector3>(new NativeArray<Vector3>(ptr, (int)mesh.NormalCount()));
            }

            return normals;
        }

        /// <summary>
        /// Gets the collection of triangle indexes. There are 3 indexes per triangle.
        /// </summary>
        /// <returns>Returns a reference to the read only collection of the triangle indexes.</returns>
        /// <exception cref="InvalidOperationException">
        /// Thrown when the call failed for an unknown reason.
        /// </exception>
        public ReadOnlyCollection<int> GetTriangleIndexes()
        {
            if (null == triangleIndexes)
            {
                IntPtr ptr = IntPtr.Zero;
                ExceptionHelper.ThrowIfFailed(mesh.GetTriangleIndices(out ptr));

                triangleIndexes = new ReadOnlyCollection<int>(new NativeArray<int>(ptr, (int)mesh.TriangleVertexIndexCount()));
            }

            return triangleIndexes;
        }

        /// <summary>
        /// Disposes the Mesh.
        /// </summary>
        public void Dispose()
        {
            Dispose(true);

            // This object will be cleaned up by the Dispose method.
            GC.SuppressFinalize(this);
        }

        /// <summary>
        /// Dispose of the native Mesh object.
        /// </summary>
        /// <param name="disposing">Whether the function was called from Dispose.</param>
        protected virtual void Dispose(bool disposing)
        {
            if (!this.disposed)
            {
                Marshal.FinalReleaseComObject(mesh);
                disposed = true;
            }
        }
    }
}
