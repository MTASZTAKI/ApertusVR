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


#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <map>
#include "ApeSystem.h"

int main (int argc, char** argv)
{
	if (argc > 1)
	{
		std::stringstream configPath;
		std::stringstream configFilePath;
		std::string configDirName = argv[1];
		configPath << APE_SOURCE_DIR << "/samples/robotMonitoring/configs/" << configDirName;
		configFilePath << configPath.str() << "/ApeSystem.json";
		std::ifstream f(configFilePath.str().c_str());
		if (f.good())
		{
			Ape::System::Start(configPath.str().c_str(), true);
			Ape::System::Stop();
		}
		else
		{
			std::cout << "wrong configDirName: " << configPath.str() << std::endl;
			return 0;
		}
	}
	else
	{
		std::cout << "configDirName is not present as argument" << std::endl;
		return 0;
	}
	return 0;
}
