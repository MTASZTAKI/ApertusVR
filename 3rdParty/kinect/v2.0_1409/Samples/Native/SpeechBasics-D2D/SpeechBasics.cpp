//------------------------------------------------------------------------------
// <copyright file="SpeechBasics.cpp" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//------------------------------------------------------------------------------

#include "stdafx.h"
#include "SpeechBasics.h"
#include "resource.h"

#define INITGUID
#include <guiddef.h>

// Static initializers
LPCWSTR CSpeechBasics::GrammarFileName = L"SpeechBasics-D2D.grxml";

// This is the class ID we expect for the Microsoft Speech recognizer.
// Other values indicate that we're using a version of sapi.h that is
// incompatible with this sample.
DEFINE_GUID(CLSID_ExpectedRecognizer, 0x495648e7, 0xf7ab, 0x4267, 0x8e, 0x0f, 0xca, 0xfb, 0x7a, 0x33, 0xc1, 0x60);

/// <summary>
/// Entry point for the application
/// </summary>
/// <param name="hInstance">handle to the application instance</param>
/// <param name="hPrevInstance">always 0</param>
/// <param name="lpCmdLine">command line arguments</param>
/// <param name="nCmdShow">whether to display minimized, maximized, or normally</param>
/// <returns>status</returns>
int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE /* hPrevInstance */, _In_ LPWSTR /* lpCmdLine */, _In_ int nCmdShow)
{
    if (CLSID_ExpectedRecognizer != CLSID_SpInprocRecognizer)
    {
        MessageBoxW(NULL, L"This sample was compiled against an incompatible version of sapi.h.\nPlease ensure that Microsoft Speech SDK and other sample requirements are installed and then rebuild application.", L"Missing requirements", MB_OK | MB_ICONERROR);

        return EXIT_FAILURE;
    }

    HRESULT hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);

    if (SUCCEEDED(hr))
    {
        {
            CSpeechBasics application;
            application.Run(hInstance, nCmdShow);
        }

        CoUninitialize();
    }

    return EXIT_SUCCESS;
}

/// <summary>
/// Constructor
/// </summary>
CSpeechBasics::CSpeechBasics() :
    m_pD2DFactory(NULL),
    m_pTurtleController(NULL),
    m_pKinectSensor(NULL),
    m_pAudioBeam(NULL),
    m_pAudioStream(NULL),
    m_p16BitAudioStream(NULL),
    m_hSensorNotification(reinterpret_cast<WAITABLE_HANDLE>(INVALID_HANDLE_VALUE)),
    m_pSpeechStream(NULL),
    m_pSpeechRecognizer(NULL),
    m_pSpeechContext(NULL),
    m_pSpeechGrammar(NULL),
    m_hSpeechEvent(INVALID_HANDLE_VALUE)
{
}

/// <summary>
/// Destructor
/// </summary>
CSpeechBasics::~CSpeechBasics()
{
    if (m_pKinectSensor)
    {
        m_pKinectSensor->Close();
    }

    //16 bit Audio Stream
    if (NULL != m_p16BitAudioStream)
    {
        delete m_p16BitAudioStream;
        m_p16BitAudioStream = NULL;
    }
    SafeRelease(m_pAudioStream);
    SafeRelease(m_pAudioBeam);
    SafeRelease(m_pKinectSensor);

    // clean up Direct2D renderer
    if (NULL != m_pTurtleController)
    {
        delete m_pTurtleController;
        m_pTurtleController = NULL;
    }

    // clean up Direct2D
    SafeRelease(m_pD2DFactory);
}

