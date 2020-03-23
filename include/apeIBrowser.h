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

#ifndef APE_IBROWSER_H
#define APE_IBROWSER_H

#include "apeGeometry.h"
#include "apeVector2.h"
#include "apeEntity.h"

namespace ape
{
	namespace Browser
	{
		enum MouseClick
		{
			UNKNOWN,
			LEFT,
			RIGHT,
			MIDDLE,
			INVALID
		};
		struct MouseState
		{
			ape::Vector2 position;
			MouseClick click;
			bool isClickDown;
			ape::Vector2 scrollDelta;

			MouseState()
			{
				this->position = ape::Vector2();
				this->click = MouseClick::UNKNOWN;
				this->isClickDown = false;
				this->scrollDelta = ape::Vector2();
			}

			MouseState(ape::Vector2 position, MouseClick click, bool isClickDown, ape::Vector2 scrollDelta)
			{
				this->position = position;
				this->click = click;
				this->isClickDown = isClickDown;
				this->scrollDelta = scrollDelta;
			}
		};
	}
	class IBrowser : public Entity
	{
	protected:
		IBrowser(std::string name, bool replicate, std::string ownerID) : Entity(name, Entity::BROWSER, replicate, ownerID) {}

		virtual ~IBrowser() {};

	public:
		virtual void setURL(std::string url) = 0;

		virtual std::string getURL() = 0;

		virtual void setResoultion(float vertical, float horizontal) = 0;

		virtual ape::Vector2 getResoultion() = 0;

		virtual void setGeometry(ape::GeometryWeakPtr geometry) = 0;

		virtual ape::GeometryWeakPtr getGeometry() = 0;

		virtual void showOnOverlay(bool enable, int zOrder) = 0;

		virtual void setZoomLevel(int level) = 0;

		virtual int getZoomLevel() = 0;

		virtual int getZOrder() = 0;

		virtual void mouseClick(ape::Browser::MouseClick click, bool isClickDown) = 0;

		virtual void mouseMoved(ape::Vector2 position) = 0;

		virtual void mouseScroll(ape::Vector2 delta) = 0;

		virtual ape::Browser::MouseState getMouseState() = 0;

		virtual void keyASCIIValue(int keyASCIIValue) = 0;

		virtual int getLastKeyASCIIValue() = 0;

		virtual bool isFocusOnEditableField() = 0;

		virtual void setFocusOnEditableField(bool enable) = 0;

		virtual void reload() = 0;

		virtual void setOwner(std::string ownerID) = 0;

		virtual std::string getOwner() = 0;

		virtual void setClickedElementName(std::string clickedElementName) = 0;

		virtual std::string getClickedElementName() = 0;
	};
}

#endif
