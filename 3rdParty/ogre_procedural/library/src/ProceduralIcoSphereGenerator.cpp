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
#include "ProceduralIcoSphereGenerator.h"
#include "ProceduralUtils.h"

using namespace Ogre;

namespace Procedural
{
void IcoSphereGenerator::addToTriangleBuffer(TriangleBuffer& buffer) const
{
	std::vector<Vector3> vertices;
	int offset = 0;

	/// Step 1 : Generate icosahedron
	Real phi = .5f*(1.f+sqrt(5.f));
	Real invnorm = 1/sqrt(phi*phi+1);

	vertices.push_back(invnorm*Vector3(-1,  phi, 0));//0
	vertices.push_back(invnorm*Vector3( 1,  phi, 0));//1
	vertices.push_back(invnorm*Vector3(0,   1,  -phi));//2
	vertices.push_back(invnorm*Vector3(0,   1,   phi));//3
	vertices.push_back(invnorm*Vector3(-phi,0,  -1));//4
	vertices.push_back(invnorm*Vector3(-phi,0,   1));//5
	vertices.push_back(invnorm*Vector3( phi,0,  -1));//6
	vertices.push_back(invnorm*Vector3( phi,0,   1));//7
	vertices.push_back(invnorm*Vector3(0,   -1, -phi));//8
	vertices.push_back(invnorm*Vector3(0,   -1,  phi));//9
	vertices.push_back(invnorm*Vector3(-1,  -phi,0));//10
	vertices.push_back(invnorm*Vector3( 1,  -phi,0));//11

	int firstFaces[] = {0,1,2,
	                    0,3,1,
	                    0,4,5,
	                    1,7,6,
	                    1,6,2,
	                    1,3,7,
	                    0,2,4,
	                    0,5,3,
	                    2,6,8,
	                    2,8,4,
	                    3,5,9,
	                    3,9,7,
	                    11,6,7,
	                    10,5,4,
	                    10,4,8,
	                    10,9,5,
	                    11,8,6,
	                    11,7,9,
	                    10,8,11,
	                    10,11,9
	                   };

	std::vector<int> faces(firstFaces, firstFaces + sizeof(firstFaces)/sizeof(*firstFaces));
	int size = 60;

	/// Step 2 : tessellate
	for (unsigned short iteration = 0; iteration<mNumIterations; iteration++)
	{
		size*=4;
		std::vector<int> newFaces;
		newFaces.clear();
		//newFaces.resize(size);
		for (int i=0; i<size/12; i++)
		{
			int i1 = faces[i*3];
			int i2 = faces[i*3+1];
			int i3 = faces[i*3+2];
			int i12 = vertices.size();
			int i23 = i12+1;
			int i13 = i12+2;
			Vector3 v1 = vertices[i1];
			Vector3 v2 = vertices[i2];
			Vector3 v3 = vertices[i3];
			//make 1 vertice at the center of each edge and project it onto the sphere
			vertices.push_back((v1+v2).normalisedCopy());
			vertices.push_back((v2+v3).normalisedCopy());
			vertices.push_back((v1+v3).normalisedCopy());
			//now recreate indices
			newFaces.push_back(i1);
			newFaces.push_back(i12);
			newFaces.push_back(i13);
			newFaces.push_back(i2);
			newFaces.push_back(i23);
			newFaces.push_back(i12);
			newFaces.push_back(i3);
			newFaces.push_back(i13);
			newFaces.push_back(i23);
			newFaces.push_back(i12);
			newFaces.push_back(i23);
			newFaces.push_back(i13);
		}
		faces.swap(newFaces);
	}

	/// Step 3 : generate texcoords
	std::vector<Vector2> texCoords;
	for (unsigned short i=0; i<vertices.size(); i++)
	{
		const Vector3& vec = vertices[i];
		Real u, v;
		Real r0 = sqrtf(vec.x*vec.x+vec.z*vec.z);
		Real alpha;
		alpha = atan2f(vec.z,vec.x);
		u = alpha/Math::TWO_PI+.5f;
		v = atan2f(vec.y, r0)/Math::PI + .5f;
		texCoords.push_back(Vector2(u,v));
	}

	/// Step 4 : fix texcoords
	// find vertices to split
	std::vector<int> indexToSplit;

	for (unsigned int i=0; i<faces.size()/3; i++)
	{
		Vector2& t0 = texCoords[faces[i*3+0]];
		Vector2& t1 = texCoords[faces[i*3+1]];
		Vector2& t2 = texCoords[faces[i*3+2]];
		if (Math::Abs(t2.x-t0.x)>0.5)
		{
			if (t0.x<0.5)
				indexToSplit.push_back(faces[i*3]);
			else
				indexToSplit.push_back(faces[i*3+2]);
		}
		if (Math::Abs(t1.x-t0.x)>0.5)
		{
			if (t0.x<0.5)
				indexToSplit.push_back(faces[i*3]);
			else
				indexToSplit.push_back(faces[i*3+1]);
		}
		if (Math::Abs(t2.x-t1.x)>0.5)
		{
			if (t1.x<0.5)
				indexToSplit.push_back(faces[i*3+1]);
			else
				indexToSplit.push_back(faces[i*3+2]);
		}
	}

	//split vertices
	for (unsigned short i=0; i<indexToSplit.size(); i++)
	{
		int index = indexToSplit[i];
		//duplicate vertex
		Vector3 v = vertices[index];
		Vector2 t = texCoords[index] + Vector2::UNIT_X;
		vertices.push_back(v);
		texCoords.push_back(t);
		int newIndex = vertices.size()-1;
		//reassign indices
		for (unsigned short j=0; j<faces.size(); j++)
		{
			if (faces[j]==index)
			{
				int index1 = faces[(j+1)%3+(j/3)*3];
				int index2 = faces[(j+2)%3+(j/3)*3];
				if ((texCoords[index1].x>0.5) || (texCoords[index2].x>0.5))
				{
					faces[j] = newIndex;
				}
			}
		}
	}

	/// Step 5 : realize
	buffer.rebaseOffset();
	buffer.estimateVertexCount(vertices.size());
	buffer.estimateIndexCount(size);

	for (unsigned short i=0; i<vertices.size(); i++)
	{
		addPoint(buffer, mRadius*vertices[i],
		         vertices[i],//note : vertices are already normalised
		         Vector2(texCoords[i].x,texCoords[i].y));
	}
	for (unsigned short i=0; i<size; i++)
	{
		buffer.index(offset+faces[i]);
	}
	offset+=vertices.size();
}
}
