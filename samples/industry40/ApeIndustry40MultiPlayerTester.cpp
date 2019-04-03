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


#include <thread>
#include <chrono>
#include <string>
#include <sstream> 
#include <iostream> 
#include <fstream>
#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/filewritestream.h"
#include "rapidjson/writer.h"

void runHost()
{
	std::stringstream exe;
	exe << "start " << "apeIndustry40.exe host_monitor";
	std::system(exe.str().c_str());
}

void runGuest()
{
	std::stringstream exe;
	exe << "start " << "apeIndustry40.exe guest_monitor";
	std::system(exe.str().c_str());
}

int main (int argc, char** argv)
{
	std::thread host((std::bind(runHost)));
	std::this_thread::sleep_for(std::chrono::milliseconds(1000));

	std::string hostGUID;
	std::stringstream hostConfigFilePath;
	hostConfigFilePath << APE_SOURCE_DIR << "\\samples\\industry40\\configs\\host_monitor\\apeCore.json";
	FILE* apeHostCoreConfigFile = std::fopen(hostConfigFilePath.str().c_str(), "r");
	char readBuffer[65536];
	if (apeHostCoreConfigFile)
	{
		rapidjson::FileReadStream jsonFileReaderStream(apeHostCoreConfigFile, readBuffer, sizeof(readBuffer));
		rapidjson::Document jsonDocument;
		jsonDocument.ParseStream(jsonFileReaderStream);
		if (jsonDocument.IsObject())
		{
			rapidjson::Value& SceneNetwork = jsonDocument["SceneNetwork"];
			for (rapidjson::Value::MemberIterator SceneNetworkMemberIterator =
				SceneNetwork.MemberBegin();
				SceneNetworkMemberIterator != SceneNetwork.MemberEnd(); ++SceneNetworkMemberIterator)
			{
				if (SceneNetworkMemberIterator->name == "sessionGUID")
					hostGUID = jsonDocument["SceneNetwork"]["sessionGUID"].GetString();
			}
		}
		fclose(apeHostCoreConfigFile);
	}
	std::stringstream guestConfigFilePath;
	guestConfigFilePath << APE_SOURCE_DIR << "\\samples\\industry40\\configs\\guest_monitor\\apeCore.json";
	FILE* apeCoreConfigFile = std::fopen(guestConfigFilePath.str().c_str(), "r");
	char readBufferGuest[65536];
	rapidjson::Document jsonDocument;
	if (apeCoreConfigFile)
	{
		rapidjson::FileReadStream jsonFileReaderStream(apeCoreConfigFile, readBufferGuest, sizeof(readBufferGuest));
		jsonDocument.ParseStream(jsonFileReaderStream);
		if (jsonDocument.IsObject())
		{
			rapidjson::Value& SceneNetwork = jsonDocument["SceneNetwork"];
			for (rapidjson::Value::MemberIterator SceneNetworkMemberIterator =
				SceneNetwork.MemberBegin();
				SceneNetworkMemberIterator != SceneNetwork.MemberEnd(); ++SceneNetworkMemberIterator)
			{
				if (SceneNetworkMemberIterator->name == "sessionGUID")
					jsonDocument["SceneNetwork"]["sessionGUID"].SetString(rapidjson::StringRef(hostGUID.c_str()));
			}
		}
		fclose(apeCoreConfigFile);
	}

	rapidjson::StringBuffer writeBuffer;
	rapidjson::Writer<rapidjson::StringBuffer> writer(writeBuffer);
	jsonDocument.Accept(writer);
	std::stringstream contentSS;
	contentSS << writeBuffer.GetString();
	std::string content = contentSS.str();
	std::ofstream apeCoreConfigFileOut(guestConfigFilePath.str().c_str(), std::ios::binary | std::ios::out);
	apeCoreConfigFileOut.write(content.c_str(), content.size());
	apeCoreConfigFileOut.flush();
	apeCoreConfigFileOut.close();

	//std::this_thread::sleep_for(std::chrono::milliseconds(3000));
	std::cout << "Please wait until the host is up then press any key to starting the guest" << std::endl;
	getchar();

	std::thread guest((std::bind(runGuest)));
	host.join();
	guest.join();
	return 0;
}