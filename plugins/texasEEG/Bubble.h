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

#ifndef APE_BUBBLE_H
#define APE_BUBBLE_H

#include "Ape.h"
#include "ApeILogManager.h"
#include "ApeIScene.h"
#include "ApeISphereGeometry.h"
#include "ApeITextGeometry.h"

namespace TexasEEG
{
	class Bubble
	{
	private:
		Ape::IScene* mpScene;

		Ape::NodeWeakPtr mBubbleNode;

		Ape::EntityWeakPtr mGeometry;

		Ape::EntityWeakPtr mCounterText;

		Ape::Vector3 mPosition;

		std::thread* mTimerThread;

		int mTimerCount;

		static int geometryCount;

		bool mIsTimedOut;

		void setText(std::string caption);

	public:
		Bubble(Ape::Vector3 pos = Ape::Vector3(0, 0, 0), int maxCount = 10);

		~Bubble();

		void startThread();

		void init();

		void startCounter();

		void hide();

		Ape::Vector3 getPosition();

		bool isTimedOut();
		
	};
}

#endif
