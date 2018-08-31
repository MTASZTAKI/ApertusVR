//------------------------------------------------------------------------------
// <copyright file="KinectFusionHelper.cs" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//------------------------------------------------------------------------------

namespace Microsoft.Samples.Kinect.KinectFusionExplorer
{
    using System;
    using System.Globalization;
    using System.IO;
    using System.Windows;
    using System.Windows.Media.Media3D;
    using Microsoft.Kinect;
    using Microsoft.Kinect.Fusion;

    /// <summary>
    /// A helper class for common operations.
    /// </summary>
    public static class KinectFusionHelper
    {
        /// <summary>
        /// Save mesh in binary .STL file
        /// </summary>
        /// <param name="mesh">Calculated mesh object</param>
        /// <param name="writer">Binary file writer</param>
        /// <param name="flipAxes">Flag to determine whether the Y and Z values are flipped on save,
        /// default should be true.</param>
        public static void SaveBinaryStlMesh(ColorMesh mesh, BinaryWriter writer, bool flipAxes)
        {
            if (null == mesh || null == writer)
            {
                return;
            }

            var vertices = mesh.GetVertices();
            var normals = mesh.GetNormals();
            var indices = mesh.GetTriangleIndexes();

            // Check mesh arguments
            if (0 == vertices.Count || 0 != vertices.Count % 3 || vertices.Count != indices.Count)
            {
                throw new ArgumentException(Properties.Resources.InvalidMeshArgument);
            }

            char[] header = new char[80];
            writer.Write(header);

            // Write number of triangles
            int triangles = vertices.Count / 3;
            writer.Write(triangles);

            // Sequentially write the normal, 3 vertices of the triangle and attribute, for each triangle
            for (int i = 0; i < triangles; i++)
            {
                // Write normal
                var normal = normals[i * 3];
                writer.Write(normal.X);
                writer.Write(flipAxes ? -normal.Y : normal.Y);
                writer.Write(flipAxes ? -normal.Z : normal.Z);

                // Write vertices
                for (int j = 0; j < 3; j++)
                {
                    var vertex = vertices[(i * 3) + j];
                    writer.Write(vertex.X);
                    writer.Write(flipAxes ? -vertex.Y : vertex.Y);
                    writer.Write(flipAxes ? -vertex.Z : vertex.Z);
                }

                ushort attribute = 0;
                writer.Write(attribute);
            }
        }

        /// <summary>
        /// Save mesh in ASCII WaveFront .OBJ file
        /// </summary>
        /// <param name="mesh">Calculated mesh object</param>
        /// <param name="writer">The text writer</param>
        /// <param name="flipAxes">Flag to determine whether the Y and Z values are flipped on save,
        /// default should be true.</param>
        public static void SaveAsciiObjMesh(ColorMesh mesh, TextWriter writer, bool flipAxes)
        {
            if (null == mesh || null == writer)
            {
                return;
            }

            var vertices = mesh.GetVertices();
            var normals = mesh.GetNormals();
            var indices = mesh.GetTriangleIndexes();

            // Check mesh arguments
            if (0 == vertices.Count || 0 != vertices.Count % 3 || vertices.Count != indices.Count)
            {
                throw new ArgumentException(Properties.Resources.InvalidMeshArgument);
            }

            // Write the header lines
            writer.WriteLine("#");
            writer.WriteLine("# OBJ file created by Microsoft Kinect Fusion");
            writer.WriteLine("#");

            // Sequentially write the 3 vertices of the triangle, for each triangle
            for (int i = 0; i < vertices.Count; i++)
            {
                var vertex = vertices[i];

                string vertexString = "v " + vertex.X.ToString(CultureInfo.InvariantCulture) + " ";

                if (flipAxes)
                {
                    vertexString += (-vertex.Y).ToString(CultureInfo.InvariantCulture) + " " + (-vertex.Z).ToString(CultureInfo.InvariantCulture);
                }
                else
                {
                    vertexString += vertex.Y.ToString(CultureInfo.InvariantCulture) + " " + vertex.Z.ToString(CultureInfo.InvariantCulture);
                }

                writer.WriteLine(vertexString);
            }

            // Sequentially write the 3 normals of the triangle, for each triangle
            for (int i = 0; i < normals.Count; i++)
            {
                var normal = normals[i];

                string normalString = "vn " + normal.X.ToString(CultureInfo.InvariantCulture) + " ";

                if (flipAxes)
                {
                    normalString += (-normal.Y).ToString(CultureInfo.InvariantCulture) + " " + (-normal.Z).ToString(CultureInfo.InvariantCulture);
                }
                else
                {
                    normalString += normal.Y.ToString(CultureInfo.InvariantCulture) + " " + normal.Z.ToString(CultureInfo.InvariantCulture);
                }

                writer.WriteLine(normalString);
            }

            // Sequentially write the 3 vertex indices of the triangle face, for each triangle
            // Note this is typically 1-indexed in an OBJ file when using absolute referencing!
            for (int i = 0; i < vertices.Count / 3; i++)
            {
                string baseIndex0 = ((i * 3) + 1).ToString(CultureInfo.InvariantCulture);
                string baseIndex1 = ((i * 3) + 2).ToString(CultureInfo.InvariantCulture);
                string baseIndex2 = ((i * 3) + 3).ToString(CultureInfo.InvariantCulture);

                string faceString = "f " + baseIndex0 + "//" + baseIndex0 + " " + baseIndex1 + "//" + baseIndex1 + " " + baseIndex2 + "//" + baseIndex2;
                writer.WriteLine(faceString);
            }
        }

