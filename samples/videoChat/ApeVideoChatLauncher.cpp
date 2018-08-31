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

void runHost()
{
	std::stringstream exe;
	//TODO platfrom specific new window command
	exe << "start " << "ApeVideoChat.exe host";
	std::system(exe.str().c_str());
}

void runGuest()
{
	std::stringstream exe;
	//TODO platfrom specific new window command
	exe << "start " << "ApeVideoChat.exe guest";
	std::system(exe.str().c_str());
}

int main(int argc, char** argv)
{
	std::thread host((std::bind(runHost)));
	std::this_thread::sleep_for(std::chrono::milliseconds(3000));
	std::thread guest((std::bind(runGuest)));
	host.join();
	guest.join();
	return 0;
}
