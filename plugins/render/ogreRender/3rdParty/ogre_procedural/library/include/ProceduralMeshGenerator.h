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
#ifndef PROCEDURAL_MESH_GENERATOR_INCLUDED
#define PROCEDURAL_MESH_GENERATOR_INCLUDED

#include "OgreRoot.h"
#include "ProceduralPlatform.h"
#include "ProceduralTriangleBuffer.h"
#include "OgreException.h"
#include "OgreMesh.h"

namespace Procedural
{
/**
\defgroup objgengrp Object generators
Elements for procedural mesh generation of various objects.
@{
@}
*/

/**
\ingroup objgengrp
Superclass of everything that builds meshes
 */
template <typename T>
class MeshGenerator
{
protected:
	/// A pointer to the default scene manager
	//Ogre::SceneManager* mSceneMgr;

	/// U tile for texture coords generation
	Ogre::Real mUTile;

	/// V tile for texture coords generation
	Ogre::Real mVTile;

	/// Whether to produces normals or not
	bool mEnableNormals;

	/// The number of texture coordinate sets to include
	unsigned char mNumTexCoordSet;

	/// Rectangle in which the texture coordinates will be placed
	Ogre::Vector2 mUVOrigin;

	/// If set to true, the UV coordinates coming from the mesh generator will be switched.
	/// It can be used, for example, if your texture doesn't fit the mesh generator's assumptions about UV.
	/// If UV were to fit in a given rectangle, they still fit in it after the switch.
	bool mSwitchUV;

	/// Orientation to apply the mesh
	Ogre::Quaternion mOrientation;

	/// Scale to apply the mesh
	Ogre::Vector3 mScale;

	/// Position to apply to the mesh
	Ogre::Vector3 mPosition;

	// Whether a transform has been defined or not
	bool mTransform;

public:
	/// Default constructor
	/// \exception Ogre::InvalidStateException Scene Manager is not set in OGRE root object
	MeshGenerator() : mUTile(1.f),
		mVTile(1.f),
		mEnableNormals(true),
		mNumTexCoordSet(1),
		mUVOrigin(0,0),
		mSwitchUV(false),
		mOrientation(Ogre::Quaternion::IDENTITY),
		mScale(1,1,1),
		mPosition(0,0,0),
		mTransform(false)
	{
	}

	/**
	 * Builds a mesh.
	 * @param name of the mesh for the MeshManager
	 * @param group ressource group in which the mesh will be created
	 */
	Ogre::MeshPtr realizeMesh(const std::string& name = "",
	                          const Ogre::String& group = "General")
	{
		TriangleBuffer tbuffer;
		addToTriangleBuffer(tbuffer);
		Ogre::MeshPtr mesh;
		if (name == "")
			mesh = tbuffer.transformToMesh(Utils::getName(), group);
		else
			mesh = tbuffer.transformToMesh(name, group);
		return mesh;
	}

	/**
	 * Outputs a triangleBuffer
	 */
	TriangleBuffer buildTriangleBuffer() const
	{
		TriangleBuffer tbuffer;
		addToTriangleBuffer(tbuffer);
		return tbuffer;
	}

	/**
	 * Overloaded by each generator to implement the specifics
	 */
	virtual void addToTriangleBuffer(TriangleBuffer& buffer) const=0;

	/**
	 * Sets U Tile, ie the number by which u texture coordinates are multiplied (default=1)
	 */
	inline T& setUTile(Ogre::Real uTile)
	{
		mUTile = uTile;
		return static_cast<T&>(*this);
	}

	/**
	 * Sets V Tile, ie the number by which v texture coordinates are multiplied (default=1)
	 */
	inline T& setVTile(Ogre::Real vTile)
	{
		mVTile = vTile;
		return static_cast<T&>(*this);
	}