        /// <summary>
        /// Save mesh in ASCII .PLY file with per-vertex color
        /// </summary>
        /// <param name="mesh">Calculated mesh object</param>
        /// <param name="writer">The text writer</param>
        /// <param name="flipAxes">Flag to determine whether the Y and Z values are flipped on save,
        /// default should be true.</param>
        /// <param name="outputColor">Set this true to write out the surface color to the file when it has been captured.</param>
        public static void SaveAsciiPlyMesh(ColorMesh mesh, TextWriter writer, bool flipAxes, bool outputColor)
        {
            if (null == mesh || null == writer)
            {
                return;
            }

            var vertices = mesh.GetVertices();
            var indices = mesh.GetTriangleIndexes();
            var colors = mesh.GetColors();

            // Check mesh arguments
            if (0 == vertices.Count || 0 != vertices.Count % 3 || vertices.Count != indices.Count || (outputColor && vertices.Count != colors.Count))
            {
                throw new ArgumentException(Properties.Resources.InvalidMeshArgument);
            }

            int faces = indices.Count / 3;

            // Write the PLY header lines
            writer.WriteLine("ply");
            writer.WriteLine("format ascii 1.0");
            writer.WriteLine("comment file created by Microsoft Kinect Fusion");

            writer.WriteLine("element vertex " + vertices.Count.ToString(CultureInfo.InvariantCulture));
            writer.WriteLine("property float x");
            writer.WriteLine("property float y");
            writer.WriteLine("property float z");

            if (outputColor)
            {
                writer.WriteLine("property uchar red");
                writer.WriteLine("property uchar green");
                writer.WriteLine("property uchar blue");
            }

            writer.WriteLine("element face " + faces.ToString(CultureInfo.InvariantCulture));
            writer.WriteLine("property list uchar int vertex_index");
            writer.WriteLine("end_header");

            // Sequentially write the 3 vertices of the triangle, for each triangle
            for (int i = 0; i < vertices.Count; i++)
            {
                var vertex = vertices[i];

                string vertexString = vertex.X.ToString(CultureInfo.InvariantCulture) + " ";

                if (flipAxes)
                {
                    vertexString += (-vertex.Y).ToString(CultureInfo.InvariantCulture) + " " + (-vertex.Z).ToString(CultureInfo.InvariantCulture);
                }
                else
                {
                    vertexString += vertex.Y.ToString(CultureInfo.InvariantCulture) + " " + vertex.Z.ToString(CultureInfo.InvariantCulture);
                }

                if (outputColor)
                {
                    int red = (colors[i] >> 16) & 255;
                    int green = (colors[i] >> 8) & 255;
                    int blue = colors[i] & 255;

                    vertexString += " " + red.ToString(CultureInfo.InvariantCulture) + " " + green.ToString(CultureInfo.InvariantCulture) + " "
                                    + blue.ToString(CultureInfo.InvariantCulture);
                }

                writer.WriteLine(vertexString);
            }

            // Sequentially write the 3 vertex indices of the triangle face, for each triangle, 0-referenced in PLY files
            for (int i = 0; i < faces; i++)
            {
                string baseIndex0 = (i * 3).ToString(CultureInfo.InvariantCulture);
                string baseIndex1 = ((i * 3) + 1).ToString(CultureInfo.InvariantCulture);
                string baseIndex2 = ((i * 3) + 2).ToString(CultureInfo.InvariantCulture);

                string faceString = "3 " + baseIndex0 + " " + baseIndex1 + " " + baseIndex2;
                writer.WriteLine(faceString);
            }
        }

        /// <summary>
        /// Clamp a float value if outside two given thresholds
        /// </summary>summary>
        /// <param name="valueToClamp">The value to clamp.</param>
        /// <param name="minimumThreshold">The minimum inclusive threshold.</param>
        /// <param name="maximumThreshold">The maximum inclusive threshold.</param>
        /// <returns>Returns the clamped value.</returns>
        public static float ClampFloatingPoint(float valueToClamp, float minimumThreshold, float maximumThreshold)
        {
            if (valueToClamp < minimumThreshold)
            {
                return minimumThreshold;
            }
            else if (valueToClamp > maximumThreshold)
            {
                return maximumThreshold;
            }
            else
            {
                return valueToClamp;
            }
        }

