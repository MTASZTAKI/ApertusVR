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

#ifndef APE_RAYGEOMETRYIMPL_H
#define APE_RAYGEOMETRYIMPL_H

#include "managers/apeISceneManager.h"
#include "sceneelements/apeINode.h"
#include "sceneelements/apeIRayGeometry.h"
#include "apeEventManagerImpl.h"
#include "apeReplica.h"

namespace ape
{
	class RayGeometryImpl : public ape::IRayGeometry, public ape::Replica
	{
	public:
		RayGeometryImpl(std::string name, bool isHostCreated);

		~RayGeometryImpl();

		void setIntersectingEnabled(bool enable) override;

		void setIntersections(std::vector<ape::EntityWeakPtr> intersections) override;

		void fireIntersectionQuery() override;

		void setParentNode(ape::NodeWeakPtr parentNode) override;

		void WriteAllocationID(RakNet::Connection_RM3 *destinationConnection, RakNet::BitStream *allocationIdBitstream) const override;

		RakNet::RM3SerializationResult Serialize(RakNet::SerializeParameters *serializeParameters) override;

		void Deserialize(RakNet::DeserializeParameters *deserializeParameters) override;

	private:
		ape::EventManagerImpl* mpEventManagerImpl;

		ape::ISceneManager* mpSceneManager;
	};
}

#endif
