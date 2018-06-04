#include <ctime>
#include "BubbleManager.h"

TexasEEG::BubbleManager::BubbleManager(Ape::NodeWeakPtr userNode)
{
	LOG(LOG_TYPE_DEBUG, "");
	mUserNode = userNode;
}

TexasEEG::BubbleManager::~BubbleManager()
{
	LOG(LOG_TYPE_DEBUG, "");
}

void TexasEEG::BubbleManager::Run()
{
	while (isGameOver() == false)
	{
		LOG(LOG_TYPE_DEBUG, "mGameOver: " << (mGameOver ? "true" : "false"));
		UpdateTimers();
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	}
}

void TexasEEG::BubbleManager::UpdateTimers()
{
	int sumCount = 0;
	for (int i = 0; i < mActivatedBubbleQueue.size(); i++)
	{
		std::lock_guard<std::mutex> lock(lockMutex);
		mActivatedBubbleQueue.at(i)->decCounter();
		int count = mActivatedBubbleQueue.at(i)->getCounter();
		if (count <= 0)
		{
			mSkippedValue += mActivatedBubbleQueue.at(i)->getValue();
			mActivatedBubbleQueue.at(i)->finish();
			mActivatedBubbleQueue.erase(mActivatedBubbleQueue.begin() + i);

			CreateBubbles(1);
			StartBubbles(1);
			//delete mActivatedBubbleQueue.at(i);
			break;
		}
		else
		{
			sumCount += count;
		}
	}
	if (mActivatedBubbleQueue.size() == 0)
	{
		mGameOver = true;
		LOG(LOG_TYPE_DEBUG, "gameOver");
	}
	LOG(LOG_TYPE_DEBUG, "sumCount: " << sumCount);
}

void TexasEEG::BubbleManager::CreateBubbles(int num)
{
	if (auto userNode = mUserNode.lock())
	{
		LOG(LOG_TYPE_DEBUG, "userNode.Orientation: " << userNode->getOrientation().toString());

		int newXDist = 0;
		int newYDist = 0;
		int newZDist = 0;
		int radius = 200;
		for (int i = 0; i < num; i++)
		{
			LOG(LOG_TYPE_DEBUG, "i: " << i);

			newXDist = userNode->getPosition().x + ((std::rand() % 100) - 100);
			newYDist = ((std::rand() % 100) - 100);
			newZDist = userNode->getPosition().z - ((std::rand() % 400) + 300);

			// add previous Bubble's position
			if (mBubbleQueue.size() > 1)
			{
				newXDist += mBubbleQueue.back()->getPosition().x;
				newYDist += mBubbleQueue.back()->getPosition().y;
				newZDist -= mBubbleQueue.back()->getPosition().z;
			}

			Ape::Vector3 newPos = Ape::Vector3(newXDist, newYDist, newZDist);
			LOG(LOG_TYPE_DEBUG, "mBubbleQueue.push_back(): " << newPos.toString());
			mBubbleQueue.push_back(new TexasEEG::Bubble(newPos));
		}
	}
	LOG(LOG_TYPE_DEBUG, "mBubbleQueue.size: " << mBubbleQueue.size());
}

void TexasEEG::BubbleManager::RemoveBubbles(int num)
{
	int i = 0;
	num = std::min((int)mActivatedBubbleQueue.size(), num);
	while (!mActivatedBubbleQueue.empty() && i < num)
	{
		//delete mActivatedBubbleQueue.front();
		mActivatedBubbleQueue.front()->finish();
		mActivatedBubbleQueue.pop_front();
		i++;
	}
}

void TexasEEG::BubbleManager::StartBubbles(int num)
{
	int i = 0;
	num = std::min((int)mBubbleQueue.size(), num);
	LOG(LOG_TYPE_DEBUG, "num: " << num);
	while (i < num)
	{
		mActivatedBubbleQueue.push_back(mBubbleQueue.front());
		mBubbleQueue.pop_front();

		int randTime = (std::rand() % 15) + 5;
		LOG(LOG_TYPE_DEBUG, "mActivatedBubbleQueue[" << i << "]: " << mActivatedBubbleQueue.at(i)->getName() << " time: " << randTime);
		i++;
		mActivatedBubbleQueue.back()->start(randTime);
	}
}

void TexasEEG::BubbleManager::StartGame()
{
	mGameOver = false;
	mSkippedValue = 0;

	std::srand(std::time(0));
	mTimerThread = new std::thread(std::bind(&BubbleManager::Run, this));
	mTimerThread->detach();
}

bool TexasEEG::BubbleManager::isGameOver()
{
	return mGameOver;
}

int TexasEEG::BubbleManager::getSkippedValue()
{
	return mSkippedValue;
}

void TexasEEG::BubbleManager::resetSkippedValue()
{
	mSkippedValue = 0;
}

std::deque<TexasEEG::Bubble*>* TexasEEG::BubbleManager::getAvtivatedBubblesQueue()
{
	return &mActivatedBubbleQueue;
}