        /// <summary>
        /// Convert radians to degrees
        /// </summary>
        /// <param name="radians">The angle in radians.</param>
        /// <returns>Angle in degrees.</returns>
        public static float RadiansToDegrees(float radians)
        {
            return radians * (180.0f / (float)Math.PI);
        }

        /// <summary>
        /// Convert degrees to radians
        /// </summary>
        /// <param name="degrees">The angle in degrees.</param>
        /// <returns>Angle in radians.</returns>
        public static float DegreesToRadians(float degrees)
        {
            return degrees * ((float)Math.PI / 180.0f);
        }

        /// <summary>
        /// Straight Conversion from Kinect Fusion Matrix4 to WPF Matrix3D type
        /// </summary>
        /// <param name="mat">The Matrix4 to convert.</param>
        /// <returns>Returns a Matrix3D converted from the Matrix4.</returns>
        public static Matrix3D ConvertMatrix4ToMatrix3D(Matrix4 mat)
        {
            return new Matrix3D(mat.M11, mat.M12, mat.M13, mat.M14, mat.M21, mat.M22, mat.M23, mat.M24, mat.M31, mat.M32, mat.M33, mat.M34, mat.M41, mat.M42, mat.M43, mat.M44);
        }

        /// <summary>
        /// Straight Conversion from WPF Matrix3D to Kinect Fusion Matrix4 type
        /// </summary>
        /// <param name="mat">The Matrix3D to convert.</param>
        /// <returns>Returns a Matrix4 converted from the Matrix3D.</returns>
        public static Matrix4 ConvertMatrix3DToMatrix4(Matrix3D mat)
        {
            Matrix4 converted = new Matrix4();

            converted.M11 = (float)mat.M11;
            converted.M12 = (float)mat.M12;
            converted.M13 = (float)mat.M13;
            converted.M14 = (float)mat.M14;

            converted.M21 = (float)mat.M21;
            converted.M22 = (float)mat.M22;
            converted.M23 = (float)mat.M23;
            converted.M24 = (float)mat.M24;

            converted.M31 = (float)mat.M31;
            converted.M32 = (float)mat.M32;
            converted.M33 = (float)mat.M33;
            converted.M34 = (float)mat.M34;

            converted.M41 = (float)mat.OffsetX;
            converted.M42 = (float)mat.OffsetY;
            converted.M43 = (float)mat.OffsetZ;
            converted.M44 = (float)mat.M44;

            return converted;
        }

        /// <summary>
        /// Get Quaternion from Matrix3D rotation
        /// </summary>
        /// <param name="mat">A Matrix3D.</param>
        /// <returns>Returns the equivalent quaternion.</returns>
        public static Quaternion Matrix3DToQuaternion(Matrix3D mat)
        {
            double x, y, z, w;

            double trace = mat.M11 + mat.M22 + mat.M33;

            if (trace > 0)
            {
                double s = 0.5f / Math.Sqrt(trace + 1.0f);
                x = (mat.M23 - mat.M32) * s;
                y = (mat.M31 - mat.M13) * s;
                z = (mat.M12 - mat.M21) * s;
                w = 0.25f / s;
            }
            else
            {
                if (mat.M11 > mat.M22 && mat.M11 > mat.M33)
                {
                    double s = 2.0f * Math.Sqrt(1.0f + mat.M11 - mat.M22 - mat.M33);
                    x = 0.25f * s;
                    y = (mat.M12 + mat.M21) / s;
                    z = (mat.M13 + mat.M31) / s;
                    w = (mat.M23 - mat.M32) / s;
                }
                else if (mat.M22 > mat.M33)
                {
                    double s = 2.0f * Math.Sqrt(1.0f + mat.M22 - mat.M11 - mat.M33);
                    x = (mat.M12 + mat.M21) / s;
                    y = 0.25f * s;
                    z = (mat.M23 + mat.M32) / s;
                    w = (mat.M31 - mat.M13) / s;
                }
                else
                {
                    double s = 2.0f * Math.Sqrt(1.0f + mat.M33 - mat.M11 - mat.M22);
                    x = (mat.M13 + mat.M31) / s;
                    y = (mat.M23 + mat.M32) / s;
                    z = 0.25f * s;
                    w = (mat.M12 - mat.M21) / s;
                }
            }

            return new Quaternion(x, y, z, w);
        }