/// <summary>
/// Creates the main window and begins processing
/// </summary>
/// <param name="hInstance">handle to the application instance</param>
/// <param name="nCmdShow">whether to display minimized, maximized, or normally</param>
int CSpeechBasics::Run(HINSTANCE hInstance, int nCmdShow)
{
    MSG       msg = {0};
    WNDCLASS  wc;
    const int maxEventCount = 2;
    int eventCount = 1;
    HANDLE hEvents[maxEventCount];

    // Dialog custom window class
    ZeroMemory(&wc, sizeof(wc));
    wc.style         = CS_HREDRAW | CS_VREDRAW;
    wc.cbWndExtra    = DLGWINDOWEXTRA;
    wc.hInstance     = hInstance;
    wc.hCursor       = LoadCursorW(NULL, IDC_ARROW);
    wc.hIcon         = LoadIconW(hInstance, MAKEINTRESOURCE(IDI_APP));
    wc.lpfnWndProc   = DefDlgProcW;
    wc.lpszClassName = L"SpeechBasicsAppDlgWndClass";

    if (!RegisterClassW(&wc))
    {
        return 0;
    }

    // Create main application window
    HWND hWndApp = CreateDialogParamW(
        hInstance,
        MAKEINTRESOURCE(IDD_APP),
        NULL,
        (DLGPROC)CSpeechBasics::MessageRouter, 
        reinterpret_cast<LPARAM>(this));

    // Show window
    ShowWindow(hWndApp, nCmdShow);

    // Main message loop
    while (WM_QUIT != msg.message)
    {
        if (m_hSpeechEvent != INVALID_HANDLE_VALUE)
        {
            hEvents[1] = m_hSpeechEvent;
            eventCount = 2;
        }

        hEvents[0] = reinterpret_cast<HANDLE>(m_hSensorNotification);

        // Check to see if we have either a message (by passing in QS_ALLINPUT)
        // Or sensor notification (hEvents[0])
        // Or a speech event (hEvents[1])
        DWORD waitResult = MsgWaitForMultipleObjectsEx(eventCount, hEvents, 50, QS_ALLINPUT, MWMO_INPUTAVAILABLE);

        switch (waitResult)
        {
        case WAIT_OBJECT_0:
            {
                BOOLEAN sensorState = FALSE;

                // Getting the event data will reset the event.
                IIsAvailableChangedEventArgs* pEventData = nullptr;
                if (FAILED(m_pKinectSensor->GetIsAvailableChangedEventData(m_hSensorNotification, &pEventData)))
                {
                    SetStatusMessage(L"Failed to get sensor availability.");
                    break;
                }

                pEventData->get_IsAvailable(&sensorState);
                SafeRelease(pEventData);

                if (sensorState == FALSE)
                {
                    SetStatusMessage(L"Sensor has been disconnected - attach Sensor");
                }
                else
                {
                    HRESULT hr = S_OK;

                    if (m_pSpeechRecognizer == NULL)
                    {
                        hr =InitializeSpeech();
                    }
                    if (SUCCEEDED(hr))
                    {
                        SetStatusMessage(L"Say: \"Forward\", \"Back\", \"Turn Left\" or \"Turn Right\"");
                    }
                    else
                    {
                        SetStatusMessage(L"Speech Initialization Failed");
                    }
                }
            }
            break;
        case WAIT_OBJECT_0 + 1:
            if (eventCount == 2)
            {
                ProcessSpeech();
            }
            break;
        }

        while (PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE))
        {
            // If a dialog message will be taken care of by the dialog proc
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
/// <param name="hWnd">window message is for</param>
/// <param name="uMsg">message</param>
/// <param name="wParam">message data</param>
/// <param name="lParam">additional message data</param>
/// <returns>result of message processing</returns>
LRESULT CALLBACK CSpeechBasics::MessageRouter(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    CSpeechBasics* pThis = NULL;

    if (WM_INITDIALOG == uMsg)
    {
        pThis = reinterpret_cast<CSpeechBasics*>(lParam);
        SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pThis));
    }
    else
    {
        pThis = reinterpret_cast<CSpeechBasics*>(::GetWindowLongPtr(hWnd, GWLP_USERDATA));
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
/// <param name="hWnd">window message is for</param>
/// <param name="uMsg">message</param>
/// <param name="wParam">message data</param>
/// <param name="lParam">additional message data</param>
/// <returns>result of message processing</returns>
LRESULT CALLBACK CSpeechBasics::DlgProc(HWND hWnd, UINT message, WPARAM /* wParam */, LPARAM /* lParam */)
{
    LRESULT result = FALSE;

    switch (message)
    {
    case WM_INITDIALOG:
        {
            // Bind application window handle
            m_hWnd = hWnd;

            // Init Direct2D
            D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &m_pD2DFactory);

            // Create and initialize a new Direct2D image renderer
            // We'll use this to draw the data we receive from the Kinect to the screen
            m_pTurtleController = new TurtleController();
            HRESULT hr = m_pTurtleController->Initialize(GetDlgItem(m_hWnd, IDC_AUDIOVIEW), m_pD2DFactory);
            if (FAILED(hr))
            {
                SetStatusMessage(L"Failed to initialize the Direct2D draw device.");
                break;
            }

            // Look for a connected Kinect, and create it if found
            hr = StartKinect();
            if (FAILED(hr))
            {
                break;
            }

            SetStatusMessage(L"Waiting for Sensor and Speech Initialization - Please ensure Sensor is attached.");
            result = FALSE;
            break;
        }

    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            BeginPaint(hWnd, &ps);

            m_pTurtleController->Draw();

            EndPaint(hWnd, &ps);
            result = TRUE;
            break;
        }

        // If the titlebar X is clicked, destroy app
    case WM_CLOSE:
        if (NULL != m_p16BitAudioStream)
        {
            m_p16BitAudioStream->SetSpeechState(false);
        }

        if (NULL != m_pSpeechRecognizer)
        {
            m_pSpeechRecognizer->SetRecoState(SPRST_INACTIVE_WITH_PURGE);

            //cleanup here
            SafeRelease(m_pSpeechStream);
            SafeRelease(m_pSpeechRecognizer);
            SafeRelease(m_pSpeechContext);
            SafeRelease(m_pSpeechGrammar);
        }

        DestroyWindow(hWnd);
        result = TRUE;
        break;

    case WM_DESTROY:
        // Quit the main message pump
        PostQuitMessage(0);
        result = TRUE;
        break;
    }

    return result;
}

