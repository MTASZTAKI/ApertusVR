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

#ifndef APE_POINTCLOUDIMPL_H
#define APE_POINTCLOUDIMPL_H

#include "ApeIPointCloud.h"
#include "ApeEventManagerImpl.h"
#include "ApeReplica.h"
#include "ApeIScene.h"

namespace Ape
{
	class PointCloudImpl : public IPointCloud, public Ape::Replica
	{
	public:

		PointCloudImpl(std::string name, bool isHostCreated);

		~PointCloudImpl();

		void setParameters(Ape::PointCloudPoints points, Ape::PointCloudColors colors, float boundigSphereRadius) override;

		Ape::PointCloudSetParameters getParameters() override;

		void updatePoints(Ape::PointCloudPoints points) override;

		void updateColors(Ape::PointCloudColors colors) override;

		Ape::PointCloudPoints getCurrentPoints() override;

		Ape::PointCloudColors getCurrentColors() override;

		void setParentNode(Ape::NodeWeakPtr parentNode) override;

		Ape::NodeWeakPtr getParentNode() override;

		void WriteAllocationID(RakNet::Connection_RM3 *destinationConnection, RakNet::BitStream *allocationIdBitstream) const override;

		RakNet::RM3SerializationResult Serialize(RakNet::SerializeParameters *serializeParameters) override;

		void Deserialize(RakNet::DeserializeParameters *deserializeParameters) override;

	private:
		Ape::EventManagerImpl* mpEventManagerImpl;

		Ape::IScene* mpScene;

		Ape::NodeWeakPtr mParentNode;

		std::string mParentNodeName;

		Ape::PointCloudSetParameters mParameters;

		Ape::PointCloudPoints mCurrentPoints;

		Ape::PointCloudColors mCurrentColors;
	};
}

#endif
