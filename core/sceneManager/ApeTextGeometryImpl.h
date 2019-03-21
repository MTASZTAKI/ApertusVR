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

#ifndef APE_TEXTGEOMETRYIMPL_H
#define APE_TEXTGEOMETRYIMPL_H

#include "managers/ApeISceneManager.h"
#include "sceneelements/ApeITextGeometry.h"
#include "sceneelements/ApeINode.h"
#include "ApeEventManagerImpl.h"
#include "ApeReplica.h"

namespace ape
{
	class TextGeometryImpl : public ape::ITextGeometry, public ape::Replica
	{
	public:
		TextGeometryImpl(std::string name, bool isHostCreated);

		~TextGeometryImpl();
		
		std::string getCaption() override;
		
		void setCaption(std::string caption) override;

		void clearCaption() override;

		bool isVisible() override;
		
		void setVisible(bool enabled) override;
	
		void showOnTop(bool show) override;

		bool isShownOnTop() override;

		void setParentNode(ape::NodeWeakPtr parentNode) override;

		void WriteAllocationID(RakNet::Connection_RM3 *destinationConnection, RakNet::BitStream *allocationIdBitstream) const override;

		RakNet::RM3SerializationResult Serialize(RakNet::SerializeParameters *serializeParameters) override;

		void Deserialize(RakNet::DeserializeParameters *deserializeParameters) override;

	private:
		ape::EventManagerImpl* mpEventManagerImpl;

		ape::ISceneManager* mpSceneManager;

		std::string mCaption;

		bool mVisibility;

		bool mShowOnTop;
	};
}

#endif
