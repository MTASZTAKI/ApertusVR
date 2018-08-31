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

#include "OgreGLSLProgramFactory.h"
#include "OgreGLSLLinkProgramManager.h"
#include "OgreGLSLProgramPipelineManager.h"
#include "OgreGLSLProgram.h"
#include "OgreRoot.h"

namespace Ogre {
    GLSLLinkProgramManager* GLSLProgramFactory::mLinkProgramManager = NULL;
    GLSLProgramPipelineManager* GLSLProgramFactory::mProgramPipelineManager = NULL;
    //-----------------------------------------------------------------------
    String GLSLProgramFactory::sLanguageName = "glsl";
    //-----------------------------------------------------------------------
    GLSLProgramFactory::GLSLProgramFactory(void)
    {
        if (mLinkProgramManager == NULL)
        {
            mLinkProgramManager = new GLSLLinkProgramManager();
        }
        if(Root::getSingleton().getRenderSystem()->getCapabilities()->hasCapability(RSC_SEPARATE_SHADER_OBJECTS))
        {
            if (mProgramPipelineManager == NULL)
            {
                mProgramPipelineManager = new GLSLProgramPipelineManager();
            }
        }
    }
    //-----------------------------------------------------------------------
    GLSLProgramFactory::~GLSLProgramFactory(void)
    {
        if (mLinkProgramManager)
        {
            delete mLinkProgramManager;
            mLinkProgramManager = NULL;
        }

        if(Root::getSingleton().getRenderSystem()->getCapabilities()->hasCapability(RSC_SEPARATE_SHADER_OBJECTS))
        {
            if (mProgramPipelineManager)
            {
                delete mProgramPipelineManager;
                mProgramPipelineManager = NULL;
            }
        }
    }
    //-----------------------------------------------------------------------
    const String& GLSLProgramFactory::getLanguage(void) const
    {
        return sLanguageName;
    }
    //-----------------------------------------------------------------------
    HighLevelGpuProgram* GLSLProgramFactory::create(ResourceManager* creator, 
        const String& name, ResourceHandle handle,
        const String& group, bool isManual, ManualResourceLoader* loader)
    {
        return OGRE_NEW GLSLProgram(creator, name, handle, group, isManual, loader);
    }
    //-----------------------------------------------------------------------
    void GLSLProgramFactory::destroy(HighLevelGpuProgram* prog)
    {
        OGRE_DELETE prog;
    }
    //-----------------------------------------------------------------------

}
