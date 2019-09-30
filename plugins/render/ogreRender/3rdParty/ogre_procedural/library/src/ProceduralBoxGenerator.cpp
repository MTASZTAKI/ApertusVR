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
#include "ProceduralBoxGenerator.h"
#include "ProceduralPlaneGenerator.h"
#include "ProceduralUtils.h"

using namespace Ogre;

namespace Procedural
{
#define TAG_NEGX "box.negx"
#define TAG_NEGY "box.negy"
#define TAG_NEGZ "box.negz"
#define TAG_X "box.x"
#define TAG_Y "box.y"
#define TAG_Z "box.z"

void BoxGenerator::addToTriangleBuffer(TriangleBuffer& buffer) const
{
	PlaneGenerator pg;
	pg.setUTile(mUTile).setVTile(mVTile);
	if (mTransform)
	{
		pg.setScale(mScale);
		pg.setOrientation(mOrientation);
	}
	TriangleBuffer::Section section = buffer.beginSection(TAG_NEGZ);
	pg.setNumSegX(mNumSegY).setNumSegY(mNumSegX).setSizeX(mSizeY).setSizeY(mSizeX)
	.setNormal(Vector3::NEGATIVE_UNIT_Z)
	.setPosition(mScale*(mPosition+.5f*mSizeZ*(mOrientation*Vector3::NEGATIVE_UNIT_Z)))
	.addToTriangleBuffer(buffer);
	buffer.endSection(section);

	section = buffer.beginSection(TAG_Z);
	pg.setNumSegX(mNumSegY).setNumSegY(mNumSegX).setSizeX(mSizeY).setSizeY(mSizeX)
	.setNormal(Vector3::UNIT_Z)
	.setPosition(mScale*(mPosition+.5f*mSizeZ*(mOrientation*Vector3::UNIT_Z)))
	.addToTriangleBuffer(buffer);
	buffer.endSection(section);

	section = buffer.beginSection(TAG_NEGY);
	pg.setNumSegX(mNumSegZ).setNumSegY(mNumSegX).setSizeX(mSizeZ).setSizeY(mSizeX)
	.setNormal(Vector3::NEGATIVE_UNIT_Y)
	.setPosition(mScale*(mPosition+.5f*mSizeY*(mOrientation*Vector3::NEGATIVE_UNIT_Y)))
	.addToTriangleBuffer(buffer);
	buffer.endSection(section);

	section = buffer.beginSection(TAG_Y);
	pg.setNumSegX(mNumSegZ).setNumSegY(mNumSegX).setSizeX(mSizeZ).setSizeY(mSizeX)
	.setNormal(Vector3::UNIT_Y)
	.setPosition(mScale*(mPosition+.5f*mSizeY*(mOrientation*Vector3::UNIT_Y)))
	.addToTriangleBuffer(buffer);
	buffer.endSection(section);

	section = buffer.beginSection(TAG_NEGX);
	pg.setNumSegX(mNumSegZ).setNumSegY(mNumSegY).setSizeX(mSizeZ).setSizeY(mSizeY)
	.setNormal(Vector3::NEGATIVE_UNIT_X)
	.setPosition(mScale*(mPosition+.5f*mSizeX*(mOrientation*Vector3::NEGATIVE_UNIT_X)))
	.addToTriangleBuffer(buffer);
	buffer.endSection(section);

	section = buffer.beginSection(TAG_X);
	pg.setNumSegX(mNumSegZ).setNumSegY(mNumSegY).setSizeX(mSizeZ).setSizeY(mSizeY)
	.setNormal(Vector3::UNIT_X)
	.setPosition(mScale*(mPosition+.5f*mSizeX*(mOrientation*Vector3::UNIT_X)))
	.addToTriangleBuffer(buffer);
	buffer.endSection(section);
}
}
