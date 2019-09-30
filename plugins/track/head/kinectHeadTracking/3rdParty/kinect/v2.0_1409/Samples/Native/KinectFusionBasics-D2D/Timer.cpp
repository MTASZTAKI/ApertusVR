//------------------------------------------------------------------------------
// <copyright file="Timer.cpp" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//------------------------------------------------------------------------------

#include "Timer.h"

namespace Timing
{

Timer::Timer()
{
    if (QueryPerformanceFrequency(&frequency) == 0)
    {
        // Frequency not supported - cannot get time!
        initialized = false;
        return;
    }

    initialized = true;
}

/// <summary>
///  Gets the absolute time.
/// </summary>
/// <returns>Returns the absolute time in s.</returns>
double Timer::AbsoluteTime()
{
    if (initialized == false)
    {
        return 0;
    }

    LARGE_INTEGER systemTime;
    if (QueryPerformanceCounter(&systemTime) == 0)
    {
        return 0;
    }
    double realTime = (double)systemTime.QuadPart / (double)frequency.QuadPart;
    return realTime;
}

};
