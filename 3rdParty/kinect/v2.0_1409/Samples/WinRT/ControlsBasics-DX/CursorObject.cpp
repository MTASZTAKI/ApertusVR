//------------------------------------------------------------------------------
// <copyright file="CursorObject.cpp" company="Microsoft">
//     2D cursor
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//------------------------------------------------------------------------------

#include "pch.h"
#include "CursorObject.h"

#include "ControlsBasicsDX.h"

using namespace WindowsPreview::Kinect::Input;

//--------------------------------------------------------------------------------------
// Name: CursorObject()
// Desc: Constructor
//--------------------------------------------------------------------------------------
CursorObject::CursorObject(WindowsPreview::Kinect::Input::HandType handType)
{
    auto cursorSpriteSheetDefinition = ref new Microsoft::Kinect::Toolkit::Input::CursorSpriteSheetDefinition(
		ref new Windows::Foundation::Uri(L"ms-appx:///CursorSpriteSheet.dds"), 
		CURSOR_SPRITE_COLS, 
		CURSOR_SPRITE_ROWS,
        CURSOR_WIDTH,
        CURSOR_HEIGHT,
        CURSOR_PRESS_DOWN_START_FRAME,
        CURSOR_PRESS_DOWN_FINAL_FRAME,
        CURSOR_PRESSING_START_FRAME,
        CURSOR_PRESSING_FINAL_FRAME,
        CURSOR_PRESS_HOLD_START_FRAME,
        CURSOR_PRESS_HOLD_FINAL_FRAME,
        CURSOR_GRIPPING_START_FRAME,
        CURSOR_GRIPPING_FINAL_FRAME
    );

    auto cursorModel = ref new Microsoft::Kinect::Toolkit::Input::CursorModel(handType, cursorSpriteSheetDefinition);

    m_cursorViewModel = ref new Microsoft::Kinect::Toolkit::Input::CursorViewModel(cursorModel);
}