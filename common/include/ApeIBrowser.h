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

#ifndef APE_IBROWSER_H
#define APE_IBROWSER_H

#include "ApeEntity.h"
#include "ApeGeometry.h"
#include "ApeVector2.h"

namespace Ape
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
			Ape::Vector2 position;
			MouseClick click;

			MouseState()
			{
				this->position = Ape::Vector2();
				this->click = MouseClick::UNKNOWN;
			}

			MouseState(Ape::Vector2 position, MouseClick click)
			{
				this->position = position;
				this->click = click;
			}
		};
	}
	class IBrowser : public Entity
	{
	protected:
	    IBrowser(std::string name) : Entity(name, Entity::BROWSER) {}
		
		virtual ~IBrowser() {};
		
	public:
		virtual void setURL(std::string url) = 0;

		virtual std::string getURL() = 0;

		virtual void setResoultion(float vertical, float horizontal) = 0;

		virtual Ape::Vector2 getResoultion() = 0;

		virtual void setGeometry(Ape::GeometryWeakPtr geometry) = 0;

		virtual Ape::GeometryWeakPtr getGeometry() = 0;

		virtual void showOnOverlay(bool enable, int zOrder) = 0;

		virtual void setZoomLevel(int level) = 0;

		virtual int getZoomLevel() = 0;

		virtual int getZOrder() = 0;

		virtual void mouseClick(Ape::Browser::MouseClick click) = 0;

		virtual void mouseMoved(Ape::Vector2 position) = 0;

		virtual Ape::Browser::MouseState getMouseState() = 0;
	};
}

#endif
