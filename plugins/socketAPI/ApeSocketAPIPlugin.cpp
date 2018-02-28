#include <iostream>
#include "ApeSocketAPIPlugin.h"

ApeSocketAPIPlugin::ApeSocketAPIPlugin()
{
	mpSystemConfig = Ape::ISystemConfig::getSingletonPtr();
	mpEventManager = Ape::IEventManager::getSingletonPtr();
	mpEventManager->connectEvent(Ape::Event::Group::NODE, std::bind(&ApeSocketAPIPlugin::eventCallBack, this, std::placeholders::_1));
	mpScene = Ape::IScene::getSingletonPtr();
}

ApeSocketAPIPlugin::~ApeSocketAPIPlugin()
{
	std::cout << THIS_PLUGINNAME << "::dtor" << std::endl;
}

void ApeSocketAPIPlugin::eventCallBack(const Ape::Event& event)
{
	std::cout << THIS_PLUGINNAME << "::eventCallBack: boradcasting: " << std::endl;

	std::stringstream json;
	json << "{" << std::endl;
	json << "\"subjectName\": " << "\"" << event.subjectName << "\"," << std::endl;
	json << "\"type\": " << (int)event.type << "," << std::endl;
	json << "\"group\": " << (int)event.group << std::endl;
	json << "}" << std::endl;

	std::cout << json.str().c_str() << std::endl;

	mUwsHub.getDefaultGroup<uWS::SERVER>().broadcast(json.str().c_str(), json.str().length(), uWS::OpCode::TEXT);
}

void ApeSocketAPIPlugin::Init()
{
	std::cout << THIS_PLUGINNAME << "::init" << std::endl;

	mUwsHub.onMessage([](uWS::WebSocket<uWS::SERVER> *ws, char *message, size_t length, uWS::OpCode opCode) {
		std::cout << THIS_PLUGINNAME << "::socket> onMessage" << std::endl;
		ws->send(message, length, opCode);
	});

	/*mUwsHub.onHttpRequest([](uWS::HttpResponse *res, uWS::HttpRequest req, char *data, size_t length, size_t remainingBytes) {
		res->end();
	});*/
}

void ApeSocketAPIPlugin::Run()
{
	if (mUwsHub.listen(3002)) {
		std::cout << THIS_PLUGINNAME << "::Run: websocket server is listening on port 3001" << std::endl;
		mUwsHub.run();
	}
	mpEventManager->disconnectEvent(Ape::Event::Group::NODE, std::bind(&ApeSocketAPIPlugin::eventCallBack, this, std::placeholders::_1));
}

void ApeSocketAPIPlugin::Step()
{

}

void ApeSocketAPIPlugin::Stop()
{

}

void ApeSocketAPIPlugin::Suspend()
{

}

void ApeSocketAPIPlugin::Restart()
{

}
