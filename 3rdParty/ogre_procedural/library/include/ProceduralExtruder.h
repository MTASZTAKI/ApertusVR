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
#ifndef PROCEDURAL_EXTRUDER_INCLUDED
#define PROCEDURAL_EXTRUDER_INCLUDED

#include "ProceduralShape.h"
#include "ProceduralPath.h"
#include "ProceduralPlatform.h"
#include "ProceduralMeshGenerator.h"
#include "ProceduralMultiShape.h"
#include "ProceduralTrack.h"

namespace Procedural
{
/**
 * Extrudes a 2D shape along a path to build an extruded mesh.
 * Can be used to build things such as pipelines, roads...
 * <table border="0" width="100%"><tr><td>\image html extruder_generic.png "Generic extrusion"</td>
 * <td>\image html extruder_rotationtrack.png "Extrusion with rotation track"</td></tr>
 * <tr><td>\image html extruder_scaletrack.png "Extrusion with scale track"</td>
 * <td>\image html extruder_texturetrack.png "Extrusion with texture track"</td></tr>
 * <tr><td>\image html extruder_multishape.png "Multishape extrusion"</td><td>&nbsp;</td></tr></table>
 * \note Concerning UV texCoords, U is along the path and V along the shape.
 */
class _ProceduralExport Extruder : public MeshGenerator<Extruder>
{
public:
	typedef std::map<unsigned int, const Track*> TrackMap;
private:
	MultiShape mMultiShapeToExtrude;
	MultiPath mMultiExtrusionPath;
	bool mCapped;	
	
	TrackMap mRotationTracks;
	TrackMap mScaleTracks;
	TrackMap mShapeTextureTracks;
	TrackMap mPathTextureTracks;

public:
	/// Default constructor
	Extruder() : mCapped(true)
	{}

	/**
	 * Builds the mesh into the given TriangleBuffer
	 * @param buffer The TriangleBuffer on where to append the mesh.
	 * @exception Ogre::InvalidStateException Either shape or multishape must be defined!
	 * @exception Ogre::InvalidStateException Required parameter is zero!
	 */
	void addToTriangleBuffer(TriangleBuffer& buffer) const;

	/** Sets the shape to extrude. Mutually exclusive with setMultiShapeToExtrude. */
	inline Extruder& setShapeToExtrude(const Shape* shapeToExtrude)
	{
		mMultiShapeToExtrude.clear();		
		mMultiShapeToExtrude.addShape(*shapeToExtrude);
		return *this;
	}

	/** Sets the multishape to extrude. Mutually exclusive with setShapeToExtrude. */
	inline Extruder& setMultiShapeToExtrude(const MultiShape* multiShapeToExtrude)
	{
		mMultiShapeToExtrude.clear();
		mMultiShapeToExtrude.addMultiShape(*multiShapeToExtrude);
		return *this;
	}

	/** Sets the extrusion path */
	inline Extruder& setExtrusionPath(const Path* extrusionPath)
	{
		mMultiExtrusionPath.clear();
		mMultiExtrusionPath.addPath(*extrusionPath);
		mMultiExtrusionPath._calcIntersections();
		return *this;
	}

	/** Sets the extrusion multipath */
	inline Extruder& setExtrusionPath(const MultiPath* multiExtrusionPath)
	{
		mMultiExtrusionPath.clear();
		mMultiExtrusionPath.addMultiPath(*multiExtrusionPath);
		mMultiExtrusionPath._calcIntersections();
		return *this;
	}

	/** Sets the rotation track (optional) */
	inline Extruder& setRotationTrack(const Track* rotationTrack, unsigned int index = 0)
	{
		if (!rotationTrack && mRotationTracks.find(index) != mRotationTracks.end())
			mRotationTracks.erase(mRotationTracks.find(index));
		if (rotationTrack)
			mRotationTracks[index] = rotationTrack;
		return *this;
	}

	/** Sets the scale track (optional) */
	inline Extruder& setScaleTrack(const Track* scaleTrack, unsigned int index = 0)
	{
		if (!scaleTrack && mScaleTracks.find(index) != mScaleTracks.end())
			mRotationTracks.erase(mScaleTracks.find(index));
		if (scaleTrack)
			mScaleTracks[index] = scaleTrack;
		return *this;
	}

	/// Sets the track that maps shape points to V texture coords (optional).
	/// Warning : if used with multishape, all shapes will have the same track.
	inline Extruder& setShapeTextureTrack(const Track* shapeTextureTrack, unsigned int index = 0)
	{
		if (!shapeTextureTrack && mShapeTextureTracks.find(index) != mShapeTextureTracks.end())
			mShapeTextureTracks.erase(mShapeTextureTracks.find(index));
		if (shapeTextureTrack)
			mShapeTextureTracks[index] = shapeTextureTrack;
		return *this;
	}

	/// Sets the track that maps path points to V texture coord (optional).
	inline Extruder& setPathTextureTrack(const Track* pathTextureTrack, unsigned int index = 0)
	{
		if (!pathTextureTrack && mPathTextureTracks.find(index) != mPathTextureTracks.end())
			mPathTextureTracks.erase(mPathTextureTracks.find(index));
		if (pathTextureTrack)
			mPathTextureTracks[index] = pathTextureTrack;
		return *this;
	}

	/** Sets whether caps are added to the extremities or not (not closed paths only) */
	inline Extruder& setCapped(bool capped)
	{
		mCapped = capped;
		return *this;
	}
};
}

#endif