        /// <summary>
        /// Set Identity in a Matrix4
        /// </summary>
        /// <param name="mat">The matrix to set to identity</param>
        public static void SetIdentityMatrix(ref Matrix4 mat)
        {
            mat.M11 = 1; 
            mat.M12 = 0; 
            mat.M13 = 0; 
            mat.M14 = 0;
            mat.M21 = 0; 
            mat.M22 = 1; 
            mat.M23 = 0; 
            mat.M24 = 0;
            mat.M31 = 0; 
            mat.M32 = 0; 
            mat.M33 = 1; 
            mat.M34 = 0;
            mat.M41 = 0; 
            mat.M42 = 0;
            mat.M43 = 0; 
            mat.M44 = 1;
        }

        /// <summary>
        /// Set translation vector into the Matrix4 4x4 transformation in M41,M42,M43
        /// </summary>
        /// <param name="transform">The transform matrix.</param>
        /// <param name="translationX">Floating point value for translation in X.</param>
        /// <param name="translationY">Floating point value for translation in Y.</param>
        /// <param name="translationZ">Floating point value for translation in Z.</param>
        public static void SetTranslationMatrix(ref Matrix4 transform, float translationX, float translationY, float translationZ)
        {
            transform.M41 = translationX;
            transform.M42 = translationY;
            transform.M43 = translationZ;
        }

        /// <summary>
        /// Create a translation Matrix4
        /// </summary>
        /// <param name="translationX">Floating point value for translation in X.</param>
        /// <param name="translationY">Floating point value for translation in Y.</param>
        /// <param name="translationZ">Floating point value for translation in Z.</param>
        /// <returns>A Matrix4 translation matrix.</returns>
        public static Matrix4 CreateTranslationMatrix(float translationX, float translationY, float translationZ)
        {
            Matrix4 transform = Matrix4.Identity;

            transform.M41 = translationX;
            transform.M42 = translationY;
            transform.M43 = translationZ;

            return transform;
        }

        /// <summary>
        /// Extract translation Vector3 from the Matrix4 4x4 transformation in M41,M42,M43
        /// </summary>
        /// <param name="transform">The transform matrix.</param>
        /// <param name="translationX">Floating point value for translation in X.</param>
        /// <param name="translationY">Floating point value for translation in Y.</param>
        /// <param name="translationZ">Floating point value for translation in Z.</param>
        public static void ExtractTranslation(Matrix4 transform, out float translationX, out float translationY, out float translationZ)
        {
            translationX = transform.M41;
            translationY = transform.M42;
            translationZ = transform.M43;
        }

        /// <summary>
        /// Extract translation Vector3 from the 4x4 Matrix in M41,M42,M43
        /// </summary>
        /// <param name="transform">The transform matrix.</param>
        /// <returns>Returns a Vector3 containing the translation.</returns>
        public static Vector3 ExtractTranslationVector3(Matrix4 transform)
        {
            Vector3 translation = new Vector3();
            translation.X = transform.M41;
            translation.Y = transform.M42;
            translation.Z = transform.M43;
            return translation;
        }

        /// <summary>
        /// Extract translation from the Matrix4 4x4 transformation in M41,M42,M43
        /// </summary>
        /// <param name="transform">The transform matrix.</param>
        /// <returns>Array of floating point values for translation in x,y,z.</returns>
        public static float[] ExtractTranslationFloatArray(Matrix4 transform)
        {
            float[] translation = new float[3];

            translation[0] = transform.M41;
            translation[1] = transform.M42;
            translation[2] = transform.M43;

            return translation;
        }

        /// <summary>
        /// Extract 3x3 rotation from the 4x4 Matrix and return in new Matrix4
        /// </summary>
        /// <param name="transform">The transform matrix.</param>
        /// <returns>Returns a Matrix4 containing the rotation.</returns>
        public static Matrix4 ExtractRotationMatrix(Matrix4 transform)
        {
            Matrix4 rotation = Matrix4.Identity;

            rotation.M11 = transform.M11;
            rotation.M12 = transform.M12;
            rotation.M13 = transform.M13;
            rotation.M14 = 0;

            rotation.M21 = transform.M21;
            rotation.M22 = transform.M22;
            rotation.M23 = transform.M23;
            rotation.M24 = 0;

            rotation.M31 = transform.M31;
            rotation.M32 = transform.M32;
            rotation.M33 = transform.M33;
            rotation.M34 = 0;

            rotation.M41 = 0;
            rotation.M42 = 0;
            rotation.M43 = 0;
            rotation.M44 = 1;

            return rotation;
        }

        /// <summary>
        /// Extract 3x3 rotation matrix from the Matrix4 4x4 transformation:
        /// Then convert to Euler angles.
        /// </summary>
        /// <param name="transform">The transform matrix.</param>
        /// <param name="rotationX">Floating point value for euler angle rotation around X.</param>
        /// <param name="rotationY">Floating point value for euler angle rotation around Y.</param>
        /// <param name="rotationZ">Floating point value for euler angle rotation around Z.</param>
        public static void RotationMatrixToEuler(Matrix4 transform, out float rotationX, out float rotationY, out float rotationZ)
        {
            float phi = (float)Math.Atan2(transform.M23, transform.M33);
            float theta = (float)Math.Asin(-transform.M13);
            float psi = (float)Math.Atan2(transform.M12, transform.M11);

            rotationX = phi; // This is rotation about x,y,z, or pitch, yaw, roll respectively
            rotationY = theta;
            rotationZ = psi;
        }

