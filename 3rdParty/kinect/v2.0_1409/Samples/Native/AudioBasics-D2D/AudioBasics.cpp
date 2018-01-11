//------------------------------------------------------------------------------
// <copyright file="AudioBasics.cpp" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//------------------------------------------------------------------------------

#include "stdafx.h"
#include "AudioBasics.h"
#include "resource.h"

// For M_PI and log definitions
#define _USE_MATH_DEFINES
#include <math.h>

/// <summary>
/// Entry point for the application
/// </summary>
/// <param name="hInstance">Handle to the application instance</param>
/// <param name="hPrevInstance">Always 0</param>
/// <param name="lpCmdLine">Command line arguments</param>
/// <param name="nCmdShow">Whether to display minimized, maximized, or normally</param>
/// <returns>status</returns>
int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
    HRESULT hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);

    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    if (SUCCEEDED(hr))
    {
        {
            CAudioBasics application;
            application.Run(hInstance, nCmdShow);
        }

        CoUninitialize();
    }

    return EXIT_SUCCESS;
}

/// <summary>
/// Constructor
/// </summary>
CAudioBasics::CAudioBasics() :
    m_pD2DFactory(NULL),
    m_pAudioPanel(NULL),
    m_pKinectSensor(NULL),
    m_pAudioBeamFrameReader(NULL),
    m_hFrameArrivedEvent(NULL),
    m_hTerminateWorkerThread(NULL),
    m_hWorkerThread(NULL),
    m_fBeamAngle(0.0f),
    m_fBeamAngleConfidence(0.0f),
    m_fAccumulatedSquareSum(0.0f),
    m_fEnergyError(0.0f),
    m_nAccumulatedSampleCount(0),
    m_nEnergyIndex(0),
    m_nEnergyRefreshIndex(0),
    m_nNewEnergyAvailable(0),
    m_nLastEnergyRefreshTime(NULL)
{
    InitializeCriticalSection(&m_csLock);

    ZeroMemory(m_fEnergyBuffer, sizeof(m_fEnergyBuffer));
    ZeroMemory(m_fEnergyDisplayBuffer, sizeof(m_fEnergyDisplayBuffer));
}

/// <summary>
/// Destructor
/// </summary>
CAudioBasics::~CAudioBasics()
{
    // Signal the worker thread to terminate
    if (NULL != m_hTerminateWorkerThread)
    {
        SetEvent(m_hTerminateWorkerThread);
    }

    // Wait for the worker thread to terminate
    if (NULL != m_hWorkerThread)
    {
        WaitForSingleObject(m_hWorkerThread, INFINITE);
        CloseHandle(m_hWorkerThread);
    }

    if (NULL != m_hTerminateWorkerThread)
    {
        CloseHandle(m_hTerminateWorkerThread);
    }

    // Clean up Direct2D renderer
    delete m_pAudioPanel;
    m_pAudioPanel = NULL;

    // Clean up Direct2D
    SafeRelease(m_pD2DFactory);

    if (NULL != m_pAudioBeamFrameReader)
    {
        if (NULL != m_hFrameArrivedEvent)
        {
            m_pAudioBeamFrameReader->UnsubscribeFrameArrived(m_hFrameArrivedEvent);
        }
        
        SafeRelease(m_pAudioBeamFrameReader);
    }
    
    if (m_pKinectSensor)
    {
        m_pKinectSensor->Close();
    }

    SafeRelease(m_pKinectSensor);

    DeleteCriticalSection(&m_csLock);
}

/// <summary>
/// Creates the main window and begins processing
/// </summary>
/// <param name="hInstance">Handle to the application instance</param>
/// <param name="nCmdShow">Whether to display minimized, maximized, or normally</param>
int CAudioBasics::Run(HINSTANCE hInstance, int nCmdShow)
{
    MSG       msg = {0};
    WNDCLASS  wc;

    // Dialog custom window class
    ZeroMemory(&wc, sizeof(wc));
    wc.style         = CS_HREDRAW | CS_VREDRAW;
    wc.cbWndExtra    = DLGWINDOWEXTRA;
    wc.hInstance     = hInstance;
    wc.hCursor       = LoadCursorW(NULL, IDC_ARROW);
    wc.hIcon         = LoadIconW(hInstance, MAKEINTRESOURCE(IDI_APP));
    wc.lpfnWndProc   = DefDlgProcW;
    wc.lpszClassName = L"AudioBasicsAppDlgWndClass";

    if (!RegisterClassW(&wc))
    {
        return 0;
    }

    // Create main application window
    HWND hWndApp = CreateDialogParamW(
        hInstance,
        MAKEINTRESOURCE(IDD_APP),
        NULL,
        (DLGPROC)CAudioBasics::MessageRouter, 
        reinterpret_cast<LPARAM>(this));

    // Show window
    ShowWindow(hWndApp, nCmdShow);

    // Main message loop
    while (WM_QUIT != msg.message)
    {
        if (PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE))
        {
            // If it's a dialog message it will be taken care of by the dialog proc
            if ((hWndApp != NULL) && IsDialogMessageW(hWndApp, &msg))
            {
                continue;
            }

            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
    }

    return static_cast<int>(msg.wParam);
}

