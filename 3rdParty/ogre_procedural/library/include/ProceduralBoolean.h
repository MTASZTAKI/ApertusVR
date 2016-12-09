/*
-----------------------------------------------------------------------------
This source file is part of ogre-procedural

For the latest info, see http://code.google.com/p/ogre-procedural/

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
#include "Ogre.h"
#include "ProceduralPlatform.h"
#include "ProceduralMeshGenerator.h"

namespace Procedural
{

class _ProceduralExport Boolean : public MeshGenerator<Boolean>
{
public:
	enum BooleanOperation
	{
	    BT_UNION, BT_INTERSECTION, BT_DIFFERENCE
	};
private:
	BooleanOperation mBooleanOperation;
	TriangleBuffer* mMesh1;
	TriangleBuffer* mMesh2;
public:

	Boolean() : mMesh1(0), mMesh2(0), mBooleanOperation(BT_UNION) {}

	Boolean& setMesh1(TriangleBuffer* tb)
	{
		mMesh1 = tb;
		return *this;
	}

	Boolean& setMesh2(TriangleBuffer* tb)
	{
		mMesh2 = tb;
		return *this;
	}

	Boolean& setBooleanOperation(BooleanOperation op)
	{
		mBooleanOperation = op;
		return *this;
	}

	void addToTriangleBuffer(TriangleBuffer& buffer) const;
};
}