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

#ifndef APE_FILEMATERIALIMPL_H
#define APE_FILEMATERIALIMPL_H

#include "ApeIFileMaterial.h"
#include "ApeEventManagerImpl.h"
#include "ApeReplica.h"
#include "ApeIScene.h"

namespace Ape
{

	class FileMaterialImpl : public Ape::IFileMaterial, public Ape::Replica
	{
	public:
		FileMaterialImpl(std::string name, bool isHostCreated);

		~FileMaterialImpl();
		
		void setFileName(std::string fileName) override;

		std::string getfFileName() override;

		void setAsSkyBox() override;

		void setPassTexture(Ape::TextureWeakPtr texture) override;

		void setPassGpuParameters(Ape::PassGpuParameters passGpuParameters) override;

		Ape::TextureWeakPtr getPassTexture() override;

		Ape::PassGpuParameters getPassGpuParameters() override;

		void WriteAllocationID(RakNet::Connection_RM3 *destinationConnection, RakNet::BitStream *allocationIdBitstream) const override;

		RakNet::RM3SerializationResult Serialize(RakNet::SerializeParameters *serializeParameters) override;

		void Deserialize(RakNet::DeserializeParameters *deserializeParameters) override;

	private:
		Ape::EventManagerImpl* mpEventManagerImpl;

		Ape::IScene* mpScene;

		std::string mFileName;

		bool mIsSkyBox;

		Ape::TextureWeakPtr mTexture;

		std::string mTextureName;

		Ape::PassGpuParameters mPassGpuParameters;
	};
}

#endif
