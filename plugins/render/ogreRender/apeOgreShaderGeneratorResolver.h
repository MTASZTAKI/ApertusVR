/*MIT License

Copyright (c) 2018 MTA SZTAKI

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

#ifndef APE_OGRESHADERGENERATORRESOLVER_H
#define APE_OGRESHADERGENERATORRESOLVER_H

#include "OgreTechnique.h"
#include "OgreMaterialManager.h"
#include "OgreRTShaderSystem.h"

namespace ape
{
    class ShaderGeneratorResolver : public Ogre::MaterialManager::Listener
    {
	private:
		Ogre::RTShader::ShaderGenerator* mpShaderGenerator;
		std::vector<std::string> mIgnoreList;

    public:
		ShaderGeneratorResolver(Ogre::RTShader::ShaderGenerator* shaderGenerator)
        {
			mpShaderGenerator = shaderGenerator;
			mIgnoreList = std::vector<std::string>();
        }

		void appendIgnoreList(std::string materialName)
		{
			mIgnoreList.push_back(materialName);
		}

        Ogre::Technique* handleSchemeNotFound(unsigned short schemeIndex, const Ogre::String& schemeName, Ogre::Material* originalMaterial, unsigned short lodIndex, const Ogre::Renderable* rend)
		{
			for (auto ignoreItem : mIgnoreList)
			{
				if (ignoreItem == originalMaterial->getName())
					return NULL;
			}
            if (schemeName != Ogre::RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME)
                return NULL;
			bool techniqueCreated = mpShaderGenerator->createShaderBasedTechnique(originalMaterial->getName(), Ogre::MaterialManager::DEFAULT_SCHEME_NAME, schemeName);
            if (!techniqueCreated)
                return NULL;
			mpShaderGenerator->validateMaterial(schemeName, originalMaterial->getName());
            Ogre::Material::TechniqueIterator itTech = originalMaterial->getTechniqueIterator();
            while (itTech.hasMoreElements())
            {
                Ogre::Technique* curTech = itTech.getNext();
                if (curTech->getSchemeName() == schemeName)
                    return curTech;
            }
            return NULL;
        }

		bool afterIlluminationPassesCreated(Ogre::Technique* tech)
		{
			if(tech->getSchemeName() == Ogre::RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME)
			{
				Ogre::Material* mat = tech->getParent();
				mpShaderGenerator->validateMaterialIlluminationPasses(tech->getSchemeName(), mat->getName(), mat->getGroup());
				return true;
			}
			return false;
		}

		bool beforeIlluminationPassesCleared(Ogre::Technique* tech)
		{

			if(tech->getSchemeName() == Ogre::RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME)
			{
				Ogre::Material* mat = tech->getParent();
				mpShaderGenerator->invalidateMaterialIlluminationPasses(tech->getSchemeName(), mat->getName(), mat->getGroup());
				return true;
			}
			return false;
		}
    };
}
#endif 
