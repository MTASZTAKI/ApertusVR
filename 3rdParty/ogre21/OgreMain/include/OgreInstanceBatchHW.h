/*
-----------------------------------------------------------------------------
This source file is part of OGRE
    (Object-oriented Graphics Rendering Engine)
For the latest info, see http://www.ogre3d.org/

Copyright (c) 2000-2014 Torus Knot Software Ltd

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
#ifndef __InstanceBatchHW_H__
#define __InstanceBatchHW_H__

#include "OgreInstanceBatch.h"

namespace Ogre
{
namespace v1
{
    /** \addtogroup Core
    *  @{
    */
    /** \addtogroup Scene
    *  @{
    */

    /** This is technique requires true instancing hardware support.
        Basically it creates a cloned vertex buffer from the original, with an extra buffer containing
        3 additional TEXCOORDS (12 bytes) repeated as much as the instance count.
        That will be used for each instance data.
        @par
        The main advantage of this technique is that it's <u>VERY</u> fast; but it doesn't support
        skeletal animation at all. Very reduced memory consumption and bandwidth. Great for particles,
        debris, bricks, trees, sprites.
        This batch is one of the few (if not the only) techniques that allows culling on an individual
        basis. This means we can save vertex shader performance for instances that aren't in scene or
        just not focused by the camera.

        @remarks
            Design discussion webpage: http://www.ogre3d.org/forums/viewtopic.php?f=4&t=59902
        @author
            Matias N. Goldberg ("dark_sylinc")
        @version
            1.1
     */
    class _OgreExport InstanceBatchHW : public InstanceBatch
    {
    protected:
        void setupVertices( const SubMesh* baseSubMesh );
        void setupIndices( const SubMesh* baseSubMesh );

        void removeBlendData();
        virtual bool checkSubMeshCompatibility( const SubMesh* baseSubMesh );

        size_t updateVertexBuffer( Camera *currentCamera , const Camera *lodCamera );

        /// Overloaded to reserve enough space in mCulledInstances
        virtual void createAllInstancedEntities(void);

    public:
        InstanceBatchHW( IdType id, ObjectMemoryManager *objectMemoryManager, InstanceManager *creator,
                            MeshPtr &meshReference, const MaterialPtr &material,
                            size_t instancesPerBatch, const Mesh::IndexMap *indexToBoneMap );
        virtual ~InstanceBatchHW();

        /** @see InstanceBatch::calculateMaxNumInstances */
        size_t calculateMaxNumInstances( const SubMesh *baseSubMesh, uint16 flags ) const;

        /** @see InstanceBatch::buildFrom */
        void buildFrom( const SubMesh *baseSubMesh, const RenderOperation &renderOperation );

        //Renderable overloads
        void getWorldTransforms( Matrix4* xform ) const;
        unsigned short getNumWorldTransforms(void) const;

        /** Overloaded to avoid updating skeletons (which we don't support), check visibility on a
            per unit basis and finally updated the vertex buffer */
        virtual void _updateRenderQueue( RenderQueue* queue, Camera *camera, const Camera *lodCamera );

        virtual void instanceBatchCullFrustumThreaded( const Camera *frustum,
                                                       const Camera *lodCamera,
                                                        uint32 combinedVisibilityFlags )
        {
            instanceBatchCullFrustumThreadedImpl( frustum, lodCamera, combinedVisibilityFlags );
        }
    };
}
}

#endif
