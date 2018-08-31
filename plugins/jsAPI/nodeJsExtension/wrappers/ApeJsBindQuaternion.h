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

#ifndef APE_JSBIND_QUATERNION_H
#define APE_JSBIND_QUATERNION_H

#include "nbind/nbind.h"
#include "nbind/api.h"
#include "ApeQuaternion.h"

using namespace Ape;

#ifdef NBIND_CLASS

NBIND_CLASS(Quaternion)
{
	construct<>();
	construct<float, float, float, float>();
	construct<Degree, Vector3>();
	construct<Radian, Vector3>();

	method(getW);
	method(getX);
	method(getY);
	method(getZ);

	// TODO: check/fix
	//multimethod(FromAngleAxis, args(Radian, Vector3));
	//multimethod(FromAngleAxis, args(Degree, Vector3));
	method(equals);
	method(Norm);
	method(normalise);
	method(Inverse);
	method(Dot);
	method(Slerp);

	method(toString);
	method(toJsonString);
	method(toVector);
}

#endif

#endif