/// <summary>
/// Open the KinectSensor and its Audio Stream
/// </summary>
/// <returns>S_OK on success, otherwise failure code.</returns>
HRESULT CSpeechBasics::StartKinect()
{
    HRESULT hr = S_OK;
    IAudioSource* pAudioSource = NULL;
    IAudioBeamList* pAudioBeamList = NULL;
    BOOLEAN sensorState = TRUE;

    hr = GetDefaultKinectSensor(&m_pKinectSensor);
    if (FAILED(hr))
    {
        SetStatusMessage(L"Failed getting default sensor!");
        return hr;
    }

    hr = m_pKinectSensor->SubscribeIsAvailableChanged(&m_hSensorNotification);

    if (SUCCEEDED(hr))
    {
        hr = m_pKinectSensor->Open();
    }

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
        m_p16BitAudioStream = new KinectAudioStream(m_pAudioStream);
    }

    if (FAILED(hr))
    {
        SetStatusMessage(L"Failed opening an audio stream!");
    }

    SafeRelease(pAudioBeamList);
    SafeRelease(pAudioSource);
    return hr;
}

/// <summary>
/// Open the KinectSensor and its Audio Stream
/// </summary>
/// <returns>S_OK on success, otherwise failure code.</returns>
HRESULT CSpeechBasics::InitializeSpeech()
{

    // Audio Format for Speech Processing
    WORD AudioFormat = WAVE_FORMAT_PCM;
    WORD AudioChannels = 1;
    DWORD AudioSamplesPerSecond = 16000;
    DWORD AudioAverageBytesPerSecond = 32000;
    WORD AudioBlockAlign = 2;
    WORD AudioBitsPerSample = 16;

    WAVEFORMATEX wfxOut = {AudioFormat, AudioChannels, AudioSamplesPerSecond, AudioAverageBytesPerSecond, AudioBlockAlign, AudioBitsPerSample, 0};

    HRESULT hr = CoCreateInstance(CLSID_SpStream, NULL, CLSCTX_INPROC_SERVER, __uuidof(ISpStream), (void**)&m_pSpeechStream);

    if (SUCCEEDED(hr))
    {

        m_p16BitAudioStream->SetSpeechState(true);        
        hr = m_pSpeechStream->SetBaseStream(m_p16BitAudioStream, SPDFID_WaveFormatEx, &wfxOut);
    }

    if (SUCCEEDED(hr))
    {
        hr = CreateSpeechRecognizer();
    }

    if (FAILED(hr))
    {
        SetStatusMessage(L"Could not create speech recognizer. Please ensure that Microsoft Speech SDK and other sample requirements are installed.");
        return hr;
    }

    hr = LoadSpeechGrammar();

    if (FAILED(hr))
    {
        SetStatusMessage(L"Could not load speech grammar. Please ensure that grammar configuration file was properly deployed.");
        return hr;
    }

    hr = StartSpeechRecognition();

    if (FAILED(hr))
    {
        SetStatusMessage(L"Could not start recognizing speech.");
        return hr;
    }

    return hr;
}


