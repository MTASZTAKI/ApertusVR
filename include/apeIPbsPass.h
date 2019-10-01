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

#ifndef APE_IPBSPASS_H
#define APE_IPBSPASS_H

#include <string>
#include <vector>
#include "ape.h"
#include "apeColor.h"
#include "apeEntity.h"
#include "apePass.h"
#include "apeTexture.h"

namespace ape
{
	class IPbsPass : public Pass
	{
	protected:
		IPbsPass(std::string name) : Pass(name, Entity::PASS_PBS) {}

		virtual ~IPbsPass() {};

	public:
		virtual void setAlbedo(ape::Color albedo) = 0;

		virtual void setRoughness(float roughness) = 0;

		virtual void setLightRoughnessOffset(float lightRoughnessOffset) = 0;

		virtual void setF0(ape::Color f0) = 0;

		virtual ape::Color getAlbedo() = 0;

		virtual float getRoughness() = 0;

		virtual float getLightRoughnessOffset() = 0;

		virtual ape::Color getF0() = 0;

		virtual void setDiffuseColor(Color diffuse) = 0;

		virtual void setSpecularColor(Color specular) = 0;

		virtual void setAmbientColor(Color ambient) = 0;

		virtual void setEmissiveColor(Color emissive) = 0;

		virtual void setShininess(float shininess) = 0;
	};
}

#endif
