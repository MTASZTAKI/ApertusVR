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

#ifndef APE_UNITTEXTUREIMPL_H
#define APE_UNITTEXTUREIMPL_H

#include "ApeIUnitTexture.h"
#include "ApeEventManagerImpl.h"
#include "ApeISceneManager.h"
#include "ApeINode.h"
#include "ApeReplica.h"

namespace Ape
{
	class UnitTextureImpl : public Ape::IUnitTexture, public Ape::Replica
	{
	public:
		UnitTextureImpl(std::string name, bool isHostCreated);

		~UnitTextureImpl();

		void setParameters(Ape::MaterialWeakPtr material, std::string fileName) override;

		Ape::IUnitTexture::Parameters getParameters() override;

		void setTextureScroll(float u, float v) override;

		Ape::Vector2 getTextureScroll() override;

		void setTextureAddressingMode(Ape::Texture::AddressingMode addressingMode) override;

		Ape::Texture::AddressingMode getTextureAddressingMode() override;

		void setTextureFiltering(Ape::Texture::Filtering minFilter, Ape::Texture::Filtering magFilter, Ape::Texture::Filtering mipFilter) override;

		Filtering getTextureFiltering() override;

		void WriteAllocationID(RakNet::Connection_RM3 *destinationConnection, RakNet::BitStream *allocationIdBitstream) const override;

		RakNet::RM3SerializationResult Serialize(RakNet::SerializeParameters *serializeParameters) override;

		void Deserialize(RakNet::DeserializeParameters *deserializeParameters) override;

	private:
		Ape::EventManagerImpl* mpEventManagerImpl;

		Ape::ISceneManager* mpSceneManager;

		Ape::Vector2 mScroll;

		Ape::Texture::AddressingMode mAddressingMode;

		Ape::IUnitTexture::Filtering mFiltering;

		Ape::IUnitTexture::Parameters mParameters;

	};
}

#endif
