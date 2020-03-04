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

#ifndef APE_CYLINDERGEOMETRYIMPL_H
#define APE_CYLINDERGEOMETRYIMPL_H

#include "apeISceneManager.h"
#include "apeICylinderGeometry.h"
#include "apeEventManagerImpl.h"
#include "apeReplica.h"

namespace ape
{
	class CylinderGeometryImpl : public ape::ICylinderGeometry, public ape::Replica
	{
	public:
		CylinderGeometryImpl(std::string name, bool replicate, std::string ownerID, bool isHost);

		~CylinderGeometryImpl();

		void setParameters(float radius, float height, float tile) override;

		ape::GeometryCylinderParameters getParameters() override;

		void setParentNode(ape::NodeWeakPtr parentNode) override;

		void setMaterial(ape::MaterialWeakPtr material) override;

		ape::MaterialWeakPtr getMaterial() override;

		void setOwner(std::string ownerID) override;

		std::string getOwner() override;

		void WriteAllocationID(RakNet::Connection_RM3 *destinationConnection, RakNet::BitStream *allocationIdBitstream) const override;

		RakNet::RM3SerializationResult Serialize(RakNet::SerializeParameters *serializeParameters) override;

		void Deserialize(RakNet::DeserializeParameters *deserializeParameters) override;

	private:
		ape::EventManagerImpl* mpEventManagerImpl;

		ape::ISceneManager* mpSceneManager;

		ape::GeometryCylinderParameters mParameters;

		ape::MaterialWeakPtr mMaterial;

		std::string mMaterialName;
	};
}

#endif
