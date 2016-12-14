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
#include "ProceduralExtruder.h"
#include "ProceduralTriangulator.h"
#include "ProceduralGeometryHelpers.h"
#include "ProceduralMeshModifiers.h"

using namespace Ogre;

namespace Procedural
{
//-----------------------------------------------------------------------
void _extrudeShape(TriangleBuffer& buffer, const Shape& shape, const Vector3& position, const Quaternion& orientationLeft, const Quaternion& orientationRight, Real scale, Real scaleCorrectionLeft, Real scaleCorrectionRight, Real totalShapeLength, Real uTexCoord, bool joinToTheNextSection, const Track* shapeTextureTrack)
{
	Real lineicShapePos = 0.;
	size_t numSegShape = shape.getSegCount();
	// Insert new points
	for (unsigned int j =0; j <= numSegShape; ++j)
	{
		Vector2 vp2 = shape.getPoint(j);
		Quaternion orientation = (vp2.x>0) ? orientationRight : orientationLeft;
		Vector2 vp2normal = shape.getAvgNormal(j);
		Vector3 vp;
		if (vp2.x>0)
			vp = Vector3(scaleCorrectionRight * vp2.x, vp2.y, 0);		
		else
			vp = Vector3(scaleCorrectionLeft * vp2.x, vp2.y, 0);
		Vector3 normal(vp2normal.x, vp2normal.y, 0);
		buffer.rebaseOffset();
		Vector3 newPoint = position+orientation*(scale*vp);
		if (j>0)
			lineicShapePos += (vp2 - shape.getPoint(j-1)).length();
		Real vTexCoord;
		if (shapeTextureTrack)
			vTexCoord = shapeTextureTrack->getValue(lineicShapePos, lineicShapePos / totalShapeLength, j);
		else
			vTexCoord = lineicShapePos / totalShapeLength;

		buffer.vertex(newPoint,
		              orientation*normal,
		              Vector2(uTexCoord, vTexCoord));

		if (j <numSegShape && joinToTheNextSection)
		{
			if (shape.getOutSide() == SIDE_LEFT)
			{
				buffer.triangle(numSegShape + 1, numSegShape + 2, 0);
				buffer.triangle(0, numSegShape + 2, 1);
			}
			else
			{
				buffer.triangle(numSegShape + 2, numSegShape + 1, 0);
				buffer.triangle(numSegShape + 2, 0, 1);
			}
		}
	}
}
//-----------------------------------------------------------------------
void _extrudeBodyImpl(TriangleBuffer& buffer, const Shape* shapeToExtrude, const Path* pathToExtrude, size_t pathBeginIndex, size_t pathEndIndex, const Track* shapeTextureTrack, const Track* rotationTrack, const Track* scaleTrack, const Track* pathTextureTrack)
{
	if (pathToExtrude == NULL || shapeToExtrude == NULL)
		OGRE_EXCEPT(Ogre::Exception::ERR_INVALID_STATE, "Shape and Path must not be null!", "Procedural::Extruder::_extrudeBodyImpl(Procedural::TriangleBuffer&, const Procedural::Shape*)");

	unsigned int numSegPath = pathEndIndex - pathBeginIndex;
	unsigned int numSegShape = shapeToExtrude->getSegCount();

	if (numSegPath == 0 || numSegShape == 0)
		OGRE_EXCEPT(Ogre::Exception::ERR_INVALID_STATE, "Shape and path must contain at least two points", "Procedural::Extruder::_extrudeBodyImpl(Procedural::TriangleBuffer&, const Procedural::Shape*)");

	Real totalPathLength = pathToExtrude->getTotalLength();
	Real totalShapeLength = shapeToExtrude->getTotalLength();

	// Merge shape and path with tracks
	Ogre::Real lineicPos = pathToExtrude->getLengthAtPoint(pathBeginIndex);
	Path path = *pathToExtrude;
	numSegPath = pathEndIndex - pathBeginIndex;
	numSegShape = shapeToExtrude->getSegCount();

	// Estimate vertex and index count
	buffer.rebaseOffset();
	buffer.estimateIndexCount(numSegShape*numSegPath*6);
	buffer.estimateVertexCount((numSegShape+1)*(numSegPath+1));

	Vector3 oldup;
	for (unsigned int i = pathBeginIndex; i <= pathEndIndex; ++i)
	{
		Vector3 v0 = path.getPoint(i);
		Vector3 direction = path.getAvgDirection(i);

		Quaternion q = Utils::_computeQuaternion(direction);

		Radian angle = (q*Vector3::UNIT_Y).angleBetween(oldup);
		if (i>pathBeginIndex && angle>(Radian)Math::HALF_PI/2.)
		{
			q = Utils::_computeQuaternion(direction, oldup);
		}
		oldup = q * Vector3::UNIT_Y;

		Real scale=1.;

		if (i>pathBeginIndex) lineicPos += (v0-path.getPoint(i-1)).length();

		// Get the values of angle and scale
		if (rotationTrack)
		{
			Real angle;
			angle = rotationTrack->getValue(lineicPos, lineicPos / totalPathLength, i);

			q = q*Quaternion((Radian)angle, Vector3::UNIT_Z);
		}
		if (scaleTrack)
		{
			scale = scaleTrack->getValue(lineicPos, lineicPos / totalPathLength, i);
		}
		Real uTexCoord;
		if (pathTextureTrack)
			uTexCoord = pathTextureTrack->getValue(lineicPos, lineicPos / totalPathLength, i);
		else
			uTexCoord = lineicPos / totalPathLength;

		_extrudeShape(buffer, *shapeToExtrude, v0, q, q, scale, 1.0, 1.0, totalShapeLength, uTexCoord, i<pathEndIndex, shapeTextureTrack);
	}
}
//-----------------------------------------------------------------------
void _extrudeCapImpl(TriangleBuffer& buffer, const MultiShape& multiShapeToExtrude, const MultiPath& extrusionMultiPath, const Extruder::TrackMap& scaleTracks, const Extruder::TrackMap& rotationTracks)
{
	std::vector<int> indexBuffer;
	PointList pointList;
	
	Triangulator t;
	t.setMultiShapeToTriangulate(&multiShapeToExtrude);
	t.triangulate(indexBuffer, pointList);	

	for (unsigned int i=0; i<extrusionMultiPath.getPathCount(); ++i)
	{
		Path extrusionPath = extrusionMultiPath.getPath(i);
		const Track* scaleTrack = 0;
		const Track* rotationTrack = 0;
		if (scaleTracks.find(i) != scaleTracks.end())
			scaleTrack = scaleTracks.find(i)->second;
		if (rotationTracks.find(i) != rotationTracks.end())
			rotationTrack = rotationTracks.find(i)->second;

		//begin cap
		if (extrusionMultiPath.getIntersectionsMap().find(MultiPath::PathCoordinate(i, 0)) == extrusionMultiPath.getIntersectionsMap().end())
		{			
			buffer.rebaseOffset();
			buffer.estimateIndexCount(indexBuffer.size());
			buffer.estimateVertexCount(pointList.size());

			Quaternion qBegin = Utils::_computeQuaternion(extrusionPath.getDirectionAfter(0));
			if (rotationTrack)
			{
				Real angle = rotationTrack->getFirstValue();
				qBegin = qBegin*Quaternion((Radian)angle, Vector3::UNIT_Z);
			}
			Real scaleBegin=1.;
			if (scaleTrack)
				scaleBegin = scaleTrack->getFirstValue();

			for (size_t j =0; j<pointList.size(); j++)
			{
				Vector2 vp2 = pointList[j];
				Vector3 vp(vp2.x, vp2.y, 0);
				Vector3 normal = -Vector3::UNIT_Z;

				Vector3 newPoint = extrusionPath.getPoint(0)+qBegin*(scaleBegin*vp);
				buffer.vertex(newPoint,
				              qBegin*normal,
				              vp2);
			}
			for (size_t i=0; i<indexBuffer.size()/3; i++)
			{
				buffer.index(indexBuffer[i*3]);
				buffer.index(indexBuffer[i*3+2]);
				buffer.index(indexBuffer[i*3+1]);
			}
		}

		//end cap
		if (extrusionMultiPath.getIntersectionsMap().find(MultiPath::PathCoordinate(i, extrusionPath.getSegCount())) == extrusionMultiPath.getIntersectionsMap().end())
		{			
			buffer.rebaseOffset();
			buffer.estimateIndexCount(indexBuffer.size());
			buffer.estimateVertexCount(pointList.size());

			Quaternion qEnd = Utils::_computeQuaternion(extrusionPath.getDirectionBefore(extrusionPath.getSegCount()));
			if (rotationTrack)
			{
				Real angle = rotationTrack->getLastValue();
				qEnd = qEnd*Quaternion((Radian)angle, Vector3::UNIT_Z);
			}
			Real scaleEnd=1.;
			if (scaleTrack)
				scaleEnd = scaleTrack->getLastValue();

			for (size_t j =0; j<pointList.size(); j++)
			{
				Vector2 vp2 = pointList[j];
				Vector3 vp(vp2.x, vp2.y, 0);
				Vector3 normal = Vector3::UNIT_Z;

				Vector3 newPoint = extrusionPath.getPoint(extrusionPath.getSegCount())+qEnd*(scaleEnd*vp);
				buffer.vertex(newPoint,
				              qEnd*normal,
				              vp2);
			}
			for (size_t i=0; i<indexBuffer.size()/3; i++)
			{
				buffer.index(indexBuffer[i*3]);
				buffer.index(indexBuffer[i*3+1]);
				buffer.index(indexBuffer[i*3+2]);
			}
		}
	}
}
//-----------------------------------------------------------------------
void _extrudeIntersectionImpl(TriangleBuffer& buffer, const MultiPath::PathIntersection& intersection, const MultiPath& multiPath, const Shape& shape, const Track* shapeTextureTrack)
{
	Vector3 intersectionLocation = multiPath.getPath(intersection[0].pathIndex).getPoint(intersection[0].pointIndex);
	Quaternion firstOrientation = Utils::_computeQuaternion(multiPath.getPath(intersection[0].pathIndex).getDirectionBefore(intersection[0].pointIndex));
	Vector3 refX = firstOrientation * Vector3::UNIT_X;
	Vector3 refZ = firstOrientation * Vector3::UNIT_Z;

	std::vector<Vector2> v2s;
	std::vector<MultiPath::PathCoordinate> coords;
	std::vector<Real> direction;

	for (size_t i = 0; i < intersection.size(); ++i)
	{
		const Path& path = multiPath.getPath(intersection[i].pathIndex);
		size_t pointIndex = intersection[i].pointIndex;
		if (pointIndex>0 || path.isClosed())
		{
			Vector3 vb = path.getDirectionBefore(pointIndex);
			Vector2 vb2 = Vector2(vb.dotProduct(refX), vb.dotProduct(refZ));
			v2s.push_back(vb2);
			coords.push_back(intersection[i]);
			direction.push_back(1);
		}
		if (pointIndex<path.getSegCount() || path.isClosed())
		{
			Vector3 va = -path.getDirectionAfter(pointIndex);
			Vector2 va2 = Vector2(va.dotProduct(refX), va.dotProduct(refZ));
			v2s.push_back(va2);
			coords.push_back(intersection[i]);
			direction.push_back(-1);
		}
	}

	std::map<Radian, size_t> angles;
	for (size_t i=1; i<v2s.size(); ++i)
		angles[Utils::angleTo(v2s[0], v2s[i])]=i;

	std::vector<size_t> orderedIndices;
	orderedIndices.push_back(0);
	for (std::map<Radian, size_t>::iterator it = angles.begin(); it!=angles.end(); ++it)
		orderedIndices.push_back(it->second);
	for (size_t i = 0; i < orderedIndices.size(); ++i)
	{
		size_t idx = orderedIndices[i];
		size_t idxBefore = orderedIndices[Utils::modulo(i-1, orderedIndices.size())];
		size_t idxAfter = orderedIndices[Utils::modulo(i+1, orderedIndices.size())];
		Radian angleBefore = (Utils::angleBetween(v2s[idx], v2s[idxBefore]) - (Radian)Math::PI)/2;
		Radian angleAfter = ((Radian)Math::PI - Utils::angleBetween(v2s[idx], v2s[idxAfter]))/2;

		int pointIndex = coords[idx].pointIndex - direction[idx];
		const Path& path = multiPath.getPath(coords[idx].pathIndex);

		Quaternion qStd = Utils::_computeQuaternion(path.getAvgDirection(pointIndex) * direction[idx]);		
		Real lineicPos;
		Real uTexCoord = path.getLengthAtPoint(pointIndex) / path.getTotalLength();

		// Shape making the joint with "standard extrusion"
		_extrudeShape(buffer, shape, path.getPoint(pointIndex), qStd, qStd, 1.0, 1.0, 1.0, shape.getTotalLength(), uTexCoord, true, shapeTextureTrack);

		// Modified shape at the intersection
		Quaternion q;
		if (direction[idx]>0)
			q = Utils::_computeQuaternion(path.getDirectionBefore(coords[idx].pointIndex));
		else
			q = Utils::_computeQuaternion(-path.getDirectionAfter(coords[idx].pointIndex));		
		Quaternion qLeft = q * Quaternion(angleBefore, Vector3::UNIT_Y);
		Quaternion qRight = q * Quaternion(angleAfter, Vector3::UNIT_Y);
		Real scaleLeft = 1.0/Math::Abs(Math::Cos(angleBefore));
		Real scaleRight = 1.0/Math::Abs(Math::Cos(angleAfter));

		uTexCoord = path.getLengthAtPoint(coords[idx].pointIndex) / path.getTotalLength();
		_extrudeShape(buffer, shape, path.getPoint(coords[idx].pointIndex), qLeft, qRight, 1.0, scaleLeft, scaleRight, shape.getTotalLength(), uTexCoord, false, shapeTextureTrack);
	}
}
//-----------------------------------------------------------------------
void Extruder::addToTriangleBuffer(TriangleBuffer& buffer) const
{
	if (mMultiShapeToExtrude.getShapeCount() == 0)
		OGRE_EXCEPT(Ogre::Exception::ERR_INVALID_STATE, "At least one shape must be defined!", "Procedural::Extruder::addToTriangleBuffer(Procedural::TriangleBuffer)");

	TriangleBuffer::Section section = buffer.beginSection();

	// Triangulate the begin and end caps
	if (mCapped && mMultiShapeToExtrude.isClosed())
	{
		_extrudeCapImpl(buffer, mMultiShapeToExtrude, mMultiExtrusionPath, mScaleTracks, mRotationTracks);
	}

	MultiPath multiExtrusionPath = mMultiExtrusionPath;
	
	for (unsigned int j = 0; j < multiExtrusionPath.getPathCount(); ++j)
	{
		Path extrusionPath = multiExtrusionPath.getPath(j);
		bool keysMerged = false;
		const Track* rotationTrack = 0;
		if (mRotationTracks.find(j) != mRotationTracks.end())
		{
			rotationTrack = mRotationTracks.find(j)->second;
			extrusionPath = extrusionPath.mergeKeysWithTrack(*mRotationTracks.find(j)->second);
			keysMerged = true;
		}
		const Track* scaleTrack = 0;
		if (mScaleTracks.find(j) != mScaleTracks.end())
		{
			rotationTrack = mScaleTracks.find(j)->second;
			extrusionPath = extrusionPath.mergeKeysWithTrack(*mScaleTracks.find(j)->second);
			keysMerged = true;
		}
		const Track* pathTextureTrack = 0;
		if (mPathTextureTracks.find(j) != mPathTextureTracks.end())
		{
			pathTextureTrack = mPathTextureTracks.find(j)->second;
			extrusionPath = extrusionPath.mergeKeysWithTrack(*mPathTextureTracks.find(j)->second);
			keysMerged = true;
		}
		if (keysMerged) {
			multiExtrusionPath.setPath(j, extrusionPath);
		}
	}

	// Extrude the paths contained in multiExtrusionPath
	for (unsigned int j = 0; j<multiExtrusionPath.getPathCount(); ++j)
	{
		Path extrusionPath = multiExtrusionPath.getPath(j);
		const Track* rotationTrack = 0;
		if (mRotationTracks.find(j) != mRotationTracks.end())
		{
			rotationTrack = mRotationTracks.find(j)->second;
			extrusionPath = extrusionPath.mergeKeysWithTrack(*mRotationTracks.find(j)->second);
		}
		const Track* scaleTrack = 0;
		if (mScaleTracks.find(j) != mScaleTracks.end())
		{
			scaleTrack = mScaleTracks.find(j)->second;
			extrusionPath = extrusionPath.mergeKeysWithTrack(*mScaleTracks.find(j)->second);
		}
		const Track* pathTextureTrack = 0;
		if (mPathTextureTracks.find(j) != mPathTextureTracks.end())
		{
			pathTextureTrack = mPathTextureTracks.find(j)->second;
			extrusionPath = extrusionPath.mergeKeysWithTrack(*mPathTextureTracks.find(j)->second);
		}

		std::vector<std::pair<unsigned int, unsigned int> > segs = multiExtrusionPath.getNoIntersectionParts(j);

		for (std::vector<std::pair<unsigned int, unsigned int> >::iterator it = segs.begin(); it != segs.end(); ++it)
		{
			for (unsigned int i=0; i<mMultiShapeToExtrude.getShapeCount(); i++)
			{
				Shape shapeToExtrude = mMultiShapeToExtrude.getShape(i);
				const Track* shapeTextureTrack = 0;
				if (mShapeTextureTracks.find(i) != mShapeTextureTracks.end())
				{
					shapeTextureTrack = mShapeTextureTracks.find(i)->second;
					shapeToExtrude.mergeKeysWithTrack(*shapeTextureTrack);
				}
				_extrudeBodyImpl(buffer, &shapeToExtrude, &extrusionPath, it->first, it->second, shapeTextureTrack, rotationTrack, scaleTrack, pathTextureTrack);
			}
		}

		// Make the intersections
		const std::vector<MultiPath::PathIntersection>& intersections = multiExtrusionPath.getIntersections();
		for (std::vector<MultiPath::PathIntersection>::const_iterator it = intersections.begin(); it!= intersections.end(); ++it)
		{
			for (unsigned int i=0; i<mMultiShapeToExtrude.getShapeCount(); i++)
			{
				const Track* shapeTextureTrack = 0;
				if (mShapeTextureTracks.find(i) != mShapeTextureTracks.end())
					shapeTextureTrack = mShapeTextureTracks.find(i)->second;
				_extrudeIntersectionImpl(buffer, *it, mMultiExtrusionPath, mMultiShapeToExtrude.getShape(i), shapeTextureTrack);
			}
		}
	}
	buffer.endSection(section);
	// Chain with linear transforms
	MeshLinearTransform().setTranslation(mPosition).setRotation(mOrientation).modify(section);
	MeshUVTransform().setOrigin(mUVOrigin).setTile(Ogre::Vector2(mUTile, mVTile)).setSwitchUV(mSwitchUV).modify(section);
}
}
