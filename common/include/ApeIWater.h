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

#ifndef APE_IWATER_H
#define APE_IWATER_H

#include "ApeEntity.h"
#include "ApeVector3.h"
#include "ApeColor.h"
#include "ApeDegree.h"
#include "ApeRadian.h"
#include "ApeINode.h"

namespace Ape
{
	class IWater : public Entity
	{
	protected:
	    IWater(std::string name) : Entity(name, Entity::WATER) {}
		
		virtual ~IWater() {};
		
	public:
		virtual void setParentNode(Ape::NodeWeakPtr parentNode) = 0;

		virtual Ape::NodeWeakPtr getParentNode() = 0;

	};
}

#endif
