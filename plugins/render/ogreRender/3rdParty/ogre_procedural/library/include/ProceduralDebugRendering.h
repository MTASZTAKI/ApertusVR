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
#ifndef PROCEDURAL_DEBUG_RENDERING_INCLUDED
#define PROCEDURAL_DEBUG_RENDERING_INCLUDED

#include "ProceduralPlatform.h"
#include "ProceduralTriangleBuffer.h"
#include "OgreManualObject.h"

namespace Procedural
{
/// This class creates a visualisation of the normals of a TriangleBuffer
class _ProceduralExport ShowNormalsGenerator
{
public:
	enum VisualStyle
	{
	    VS_LINE, VS_ARROW
	};

private:
	VisualStyle mVisualStyle;

	TriangleBuffer* mTriangleBuffer;

	Ogre::Real mSize;
public:
	ShowNormalsGenerator() : mTriangleBuffer(0), mSize(1.0), mVisualStyle(VS_LINE) {}

	/// Sets the input Triangle Buffer
	ShowNormalsGenerator& setTriangleBuffer(TriangleBuffer* triangleBuffer)
	{
		mTriangleBuffer = triangleBuffer;
		return *this;
	}

	/// Sets the size of the normals representation (default = 1.0)
	ShowNormalsGenerator& setSize(Ogre::Real size)
	{
		mSize = size;
		return *this;
	}


	/// Sets the visual style, line or arrow (default = line)
	ShowNormalsGenerator& setVisualStyle(VisualStyle visualStyle)
	{
		mVisualStyle = visualStyle;
		return *this;
	}


	/// Builds the normals representation as a manual object
	/// \exception Ogre::InvalidStateException The input triangle buffer must not be null
	/// \exception Ogre::InvalidStateException Scene Manager is not set in OGRE root object
	Ogre::ManualObject* buildManualObject() const;

	/// Builds the normals representation as a mesh
	Ogre::MeshPtr buildMesh(const std::string& name = "",
	                        const Ogre::String& group = "General") const;
};
}

#endif