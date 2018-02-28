//------------------------------------------------------------------------------
// <copyright file="AudioBasics.h" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//------------------------------------------------------------------------------

#pragma once

#include "AudioPanel.h"
#include "resource.h"

/// <summary>
/// Main application class for AudioBasics sample.
/// </summary>
class CAudioBasics
{
public:
    /// <summary>
    /// Constructor
    /// </summary>
    CAudioBasics();

    /// <summary>
    /// Destructor
    /// </summary>
    ~CAudioBasics();

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
    // Audio samples per second in Kinect audio stream
    static const int        cAudioSamplesPerSecond = 16000;

    // ID of timer that drives energy stream display.
    static const int        cEnergyRefreshTimerId = 2;

    // Time interval, in milliseconds, for timer that drives energy stream display.
    static const int        cEnergyRefreshTimerInterval = 10;

    // Number of audio samples captured from Kinect audio stream accumulated into a single
    // energy measurement that will get displayed.
    static const int        cAudioSamplesPerEnergySample = 40;

    // Number of energy samples that will be visible in display at any given time.
    static const int        cEnergySamplesToDisplay = 780;

    // Number of energy samples that will be stored in the circular buffer.
    // Always keep it higher than the energy display length to avoid overflow.
    static const int        cEnergyBufferLength = 1000;

    // Minimum energy of audio to display (in dB value, where 0 dB is full scale)
    static const int        cMinEnergy = -90;

    // To manage access to shared resources between worker thread and UI update thread
    CRITICAL_SECTION        m_csLock;

    // Main application dialog window.
    HWND                    m_hWnd;

    // Factory used to create Direct2D objects.
    ID2D1Factory*           m_pD2DFactory;

    // Object that controls displaying Kinect audio data.
    AudioPanel*             m_pAudioPanel;    
    
    // Current Kinect sensor.
    IKinectSensor*          m_pKinectSensor;

    // Audio beam frame reader
    IAudioBeamFrameReader*  m_pAudioBeamFrameReader;

    // Event handle to signal arrival of new audio beam frame
    WAITABLE_HANDLE         m_hFrameArrivedEvent;

    // Used to terminate the worker thread
    HANDLE                  m_hTerminateWorkerThread;

    // Worker thread for process new frame events
    HANDLE                  m_hWorkerThread;

    // Latest audio beam angle in radians
    float                   m_fBeamAngle;

    // Latest audio beam angle confidence, in the range [0,1]
    float                   m_fBeamAngleConfidence;
       
    // Buffer used to store audio stream energy data as we read audio.
    float                   m_fEnergyBuffer[cEnergyBufferLength];

    // Buffer used to store audio stream energy data ready to be displayed.
    float                   m_fEnergyDisplayBuffer[cEnergySamplesToDisplay];

    // Sum of squares of audio samples being accumulated to compute the next energy value.
    float                   m_fAccumulatedSquareSum;

    // Error between time slice we wanted to display and time slice that we ended up
    // displaying, given that we have to display in integer pixels.
    float                   m_fEnergyError;

    // Number of audio samples accumulated so far to compute the next energy value.
    int                     m_nAccumulatedSampleCount;

    // Index of next element available in audio energy buffer.
    int                     m_nEnergyIndex;

    // Number of newly calculated audio stream energy values that have not yet been displayed.
    volatile int            m_nNewEnergyAvailable;

    // Index of first energy element that has never (yet) been displayed to screen.
    int                     m_nEnergyRefreshIndex;

    // Last time energy visualization was rendered to screen.
    ULONGLONG               m_nLastEnergyRefreshTime;        

    // String to store the beam and confidence for display
    wchar_t                 m_szBeamText[MAX_PATH];

    /// <summary>
    /// Initializes the default Kinect sensor
    /// </summary>
    /// <returns>S_OK on success, otherwise failure code</returns>
    HRESULT                 InitializeDefaultSensor();

    /// <summary>
    /// Static method. 
    /// Thread that handles frame arrived events
    /// </summary>
    static DWORD WINAPI     WorkerThread(_In_  LPVOID lpParameter);

    /// <summary>
    /// Handles frame arrived events
    /// </summary>
    HRESULT                 WorkerThread();

    /// <summary>
    /// Capture new audio data.
    /// </summary>
    void                    ProcessAudio(IAudioBeamSubFrame* pAudioBeamSubFrame);

    /// <summary>
    /// Display latest audio data.
    /// </summary>
    void                    Update();

    /// <summary>
    /// Set the status bar message.
    /// </summary>
    /// <param name="szMessage">message to display.</param>
    void                    SetStatusMessage(_In_z_ WCHAR* szMessage);

    /// <summary>
    /// Set the status bar message to beam angle and angle confidence.
    /// </summary>
    void                    SetBeamText();
};
