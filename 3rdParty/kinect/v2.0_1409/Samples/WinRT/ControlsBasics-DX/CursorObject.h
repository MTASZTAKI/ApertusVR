//------------------------------------------------------------------------------
// <copyright file="CursorObject.h" company="Microsoft">
//     2D cursor
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//------------------------------------------------------------------------------

#pragma once

#include "UIElement.h"

#define CURSOR_SPRITE_ROWS 20
#define CURSOR_SPRITE_COLS 4
#define CURSOR_WIDTH 137
#define CURSOR_HEIGHT 137
#define CURSOR_DEFAULT_FOREGROUND_FRAME 23
#define CURSOR_DEFAULT_BACKGROUND_FRAME 79
#define CURSOR_PRESS_DOWN_START_FRAME 0
#define CURSOR_PRESS_DOWN_FINAL_FRAME 15
#define CURSOR_GRIPPING_START_FRAME 16
#define CURSOR_GRIPPING_FINAL_FRAME 22
#define CURSOR_PRESSING_START_FRAME 23
#define CURSOR_PRESSING_FINAL_FRAME 43
#define CURSOR_PRESS_HOLD_START_FRAME 44
#define CURSOR_PRESS_HOLD_FINAL_FRAME 79

ref class CursorObject sealed
{
public:
    CursorObject(WindowsPreview::Kinect::Input::HandType handType);

    property Microsoft::Kinect::Toolkit::Input::CursorViewModel^ CursorViewModel
    {
        Microsoft::Kinect::Toolkit::Input::CursorViewModel^ get()
        {
            return m_cursorViewModel;
        }
    }

private:
    Microsoft::Kinect::Toolkit::Input::CursorViewModel^ m_cursorViewModel;
};
