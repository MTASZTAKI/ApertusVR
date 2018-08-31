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

#ifndef APE_BUBBLEMANAGER_H
#define APE_BUBBLEMANAGER_H

#include <deque>
#include <thread>
#include <mutex>
#include "Ape.h"
#include "ApeILogManager.h"
#include "Bubble.h"

namespace TexasEEG
{
	class BubbleManager
	{
	private:
		Ape::NodeWeakPtr mUserNode;

		std::thread* mTimerThread;

		std::deque<TexasEEG::Bubble*> mBubbleQueue;

		std::deque<TexasEEG::Bubble*> mActivatedBubbleQueue;

		std::mutex lockMutex;

		int mSkippedValue;

		bool mGameOver;

		void Run();

		void UpdateTimers();

	public:
		BubbleManager(Ape::NodeWeakPtr userNode);

		~BubbleManager();

		void CreateBubbles(int num);

		void RemoveBubbles(int num);

		void StartBubbles(int num);

		void StartGame();

		bool isGameOver();

		int getSkippedValue();

		void resetSkippedValue();

		std::deque<TexasEEG::Bubble*>* getAvtivatedBubblesQueue();
	};
}

#endif
