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

#ifndef APE_IFILETEXTURE_H
#define APE_IFILETEXTURE_H

#include <string>
#include <vector>
#include "ApeGeometry.h"

namespace Ape
{
	class IFileTexture : public Texture
	{
	public:
		enum MapType
		{
			PF_NONE,
			DIFFUSE,
			SPECULAR,
			NORMAL,
			BUMP,
			PF_INVALID
		};

	protected:
		IFileTexture(std::string name) : Texture(name, Entity::TEXTURE_FILE) {}
		virtual ~IFileTexture() {};

	public:
		virtual void setFileName (std::string fileName) = 0;

		virtual std::string getFileName () = 0;
		
		virtual void setMapType (MapType mapType) = 0;

		virtual MapType getMapType () = 0;
	};
}

#endif
