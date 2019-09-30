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
#ifndef PROCEDURAL_MULTISHAPE_INCLUDED
#define PROCEDURAL_MULTISHAPE_INCLUDED

#include "ProceduralGeometryHelpers.h"

namespace Procedural
{
class Shape;

/**
 * \ingroup shapegrp
 * Holds a bunch of shapes.
 * There are a number of assumptions that are made and are not checked
 * against : the shapes must not cross each other
 *
 */
class _ProceduralExport MultiShape
{
	std::vector<Shape> mShapes;

public:
	/// Default constructor
	MultiShape()
	{}

	/// Constructor from a single shape
	MultiShape(const Shape& shape)
	{
		mShapes.push_back(shape);
	}

	/// Constructor from a variable number of shapes
	/// @param count the number of shapes to add
	/// @param ... pointer to the shapes to add
	MultiShape(int count, ...);

	/// Adds a shape to the list of shapes
	MultiShape& addShape(const Shape& shape)
	{
		mShapes.push_back(shape);
		return *this;
	}

	/// Clears all the content
	void clear()
	{
		mShapes.clear();
	}

	/// Returns the i-th shape
	const Shape& getShape(unsigned int i) const
	{
		return mShapes[i];
	}

	/// Returns the i-th shape
	Shape& getShape(unsigned int i)
	{
		return mShapes[i];
	}

	/// Builds an aggregated list of all points contained in all shapes
	std::vector<Ogre::Vector2> getPoints() const;

	/// Returns the number of shapes in that MultiShape
	unsigned int getShapeCount() const
	{
		return mShapes.size();
	}

	/// Append every shape of an other multishape to the current multiShape
	void addMultiShape(const MultiShape& other)
	{
		for (std::vector<Shape>::const_iterator it = other.mShapes.begin(); it!=other.mShapes.end(); ++it)
		{
			mShapes.push_back(*it);
		}
	}

	/// Outputs the Multi Shape to a Mesh, mostly for visualisation or debugging purposes
	Ogre::MeshPtr realizeMesh(const std::string& name="");

	/// Tells whether a point is located inside that multishape
	/// It assumes that all of the shapes in that multishape are closed,
	/// and that they don't contradict each other,
	/// ie a point cannot be outside and inside at the same time
	bool isPointInside(const Ogre::Vector2& point) const;

	/**
	 * Tells whether multishape is "closed" or not.
	 * MultiShape is considered to be closed if and only if all shapes are closed
	 */
	bool isClosed() const;

	/**
	 * Closes all shapes included in this multiShape
	 */
	void close();

	/**
	 * Determines whether the outside as defined by user equals "real" outside
	 */
	bool isOutsideRealOutside() const;

	void buildFromSegmentSoup(const std::vector<Segment2D>& segList);

};
}
#endif
