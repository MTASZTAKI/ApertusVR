/*MIT License

Copyright (c) 2016 MTA SZTAKI

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.*/

#ifndef APE_GEOMETRY_H
#define APE_GEOMETRY_H

#include <memory>
#include <string>
#include <vector>
#include "ApeEntity.h"
#include "ApeMaterial.h"

namespace Ape
{		
	typedef std::map<int, EntityWeakPtrVector> GeometryLodLevelVectorMap;	
	
	class Geometry : public Entity
	{
	public:
		enum OperationType
		{
			POINTLIST,
			LINELIST,
			LINESTRIP,
			TRIANGLELIST,
			TRIANGLESTRIP,
			TRIANGLEFAN,
			INVALID
		};
		
	protected:
	    Geometry(std::string name, std::string parentNodeName, Entity::Type entityType) : Entity(name, parentNodeName, entityType) 
			, mGeometries(GeometryLodLevelVectorMap()), mMaterial(MaterialWeakPtr()), mOperationType(OperationType::INVALID) {};
		
		virtual ~Geometry() {};
		
		GeometryLodLevelVectorMap mGeometries;
		
		MaterialWeakPtr mMaterial;
		
		OperationType mOperationType;
		
	public:
		
		GeometryLodLevelVectorMap getGeometries() { return mGeometries; };
		
		MaterialWeakPtr getMaterial() { return mMaterial; };
		
		OperationType getOperationType() { return mOperationType; };	
	};

	typedef std::weak_ptr<Geometry> GeometryWeakPtr;
}

#endif
