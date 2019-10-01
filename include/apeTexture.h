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

#ifndef APE_TEXTURE_H
#define APE_TEXTURE_H

#include <string>
#include <vector>
#include "apeEntity.h"
#include "apePass.h"

namespace ape
{
	class Texture : public Entity
	{
	public:
		enum PixelFormat
		{
			PF_NONE,
			R8G8B8,
			R8G8B8A8,
			A8R8G8B8,
			PF_INVALID
		};
		enum Usage
		{
			U_NONE,
			RENDERTARGET,
			DYNAMIC_WRITE_ONLY,
			U_INVALID
		};
		enum AddressingMode
		{
			AM_NONE,
			WRAP,
			MIRROR,
			CLAMP,
			BORDER,
			AM_INVALID
		};
		enum Filtering
		{
			F_NONE,
			POINT,
			LINEAR,
			ANISOTROPIC,
			F_INVALID
		};

	protected:
		Texture(std::string name, Entity::Type entityType) : Entity(name, entityType) {}

		virtual ~Texture() {};

	};
}

#endif
