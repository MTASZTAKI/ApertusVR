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

#ifndef APE_GAMEMANAGER_H
#define APE_GAMEMANAGER_H

#include <thread>
#include <mutex>
#include "ape.h"
#include "managers/apeISceneManager.h"
#include "managers/apeICoreConfig.h"
#include "managers/apeILogManager.h"
#include "sceneelements/apeIPlaneGeometry.h"
#include "sceneelements/apeIManualPass.h"
#include "BubbleManager.h"

namespace TexasEEG
{
	class GameManager
	{
	private:
		ape::ISceneManager* mpSceneManager;

		std::thread* mGameThread;

		std::thread* mTimerThread;

		TexasEEG::BubbleManager* mBubbleManager;

		ape::NodeWeakPtr mUserNode;

		ape::NodeWeakPtr mUserBodyNode;

		std::mutex lockMutex;

		ape::EntityWeakPtr mStatusText;

		ape::EntityWeakPtr mTimeText;

		ape::EntityWeakPtr mScoreText;

		int mTime;

		int mScore;

		void Timer();

		void Init();

		void Run();

	public:
		GameManager(ape::NodeWeakPtr userNode, ape::NodeWeakPtr userBodyNode);

		~GameManager();

		void Start();

		void Pause();

		void Stop();

		int GetScore();

		void UpdateTime();

		void UpdateScore(int score);

		void UpdateStatus();
	};
}

#endif
