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
#ifndef PROCEDURAL_TRIANGLEBUFFER_INCLUDED
#define PROCEDURAL_TRIANGLEBUFFER_INCLUDED

#include "ProceduralUtils.h"

namespace Procedural
{
/** This is ogre-procedural's temporary mesh buffer.
 * It stores all the info needed to build an Ogre Mesh, yet is intented to be more flexible, since
 * there is no link towards hardware.
 */
class _ProceduralExport TriangleBuffer
{
public:
	struct Vertex
	{
		Ogre::Vector3 mPosition;
		Ogre::Vector3 mNormal;
		Ogre::Vector2 mUV;
	};
	struct Section
	{
		std::string mSectionName;
		unsigned int mFirstIndex;
		unsigned int mLastIndex;
		unsigned int mFirstVertex;
		unsigned int mLastVertex;
		TriangleBuffer* buffer;
	};
protected:

	std::vector<int> mIndices;

	std::vector<Vertex> mVertices;
	int globalOffset;
	int mEstimatedVertexCount;
	int mEstimatedIndexCount;
	Vertex* mCurrentVertex;

	std::map<std::string, Section> mSections;


public:
	TriangleBuffer() : globalOffset(0), mEstimatedVertexCount(0), mEstimatedIndexCount(0), mCurrentVertex(0)
	{}

	void append(const TriangleBuffer& other)
	{
		rebaseOffset();
		for (std::vector<int>::const_iterator it = other.mIndices.begin(); it != other.mIndices.end(); ++it)
			mIndices.push_back(globalOffset+ (*it));
		
		for (std::vector<Vertex>::const_iterator it = other.mVertices.begin(); it != other.mVertices.end(); ++it)
			mVertices.push_back(*it);
	}

	Section beginSection(std::string sectionName = "")
	{
		rebaseOffset();
		Section section;
		section.mSectionName = "";
		section.mFirstIndex = mIndices.size();
		section.mFirstVertex = mVertices.size();
		section.buffer = this;
		return section;
	}

	void endSection(Section& section)
	{
		section.mLastIndex = mIndices.size() - 1;
		section.mLastVertex = mVertices.size() - 1;
		if (section.mSectionName != "")
			mSections[section.mSectionName] = section;		
	}

	Section getFullSection()
	{
		Section section;
		section.mFirstIndex = 0;
		section.mLastIndex = mIndices.size() - 1;
		section.mFirstVertex = 0;
		section.mLastVertex = mVertices.size() - 1;
		section.mSectionName = "";
		section.buffer = this;
		return section;
	}

	/// Gets a modifiable reference to vertices
	std::vector<Vertex>& getVertices()
	{
		return mVertices;
	}

	/// Gets a non-modifiable reference to vertices
	const std::vector<Vertex>& getVertices() const
	{
		return mVertices;
	}

	/// Gets a modifiable reference to vertices
	std::vector<int>& getIndices()
	{
		return mIndices;
	}

	/// Gets a non-modifiable reference to indices
	const std::vector<int>& getIndices() const
	{
		return mIndices;
	}

	/**
	 * Rebase index offset : call that function before you add a new mesh to the triangle buffer
	 */
	void rebaseOffset()
	{
		globalOffset = mVertices.size();
	}

	/**
	 * Builds an Ogre Mesh from this buffer.
	 */
	Ogre::MeshPtr transformToMesh(const std::string& name,
	                              const Ogre::String& group = "General") const;

	/** Adds a new vertex to the buffer */
	inline TriangleBuffer& vertex(const Vertex& v)
	{
		mVertices.push_back(v);
		mCurrentVertex = &mVertices.back();
		return *this;
	}

	/** Adds a new vertex to the buffer */
	inline TriangleBuffer& vertex(const Ogre::Vector3& position, const Ogre::Vector3& normal, const Ogre::Vector2& uv)
	{
		Vertex v;
		v.mPosition = position;
		v.mNormal = normal;
		v.mUV = uv;
		mVertices.push_back(v);
		mCurrentVertex = &mVertices.back();
		return *this;
	}

	/** Adds a new vertex to the buffer */
	inline TriangleBuffer& position(const Ogre::Vector3& pos)
	{
		Vertex v;
		v.mPosition = pos;
		mVertices.push_back(v);
		mCurrentVertex = &mVertices.back();
		return *this;
	}

	/** Adds a new vertex to the buffer */
	inline TriangleBuffer& position(Ogre::Real x, Ogre::Real y, Ogre::Real z)
	{
		Vertex v;
		v.mPosition = Ogre::Vector3(x,y,z);
		mVertices.push_back(v);
		mCurrentVertex = &mVertices.back();
		return *this;
	}

