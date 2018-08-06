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

#ifndef APE_IMANUALTEXTURE_H
#define APE_IMANUALTEXTURE_H

#include <string>
#include <vector>
#include "ApeEntity.h"
#include "ApeICamera.h"
#include "ApeTexture.h"
#include "ApeVector2.h"

namespace Ape
{
	struct ManualTextureParameters
	{
		unsigned int height;
		unsigned int width;
		Ape::Texture::PixelFormat pixelFormat;
		Ape::Texture::Usage usage;

		ManualTextureParameters()
		{
			this->height = 0;
			this->width = 0;
			pixelFormat = Ape::Texture::PixelFormat::PF_INVALID;
			usage = Ape::Texture::Usage::U_INVALID;
		}

		ManualTextureParameters(unsigned int height, unsigned int width, Ape::Texture::PixelFormat pixelFormat, Ape::Texture::Usage usage)
		{
			this->height = height;
			this->width = width;
			this->pixelFormat = pixelFormat;
			this->usage = usage;
		}
	};

	class IManualTexture : public Texture
	{
	protected:
		IManualTexture(std::string name) : Texture(name, Entity::TEXTURE_MANUAL) {}

		virtual ~IManualTexture() {};

	public:
		virtual void setParameters(unsigned int width, unsigned int height, Ape::Texture::PixelFormat pixelFormat, Ape::Texture::Usage usage) = 0;

		virtual Ape::ManualTextureParameters getParameters() = 0;

		virtual void setSourceCamera(Ape::CameraWeakPtr camera) = 0;

		virtual Ape::CameraWeakPtr getSourceCamera() = 0;

		virtual void setGraphicsApiID(void* id) = 0;

		virtual void* getGraphicsApiID() = 0;

		virtual void setBuffer(const void* buffer) = 0;

		virtual const void* getBuffer() = 0;
	};
}

#endif
