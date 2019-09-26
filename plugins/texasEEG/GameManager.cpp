#include "GameManager.h"

TexasEEG::GameManager::GameManager(ape::NodeWeakPtr userNode)
{
	mpSceneManager = ape::ISceneManager::getSingletonPtr();
	mTime = 0;
	mScore = 0;
	mUserNode = userNode;
	mBubbleManager = new TexasEEG::BubbleManager(userNode);

	Init();
}

TexasEEG::GameManager::~GameManager()
{
}

void TexasEEG::GameManager::Init()
{
	APE_LOG_TRACE("timeText create");
	if (auto timerNode = mpSceneManager->createNode("timerNode").lock())
	{
		timerNode->setParentNode(mUserNode);
		timerNode->setPosition(ape::Vector3(-18, 10, -30));

		mTimeText = mpSceneManager->createEntity("timeTextKrixkrax", ape::Entity::GEOMETRY_TEXT);
		if (auto timeText = std::static_pointer_cast<ape::ITextGeometry>(mTimeText.lock()))
		{
			timeText->setCaption(std::to_string(mTime));
			timeText->showOnTop(true);
			timeText->setParentNode(timerNode);
			APE_LOG_TRACE("timeText create ok");
		}
	}

	APE_LOG_TRACE("scoreText create");
	if (auto scoreNode = mpSceneManager->createNode("scoreNode").lock())
	{
		scoreNode->setParentNode(mUserNode);
		scoreNode->setPosition(ape::Vector3(18, 10, -30));

		mScoreText = mpSceneManager->createEntity("scoreText", ape::Entity::GEOMETRY_TEXT);
		if (auto scoreText = std::static_pointer_cast<ape::ITextGeometry>(mScoreText.lock()))
		{
			scoreText->setCaption(std::to_string(mScore));
			scoreText->showOnTop(true);
			scoreText->setParentNode(scoreNode);
			APE_LOG_TRACE("scoreText ok");
		}
	}

	mTimerThread = new std::thread(std::bind(&GameManager::Timer, this));
	mTimerThread->detach();
}

void TexasEEG::GameManager::Timer()
{
	while (true)
	{
		UpdateTime();
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	}
}

void TexasEEG::GameManager::Run()
{
	while (mBubbleManager->isGameOver() == false)
	{
		if (auto userNode = mUserNode.lock())
		{
			for (int i = 0; i < mBubbleManager->getAvtivatedBubblesQueue()->size(); i++)
			{
				if (userNode->getPosition().distance(mBubbleManager->getAvtivatedBubblesQueue()->at(i)->getPosition()) < 30)
				{					
					std::lock_guard<std::mutex> lock(lockMutex);

					mScore += mBubbleManager->getAvtivatedBubblesQueue()->at(i)->getCounter();

					mBubbleManager->getAvtivatedBubblesQueue()->at(i)->finish();
					mBubbleManager->getAvtivatedBubblesQueue()->erase(mBubbleManager->getAvtivatedBubblesQueue()->begin() + i);

					mBubbleManager->CreateBubbles(1);
					mBubbleManager->StartBubbles(1);

					break;
				}
				mScore -= mBubbleManager->getSkippedValue();
				mBubbleManager->resetSkippedValue();
				UpdateScore(mScore);
			}
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(20));
	}
	APE_LOG_TRACE("gameOver");
}

void TexasEEG::GameManager::Start()
{
	mBubbleManager->CreateBubbles(3);
	mBubbleManager->StartBubbles(3);
	mBubbleManager->StartGame();

	mGameThread = new std::thread(std::bind(&GameManager::Run, this));
	mGameThread->detach();
}

void TexasEEG::GameManager::Pause()
{
}

void TexasEEG::GameManager::Stop()
{
}

int TexasEEG::GameManager::GetScore()
{
	return mScore;
}

void TexasEEG::GameManager::UpdateTime()
{
	mTime++;
	if (auto timeText = std::static_pointer_cast<ape::ITextGeometry>(mTimeText.lock()))
	{
		timeText->setCaption(std::to_string(mTime));
	}
}

void TexasEEG::GameManager::UpdateScore(int score)
{
	if (auto scoreText = std::static_pointer_cast<ape::ITextGeometry>(mScoreText.lock()))
	{
		scoreText->setCaption(std::to_string(mScore));
	}
}

void TexasEEG::GameManager::UpdateStatus()
{
	mStatusText = mpSceneManager->createEntity("statusText", ape::Entity::GEOMETRY_TEXT);
	if (auto statusText = std::static_pointer_cast<ape::ITextGeometry>(mStatusText.lock()))
	{
		statusText->setCaption("Game Over");
		statusText->setParentNode(mUserNode);
	}
}
