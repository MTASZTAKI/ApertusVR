/*MIT License

Copyright (c) 2016 MTA SZTAKI

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

#ifndef APE_DOUBLEQUEUE_H
#define APE_DOUBLEQUEUE_H

#include <vector>
#include <mutex>
#include <set>
#include <algorithm>

namespace Ape
{
	template<typename T>
    class DoubleQueue
    {
	private:
		std::vector<T> mPush;
		
		std::vector<T> mPop;
		
		std::mutex mPushMutex;
		
    public:
		DoubleQueue() :mPush(std::vector<T>()), mPop(std::vector<T>())
		{
		}
		
		~DoubleQueue()
		{
			mPush.clear();
			mPop.clear();
		}
        void swap()
		{
			std::lock_guard<std::mutex> lock(mPushMutex);
			mPush.swap(mPop);
		}

        bool empty()
		{
			return emptyPop() && emptyPush();
		}

        bool emptyPop()
		{
			return mPop.empty();
		}

        bool emptyPush()
		{
			std::lock_guard<std::mutex> lock(mPushMutex);
			return mPush.empty();
		}

        T front()
		{
			return mPop.empty() ? T() : (*mPop.begin());
		}

        void push(T elem)
		{
			std::lock_guard<std::mutex> lock(mPushMutex);
			#ifdef APE_DOUBLEQUEUE_UNIQUE
				if (std::find(mPush.begin(), mPush.end(), elem) == mPush.end())
					mPush.push_back(elem);
			#else
				mPush.push_back(elem);
			#endif
		}

        void pop()
		{
			if (!mPop.empty()) mPop.erase(mPop.begin());
		}

        size_t size()
		{
			return mPop.size() + mPush.size();
		}

        size_t sizePop()
		{
			return mPop.size();
		}

        size_t sizePush()
		{
			return mPush.size();
		}

		DoubleQueue& operator =(const DoubleQueue& other)
		{
			mPush = other.mPush;
			mPop = other.mPop;
			return *this;
		}
    };
}

#endif
