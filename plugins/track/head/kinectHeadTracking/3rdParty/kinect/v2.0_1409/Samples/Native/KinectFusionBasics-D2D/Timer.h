//------------------------------------------------------------------------------
// <copyright file="Timer.h" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//------------------------------------------------------------------------------

#pragma once

#include <Windows.h>

namespace Timing
{
    /// <summary>
    /// Timer is a minimal class to get the system time in seconds
    /// </summary>
    class Timer
    {
    private:
        /// <summary>
        ///  Whether the timer is initialized and can be used.
        /// </summary>
        bool initialized;

        /// <summary>
        ///  The clock frequency in ticks per second.
        /// </summary>
        LARGE_INTEGER frequency;

    public:
        /// <summary>
        ///  Initializes a new instance of the <see cref="Timer"/> class.
        /// </summary>
        Timer();

        /// <summary>
        ///  Gets the absolute time.
        /// </summary>
        /// <returns>Returns the absolute time in s.</returns>
        double AbsoluteTime();
    };
};