        /// <summary>
        /// Extract 3x3 rotation matrix from the Matrix4 4x4 transformation,
        /// then convert to Euler angles.
        /// </summary>
        /// <param name="transform">The transform matrix.</param>
        /// <returns>Array of floating point values for Euler angle rotations around x,y,z.</returns>
        public static Vector3 RotationMatrixToEulerVector3(Matrix4 transform)
        {
            Vector3 rotation = new Vector3();

            float phi = (float)Math.Atan2(transform.M23, transform.M33);
            float theta = (float)Math.Asin(-transform.M13);
            float psi = (float)Math.Atan2(transform.M12, transform.M11);

            rotation.X = phi;  // This is rotation about x,y,z, or pitch, yaw, roll respectively
            rotation.Y = theta;
            rotation.Z = psi;

            return rotation;
        }

        /// <summary>
        /// Extract 3x3 rotation matrix from the Matrix4 4x4 transformation,
        /// then convert to Euler angles.
        /// </summary>
        /// <param name="transform">The transform matrix.</param>
        /// <returns>Array of floating point values for Euler angle rotations around x,y,z.</returns>
        public static float[] RotationMatrixToEulerFloatArray(Matrix4 transform)
        {
            float[] rotation = new float[3];

            float phi = (float)Math.Atan2(transform.M23, transform.M33);
            float theta = (float)Math.Asin(-transform.M13);
            float psi = (float)Math.Atan2(transform.M12, transform.M11);

            rotation[0] = phi;  // This is rotation about x,y,z, or pitch, yaw, roll respectively
            rotation[1] = theta;
            rotation[2] = psi;

            return rotation;
        }

        /// <summary>
        /// Test whether the camera moved too far between sequential frames by looking at starting
        /// and end transformation matrix. We assume that if the camera moves or rotates beyond a
        /// reasonable threshold, that we have lost track.
        /// Note that on lower end machines, if the processing frame rate decreases below 30Hz,
        /// this limit will potentially have to be increased as frames will be dropped and hence
        /// there will be a greater motion between successive frames.
        /// </summary>
        /// <param name="initial">The transform matrix from the previous frame.</param>
        /// <param name="final">The transform matrix from the current frame.</param>
        /// <param name="maxTrans">
        /// The maximum translation in meters we expect per x,y,z component between frames under normal motion.
        /// </param>
        /// <param name="maxRotDegrees">
        /// The maximum rotation in degrees we expect about the x,y,z axes between frames under normal motion.
        /// </param>
        /// <returns>
        /// True if camera transformation is greater than the threshold, otherwise false.
        /// </returns>
        public static bool CameraTransformFailed(Matrix4 initial, Matrix4 final, float maxTrans, float maxRotDegrees)
        {
            // Check if the transform is too far out to be reasonable 
            float deltaTrans = maxTrans;
            float angDeg = maxRotDegrees;
            float deltaRot = (angDeg * (float)Math.PI) / 180.0f;

            // Calculate the deltas
            float[] eulerInitial = RotationMatrixToEulerFloatArray(initial);
            float[] eulerFinal = RotationMatrixToEulerFloatArray(final);

            float[] transInitial = ExtractTranslationFloatArray(initial);
            float[] transFinal = ExtractTranslationFloatArray(final);

            bool failRot = false;
            bool failTrans = false;

            float[] eulerDeltas = new float[3];
            float[] transDeltas = new float[3];

            for (int i = 0; i < 3; i++)
            {
                // Handle when one angle is near PI, and the other is near -PI.
                if (eulerInitial[i] >= Math.PI - deltaRot && eulerFinal[i] < deltaRot - Math.PI)
                {
                    eulerInitial[i] -= (float)(Math.PI * 2);
                }
                else if (eulerFinal[i] >= Math.PI - deltaRot && eulerInitial[i] < deltaRot - Math.PI)
                {
                    eulerFinal[i] -= (float)(Math.PI * 2);
                }

                eulerDeltas[i] = eulerInitial[i] - eulerFinal[i];
                transDeltas[i] = transInitial[i] - transFinal[i];

                if (Math.Abs(eulerDeltas[i]) > deltaRot)
                {
                    failRot = true;
                    break;
                }

                if (Math.Abs(transDeltas[i]) > deltaTrans)
                {
                    failTrans = true;
                    break;
                }
            }

            return failRot || failTrans;
        }

