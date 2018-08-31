//------------------------------------------------------------------------------
// <copyright file="SpeechBasics.h" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//------------------------------------------------------------------------------

#pragma once

#include "TurtleController.h"
#include "KinectAudioStream.h"
#include "resource.h"

// For FORMAT_WaveFormatEx and such
#include <uuids.h>

// For speech APIs
// NOTE: To ensure that application compiles and links against correct SAPI versions (from Microsoft Speech
//       SDK), VC++ include and library paths should be configured to list appropriate paths within Microsoft
//       Speech SDK installation directory before listing the default system include and library directories,
//       which might contain a version of SAPI that is not appropriate for use together with Kinect sensor.
#include <sapi.h>
__pragma(warning(push))
__pragma(warning(disable:6385 6001)) // Suppress warnings in public SDK header
#include <sphelper.h>
__pragma(warning(pop))

/// <summary>
/// Main application class for SpeechBasics sample.
/// </summary>
class CSpeechBasics
{
public:
    /// <summary>
    /// Constructor
    /// </summary>
    CSpeechBasics();

    /// <summary>
    /// Destructor
    /// </summary>
    ~CSpeechBasics();

    /// <summary>
    /// Handles window messages, passes most to the class instance to handle
    /// </summary>
    /// <param name="hWnd">window message is for</param>
    /// <param name="uMsg">message</param>
    /// <param name="wParam">message data</param>
    /// <param name="lParam">additional message data</param>
    /// <returns>result of message processing</returns>
    static LRESULT CALLBACK MessageRouter(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

    /// <summary>
    /// Handle windows messages for a class instance
    /// </summary>
    /// <param name="hWnd">window message is for</param>
    /// <param name="uMsg">message</param>
    /// <param name="wParam">message data</param>
    /// <param name="lParam">additional message data</param>
    /// <returns>result of message processing</returns>
    LRESULT CALLBACK        DlgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

    /// <summary>
    /// Creates the main window and begins processing
    /// </summary>
    /// <param name="hInstance">handle to the application instance</param>
    /// <param name="nCmdShow">whether to display minimized, maximized, or normally</param>
    int                     Run(HINSTANCE hInstance, int nCmdShow);

private:
    static LPCWSTR          GrammarFileName;

    // Main application dialog window
    HWND                    m_hWnd;

    // Factory used to create Direct2D objects
    ID2D1Factory*           m_pD2DFactory;

    // Object that controls moving turtle around and displaying it
    TurtleController*       m_pTurtleController;

    // Current Kinect sensor
    IKinectSensor*          m_pKinectSensor;

    // A single audio beam off the Kinect sensor.
    IAudioBeam*             m_pAudioBeam;

    // An IStream derived from the audio beam, used to read audio samples
    IStream*                m_pAudioStream;

    // Stream for converting 32bit Audio provided by Kinect to 16bit required by speeck
    KinectAudioStream*     m_p16BitAudioStream;

    // Handle for sensor notifications
    WAITABLE_HANDLE         m_hSensorNotification;

    // Stream given to speech recognition engine
    ISpStream*              m_pSpeechStream;

    // Speech recognizer
    ISpRecognizer*          m_pSpeechRecognizer;

    // Speech recognizer context
    ISpRecoContext*         m_pSpeechContext;

    // Speech grammar
    ISpRecoGrammar*         m_pSpeechGrammar;

    // Event triggered when we detect speech recognition
    HANDLE                  m_hSpeechEvent;

    /// <summary>
    /// Opens the Kinect Sensor and Initialize Audio
    /// </summary>
    /// <returns>S_OK on success, otherwise failure code.</returns>
    HRESULT                 StartKinect();

     /// <summary>
    /// Initializes Speech
    /// </summary>
    /// <returns>S_OK on success, otherwise failure code.</returns>
    HRESULT                 InitializeSpeech();

    /// <summary>
    /// Create speech recognizer that will read Kinect audio stream data.
    /// </summary>
    /// <returns>
    /// <para>S_OK on success, otherwise failure code.</para>
    /// </returns>
    HRESULT                 CreateSpeechRecognizer();

    /// <summary>
    /// Load speech recognition grammar into recognizer.
    /// </summary>
    /// <returns>
    /// <para>S_OK on success, otherwise failure code.</para>
    /// </returns>
    HRESULT                 LoadSpeechGrammar();

    /// <summary>
    /// Start recognizing speech asynchronously.
    /// </summary>
    /// <returns>
    /// <para>S_OK on success, otherwise failure code.</para>
    /// </returns>
    HRESULT                 StartSpeechRecognition();

    /// <summary>
    /// Process recently triggered speech recognition events.
    /// </summary>
    void                    ProcessSpeech();

    /// <summary>
    /// Maps a specified speech semantic tag to the corresponding action to be performed on turtle.
    /// </summary>
    /// <returns>
    /// Action that matches <paramref name="pszSpeechTag"/>, or TurtleActionNone if no matches were found.
    /// </returns>
    TurtleAction            MapSpeechTagToAction(LPCWSTR pszSpeechTag);

    /// <summary>
    /// Set the status bar message.
    /// </summary>
    /// <param name="szMessage">message to display.</param>
    void                    SetStatusMessage(const WCHAR* szMessage);
};
