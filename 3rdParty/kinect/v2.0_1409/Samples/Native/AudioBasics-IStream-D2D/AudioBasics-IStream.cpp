//------------------------------------------------------------------------------
// <copyright file="AudioBasics.cpp" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//------------------------------------------------------------------------------

#include "stdafx.h"
#include "AudioBasics-IStream.h"
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
    m_pAudioBeam(NULL),
    m_pAudioStream(NULL),
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
    // Clean up Direct2D renderer
    delete m_pAudioPanel;
    m_pAudioPanel = NULL;

    // Clean up Direct2D
    SafeRelease(m_pD2DFactory);

    SafeRelease(m_pAudioStream);
    SafeRelease(m_pAudioBeam);

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

            // Get and initialize the default Kinect sensor; open the first audio beam and get an IStream 
            if (SUCCEEDED(InitializeDefaultSensor()))
            {
                // Create timers to drive reading from audio stream and updating UI
                SetTimer(m_hWnd, cAudioReadTimerId, cAudioReadTimerInterval, NULL);
                SetTimer(m_hWnd, cEnergyRefreshTimerId, cEnergyRefreshTimerInterval, NULL);
            }

        }
        break;

    case WM_TIMER:
        // Capture all available audio or update audio panel each time a timer fires
        if (wParam == cAudioReadTimerId)
        {
            ProcessAudio();
        }
        else if(wParam == cEnergyRefreshTimerId)
        {
            Update();
        }
        break;

    case WM_CLOSE:
        // If the titlebar X is clicked, destroy app
        KillTimer(m_hWnd, cAudioReadTimerId);
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
/// Initializes the default Kinect sensor
/// </summary>
/// <returns>indicates success or failure</returns>
HRESULT CAudioBasics::InitializeDefaultSensor()
{
    HRESULT hr = S_OK;
    IAudioSource* pAudioSource = NULL;
    IAudioBeamList* pAudioBeamList = NULL;

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
        hr = pAudioSource->get_AudioBeams(&pAudioBeamList);
    }

    if (SUCCEEDED(hr))
    {
        hr = pAudioBeamList->OpenAudioBeam(0, &m_pAudioBeam);
    }

    if (SUCCEEDED(hr))
    {
        hr = m_pAudioBeam->OpenInputStream(&m_pAudioStream);
    }

#if 0
    // To overwrite the automatic mode of the audio beam, change it to
    // manual and set the desired beam angle. In this example, point it
    // straight forward.
    // Note that setting beam mode and beam angle will only work if the
    // application window is in the foreground. However, the operations below will
    // return S_OK even if the application window is in the background.
    // Furthermore, setting these values is an asynchronous operation --
    // it may take a short period of time for the beam to adjust.
    if (SUCCEEDED(hr))
    {
        hr = m_pAudioBeam->put_AudioBeamMode(AudioBeamMode_Manual);
    }

    if (SUCCEEDED(hr))
    {
        hr = m_pAudioBeam->put_BeamAngle(0);
    }
#endif

    SafeRelease(pAudioBeamList);
    SafeRelease(pAudioSource);

    if (FAILED(hr))
    {
        SetStatusMessage(L"Failed opening an audio stream!");
    }

    return hr;
}

/// <summary>
/// Capture new audio data.
/// </summary>
void CAudioBasics::ProcessAudio()
{
    float audioBuffer[cAudioBufferLength];    
    DWORD cbRead = 0;

    // S_OK will be returned when cbRead == sizeof(audioBuffer).
    // E_PENDING will be returned when cbRead < sizeof(audioBuffer).
    // For both return codes we will continue to process the audio written into the buffer.
    HRESULT hr = m_pAudioStream->Read((void *)audioBuffer, sizeof(audioBuffer), &cbRead);

    if (FAILED(hr) && hr != E_PENDING)
    {
        SetStatusMessage(L"Failed to read from audio stream.");
    }
    else if (cbRead > 0)
    {
        DWORD nSampleCount = cbRead/sizeof(float);       
        float fBeamAngle = 0.f;
        float fBeamAngleConfidence = 0.f;

        // Get most recent audio beam angle and confidence
        m_pAudioBeam->get_BeamAngle(&fBeamAngle);
        m_pAudioBeam->get_BeamAngleConfidence(&fBeamAngleConfidence);

        // Convert angles to degrees and set values in audio panel
        m_pAudioPanel->SetBeam(180.0f * fBeamAngle / static_cast<float>(M_PI), fBeamAngleConfidence);

        // Calculate energy from audio
        for (UINT i = 0; i < nSampleCount; i++)
        {
            // Compute the sum of squares of audio samples that will get accumulated
            // into a single energy value.
            m_fAccumulatedSquareSum += audioBuffer[i] * audioBuffer[i];
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

        // Apply latest beam angle and confidence
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