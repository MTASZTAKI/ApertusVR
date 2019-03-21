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

#ifndef APE_FILESYSTEM_H
#define APE_FILESYSTEM_H

#include <algorithm>
#include <iostream>
#include <stdio.h>
#include <string>
#include <vector>
#ifdef _WIN32
	#include "../../../3rdParty/dirent/dirent.h"
#elif
	#include <dirent.h>
#endif

namespace ape
{
	class FileSystem {
	public:
		struct DirInfo {
			std::vector<std::string> subDirs;
		};

		static DirInfo getDirectories(const std::string& dirName)
		{
			DirInfo info;
			DIR *dir = opendir(dirName.c_str());
			struct dirent *entry = readdir(dir);
			while (entry != NULL)
			{
				std::string folder = entry->d_name;
				if (entry->d_type == DT_DIR && folder != ".." && folder != ".")
					info.subDirs.push_back(folder);
				entry = readdir(dir);
			}
			closedir(dir);
			return info;
		}

		static void listDirectories(const std::string& dirName)
		{
			for (auto& dir : getDirectories(dirName).subDirs)
			{
				std::cout << dir << std::endl;
			}
		}

		static std::string getConfigFromCmdArgs(int argc, char** argv, std::string configDirPath)
		{
			std::string configDirName = "local_monitor";
			if (argc > 2)
			{
				if (std::string(argv[1]) == "-c")
				{
					configDirName = std::string(argv[2]);
				}
			}
			else
			{
				auto dirs = ape::FileSystem::getDirectories(configDirPath);
				std::cout << "Configurations:" << std::endl;
				std::map<int, std::string> configMap;
				int i = 0;
				for (auto& dir : dirs.subDirs)
				{
					configMap.insert(std::pair<int, std::string>(i, dir));
					std::cout << " (" << i << ") " << dir << std::endl;
					i++;
				}
				std::cout << std::endl;
				while (true)
				{
					std::cout << "Type a number to select a configuration (press enter to default=local_monitor): ";
					std::string input = "";
					std::getline(std::cin, input);
					if (input.length() == 0)
					{
						break;
					}
					else
					{
						if (std::find_if(input.begin(), input.end(), [](char c) { return !isdigit(c); }) == input.end())
						{
							int c = stoi(input);
							if (c < configMap.size())
							{
								configDirName = configMap.at(c);
								break;
							}
							else
							{
								std::cout << "Number is out of range!" << std::endl;
							}
						}
						else
						{
							std::cout << "Please type a number!" << std::endl;
						}
					}
				}
			}
			return configDirName;
		}
	};
}

#endif
