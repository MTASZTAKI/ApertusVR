/*
-----------------------------------------------------------------------------
This source file is part of ogre-procedural

For the latest info, see http://www.ogreprocedural.org

Copyright (c) 2010-2013 Michael Broutin

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
-----------------------------------------------------------------------------
*/
#include "ProceduralStableHeaders.h"
#include "ProceduralUtils.h"
#include "OgreResourceGroupManager.h"

int Procedural::Utils::counter = 0;

namespace Procedural
{
using namespace Ogre;

void Utils::log(const Ogre::String& st)
{
	Ogre::LogManager::getSingleton().logMessage("[PROCEDURAL] " + st);
}

std::string Utils::getName(const std::string& prefix)
{
	counter++;
	return prefix + Ogre::StringConverter::toString(counter);
}

//-----------------------------------------------------------------------
Quaternion Utils::_computeQuaternion(const Ogre::Vector3& direction, const Ogre::Vector3& upVector)
{
	Quaternion q;
	Vector3 zVec = direction;
	zVec.normalise();
	Vector3 xVec = upVector.crossProduct( zVec );
	if (xVec.isZeroLength())
		xVec = Ogre::Vector3::UNIT_X;
	xVec.normalise();
	Vector3 yVec = zVec.crossProduct( xVec );
	yVec.normalise();
	q.FromAxes( xVec, yVec, zVec);
	return q;
}
}
