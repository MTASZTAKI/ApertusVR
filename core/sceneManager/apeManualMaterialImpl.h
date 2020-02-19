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

#ifndef APE_MANUALMATERIALIMPL_H
#define APE_MANUALMATERIALIMPL_H

#include "apeILogManager.h"
#include "apeISceneManager.h"
#include "apeINode.h"
#include "apeTexture.h"
#include "apeIManualMaterial.h"
#include "apeEventManagerImpl.h"
#include "apeReplica.h"

namespace ape
{
	class ManualMaterialImpl : public ape::IManualMaterial, public ape::Replica
	{
	public:
		ManualMaterialImpl(std::string name, bool isHost);

		~ManualMaterialImpl();

		void setDiffuseColor(ape::Color diffuse) override;

		void setSpecularColor(ape::Color specular) override;

		void setTexture(ape::TextureWeakPtr texture) override;

		void setAmbientColor(Color ambient) override;

		void setEmissiveColor(Color emissive) override;

		ape::TextureWeakPtr getTexture() override;

		void setCullingMode(ape::Material::CullingMode cullingMode) override;

		void setSceneBlending(ape::Material::SceneBlendingType sceneBlendingType) override;

		void setDepthWriteEnabled(bool enable) override;

		void setDepthCheckEnabled(bool enable) override;

		void setLightingEnabled(bool enable) override;

		void setManualCullingMode(ape::Material::ManualCullingMode manualcullingMode) override;

		void setDepthBias(float constantBias, float slopeScaleBias) override;

		void showOnOverlay(bool enable, int zOrder) override;

		int getZOrder() override;

		void WriteAllocationID(RakNet::Connection_RM3 *destinationConnection, RakNet::BitStream *allocationIdBitstream) const override;

		RakNet::RM3SerializationResult Serialize(RakNet::SerializeParameters *serializeParameters) override;

		void Deserialize(RakNet::DeserializeParameters *deserializeParameters) override;

	private:
		ape::EventManagerImpl* mpEventManagerImpl;

		ape::ISceneManager* mpSceneManager;

		ape::TextureWeakPtr mTexture;

		std::string mTextureName;

		int mZOrder;
	};
}

#endif