        /// <summary>
        /// Invert/Transpose the 3x3 Rotation Matrix Component of a 4x4 matrix in place
        /// </summary>
        /// <param name="rot">The rotation matrix to invert.</param>
        public static void InvertRotation(ref Matrix4 rot)
        {
            // Invert equivalent to a transpose for 3x3 rotation when orthogonal
            float tmp = rot.M12;
            rot.M12 = rot.M21;
            rot.M21 = tmp;

            tmp = rot.M13;
            rot.M13 = rot.M31;
            rot.M31 = tmp;

            tmp = rot.M23;
            rot.M23 = rot.M32;
            rot.M32 = tmp;
        }

        /// <summary>
        /// Negate the 3x3 Rotation Matrix Component of a 4x4 matrix in place
        /// </summary>
        /// <param name="rot">The rotation matrix to negate.</param>
        public static void NegateRotation(ref Matrix4 rot)
        {
            rot.M11 = -rot.M11;
            rot.M12 = -rot.M12;
            rot.M13 = -rot.M13;

            rot.M21 = -rot.M21;
            rot.M22 = -rot.M22;
            rot.M23 = -rot.M23;

            rot.M31 = -rot.M31;
            rot.M32 = -rot.M32;
            rot.M33 = -rot.M33;
        }

        /// <summary>
        /// Length of a Vector3
        /// </summary>
        /// <param name="vector">The Vector3.</param>
        /// <returns>Returns a the Vector3 length as float value</returns>
        public static float Length(Vector3 vector)
        {
            return (float)Math.Sqrt((vector.X * vector.X) + (vector.Y * vector.Y) + (vector.Z * vector.Z));
        }

        /// <summary>
        /// Normalize a Vector3
        /// </summary>
        /// <param name="vector">The Vector3.</param>
        /// <returns>Returns a normalized Vector3</returns>
        public static Vector3 Normalize(Vector3 vector)
        {
            Vector3 result = new Vector3();

            float oneOverlen = 1.0f / Length(vector);

            result.X = vector.X * oneOverlen;
            result.Y = vector.Y * oneOverlen;
            result.Z = vector.Z * oneOverlen;

            return result;
        }

        /// <summary>
        /// Normalize a Vector3 in place
        /// </summary>
        /// <param name="vector">The Vector3 to normalize.</param>
        public static void NormalizeInPlace(ref Vector3 vector)
        {
            float oneOverlen = 1.0f / Length(vector);
            vector.X *= oneOverlen;
            vector.Y *= oneOverlen;
            vector.Z *= oneOverlen;
        }

        /// <summary>
        /// Rotate a vector with the 3x3 Rotation Matrix Component of a 4x4 matrix
        /// </summary>
        /// <param name="vector">The Vector3 to rotate.</param>
        /// <param name="rotation">Rotation matrix.</param>
        /// <returns>The rotated Vector3.</returns>
        public static Vector3 RotateVector(Vector3 vector, Matrix4 rotation)
        {
            // we only use the rotation component here
            Vector3 result = new Vector3();

            // Multiply row vector with column in mat
            result.X = (rotation.M11 * vector.X) + (rotation.M21 * vector.Y) + (rotation.M31 * vector.Z);
            result.Y = (rotation.M12 * vector.X) + (rotation.M22 * vector.Y) + (rotation.M32 * vector.Z);
            result.Z = (rotation.M13 * vector.X) + (rotation.M23 * vector.Y) + (rotation.M33 * vector.Z);

            return result;
        }

        /// <summary>
        /// Create Right Hand rotation matrix for counter-clockwise rotation of angle around axis
        /// </summary>
        /// <param name="axis">Rotation axis as Vector3</param>
        /// <param name="angle">Angle of rotation around axis in Radians</param>
        /// <returns>A Matrix4 rotation matrix.</returns>
        public static Matrix4 CreateRotationMatrixFromAxisAngle(Vector3 axis, float angle)
        {
            // initialize to identity
            Matrix4 result = Matrix4.Identity;

            // angle is in radians
            if (Length(axis) != 0 && angle != 0)
            {
                // Assumes normalized axis
                float c = (float)Math.Cos(angle);
                float s = (float)Math.Sin(angle);
                float t = 1.0f - c;

                result.M11 = c + (axis.X * axis.X * t);
                result.M22 = c + (axis.Y * axis.Y * t);
                result.M33 = c + (axis.Z * axis.Z * t);

                float tmp1 = axis.X * axis.Y * t;
                float tmp2 = axis.Z * s;

                result.M12 = tmp1 + tmp2;
                result.M21 = tmp1 - tmp2;

                tmp1 = axis.X * axis.Z * t;
                tmp2 = axis.Y * s;

                result.M13 = tmp1 - tmp2;
                result.M31 = tmp1 + tmp2;

                tmp1 = axis.Y * axis.Z * t;
                tmp2 = axis.X * s;

                result.M23 = tmp1 + tmp2;
                result.M32 = tmp1 - tmp2;
            }

            return result;
        }

