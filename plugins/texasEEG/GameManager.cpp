#include "GameManager.h"

TexasEEG::GameManager::GameManager(Ape::NodeWeakPtr userNode)
{
	mpScene = Ape::IScene::getSingletonPtr();
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
	LOG(LOG_TYPE_DEBUG, "timeText create");
	mTimeText = mpScene->createEntity("timeTextKrixkrax", Ape::Entity::GEOMETRY_TEXT);
	if (auto timeText = std::static_pointer_cast<Ape::ITextGeometry>(mTimeText.lock()))
	{
		timeText->setCaption(std::to_string(mTime));
		timeText->setOffset(Ape::Vector3(-18.0f, 10.0f, -30.0f));
		timeText->setParentNode(mUserNode);
		LOG(LOG_TYPE_DEBUG, "timeText create ok");
	}

	/*if (auto planeNode = mpScene->createNode("planeNode").lock())
	{
		planeNode->setPosition(Ape::Vector3(0, 0, -30));
		planeNode->setOrientation(Ape::Quaternion(Ape::Degree(90), Ape::Vector3(1, 0, 0)));
		planeNode->setParentNode(mUserNode);

		if (auto plane = std::static_pointer_cast<Ape::IPlaneGeometry>(mpScene->createEntity("timePlane", Ape::Entity::GEOMETRY_PLANE).lock()))
		{
			plane->setParameters(Ape::Vector2(1, 1), Ape::Vector2(10, 10), Ape::Vector2(1, 1));
			plane->setParentNode(planeNode);
			if (auto planeMaterial = std::static_pointer_cast<Ape::IManualMaterial>(mpScene->createEntity("planeMaterial", Ape::Entity::MATERIAL_MANUAL).lock()))
			{
				planeMaterial->setDiffuseColor(Ape::Color(0.29f, 0.266f, 0.29f));
				planeMaterial->setSpecularColor(Ape::Color(0.29f, 0.266f, 0.29f));
				if (auto planeMaterialManualPass = std::static_pointer_cast<Ape::IManualPass>(mpScene->createEntity("planeMaterialManualPass", Ape::Entity::PASS_MANUAL).lock()))
				{
					planeMaterialManualPass->setShininess(15.0f);
					planeMaterialManualPass->setDiffuseColor(Ape::Color(0.29f, 0.266f, 0.29f));
					planeMaterialManualPass->setSpecularColor(Ape::Color(0.29f, 0.266f, 0.29f));
					planeMaterial->setPass(planeMaterialManualPass);
				}
				plane->setMaterial(planeMaterial);
			}
		}
	}*/

	LOG(LOG_TYPE_DEBUG, "scoreText create");
	mScoreText = mpScene->createEntity("scoreText", Ape::Entity::GEOMETRY_TEXT);
	if (auto scoreText = std::static_pointer_cast<Ape::ITextGeometry>(mScoreText.lock()))
	{
		scoreText->setCaption(std::to_string(mScore));
		scoreText->setOffset(Ape::Vector3(20.0f, 10.0f, -30.0f));
		scoreText->setParentNode(mUserNode);
		LOG(LOG_TYPE_DEBUG, "scoreText ok");
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
	LOG(LOG_TYPE_DEBUG, "gameOver");
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
	if (auto timeText = std::static_pointer_cast<Ape::ITextGeometry>(mTimeText.lock()))
	{
		timeText->setCaption(std::to_string(mTime));
	}
}

void TexasEEG::GameManager::UpdateScore(int score)
{
	if (auto scoreText = std::static_pointer_cast<Ape::ITextGeometry>(mScoreText.lock()))
	{
		scoreText->setCaption(std::to_string(mScore));
	}
}

void TexasEEG::GameManager::UpdateStatus()
{
	mStatusText = mpScene->createEntity("statusText", Ape::Entity::GEOMETRY_TEXT);
	if (auto statusText = std::static_pointer_cast<Ape::ITextGeometry>(mStatusText.lock()))
	{
		statusText->setCaption("Game Over");
		statusText->setOffset(Ape::Vector3(0.0f, 0.0f, -30.0f));
		statusText->setParentNode(mUserNode);
	}
}
