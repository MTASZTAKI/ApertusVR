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

#ifndef APE_ISKY_H
#define APE_ISKY_H

#include "ape.h"
#include "datatypes/apeEntity.h"
#include "datatypes/apeVector3.h"
#include "datatypes/apeColor.h"
#include "datatypes/apeDegree.h"
#include "datatypes/apeRadian.h"
#include "sceneelements/apeILight.h"
#include "sceneelements/apeINode.h"
#include "sceneelements/apeICamera.h"

namespace ape
{
	class ISky : public Entity
	{
	public:
		struct Time
		{
			float currentTime;
			float sunRiseTime;
			float sunSetTime;

			Time()
			{
				this->currentTime = 0.0f;
				this->sunRiseTime = 0.0f;
				this->sunSetTime = 0.0f;
			}

			Time(float currentTime, float sunRiseTime = 6.0f, float sunSetTime = 18.0f)
			{
				this->currentTime = currentTime;
				this->sunRiseTime = sunRiseTime;
				this->sunSetTime = sunSetTime;
			}
		};
	protected:
		ISky(std::string name) : Entity(name, Entity::SKY) {}

		virtual ~ISky() {};

	public:
		virtual void setTime(float startTime, float sunRiseTime = 6.0f, float sunSetTime = 18.0f) = 0;

		virtual Time getTime() = 0;

		virtual void setSunLight(ape::LightWeakPtr sunLight) = 0;

		virtual ape::LightWeakPtr getSunLight() = 0;

		virtual void setSkyLight(ape::LightWeakPtr skyLight) = 0;

		virtual ape::LightWeakPtr getSkyLight() = 0;

		virtual void setSize(float size) = 0;

		virtual float getSize() = 0;
	};
}

#endif
