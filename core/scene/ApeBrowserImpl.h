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

#ifndef APE_BROWSERIMPL_H
#define APE_BROWSERIMPL_H

#include "ApeIBrowser.h"
#include "ApeEventManagerImpl.h"
#include "ApeReplica.h"
#include "ApeIScene.h"

namespace Ape
{
	class BrowserImpl : public IBrowser, public Ape::Replica
	{
	public:

		BrowserImpl(std::string name, bool isHostCreated);

		~BrowserImpl();

		void setURL(std::string url) override;

		std::string getURL() override;

		void setResoultion(float vertical, float horizontal) override;

		Ape::Vector2 getResoultion() override;

		void setGeometry(Ape::GeometryWeakPtr geometry) override;

		Ape::GeometryWeakPtr getGeometry() override;

		void showOnOverlay(bool enable, int zOrder) override;

		void setZoomLevel(int level) override;

		int getZoomLevel() override;

		int getZOrder() override;

		void WriteAllocationID(RakNet::Connection_RM3 *destinationConnection, RakNet::BitStream *allocationIdBitstream) const override;

		RakNet::RM3SerializationResult Serialize(RakNet::SerializeParameters *serializeParameters) override;

		void Deserialize(RakNet::DeserializeParameters *deserializeParameters) override;

	private:
		Ape::EventManagerImpl* mpEventManagerImpl;

		Ape::IScene* mpScene;

		Ape::GeometryWeakPtr mGeometry;

		std::string mGeometryName;

		std::string mURL;

		Ape::Vector2 mResoultion;

		int mZoomLevel;

		int mZOrder;
	};
}

#endif
