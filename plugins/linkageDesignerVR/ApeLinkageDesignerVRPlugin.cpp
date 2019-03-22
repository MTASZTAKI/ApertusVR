#include "apeLinkageDesignerVRPlugin.h"

ape::apeLinkageDesignerVRPlugin::apeLinkageDesignerVRPlugin()
{
	APE_LOG_FUNC_ENTER();
	mpCoreConfig = ape::ICoreConfig::getSingletonPtr();
	mpEventManager = ape::IEventManager::getSingletonPtr();
	mpEventManager->connectEvent(ape::Event::Group::CAMERA, std::bind(&apeLinkageDesignerVRPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(ape::Event::Group::NODE, std::bind(&apeLinkageDesignerVRPlugin::eventCallBack, this, std::placeholders::_1));
	mpSceneManager = ape::ISceneManager::getSingletonPtr();
	mpSceneMakerMacro = new ape::SceneMakerMacro();
	mSwitchNodeVisibilityToggleIndex = 0;
	/*mSwitchNodeVisibilityNames = std::vector<std::string>();
	mSwitchNodeVisibilityNames.push_back("WeldingFixture@base1Switch");
	mSwitchNodeVisibilityNames.push_back("WeldingFixture@base2Switch");
	mSwitchNodeVisibilityNames.push_back("WeldingFixture@base3Switch");
	mSwitchNodeVisibilityNames.push_back("WeldingFixture@base4Switch");*/
	/*mSwitchNodeVisibilityNames.push_back("WeldingFixture@p1Switch");
	mSwitchNodeVisibilityNames.push_back("WeldingFixture@p2Switch");
	mSwitchNodeVisibilityNames.push_back("WeldingFixture@p3Switch");
	mSwitchNodeVisibilityNames.push_back("WeldingFixture@p4Switch");*/
	//mSwitchNodeVisibilityNames.push_back("WeldingFixture@fixture_1Switch");
	mSwitchNodeVisibilityNames.push_back("WeldingFixture@wp_1Switch");
	mSwitchNodeVisibilityNames.push_back("WeldingFixture@wp_2Switch");
	mSwitchNodeVisibilityNames.push_back("WeldingFixture@wp_3Switch");
	mSwitchNodeVisibilityNames.push_back("WeldingFixture@wp_4Switch");
	mSwitchNodeVisibilityNames.push_back("WeldingFixture@wp_5Switch");
	mSwitchNodeVisibilityNames.push_back("WeldingFixture@wp_6Switch");
	mSwitchNodeVisibilityNames.push_back("WeldingFixture@wp_7Switch");
	mSwitchNodeVisibilityNames.push_back("WeldingFixture@wp_8Switch");
	//mSwitchNodeVisibilityNames.push_back("WeldingFixture@WorkbenchSwitch");
	/*mSwitchNodeVisibilityNames.push_back("Bounding@BoxSwitch");*/
	mSwitchNodes = std::vector<ape::NodeWeakPtr>();
	APE_LOG_FUNC_LEAVE();
}

ape::apeLinkageDesignerVRPlugin::~apeLinkageDesignerVRPlugin()
{
	APE_LOG_FUNC_ENTER();
	mpEventManager->disconnectEvent(ape::Event::Group::CAMERA, std::bind(&apeLinkageDesignerVRPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->disconnectEvent(ape::Event::Group::NODE, std::bind(&apeLinkageDesignerVRPlugin::eventCallBack, this, std::placeholders::_1));
	APE_LOG_FUNC_LEAVE();
}

void ape::apeLinkageDesignerVRPlugin::eventCallBack(const ape::Event& event)
{
	if (event.type == ape::Event::Type::NODE_CREATE)
	{
		if (auto node = mpSceneManager->getNode(event.subjectName).lock())
		{
			for (auto switchNodeName : mSwitchNodeVisibilityNames)
			{
				std::size_t found = node->getName().find(switchNodeName);
				if (found != std::string::npos)
					mSwitchNodes.push_back(node);
			}
		}
	}
}

void ape::apeLinkageDesignerVRPlugin::Init()
{
	APE_LOG_FUNC_ENTER();
	mpSceneMakerMacro->makeLit();
	mpSceneMakerMacro->makeBackground();
	APE_LOG_FUNC_LEAVE();
}

void ape::apeLinkageDesignerVRPlugin::toggleSwitchNodesVisibility()
{
	APE_LOG_FUNC_ENTER();
	if (mSwitchNodes.size() > 0)
	{
		if (auto switchNode = mSwitchNodes[mSwitchNodeVisibilityToggleIndex].lock())
		{
			if (!switchNode->getChildrenVisibility())
				switchNode->setChildrenVisibility(true);
			else
				switchNode->setChildrenVisibility(false);
		}
		mSwitchNodeVisibilityToggleIndex++;
		if (mSwitchNodes.size() == mSwitchNodeVisibilityToggleIndex)
			mSwitchNodeVisibilityToggleIndex = 0;
	}
	APE_LOG_FUNC_LEAVE();
}

void ape::apeLinkageDesignerVRPlugin::Run()
{
	APE_LOG_FUNC_ENTER();
	while (true)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(20));
	}
	APE_LOG_FUNC_LEAVE();
}

void ape::apeLinkageDesignerVRPlugin::Step()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::apeLinkageDesignerVRPlugin::Stop()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::apeLinkageDesignerVRPlugin::Suspend()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::apeLinkageDesignerVRPlugin::Restart()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}
