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

#ifndef APE_IMANUALMATERIAL_H
#define APE_IMANUALMATERIAL_H

#include "ApeMaterial.h"

namespace Ape
{	
	class IManualMaterial : public Material
	{
	protected:
	    IManualMaterial(std::string name, std::string parentNodeName) : Material(name, parentNodeName, Entity::MATERIAL_MANUAL) {}
		
		virtual ~IFileMaterial() {};
		
	public:
		virtual void setDiffuseColor(Color diffuse) = 0;

		virtual void setSpecularColor(Color specular) = 0;

		virtual void setAmbientColor(Color ambient) = 0;

		virtual void setEmissiveColor(Color emissive) = 0;

		virtual void setShininess(float shininess) = 0;
		
		virtual void setSceneBlend(bool enable) = 0;
		
		virtual void setTextures(Textures textures) = 0;
	};
}

#endif