/// <summary>
/// Handles window messages, passes most to the class instance to handle
/// </summary>
/// <param name="hWnd">A handle to the dialog box</param>
/// <param name="uMsg">The message</param>
/// <param name="wParam">Additional message-specific information</param>
/// <param name="lParam">Additional message-specific information</param>
/// <returns>Result of message processing</returns>
LRESULT CALLBACK CAudioBasics::MessageRouter(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    CAudioBasics* pThis = NULL;

    if (WM_INITDIALOG == uMsg)
    {
        pThis = reinterpret_cast<CAudioBasics*>(lParam);
        SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pThis));
    }
    else
    {
        pThis = reinterpret_cast<CAudioBasics*>(::GetWindowLongPtr(hWnd, GWLP_USERDATA));
    }

    if (NULL != pThis)
    {
        return pThis->DlgProc(hWnd, uMsg, wParam, lParam);
    }

    return 0;
}

/// <summary>
/// Handle windows messages for the class instance
/// </summary>
/// <param name="hWnd">A handle to the dialog box</param>
/// <param name="uMsg">The message</param>
/// <param name="wParam">Additional message-specific information</param>
/// <param name="lParam">Additional message-specific information</param>
/// <returns>Result of message processing</returns>
LRESULT CALLBACK CAudioBasics::DlgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);

    switch (message)
    {
    case WM_INITDIALOG:
        {
            // Bind application window handle
            m_hWnd = hWnd;

            // Init Direct2D
            D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &m_pD2DFactory);

            // Create and initialize a new Direct2D image renderer (take a look at ImageRenderer.h)
            // We'll use this to draw the data we receive from the Kinect to the screen
            m_pAudioPanel = new AudioPanel();
            HRESULT hr = m_pAudioPanel->Initialize(GetDlgItem(m_hWnd, IDC_AUDIOVIEW), m_pD2DFactory, cEnergySamplesToDisplay);
            if (FAILED(hr))
            {
                SetStatusMessage(L"Failed to initialize the Direct2D draw device.");
                break;
            }

            // Get and initialize the default Kinect sensor; Subscribe to new audio frame events
            if (SUCCEEDED(InitializeDefaultSensor()))
            {
                // Create timers to UI update
                SetTimer(m_hWnd, cEnergyRefreshTimerId, cEnergyRefreshTimerInterval, NULL);
            }

        }
        break;

    case WM_TIMER:
        // Capture all available audio or update audio panel each time a timer fires
        if(wParam == cEnergyRefreshTimerId)
        {
            Update();
        }
        break;

    case WM_CLOSE:
        // If the titlebar X is clicked, destroy app
        KillTimer(m_hWnd, cEnergyRefreshTimerId);
        DestroyWindow(hWnd);
        break;

    case WM_DESTROY:
        // Quit the main message pump
        PostQuitMessage(0);
        break;
    }

    return FALSE;
}

