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

#ifndef APE_BROWSERIMPL_H
#define APE_BROWSERIMPL_H

#include "apeISceneManager.h"
#include "apeIBrowser.h"
#include "apeEventManagerImpl.h"
#include "apeReplica.h"

namespace ape
{
	class BrowserImpl : public IBrowser, public ape::Replica
	{
	public:

		BrowserImpl(std::string name, bool replicate, std::string ownerID, bool isHost);

		~BrowserImpl();

		void setURL(std::string url) override;

		std::string getURL() override;

		void setResoultion(float vertical, float horizontal) override;

		ape::Vector2 getResoultion() override;

		void setGeometry(ape::GeometryWeakPtr geometry) override;

		ape::GeometryWeakPtr getGeometry() override;

		void showOnOverlay(bool enable, int zOrder) override;

		void setZoomLevel(int level) override;

		int getZoomLevel() override;

		int getZOrder() override;

		void mouseClick(ape::Browser::MouseClick click, bool isClickDown) override;

		void mouseMoved(ape::Vector2 position) override;

		void mouseScroll(ape::Vector2 delta) override;

		ape::Browser::MouseState getMouseState() override;

		void keyASCIIValue(int keyASCIIValue) override;

		int getLastKeyASCIIValue() override;

		bool isFocusOnEditableField() override;

		void setFocusOnEditableField(bool enable) override;

		void reload() override;

		void WriteAllocationID(RakNet::Connection_RM3 *destinationConnection, RakNet::BitStream *allocationIdBitstream) const override;

		RakNet::RM3SerializationResult Serialize(RakNet::SerializeParameters *serializeParameters) override;

		void Deserialize(RakNet::DeserializeParameters *deserializeParameters) override;

	private:
		ape::EventManagerImpl* mpEventManagerImpl;

		ape::ISceneManager* mpSceneManager;

		ape::GeometryWeakPtr mGeometry;

		std::string mGeometryName;

		std::string mURL;

		ape::Vector2 mResoultion;

		int mZoomLevel;

		int mZOrder;

		unsigned int mID;

		ape::Browser::MouseClick mMouseLastClick;

		bool mMouseLastClickIsDown;

		ape::Vector2 mMouseLastPosition;

		ape::Vector2 mMouseScrollDelta;

		int mLastKeyValue;

		bool mIsFocusOnEditableField;

		bool mReloadDeltaTrigger;
	};
}

#endif
