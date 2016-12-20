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


#ifndef APE_MATERIAL_H
#define APE_MATERIAL_H

#include <string>
#include <vector>
#include "ApeEntity.h"
#include "ApeITexture.h"
#include "ApeColor.h"

namespace Ape
{	
	typedef std::map < Texture::Type, EntityWeakPtr> Textures;
	
	class Material : public Entity
	{
	protected:
	    Material(std::string name, Entity::Type entityType) : Entity(name,entityType) {}
		
		virtual ~Material() {};
		
		Color mDiffuseColor;

		Color mSpecularColor;

		Color mAmbientColor;

		Color mEmissiveColor;

		float mShininess;
		
		bool mSceneBlend;
		
		Textures mTextures;
		
	public:
		Color getDiffuseColor() { return mDiffuseColor; };

		Color getSpecularColor() { return mSpecularColor; };

		Color getAmbientColor() { return mAmbientColor; };

		Color getEmissiveColor() { return mEmissiveColor; };

		float getShininess() { return mShininess; };
		
		bool getSceneBlend() { return mSceneBlend; };
		
		Textures getTextures() { return mTextures; };
	};
	
	typedef std::weak_ptr<Material> MaterialWeakPtr;
}

#endif