/// <summary>
/// Initializes the default Kinect sensor and create a worker thread
/// to process new frame events
/// </summary>
/// <returns>indicates success or failure</returns>
HRESULT CAudioBasics::InitializeDefaultSensor()
{
    HRESULT hr = S_OK;
    IAudioSource* pAudioSource = NULL;

    hr = GetDefaultKinectSensor(&m_pKinectSensor);
    if (FAILED(hr))
    {
        SetStatusMessage(L"Failed getting default sensor!");
        return hr;
    }

    hr = m_pKinectSensor->Open();

    if (SUCCEEDED(hr))
    {
        hr = m_pKinectSensor->get_AudioSource(&pAudioSource);
    }

    if (SUCCEEDED(hr))
    {
        hr = pAudioSource->OpenReader(&m_pAudioBeamFrameReader);
    }

#if 0
    // To overwrite the automatic mode of the audio beam, change it to
    // manual and set the desired beam angle. In this example, point it
    // straight forward.
    {
        IAudioBeam* pAudioBeam = NULL;
        IAudioBeamList* pAudioBeamList = NULL;

        if (SUCCEEDED(hr))
        {
            hr = pAudioSource->get_AudioBeams(&pAudioBeamList);
        }
    
        if (SUCCEEDED(hr))
        {        
            hr = pAudioBeamList->OpenAudioBeam(0, &pAudioBeam);
        }

        // Note that setting beam mode and beam angle will only work if the
        // application window is in the foreground. However, the operations below will
        // return S_OK even if the application window is in the background.
        // Furthermore, setting these values is an asynchronous operation --
        // it may take a short period of time for the beam to adjust.
        if (SUCCEEDED(hr))
        {
            hr = pAudioBeam->put_AudioBeamMode(AudioBeamMode_Manual);
        }

        if (SUCCEEDED(hr))
        {
            hr = pAudioBeam->put_BeamAngle(0);
        }

        SafeRelease(pAudioBeamList);
        SafeRelease(pAudioBeam);
    }

#endif

    if (SUCCEEDED(hr))
    {
        hr = m_pAudioBeamFrameReader->SubscribeFrameArrived(&m_hFrameArrivedEvent);
    }

    if (SUCCEEDED(hr))
    {
        m_hTerminateWorkerThread = CreateEvent(NULL, FALSE, FALSE, NULL);
        if (NULL == m_hTerminateWorkerThread)
        {
            hr = HRESULT_FROM_WIN32(GetLastError());
        }
    }

    if (SUCCEEDED(hr))
    {
        m_hWorkerThread = CreateThread(NULL, 0, &CAudioBasics::WorkerThread, this, 0, NULL);
        if (NULL == m_hWorkerThread)
        {
            hr = HRESULT_FROM_WIN32(GetLastError());
        }
    }

    SafeRelease(pAudioSource);

    if (FAILED(hr))
    {
        SetStatusMessage(L"Failed opening an audio stream!");
    }

    return hr;
}

/// <summary>
/// Static method. 
/// Thread that handles frame arrived events
/// </summary>
DWORD WINAPI CAudioBasics::WorkerThread(_In_ LPVOID lpParameter)
{
    HRESULT hr = S_OK;
    CAudioBasics* pThis = static_cast<CAudioBasics *>(lpParameter);

    hr = pThis->WorkerThread();

    return SUCCEEDED(hr) ? 0 : 1;
}