/// <summary>
/// Create speech recognizer that will read Kinect audio stream data.
/// </summary>
/// <returns>
/// <para>S_OK on success, otherwise failure code.</para>
/// </returns>
HRESULT CSpeechBasics::CreateSpeechRecognizer()
{
    ISpObjectToken *pEngineToken = NULL;

    HRESULT hr = CoCreateInstance(CLSID_SpInprocRecognizer, NULL, CLSCTX_INPROC_SERVER, __uuidof(ISpRecognizer), (void**)&m_pSpeechRecognizer);

    if (SUCCEEDED(hr))
    {
        m_pSpeechRecognizer->SetInput(m_pSpeechStream, TRUE);

        // If this fails here, you have not installed the acoustic models for Kinect
        hr = SpFindBestToken(SPCAT_RECOGNIZERS, L"Language=409;Kinect=True", NULL, &pEngineToken);

        if (SUCCEEDED(hr))
        {
            m_pSpeechRecognizer->SetRecognizer(pEngineToken);
            hr = m_pSpeechRecognizer->CreateRecoContext(&m_pSpeechContext);

            // For long recognition sessions (a few hours or more), it may be beneficial to turn off adaptation of the acoustic model. 
            // This will prevent recognition accuracy from degrading over time.
            if (SUCCEEDED(hr))
            {
                hr = m_pSpeechRecognizer->SetPropertyNum(L"AdaptationOn", 0);                
            }
        }
    }
    SafeRelease(pEngineToken);
    return hr;
}

/// <summary>
/// Load speech recognition grammar into recognizer.
/// </summary>
/// <returns>
/// <para>S_OK on success, otherwise failure code.</para>
/// </returns>
HRESULT CSpeechBasics::LoadSpeechGrammar()
{
    HRESULT hr = m_pSpeechContext->CreateGrammar(1, &m_pSpeechGrammar);

    if (SUCCEEDED(hr))
    {
        // Populate recognition grammar from file
        hr = m_pSpeechGrammar->LoadCmdFromFile(GrammarFileName, SPLO_STATIC);
    }

    return hr;
}

