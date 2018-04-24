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


#ifndef APE_IUNITTEXTURE_H
#define APE_IUNITTEXTURE_H

#include <string>
#include <vector>
#include "ApeEntity.h"
#include "ApeTexture.h"
#include "ApeVector2.h"
#include "ApeICamera.h"

namespace Ape
{	
	class IUnitTexture : public Texture
	{
	public:
		struct Parameters
		{
			Ape::MaterialWeakPtr material;
			
			std::string fileName;

			Parameters()
			{
				this->material = Ape::MaterialWeakPtr();
				this->fileName = std::string();
			}

			Parameters(Ape::MaterialWeakPtr material, std::string fileName)
			{
				this->material = material;
				this->fileName = fileName;
			}

		};

		struct Filtering
		{
			Ape::Texture::Filtering minFilter;

			Ape::Texture::Filtering magFilter;

			Ape::Texture::Filtering mipFilter;

			Filtering()
			{
				this->minFilter = Ape::Texture::Filtering::F_NONE;
				this->magFilter = Ape::Texture::Filtering::F_NONE;
				this->mipFilter = Ape::Texture::Filtering::F_NONE;
			}

			Filtering(Ape::Texture::Filtering minFilter, Ape::Texture::Filtering magFilter, Ape::Texture::Filtering mipFilter)
			{
				this->minFilter = minFilter;
				this->magFilter = magFilter;
				this->mipFilter = mipFilter;
			}

		};
	protected:
		IUnitTexture(std::string name) : Texture(name, Entity::TEXTURE_UNIT) {}

		virtual ~IUnitTexture() {};

	public:
		virtual void setParameters(Ape::MaterialWeakPtr material, std::string fileName) = 0;

		virtual Parameters getParameters() = 0;

		virtual void setTextureScroll(float u, float v) = 0;

		virtual Ape::Vector2 getTextureScroll() = 0;

		virtual void setTextureAddressingMode(Ape::Texture::AddressingMode addressingMode) = 0;

		virtual Ape::Texture::AddressingMode getTextureAddressingMode() = 0;

		virtual void setTextureFiltering(Ape::Texture::Filtering minFilter, Ape::Texture::Filtering magFilter, Ape::Texture::Filtering mipFilter) = 0;

		virtual Filtering getTextureFiltering() = 0;
	};
}

#endif