/// <summary>
/// Handles frame arrived events
/// </summary>
HRESULT CAudioBasics::WorkerThread()
{
    HRESULT hr = S_OK;
    BOOL workerThreadRunning = TRUE;
    BOOL clearStatusMessage = FALSE;
    UINT statusMessageFramesToPersistRemaining = 0;
    DWORD timeout = 2000; // In msec. If we don't get a new frame event by this time, display an error message. Any number of seconds will do here.
    UINT32 subFrameCount = 0;
    IAudioBeamFrameArrivedEventArgs* pAudioBeamFrameArrivedEventArgs = NULL;
    IAudioBeamFrameReference* pAudioBeamFrameReference = NULL;
    IAudioBeamFrameList* pAudioBeamFrameList = NULL;
    IAudioBeamFrame* pAudioBeamFrame = NULL;
    HANDLE handles[]  = {(HANDLE)m_hFrameArrivedEvent, m_hTerminateWorkerThread};

    while (workerThreadRunning)
    {
        // Wait for a new audio frame
        DWORD result = WaitForMultipleObjects(_countof(handles), handles, FALSE, timeout);

        if (WAIT_OBJECT_0 == result)
        {
            // Process new audio frame
            hr = m_pAudioBeamFrameReader->GetFrameArrivedEventData(m_hFrameArrivedEvent, &pAudioBeamFrameArrivedEventArgs);

            if (SUCCEEDED(hr))
            {
                hr = pAudioBeamFrameArrivedEventArgs->get_FrameReference(&pAudioBeamFrameReference);
            }

            if (SUCCEEDED(hr))
            {
                hr = pAudioBeamFrameReference->AcquireBeamFrames(&pAudioBeamFrameList);
            }

            if (SUCCEEDED(hr))
            {
                // Only one audio beam is currently supported
                hr = pAudioBeamFrameList->OpenAudioBeamFrame(0, &pAudioBeamFrame);
            }

            if (SUCCEEDED(hr))
            {
                hr = pAudioBeamFrame->get_SubFrameCount(&subFrameCount);
            }

            if (SUCCEEDED(hr) && subFrameCount > 0)
            {
                for (UINT32 i = 0; i < subFrameCount; i++)
                {
                    // Process all subframes
                    IAudioBeamSubFrame* pAudioBeamSubFrame = NULL;

                    hr = pAudioBeamFrame->GetSubFrame(i, &pAudioBeamSubFrame);

                    if (SUCCEEDED(hr))
                    {
                        ProcessAudio(pAudioBeamSubFrame);
                    }

                    SafeRelease(pAudioBeamSubFrame);
                }
            }

            SafeRelease(pAudioBeamFrame);
            SafeRelease(pAudioBeamFrameList);
            SafeRelease(pAudioBeamFrameReference);
            SafeRelease(pAudioBeamFrameArrivedEventArgs);

            if (FAILED(hr))
            {
                SetStatusMessage(L"Failed processing audio frame");

                // Persist the status message for some arbitrary amount of time, for example 30 successfully acquired audio frames
                statusMessageFramesToPersistRemaining = 30;
            }
            else if (clearStatusMessage)
            {
                // Clear any previous status messages if needed
                SetStatusMessage(L"");
                clearStatusMessage = FALSE;
            }
            else if (statusMessageFramesToPersistRemaining > 0)
            {
                // Update frame counter and signal a reset of status message when the counter hits zero
                if (--statusMessageFramesToPersistRemaining == 0)
                {
                    clearStatusMessage = TRUE;
                }
            }
        }
        else if (WAIT_OBJECT_0 + 1 == result)
        {
            // Terminate worker thread
            break;
        }
        else if (WAIT_TIMEOUT == result) 
        {
            // Display message, but continue waiting for frame events. 
            SetStatusMessage(L"Frame arrived event not received. Make sure Kinect Monitor service is running");

            // This will clear the error message once a new frame is arrived
            clearStatusMessage = TRUE;
        }
        else
        {
            hr = E_FAIL;
            break;
        }
    }

    return hr;
}

/// <summary>
/// Process a new audio subframe
/// </summary>
void CAudioBasics::ProcessAudio(IAudioBeamSubFrame* pAudioBeamSubFrame)
{
    HRESULT hr = S_OK;
    float* pAudioBuffer = NULL;
    UINT cbRead = 0;

    hr = pAudioBeamSubFrame->AccessUnderlyingBuffer(&cbRead, (BYTE **)&pAudioBuffer);

    if (FAILED(hr))
    {
        SetStatusMessage(L"Failed to read buffer from audio subframe.");
    }
    else if (cbRead > 0)
    {
        DWORD nSampleCount = cbRead/sizeof(float);
        float fBeamAngle = 0.f;
        float fBeamAngleConfidence = 0.0f;

        // Get audio beam angle and confidence
        pAudioBeamSubFrame->get_BeamAngle(&fBeamAngle);
        pAudioBeamSubFrame->get_BeamAngleConfidence(&fBeamAngleConfidence);

        // Calculate energy from audio
        for (UINT i = 0; i < nSampleCount; i++)
        {
            // Compute the sum of squares of audio samples that will get accumulated
            // into a single energy value.
        __pragma(warning(push))
        __pragma(warning(disable:6385)) // Suppress warning about the range of i. The range is correct.
            m_fAccumulatedSquareSum += pAudioBuffer[i] * pAudioBuffer[i];
        __pragma(warning(pop))
            ++m_nAccumulatedSampleCount;

            if (m_nAccumulatedSampleCount < cAudioSamplesPerEnergySample)
            {
                continue;
            }

            // Each energy value will represent the logarithm of the mean of the
            // sum of squares of a group of audio samples.
            float fMeanSquare = m_fAccumulatedSquareSum / cAudioSamplesPerEnergySample;

            if (fMeanSquare > 1.0f)
            {
                // A loud audio source right next to the sensor may result in mean square values
                // greater than 1.0. Cap it at 1.0f for display purposes.
                fMeanSquare = 1.0f;
            }

            float fEnergy = cMinEnergy;
            if (fMeanSquare > 0.f)
            {
                // Convert to dB
                fEnergy = 10.0f*log10(fMeanSquare);
            }

            {
                // Protect shared resources with Update() method on another thread
                EnterCriticalSection(&m_csLock);

                m_fBeamAngle = fBeamAngle;
                m_fBeamAngleConfidence = fBeamAngleConfidence;

                // Renormalize signal above noise floor to [0,1] range for visualization.
                m_fEnergyBuffer[m_nEnergyIndex] = (cMinEnergy - fEnergy) / cMinEnergy;
                m_nNewEnergyAvailable++;
                m_nEnergyIndex = (m_nEnergyIndex + 1) % cEnergyBufferLength;

                LeaveCriticalSection(&m_csLock);
            }

            m_fAccumulatedSquareSum = 0.f;
            m_nAccumulatedSampleCount = 0;
        }
    }
}