        /// <summary>
        /// Multiply4 with another Matrix4
        /// </summary>
        /// <param name="matA">First Matrix4.</param>
        /// <param name="matB">Second Matrix4.</param>/// 
        /// <returns>Returns multiplication result Matrix.</returns>
        public static Matrix4 MultiplyMatrix4(Matrix4 matA, Matrix4 matB)
        {
            Matrix4 result = Matrix4.Identity;

            result.M11 = (matA.M11 * matB.M11) + (matA.M12 * matB.M21) + (matA.M13 * matB.M31) + (matA.M14 * matB.M41);
            result.M12 = (matA.M11 * matB.M12) + (matA.M12 * matB.M22) + (matA.M13 * matB.M32) + (matA.M14 * matB.M42);
            result.M13 = (matA.M11 * matB.M13) + (matA.M12 * matB.M23) + (matA.M13 * matB.M33) + (matA.M14 * matB.M43);
            result.M14 = (matA.M11 * matB.M14) + (matA.M12 * matB.M24) + (matA.M13 * matB.M34) + (matA.M14 * matB.M44);
            result.M21 = (matA.M21 * matB.M11) + (matA.M22 * matB.M21) + (matA.M23 * matB.M31) + (matA.M24 * matB.M41);
            result.M22 = (matA.M21 * matB.M12) + (matA.M22 * matB.M22) + (matA.M23 * matB.M32) + (matA.M24 * matB.M42);
            result.M23 = (matA.M21 * matB.M13) + (matA.M22 * matB.M23) + (matA.M23 * matB.M33) + (matA.M24 * matB.M43);
            result.M24 = (matA.M21 * matB.M14) + (matA.M22 * matB.M24) + (matA.M23 * matB.M34) + (matA.M24 * matB.M44);
            result.M31 = (matA.M31 * matB.M11) + (matA.M32 * matB.M21) + (matA.M33 * matB.M31) + (matA.M34 * matB.M41);
            result.M32 = (matA.M31 * matB.M12) + (matA.M32 * matB.M22) + (matA.M33 * matB.M32) + (matA.M34 * matB.M42);
            result.M33 = (matA.M31 * matB.M13) + (matA.M32 * matB.M23) + (matA.M33 * matB.M33) + (matA.M34 * matB.M43);
            result.M34 = (matA.M31 * matB.M14) + (matA.M32 * matB.M24) + (matA.M33 * matB.M34) + (matA.M34 * matB.M44);
            result.M41 = (matA.M41 * matB.M11) + (matA.M42 * matB.M21) + (matA.M43 * matB.M31) + (matA.M44 * matB.M41);
            result.M42 = (matA.M41 * matB.M12) + (matA.M42 * matB.M22) + (matA.M43 * matB.M32) + (matA.M44 * matB.M42);
            result.M43 = (matA.M41 * matB.M13) + (matA.M42 * matB.M23) + (matA.M43 * matB.M33) + (matA.M44 * matB.M43);
            result.M44 = (matA.M41 * matB.M14) + (matA.M42 * matB.M24) + (matA.M43 * matB.M34) + (matA.M44 * matB.M44);

            return result;
        }

        /// <summary>
        /// Multiply Rotation 3x3 with another Rotation 3x3 inside Matrix4s
        /// </summary>
        /// <param name="matA">First Matrix4.</param>
        /// <param name="matB">Second Matrix4.</param>/// 
        /// <returns>Returns multiplication result Matrix</returns>
        public static Matrix4 MultiplyRotationMatrix4(Matrix4 matA, Matrix4 matB)
        {
            Matrix4 result = Matrix4.Identity;

            result.M11 = (matA.M11 * matB.M11) + (matA.M12 * matB.M21) + (matA.M13 * matB.M31);
            result.M12 = (matA.M11 * matB.M12) + (matA.M12 * matB.M22) + (matA.M13 * matB.M32);
            result.M13 = (matA.M11 * matB.M13) + (matA.M12 * matB.M23) + (matA.M13 * matB.M33);

            result.M21 = (matA.M21 * matB.M11) + (matA.M22 * matB.M21) + (matA.M23 * matB.M31);
            result.M22 = (matA.M21 * matB.M12) + (matA.M22 * matB.M22) + (matA.M23 * matB.M32);
            result.M23 = (matA.M21 * matB.M13) + (matA.M22 * matB.M23) + (matA.M23 * matB.M33);

            result.M31 = (matA.M31 * matB.M11) + (matA.M32 * matB.M21) + (matA.M33 * matB.M31);
            result.M32 = (matA.M31 * matB.M12) + (matA.M32 * matB.M22) + (matA.M33 * matB.M32);
            result.M33 = (matA.M31 * matB.M13) + (matA.M32 * matB.M23) + (matA.M33 * matB.M33);

            return result;
        }

