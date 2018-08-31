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

#ifndef __GLES2StateCacheManagerImp_H__
#define __GLES2StateCacheManagerImp_H__

#include "OgreGLES2Prerequisites.h"

typedef Ogre::GeneralAllocatedObject StateCacheAlloc;

namespace Ogre
{
    /** An in memory cache of the OpenGL ES2 state.
     @see GLES2StateCacheManager
     */
    class _OgreGLES2Export GLES2StateCacheManagerImp : public StateCacheAlloc
    {
    private:
        typedef OGRE_HashMap<GLenum, GLuint> BindBufferMap;
        typedef OGRE_HashMap<GLenum, GLint> TexParameteriMap;
        typedef OGRE_HashMap<GLenum, GLfloat> TexParameterfMap;

        struct TextureUnitParams
        {
            ~TextureUnitParams()
            {
                mTexParameteriMap.clear();
                mTexParameterfMap.clear();
            }

            TexParameteriMap mTexParameteriMap;
            TexParameterfMap mTexParameterfMap;
        };

        typedef OGRE_HashMap<GLuint, TextureUnitParams> TexUnitsMap;

        /* These variables are used for caching OpenGL state.
         They are cached because state changes can be quite expensive,
         which is especially important on mobile or embedded systems.
         */

        /// A map of different buffer types and the currently bound buffer for each type
        BindBufferMap mActiveBufferMap;
        /// A map of texture parameters for each texture unit
        TexUnitsMap mTexUnitsMap;
        /// Array of each OpenGL feature that is enabled i.e. blending, depth test, etc.
        vector<GLenum>::type mEnableVector;
        /// Stores the current clear colour
        vector<GLclampf>::type mClearColour;
        /// Stores the current colour write mask
        vector<GLboolean>::type mColourMask;
        /// Stores the currently enabled vertex attributes
        vector<GLuint>::type mEnabledVertexAttribs;
        /// Stores the current depth write mask
        GLboolean mDepthMask;
        /// Stores the current blend equation
        GLenum mBlendEquation;
        /// Stores the current blend source function
        GLenum mBlendFuncSource;
        /// Stores the current blend destination function
        GLenum mBlendFuncDest;
        /// Stores the current depth test function
        GLenum mDepthFunc;
        /// Stores the current stencil mask
        GLuint mStencilMask;
        /// Stores the last bound texture id
        GLuint mLastBoundTexID;
        /// Stores the currently active texture unit
        GLenum mActiveTextureUnit;
        /// Mask of buffers who contents can be discarded if GL_EXT_discard_framebuffer is supported
        unsigned int mDiscardBuffers;
        /// Stores the current depth clearing colour
        GLclampf mClearDepth;
        
    public:
        GLES2StateCacheManagerImp(void);
        ~GLES2StateCacheManagerImp(void);
        
        /// See GLES2StateCacheManager.initializeCache.
        void initializeCache();
        
        /// See GLES2StateCacheManager.clearCache.
        void clearCache();
        
        /// See GLES2StateCacheManager.bindGLBuffer.
        void bindGLBuffer(GLenum target, GLuint buffer, GLenum attach = 0, bool force = false);
        
        /// See GLES2StateCacheManager.deleteGLBuffer.
        void deleteGLBuffer(GLenum target, GLuint buffer, GLenum attach = 0, bool force = false);
        
        /// See GLES2StateCacheManager.bindGLTexture.
        void bindGLTexture(GLenum target, GLuint texture);
        
        /// See GLES2StateCacheManager.setTexParameteri.
        void setTexParameteri(GLenum target, GLenum pname, GLint param);

        /// See GLES2StateCacheManager.setTexParameterf.
        void setTexParameterf(GLenum target, GLenum pname, GLfloat params);

        /// See GLES2StateCacheManager.getTexParameterfv.
        void getTexParameterfv(GLenum target, GLenum pname, GLfloat *params);

        /// See GLES2StateCacheManager.invalidateStateForTexture.
        void invalidateStateForTexture(GLuint texture);

        /// See GLES2StateCacheManager.activateGLTextureUnit.
        bool activateGLTextureUnit(size_t unit);
        
        /// See GLES2StateCacheManager.getBlendEquation.
        GLenum getBlendEquation(void) const { return mBlendEquation; }
        
        /// See GLES2StateCacheManager.setBlendEquation.
        void setBlendEquation(GLenum eq);
        
        /// See GLES2StateCacheManager.setBlendFunc.
        void setBlendFunc(GLenum source, GLenum dest);
        
        /// See GLES2StateCacheManager.getDepthMask.
        GLboolean getDepthMask(void) const { return mDepthMask; }
        
        /// See GLES2StateCacheManager.setDepthMask.
        void setDepthMask(GLboolean mask);
        
        /// See GLES2StateCacheManager.getDepthFunc.
        GLenum getDepthFunc(void) const { return mDepthFunc; }
        
        /// See GLES2StateCacheManager.setDepthFunc.
        void setDepthFunc(GLenum func);
        
        /// See GLES2StateCacheManager.getClearDepth.
        GLclampf getClearDepth(void) const { return mClearDepth; }
        
        /// See GLES2StateCacheManager.setClearDepth.
        void setClearDepth(GLclampf depth);
        
        /// See GLES2StateCacheManager.setClearColour.
        void setClearColour(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha);
        
        /// See GLES2StateCacheManager.getColourMask.
        vector<GLboolean>::type & getColourMask(void) { return mColourMask; }
        
        /// See GLES2StateCacheManager.setColourMask.
        void setColourMask(GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha);
        
        /// See GLES2StateCacheManager.getStencilMask.
        GLuint getStencilMask(void) const { return mStencilMask; }
        
        /// See GLES2StateCacheManager.setStencilMask.
        void setStencilMask(GLuint mask);
        
        /// See GLES2StateCacheManager.setEnabled.
        void setEnabled(GLenum flag);
        
        /// See GLES2StateCacheManager.setDisabled.
        void setDisabled(GLenum flag);

        /// See GLES2StateCacheManager.setVertexAttribEnabled.
        void setVertexAttribEnabled(GLuint attrib);

        /// See GLES2StateCacheManager.setVertexAttribDisabled.
        void setVertexAttribDisabled(GLuint attrib);

        /// See GLES2StateCacheManager.getDiscardBuffers.
        unsigned int getDiscardBuffers(void) const { return mDiscardBuffers; }
        
        /// See GLES2StateCacheManager.setDiscardBuffers.
        void setDiscardBuffers(unsigned int flags) { mDiscardBuffers = flags; }
    };
}

#endif