/// <summary>
/// Display latest audio data
/// </summary>
void CAudioBasics::Update()
{
    ULONGLONG previousRefreshTime = m_nLastEnergyRefreshTime;
    ULONGLONG now = GetTickCount64();

    m_nLastEnergyRefreshTime = now;

    // No need to refresh if there is no new energy available to render
    if(m_nNewEnergyAvailable <= 0)
    {
        return;
    }

    {
        EnterCriticalSection(&m_csLock);

        if (previousRefreshTime != NULL)
        {
            // Calculate how many energy samples we need to advance since the last Update() call in order to
            // have a smooth animation effect.
            float energyToAdvance = m_fEnergyError +(((now - previousRefreshTime) * cAudioSamplesPerSecond/(float)1000.0) / cAudioSamplesPerEnergySample); 
            int energySamplesToAdvance = min(m_nNewEnergyAvailable, (int)(energyToAdvance));
            m_fEnergyError = energyToAdvance - energySamplesToAdvance;
            m_nEnergyRefreshIndex = (m_nEnergyRefreshIndex + energySamplesToAdvance) % cEnergyBufferLength;
            m_nNewEnergyAvailable -= energySamplesToAdvance;
        }

        // Copy energy samples into buffer to be displayed, taking into account that energy
        // wraps around in a circular buffer.
        int baseIndex = (m_nEnergyRefreshIndex + cEnergyBufferLength - cEnergySamplesToDisplay) % cEnergyBufferLength;
        int samplesUntilEnd = cEnergyBufferLength - baseIndex;
        if(samplesUntilEnd>cEnergySamplesToDisplay)
        {
            memcpy_s(m_fEnergyDisplayBuffer, cEnergySamplesToDisplay*sizeof(float), m_fEnergyBuffer + baseIndex, cEnergySamplesToDisplay*sizeof(float));
        }
        else
        {
            int samplesFromBeginning = cEnergySamplesToDisplay-samplesUntilEnd;
            memcpy_s(m_fEnergyDisplayBuffer, cEnergySamplesToDisplay*sizeof(float), m_fEnergyBuffer + baseIndex, samplesUntilEnd*sizeof(float));
            memcpy_s(m_fEnergyDisplayBuffer + samplesUntilEnd, (cEnergySamplesToDisplay - samplesUntilEnd)*sizeof(float), m_fEnergyBuffer, samplesFromBeginning*sizeof(float));
        }

        // Apply latest beam angle and confidence. 
        // SetBeam() expects the angle in degrees, whereas Kinect gives it in radians. Convert from radians to degrees.
        m_pAudioPanel->SetBeam(180.0f * m_fBeamAngle / static_cast<float>(M_PI), m_fBeamAngleConfidence);
        SetBeamText();

        LeaveCriticalSection(&m_csLock);
    }

    m_pAudioPanel->UpdateEnergy(m_fEnergyDisplayBuffer, cEnergySamplesToDisplay);

    m_pAudioPanel->Draw();      
}

/// <summary>
/// Set the status bar message
/// </summary>
/// <param name="szMessage">message to display</param>
void CAudioBasics::SetStatusMessage(_In_z_ WCHAR * szMessage)
{
    SendDlgItemMessageW(m_hWnd, IDC_STATUS, WM_SETTEXT, 0, (LPARAM)szMessage);
}

/// <summary>
/// Set the status bar message to beam angle and angle confidence.
/// </summary>
void CAudioBasics::SetBeamText()
{
    StringCbPrintf(m_szBeamText, sizeof(m_szBeamText), L"Beam Angle: %2.0f deg, Beam Angle Confidence: %1.2f", 180.0f * m_fBeamAngle/static_cast<float>(M_PI), m_fBeamAngleConfidence); 
    SetStatusMessage(m_szBeamText);
}