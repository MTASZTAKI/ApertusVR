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
#include "ProceduralTriangleBuffer.h"
#include "OgreManualObject.h"
#include "OgreSceneManager.h"
#include "OgreRoot.h"

using namespace Ogre;

namespace Procedural
{
Ogre::MeshPtr TriangleBuffer::transformToMesh(const std::string& name,
        const Ogre::String& group) const
{
	Ogre::SceneManager* sceneMgr = Ogre::Root::getSingleton().getSceneManagerIterator().begin()->second;
	Ogre::ManualObject* manual = sceneMgr->createManualObject();
	manual->begin("BaseWhiteNoLighting", Ogre::RenderOperation::OT_TRIANGLE_LIST);

#if OGRE_VERSION >= ((2 << 16) | (0 << 8) | 0)
	Ogre::Vector3 aabb_min = Ogre::Vector3::ZERO;
	Ogre::Vector3 aabb_max = Ogre::Vector3::ZERO;
#endif
	for (std::vector<Vertex>::const_iterator it = mVertices.begin(); it != mVertices.end(); ++it)
	{
		manual->position(it->mPosition);
		manual->textureCoord(it->mUV);
		manual->normal(it->mNormal);
#if OGRE_VERSION >= ((2 << 16) | (0 << 8) | 0)
		if(it->mPosition.x < aabb_min.x) aabb_min.x = it->mPosition.x;
		if(it->mPosition.y < aabb_min.y) aabb_min.y = it->mPosition.y;
		if(it->mPosition.z < aabb_min.z) aabb_min.z = it->mPosition.z;
		if(it->mPosition.x > aabb_max.x) aabb_max.x = it->mPosition.x;
		if(it->mPosition.y > aabb_max.y) aabb_max.y = it->mPosition.y;
		if(it->mPosition.z > aabb_max.z) aabb_max.z = it->mPosition.z;
#endif
	}
	for (std::vector<int>::const_iterator it = mIndices.begin(); it!=mIndices.end(); ++it)
	{
		manual->index(*it);
	}
	manual->end();
#if OGRE_VERSION >= ((2 << 16) | (0 << 8) | 0)
	manual->setLocalAabb(Ogre::Aabb::newFromExtents(aabb_min, aabb_max));
#endif
	Ogre::MeshPtr mesh = manual->convertToMesh(name, group);

	sceneMgr->destroyManualObject(manual);

	return mesh;
}

/*void TriangleBuffer::importEntity(Entity* entity)
	{
		bool added_shared = false;
	size_t current_offset = 0;
	size_t shared_offset = 0;
	size_t next_offset = 0;
	size_t index_offset = 0;
	size_t vertex_count = 0;
	size_t index_count = 0;

	Ogre::MeshPtr mesh = entity->getMesh();


	bool useSoftwareBlendingVertices = entity->hasSkeleton();

	if (useSoftwareBlendingVertices)
	  entity->_updateAnimation();

	// Calculate how many vertices and indices we're going to need
	for (unsigned short i = 0; i < mesh->getNumSubMeshes(); ++i)
	{
		Ogre::SubMesh* submesh = mesh->getSubMesh( i );

		// We only need to add the shared vertices once
		if(submesh->useSharedVertices)
		{
			if( !added_shared )
			{
				vertex_count += mesh->sharedVertexData->vertexCount;
				added_shared = true;
			}
		}
		else
		{
			vertex_count += submesh->vertexData->vertexCount;
		}

		// Add the indices
		index_count += submesh->indexData->indexCount;
	}


	// Allocate space for the vertices and indices
	estimateVertexCount(vertex_count);
	estimateIndexCount(index_count);

	added_shared = false;

	// Run through the submeshes again, adding the data into the arrays
	for ( unsigned short i = 0; i < mesh->getNumSubMeshes(); ++i)
	{
		Ogre::SubMesh* submesh = mesh->getSubMesh(i);

		//----------------------------------------------------------------
		// GET VERTEXDATA
		//----------------------------------------------------------------
		Ogre::VertexData* vertex_data;

		//When there is animation:
		if(useSoftwareBlendingVertices)
			vertex_data = submesh->useSharedVertices ? entity->_getSkelAnimVertexData() : entity->getSubEntity(i)->_getSkelAnimVertexData();
		else
			vertex_data = submesh->useSharedVertices ? mesh->sharedVertexData : submesh->vertexData;


		if((!submesh->useSharedVertices)||(submesh->useSharedVertices && !added_shared))
		{
			if(submesh->useSharedVertices)
			{
				added_shared = true;
				shared_offset = current_offset;
			}

			const Ogre::VertexElement* posElem =
				vertex_data->vertexDeclaration->findElementBySemantic(Ogre::VES_POSITION);

			Ogre::HardwareVertexBufferSharedPtr vbuf =
				vertex_data->vertexBufferBinding->getBuffer(posElem->getSource());

			unsigned char* vertex =
				static_cast<unsigned char*>(vbuf->lock(Ogre::HardwareBuffer::HBL_READ_ONLY));

			// There is _no_ baseVertexPointerToElement() which takes an Ogre::Real or a double
			//  as second argument. So make it float, to avoid trouble when Ogre::Real will
			//  be comiled/typedefed as double:
			//      Ogre::Real* pReal;
			float* pReal;

			for( size_t j = 0; j < vertex_data->vertexCount; ++j, vertex += vbuf->getVertexSize())
			{
				posElem->baseVertexPointerToElement(vertex, &pReal);

				Ogre::Vector3 pt(pReal[0], pReal[1], pReal[2]);

				vertices[current_offset + j] = (orient * (pt * scale)) + position;
			}

			vbuf->unlock();
			next_offset += vertex_data->vertexCount;
		}


		Ogre::IndexData* index_data = submesh->indexData;
		size_t numTris = index_data->indexCount / 3;
		Ogre::HardwareIndexBufferSharedPtr ibuf = index_data->indexBuffer;

		bool use32bitindexes = (ibuf->getType() == Ogre::HardwareIndexBuffer::IT_32BIT);

		void* hwBuf = ibuf->lock(Ogre::HardwareBuffer::HBL_READ_ONLY);

		size_t offset = (submesh->useSharedVertices)? shared_offset : current_offset;
		size_t index_start = index_data->indexStart;
		size_t last_index = numTris*3 + index_start;

		if (use32bitindexes) {
			Ogre::uint32* hwBuf32 = static_cast<Ogre::uint32*>(hwBuf);
			for (size_t k = index_start; k < last_index; ++k)
			{
				indices[index_offset++] = hwBuf32[k] + static_cast<Ogre::uint32>( offset );
			}
		} else {
			Ogre::uint16* hwBuf16 = static_cast<Ogre::uint16*>(hwBuf);
			for (size_t k = index_start; k < last_index; ++k)
			{
				indices[ index_offset++ ] = static_cast<Ogre::uint32>( hwBuf16[k] ) +
					static_cast<Ogre::uint32>( offset );
			}
		}

		ibuf->unlock();
		current_offset = next_offset;
	}
	}*/
}
