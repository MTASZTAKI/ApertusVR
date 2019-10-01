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

#ifndef APE_INTERPOLATOR_H
#define APE_INTERPOLATOR_H

#include <algorithm>
#include <array>
#include <chrono>
#include <fstream>
#include <functional>
#include <iostream>
#include <memory>
#include <queue>
#include <string>
#include <typeinfo>
#include <vector>
#include "apeQuaternion.h"

namespace ape
{
	typedef std::chrono::high_resolution_clock::time_point TimePoint;

	class Interpolator
	{
	#define MAX_QUEUE_SIZE 20
	#define TIME_CONTROL_SPLINE_RESOLUTION 50
	public:
		Interpolator() : mQueueSize(MAX_QUEUE_SIZE), mRepeat(false)
		{
			initTCSplineToLinear();
		}

		Interpolator(bool repeat, unsigned int queue_size = MAX_QUEUE_SIZE) : mQueueSize(queue_size), mRepeat(repeat)
		{
			initTCSplineToLinear();
		}

		~Interpolator()
		{
			clearQueue();
		}

		void setTimeControlPoints(double x1, double y1, double x2, double y2)
		{
			double y0 = 0.00;
			double x0 = 0.00;
			double y3 = 1.00;
			double x3 = 1.00;

			double A = x3 - 3 * x2 + 3 * x1 - x0;
			double B = 3 * x2 - 6 * x1 + 3 * x0;
			double C = 3 * x1 - 3 * x0;
			double D = x0;

			double E = y3 - 3 * y2 + 3 * y1 - y0;
			double F = 3 * y2 - 6 * y1 + 3 * y0;
			double G = 3 * y1 - 3 * y0;
			double H = y0;

			for (unsigned int it = 0; it < mTimeControlSpline.size(); it++)
			{
				double t = (double)it / (double)TIME_CONTROL_SPLINE_RESOLUTION;
				int nRefinementIterations = 5;
				for (int i = 0; i < nRefinementIterations; i++)
				{
					double currentx = A * (t * t * t) + B * (t * t) + C * t + D;
					double currentslope = 1.0 / (3.0 * A * t * t + 2.0 * B * t + C);
					t -= (currentx - it / (double)TIME_CONTROL_SPLINE_RESOLUTION) * (currentslope);
					t = std::max<double>(0.0, t);
					t = std::min<double>(t, 1.0);
				}
				double y = E * (t * t * t) + F * (t * t) + G * t + H;

				mTimeControlSpline[it] = y;
			}
		}
		template<typename U> struct identity
		{
			using type = U;
		};

		template<typename T>
		bool addSection(T value_start, T value_goal, double t_sec_duration, typename identity<std::function<void(T)>>::type job = NULL)
		{
			Section<T>* section = new Section<T>(value_start, value_goal, t_sec_duration, mTimeControlSpline, job);
			if (mSectionQueue.size() < mQueueSize)
			{
				mSectionQueue.push(section);
				return true;
			}
			else
				return false;
		}

		void clearQueue()
		{
			while (!mSectionQueue.empty())
			{
				delete mSectionQueue.front();
				mSectionQueue.pop();
			}
		}

		unsigned int getQueueSize() const
		{
			return (unsigned int)mSectionQueue.size();
		}

		bool isQueueFull() const
		{
			return mSectionQueue.size() == mQueueSize;
		}

		bool isQueueEmpty() const
		{
			return mSectionQueue.empty();
		}

		void iterateTopSection()
		{
			if (!mSectionQueue.empty() && mSectionQueue.front() != NULL)
			{
				if (mSectionQueue.front()->iterate())
				{
					if (mRepeat)
					{
						mSectionQueue.front()->resetStartTime();
						mSectionQueue.push(mSectionQueue.front());
					}
					mSectionQueue.pop();
				}
			}
		}
	private:
		struct SectionBase
		{
			virtual ~SectionBase() {}
			virtual bool iterate() = 0;
			virtual void resetStartTime() = 0;
		};

		template<typename T>
		struct Section : SectionBase
		{
			TimePoint m_t_start;
			long long m_t_duration;
			T m_value_start;
			T m_value_goal;
			std::function<void(T)> m_action;
			const std::array< double, TIME_CONTROL_SPLINE_RESOLUTION>& m_timecontrol;

			Section(T value_start, T value_goal, double t_sec_duration, const std::array<double, TIME_CONTROL_SPLINE_RESOLUTION>& timecontrol, typename identity<std::function<void(T)>>::type action)
				: m_t_start(TimePoint()),
				  m_t_duration(t_sec_duration * 1000.0),
				  m_value_start(value_start),
				  m_value_goal(value_goal),
				  m_action(action),
				  m_timecontrol(timecontrol)
			{

			}

			bool iterate() override
			{
				if (m_t_start.time_since_epoch() == std::chrono::system_clock::duration::zero())
				{
					m_t_start = std::chrono::high_resolution_clock::now();
					m_action(m_value_start);
					return false;
				}
				else
				{
					TimePoint t_current = std::chrono::high_resolution_clock::now();
					auto t_elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(t_current - m_t_start).count();
					double percent = (double)t_elapsed / (double)m_t_duration;
					if (percent >= 1.0)
					{
						m_action(m_value_goal);
						return true;
					}
					m_action(_increment_by_percent(_percent_by_spline(percent)));
					return false;
				}
			}

			void resetStartTime() override
			{
				m_t_start = TimePoint();
			}

			double _percent_by_spline(double percent)
			{
				double real_index = percent * (double)TIME_CONTROL_SPLINE_RESOLUTION;
				int spline_index = (int)std::floor(real_index);
				double in_interval_percent = real_index - (double)spline_index;
				if (spline_index == 0)
				{
					return m_timecontrol[spline_index] * in_interval_percent;
				}
				else
				{
					return m_timecontrol[spline_index - 1] + (m_timecontrol[spline_index] - m_timecontrol[spline_index - 1]) * in_interval_percent;
				}
			}
			T _increment_by_percent(double percent)
			{
				return m_value_start + (m_value_goal - m_value_start) * percent;
			}
		};

		std::queue< SectionBase* > mSectionQueue;

		unsigned int mQueueSize;

		bool mRepeat;

		std::array<double, TIME_CONTROL_SPLINE_RESOLUTION> mTimeControlSpline;

		void initTCSplineToLinear()
		{
			double i = 0.0;
			for (auto& it : mTimeControlSpline)
			{
				it = ++i / (double)TIME_CONTROL_SPLINE_RESOLUTION;
			}
		}

		void _writeTestFile(const char* fileName, const std::array<double, TIME_CONTROL_SPLINE_RESOLUTION>& data)
		{
			std::ofstream file(fileName);
			if (!file.is_open())
				return;
			for (size_t i = 0; i < data.size(); i++)
			{
				file << (double)i / (double)TIME_CONTROL_SPLINE_RESOLUTION << "\t" << data[i] << "\n";
			}
			file.close();
			return;
		}
	};

	typedef std::shared_ptr<Interpolator> InterpolatorPtr;

	template <>
	ape::Quaternion inline Interpolator::Section<ape::Quaternion>::_increment_by_percent(double percent)
	{
		return ape::Quaternion::Slerp((float)percent, m_value_start, m_value_goal, true);
	}
}

#endif
