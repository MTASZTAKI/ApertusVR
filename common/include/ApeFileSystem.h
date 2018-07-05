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

#include <iostream>
#include <stdio.h>
#include <string>
#include <vector>
#include <dirent.h>

namespace Ape
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
	};
}

#endif
