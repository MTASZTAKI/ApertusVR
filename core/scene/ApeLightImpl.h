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

#ifndef APE_LIGHTIMPL_H
#define APE_LIGHTIMPL_H

#include "ApeILight.h"
#include "ApeEventManagerImpl.h"
#include "ApeReplica.h"
#include "ApeIScene.h"

namespace Ape
{
	class LightImpl : public ILight, public Ape::Replica
	{
	public:

		LightImpl(std::string name, bool isHostCreated);

		~LightImpl();
		
		Light::Type getLightType() override;
		
		Color getDiffuseColor() override;
		
		Color getSpecularColor()override;
		
		LightSpotRange getLightSpotRange()override;
		
		LightAttenuation getLightAttenuation() override;
		
		Vector3 getLightDirection() override;
		
		void setLightType(Light::Type lightType) override;
		
		void setDiffuseColor(Color diffuseColor) override;
		
		void setSpecularColor(Color specularColor) override;
		
		void setLightSpotRange(LightSpotRange lightSpotRange) override;
		
		void setLightAttenuation(LightAttenuation lightAttenuation) override; 
		
		void setLightDirection(Vector3 lightDirection) override;

		void setParentNode(Ape::NodeWeakPtr parentNode) override;

		Ape::NodeWeakPtr getParentNode() override;

		void WriteAllocationID(RakNet::Connection_RM3 *destinationConnection, RakNet::BitStream *allocationIdBitstream) const override;

		RakNet::RM3SerializationResult Serialize(RakNet::SerializeParameters *serializeParameters) override;

		void Deserialize(RakNet::DeserializeParameters *deserializeParameters) override;

	private:
		Ape::EventManagerImpl* mpEventManagerImpl;

		Ape::IScene* mpScene;

		Light::Type mLightType;

		Color mDiffuseColor;

		Color mSpecularColor;

		LightSpotRange mLightSpotRange;

		LightAttenuation mLightAttenuation;

		Vector3 mLightDirection;

		NodeWeakPtr mParentNode;

		std::string mParentNodeName;
	};
}

#endif
