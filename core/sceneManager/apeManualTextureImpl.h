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

#ifndef APE_MANUALTEXTUREIMPL_H
#define APE_MANUALTEXTUREIMPL_H

#include "apeISceneManager.h"
#include "apeINode.h"
#include "apeIManualTexture.h"
#include "apeEventManagerImpl.h"
#include "apeReplica.h"

namespace ape
{
	class ManualTextureImpl : public ape::IManualTexture, public ape::Replica
	{
	public:
		ManualTextureImpl(std::string name, bool replicate, std::string ownerID, bool isHost);

		~ManualTextureImpl();

		void setParameters(unsigned int width, unsigned int height, ape::Texture::PixelFormat pixelFormat, ape::Texture::Usage usage, bool gammaCorrection, bool useFsaa, bool serializeBuffer) override;

		ape::ManualTextureParameters getParameters() override;

		void setSourceCamera(ape::CameraWeakPtr camera) override;

		ape::CameraWeakPtr getSourceCamera() override;

		void setGraphicsApiID(void* id) override;

		void* getGraphicsApiID() override;

		void setBuffer(const void* buffer) override;

		const void* getBuffer() override;

		void registerFunction(std::function<void()> callback) override;

		std::vector<std::function<void()>> getFunctionList() override;

		void unRegisterFunction(std::function<void()> callback) override;

		void WriteAllocationID(RakNet::Connection_RM3 *destinationConnection, RakNet::BitStream *allocationIdBitstream) const override;

		RakNet::RM3SerializationResult Serialize(RakNet::SerializeParameters *serializeParameters) override;

		void Deserialize(RakNet::DeserializeParameters *deserializeParameters) override;

	private:
		ape::EventManagerImpl* mpEventManagerImpl;

		ape::ISceneManager* mpSceneManager;

		ape::ManualTextureParameters mParameters;

		std::string mCameraName;

		ape::CameraWeakPtr mCamera;

		const void* mpBuffer;

		void* mpGraphicsApiID;

		std::vector<std::function<void()>> mFunctions;
	};
}

#endif