	/** Sets the normal of the current vertex */
	inline TriangleBuffer& normal(const Ogre::Vector3& normal)
	{
		mCurrentVertex->mNormal = normal;
		return *this;
	}

	/** Sets the texture coordinates of the current vertex */
	inline TriangleBuffer& textureCoord(float u, float v)
	{
		mCurrentVertex->mUV = Ogre::Vector2(u,v);
		return *this;
	}

	/** Sets the texture coordinates of the current vertex */
	inline TriangleBuffer& textureCoord(const Ogre::Vector2& vec)
	{
		mCurrentVertex->mUV = vec;
		return *this;
	}

	/**
	 * Adds an index to the index buffer.
	 * Index is relative to the latest rebaseOffset().
	 */
	inline TriangleBuffer& index(int i)
	{
		mIndices.push_back(globalOffset+i);
		return *this;
	}

	/**
	 * Adds a triangle to the index buffer.
	 * Index is relative to the latest rebaseOffset().
	 */
	inline TriangleBuffer& triangle(int i1, int i2, int i3)
	{
		mIndices.push_back(globalOffset+i1);
		mIndices.push_back(globalOffset+i2);
		mIndices.push_back(globalOffset+i3);
		return *this;
	}

	/// Applies a matrix to transform all vertices inside the triangle buffer
	TriangleBuffer& applyTransform(const Ogre::Matrix4& matrix)
	{
		for (std::vector<Vertex>::iterator it = mVertices.begin(); it!=mVertices.end(); ++it)
		{
			it->mPosition = matrix * it->mPosition;
			it->mNormal = matrix * it->mNormal;
			it->mNormal.normalise();
		}
		return *this;
	}

	/// Applies the translation immediately to all the points contained in that triangle buffer
	/// @param amount translation vector
	TriangleBuffer& translate(const Ogre::Vector3& amount)
	{
		for (std::vector<Vertex>::iterator it = mVertices.begin(); it!=mVertices.end(); ++it)
		{
			it->mPosition += amount;
		}
		return *this;
	}

	/// Applies the translation immediately to all the points contained in that triangle buffer
	TriangleBuffer& translate(Ogre::Real x, Ogre::Real y, Ogre::Real z)
	{
		return translate(Ogre::Vector3(x, y, z));
	}

	/// Applies the rotation immediately to all the points contained in that triangle buffer
	/// @param quat the rotation quaternion to apply
	TriangleBuffer& rotate(Ogre::Quaternion quat)
	{
		for (std::vector<Vertex>::iterator it = mVertices.begin(); it!=mVertices.end(); ++it)
		{
			it->mPosition = quat * it->mPosition;
			it->mNormal = quat * it->mNormal;
			it->mNormal.normalise();
		}
		return *this;
	}

	/// Applies an immediate scale operation to that triangle buffer
	/// @param scale Scale vector
	TriangleBuffer& scale(const Ogre::Vector3& scale)
	{
		for (std::vector<Vertex>::iterator it = mVertices.begin(); it!=mVertices.end(); ++it)
		{
			it->mPosition = scale * it->mPosition;
		}
		return *this;
	}

	/// Applies an immediate scale operation to that triangle buffer
	/// @param x X scale component
	/// @param y Y scale component
	/// @param z Z scale component
	TriangleBuffer& scale(Ogre::Real x, Ogre::Real y, Ogre::Real z)
	{
		return scale(Ogre::Vector3(x,y,z));
	}

	/// Applies normal inversion on the triangle buffer
	TriangleBuffer& invertNormals()
	{
		for (std::vector<Vertex>::iterator it = mVertices.begin(); it!=mVertices.end(); ++it)
		{
			it->mNormal = -it->mNormal;
		}
		for (unsigned int i=0; i < mIndices.size(); ++i)
		{
			if (i%3==1)
			{
				std::swap(mIndices[i], mIndices[i-1]);
			}
		}
		return *this;
	}

	/**
	 * Gives an estimation of the number of vertices need for this triangle buffer.
	 * If this function is called several times, it means an extra vertices count, not an absolute measure.
	 */
	void estimateVertexCount(unsigned int vertexCount)
	{
		mEstimatedVertexCount += vertexCount;
		mVertices.reserve(mEstimatedVertexCount);
	}

	/**
	 * Gives an estimation of the number of indices needed for this triangle buffer.
	 * If this function is called several times, it means an extra indices count, not an absolute measure.
	 */
	void estimateIndexCount(unsigned int indexCount)
	{
		mEstimatedIndexCount += indexCount;
		mIndices.reserve(mEstimatedIndexCount);
	}
};
}
#endif