	/**
	 * Sets the texture rectangle
	 */
	inline T& setTextureRectangle(const Ogre::RealRect& textureRectangle)
	{
		mUVOrigin = Ogre::Vector2(textureRectangle.top, textureRectangle.left);
		mUTile = textureRectangle.right-textureRectangle.left;
		mVTile = textureRectangle.bottom-textureRectangle.top;
		return static_cast<T&>(*this);
	}

	/**
	 * Sets whether normals are enabled or not (default=true)
	 */
	inline T& setEnableNormals(bool enableNormals)
	{
		mEnableNormals = enableNormals;
		return static_cast<T&>(*this);
	}

	/**
	 * Sets the number of texture coordintate sets (default=1)
	 */
	inline T& setNumTexCoordSet(unsigned char numTexCoordSet)
	{
		mNumTexCoordSet = numTexCoordSet;
		return static_cast<T&>(*this);
	}

	/// Sets whether to switch U and V texture coordinates
	inline T& setSwitchUV(bool switchUV)
	{
		mSwitchUV = switchUV;
		return static_cast<T&>(*this);
	}

	/// Sets an orientation to give when building the mesh
	inline T& setOrientation(const Ogre::Quaternion& orientation)
	{
		mOrientation = orientation;
		mTransform = true;
		return static_cast<T&>(*this);
	}

	/// Sets a translation baked into the resulting mesh
	inline T& setPosition(const Ogre::Vector3& position)
	{
		mPosition = position;
		mTransform = true;
		return static_cast<T&>(*this);
	}

	/// Sets a translation baked into the resulting mesh
	inline T& setPosition(Ogre::Real x, Ogre::Real y, Ogre::Real z)
	{
		mPosition = Ogre::Vector3(x, y, z);
		mTransform = true;
		return static_cast<T&>(*this);
	}


	/// Sets a scale baked into the resulting mesh
	inline T& setScale(const Ogre::Vector3& scale)
	{
		mScale = scale;
		mTransform = true;
		return static_cast<T&>(*this);
	}

	/// Sets a uniform scale baked into the resulting mesh
	inline T& setScale(Ogre::Real scale)
	{
		mScale = Ogre::Vector3(scale);
		mTransform = true;
		return static_cast<T&>(*this);
	}

	/// Sets a scale baked into the resulting mesh
	inline T& setScale(Ogre::Real x, Ogre::Real y, Ogre::Real z)
	{
		mScale = Ogre::Vector3(x, y, z);
		mTransform = true;
		return static_cast<T&>(*this);
	}

	/// Resets all transforms (orientation, position and scale) that would have been applied to the mesh to their default values
	inline T& resetTransforms()
	{
		mTransform = false;
		mPosition = Ogre::Vector3::ZERO;
		mOrientation = Ogre::Quaternion::IDENTITY;
		mScale = Ogre::Vector3(1);
		return static_cast<T&>(*this);
	}

protected:
	/// Adds a new point to a triangle buffer, using the format defined for that MeshGenerator
	/// @param buffer the triangle buffer to update
	/// @param position the position of the new point
	/// @param normal the normal of the new point
	/// @param uv the uv texcoord of the new point
	inline void addPoint(TriangleBuffer& buffer, const Ogre::Vector3& position, const Ogre::Vector3& normal, const Ogre::Vector2& uv) const
	{
		if (mTransform)
			buffer.position(mPosition + mOrientation * (mScale * position));
		else
			buffer.position(position);
		if (mEnableNormals)
		{
			if (mTransform)
				buffer.normal(mOrientation * normal);
			else
				buffer.normal(normal);
		}
		if (mSwitchUV)
			for (unsigned char i=0; i<mNumTexCoordSet; i++)
				buffer.textureCoord(mUVOrigin.x + uv.y*mUTile, mUVOrigin.y+uv.x*mVTile);
		else
			for (unsigned char i=0; i<mNumTexCoordSet; i++)
				buffer.textureCoord(mUVOrigin.x + uv.x*mUTile, mUVOrigin.y+uv.y*mVTile);
	}

};
//
}
#endif