        /// <summary>
        /// Create 4x4 matrix with rotation around X-axis
        /// </summary>
        /// <param name="angleDegrees">The angle to rotate.</param>
        /// <returns>Returns a Matrix4 containing the rotation matrix.</returns>
        public static Matrix4 CreateRotationMatrixX(float angleDegrees)
        {
            Matrix4 result = Matrix4.Identity;

            float angleRads = angleDegrees / 180.0f * (float)Math.PI;

            // Note this is for XNA-like row vector multiplication with row-major matrices
            result.M22 = (float)Math.Cos(angleRads);
            result.M23 = (float)Math.Sin(angleRads);

            result.M32 = -(float)Math.Sin(angleRads);
            result.M33 = (float)Math.Cos(angleRads);

            return result;
        }

        /// <summary>
        /// Create 4x4 matrix with rotation around Y-axis
        /// </summary>
        /// <param name="angleDegrees">The angle to rotate.</param>
        /// <returns>Returns a Matrix4 containing the rotation matrix.</returns>
        public static Matrix4 CreateRotationMatrixY(float angleDegrees)
        {
            Matrix4 result = Matrix4.Identity;

            float angleRads = angleDegrees / 180.0f * (float)Math.PI;

            // Note this is for XNA-like row vector multiplication with row-major matrices
            result.M11 = (float)Math.Cos(angleRads);
            result.M13 = -(float)Math.Sin(angleRads);

            result.M31 = (float)Math.Sin(angleRads);
            result.M33 = (float)Math.Cos(angleRads);

            return result;
        }

        /// <summary>
        /// Create 4x4 matrix with rotation around Z-axis
        /// </summary>
        /// <param name="angleDegrees">The angle to rotate.</param>
        /// <returns>Returns a Matrix4 containing the rotation matrix.</returns>
        public static Matrix4 CreateRotationMatrixZ(float angleDegrees)
        {
            Matrix4 result = Matrix4.Identity;

            float angleRads = angleDegrees / 180.0f * (float)Math.PI;

            // Note this is for XNA-like row vector multiplication with row-major matrices
            result.M11 = (float)Math.Cos(angleRads);
            result.M12 = (float)Math.Sin(angleRads);

            result.M21 = -(float)Math.Sin(angleRads);
            result.M22 = (float)Math.Cos(angleRads);

            return result;
        }

        /// <summary>
        /// Invert Matrix4 Pose either from WorldToCameraTransform (view) matrix to CameraToWorldTransform camera pose matrix (world/SE3) or vice versa
        /// </summary>
        /// <param name="transform">The camera pose transform matrix.</param>
        /// <returns>Returns a Matrix4 containing the inverted camera pose.</returns>
        public static Matrix4 InvertTransformationMatrixPose(Matrix4 transform)
        {
            // Given the SE3 world transform transform T = [R|t], the inverse view transform matrix is simply:
            // T^-1 = [R^T | -R^T . t ]
            // This also works the opposite way to get the world transform, given the view transform matrix.
            Matrix4 rotation = ExtractRotationMatrix(transform);

            Matrix4 invRotation = rotation;
            InvertRotation(ref invRotation);  // invert(transpose) 3x3 rotation

            Matrix4 negRotation = invRotation;
            NegateRotation(ref negRotation);  // negate 3x3 rotation

            Vector3 translation = ExtractTranslationVector3(transform);
            Vector3 invTranslation = RotateVector(translation, negRotation);

            // Add the translation back in
            invRotation.M41 = invTranslation.X;
            invRotation.M42 = invTranslation.Y;
            invRotation.M43 = invTranslation.Z;

            return invRotation;
        }

        /// <summary>
        /// Back-project 3D point assuming Z is the depth (parallel to the optical axis).
        /// </summary>
        /// <param name="x">Input x in pixels of 2D image.</param>
        /// <param name="y">Input y in pixels of 2D image.</param>
        /// <param name="z">Input depth to calculate 3D point for.</param>
        /// <param name="principalPointX">Principal point in x axis (non-normalized).</param>
        /// <param name="principalPointY">Principal point in y axis (non-normalized).</param>
        /// <param name="inverseFocalLengthX">Inverse focal length in X (non-normalized).</param>
        /// <param name="inverseFocalLengthY">Inverse focal length in Y (non-normalized).</param>
        /// <returns>Returns the back-projected Point3D.</returns>
        public static Point3D BackProject(float x, float y, float z, float principalPointX, float principalPointY, float inverseFocalLengthX, float inverseFocalLengthY)
        {
            Point3D pt = new Point3D();

            float u = (x - principalPointX) * inverseFocalLengthX;
            float v = (y - principalPointY) * inverseFocalLengthY;

            // Assuming depths are measurements along Z.
            pt.X = u * z;
            pt.Y = v * z;
            pt.Z = z;

            return pt;
        }
    }
}