/// <summary>
/// Start recognizing speech asynchronously.
/// </summary>
/// <returns>
/// <para>S_OK on success, otherwise failure code.</para>
/// </returns>
HRESULT CSpeechBasics::StartSpeechRecognition()
{
    HRESULT hr = S_OK;

    // Specify that all top level rules in grammar are now active
    hr = m_pSpeechGrammar->SetRuleState(NULL, NULL, SPRS_ACTIVE);
    if (FAILED(hr))
    {
        return hr;
    }

    // Specify that engine should always be reading audio
    hr = m_pSpeechRecognizer->SetRecoState(SPRST_ACTIVE_ALWAYS);
    if (FAILED(hr))
    {
        return hr;
    }

    // Specify that we're only interested in receiving recognition events
    hr = m_pSpeechContext->SetInterest(SPFEI(SPEI_RECOGNITION), SPFEI(SPEI_RECOGNITION));
    if (FAILED(hr))
    {
        return hr;
    }

    // Ensure that engine is recognizing speech and not in paused state
    hr = m_pSpeechContext->Resume(0);
    if (FAILED(hr))
    {
        return hr;
    }

    m_hSpeechEvent = m_pSpeechContext->GetNotifyEventHandle();
    return hr;
}

/// <summary>
/// Process recently triggered speech recognition events.
/// </summary>
void CSpeechBasics::ProcessSpeech()
{
    const float ConfidenceThreshold = 0.3f;

    SPEVENT curEvent = {SPEI_UNDEFINED, SPET_LPARAM_IS_UNDEFINED, 0, 0, 0, 0};
    ULONG fetched = 0;
    HRESULT hr = S_OK;

    m_pSpeechContext->GetEvents(1, &curEvent, &fetched);
    while (fetched > 0)
    {
        switch (curEvent.eEventId)
        {
        case SPEI_RECOGNITION:
            if (SPET_LPARAM_IS_OBJECT == curEvent.elParamType)
            {
                // this is an ISpRecoResult
                ISpRecoResult* result = reinterpret_cast<ISpRecoResult*>(curEvent.lParam);
                SPPHRASE* pPhrase = NULL;

                hr = result->GetPhrase(&pPhrase);
                if (SUCCEEDED(hr))
                {
                    if ((pPhrase->pProperties != NULL) && (pPhrase->pProperties->pFirstChild != NULL))
                    {
                        const SPPHRASEPROPERTY* pSemanticTag = pPhrase->pProperties->pFirstChild;
                        if (pSemanticTag->SREngineConfidence > ConfidenceThreshold)
                        {
                            TurtleAction action = MapSpeechTagToAction(pSemanticTag->pszValue);
                            m_pTurtleController->DoAction(action);
                        }
                    }
                    ::CoTaskMemFree(pPhrase);
                }
            }
            break;
        }

        m_pSpeechContext->GetEvents(1, &curEvent, &fetched);
    }

    return;
}

/// <summary>
/// Maps a specified speech semantic tag to the corresponding action to be performed on turtle.
/// </summary>
/// <returns>
/// Action that matches <paramref name="pszSpeechTag"/>, or TurtleActionNone if no matches were found.
/// </returns>
TurtleAction CSpeechBasics::MapSpeechTagToAction(LPCWSTR pszSpeechTag)
{
    struct SpeechTagToAction
    {
        LPCWSTR pszSpeechTag;
        TurtleAction action;
    };
    const SpeechTagToAction Map[] =
    {
        {L"FORWARD", TurtleActionForward},
        {L"BACKWARD", TurtleActionBackward},
        {L"LEFT", TurtleActionTurnLeft},
        {L"RIGHT", TurtleActionTurnRight}
    };

    TurtleAction action = TurtleActionNone;

    for (int i = 0; i < _countof(Map); ++i)
    {
        if ( (Map[i].pszSpeechTag != NULL) && (0 == wcscmp(Map[i].pszSpeechTag, pszSpeechTag)))
        {
            action = Map[i].action;
            break;
        }
    }

    return action;
}

/// <summary>
/// Set the status bar message
/// </summary>
/// <param name="szMessage">message to display</param>
void CSpeechBasics::SetStatusMessage(const WCHAR* szMessage)
{
    SendDlgItemMessageW(m_hWnd, IDC_STATUS, WM_SETTEXT, 0, (LPARAM)szMessage);
}