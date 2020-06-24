/*MIT License

Copyright (c) 2018 MTA SZTAKI

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.*/

#include "apeJNIPlugin.h"
#include "apeIPlaneGeometry.h"

extern "C"
JNIEXPORT void JNICALL
Java_org_apertusvr_ApertusJNI_setPlaneGeometryParameters(
        JNIEnv *env, jclass clazz,
        jstring native_plane_geometry, float num_seg_x, float num_seg_y,
        float size_x, float size_y, float tile_x, float tile_y)
{

}

extern "C"
JNIEXPORT jfloatArray JNICALL
Java_org_apertusvr_ApertusJNI_getPlaneGeometryNumSeg(JNIEnv *env, jclass clazz, jstring native_plane_geometry)
{

}

extern "C"
JNIEXPORT jfloatArray JNICALL
Java_org_apertusvr_ApertusJNI_getPlaneGeometrySize(JNIEnv *env, jclass clazz, jstring native_plane_geometry)
{

}

extern "C"
JNIEXPORT jfloatArray JNICALL
Java_org_apertusvr_ApertusJNI_getPlaneGeometryTile(JNIEnv *env, jclass clazz, jstring native_plane_geometry)
{

}

extern "C"
JNIEXPORT void JNICALL
Java_org_apertusvr_ApertusJNI_setPlaneGeometryParentNode(
        JNIEnv *env, jclass clazz,
        jstring native_plane_geometry, jstring parent_node_name)
{

}

extern "C"
JNIEXPORT void JNICALL
Java_org_apertusvr_ApertusJNI_setPlaneGeometryMaterial(
        JNIEnv *env, jclass clazz,
        jstring native_plane_geometry, jstring native_material)
{

}

extern "C"
JNIEXPORT jstring JNICALL
Java_org_apertusvr_ApertusJNI_getPlaneGeometryMaterial(JNIEnv *env, jclass clazz, jstring native_plane_geometry)
{

}

extern "C"
JNIEXPORT void JNICALL
Java_org_apertusvr_ApertusJNI_setPlaneGeometryOwner(
        JNIEnv *env, jclass clazz,
        jstring native_plane_geometry, jstring native_owner)
{

}

extern "C"
JNIEXPORT jstring JNICALL
Java_org_apertusvr_ApertusJNI_getPlaneGeometryOwner(JNIEnv *env, jclass clazz, jstring native_plane_geometry)
{

}