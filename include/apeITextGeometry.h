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


#ifndef APE_ITEXTGEOMETRY_H
#define APE_ITEXTGEOMETRY_H

#include <string>
#include <vector>
#include "apeGeometry.h"
#include "apeVector3.h"

namespace ape
{
	class ITextGeometry : public Geometry
	{
	protected:
		ITextGeometry(std::string name) : Geometry(name, Entity::GEOMETRY_TEXT) {}

		virtual ~ITextGeometry() {};

	public:
		virtual std::string getCaption() = 0;

		virtual void setCaption(std::string caption) = 0;

		virtual void clearCaption() = 0;

		virtual bool isVisible() = 0;

		virtual void setVisible(bool enabled) = 0;

		virtual void setParentNode(ape::NodeWeakPtr parentNode) = 0;

		virtual void showOnTop(bool show) = 0;

		virtual bool isShownOnTop() = 0;
	};
}

#endif
