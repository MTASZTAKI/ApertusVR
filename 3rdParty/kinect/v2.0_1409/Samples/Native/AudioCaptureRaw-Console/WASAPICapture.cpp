//------------------------------------------------------------------------------
// <copyright file="WASAPICapture.cpp" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
// <summary>
// This module provides sample code used to demonstrate capturing raw audio streams from
// the Kinect 4-microphone array.
// </summary>
//------------------------------------------------------------------------------

#include "StdAfx.h"
#include <assert.h>
#include <avrt.h>
#include "WASAPICapture.h"

/// <summary>
/// Initializes an instance of CWASAPICapture type.
/// </summary>
CWASAPICapture::CWASAPICapture(IMMDevice *Endpoint) : 
    _Endpoint(Endpoint),
    _AudioClient(NULL),
    _CaptureClient(NULL),
    _Resampler(NULL),
    _CaptureThread(NULL),
    _CaptureFile(INVALID_HANDLE_VALUE),
    _ShutdownEvent(NULL),
    _EngineLatencyInMS(0),
    _MixFormat(NULL),
    _MixFrameSize(0),
    _InputBufferSize(0),
    _InputBuffer(NULL),
    _InputSample(NULL),
    _OutputBufferSize(0),
    _OutputBuffer(NULL),
    _OutputSample(NULL),
    _BytesCaptured(0),
    _Gain(1.0f)
{
    _Endpoint->AddRef();    // Since we're holding a copy of the endpoint, take a reference to it.  It'll be released in Shutdown();
}

/// <summary>
/// Uninitialize an instance of CWASAPICapture type.
/// </summary>
/// <remarks>
/// Shuts down the capture code and frees all the resources.
/// </remarks>
CWASAPICapture::~CWASAPICapture(void) 
{
    if (NULL != _CaptureThread)
    {
        SetEvent(_ShutdownEvent);
        WaitForSingleObject(_CaptureThread, INFINITE);
        CloseHandle(_CaptureThread);
        _CaptureThread = NULL;
    }

    _CaptureFile = INVALID_HANDLE_VALUE;

    if (NULL != _ShutdownEvent)
    {
        CloseHandle(_ShutdownEvent);
        _ShutdownEvent = NULL;
    }

    SafeRelease(_Endpoint);
    SafeRelease(_AudioClient);
    SafeRelease(_CaptureClient);
    SafeRelease(_Resampler);

    if (NULL != _MixFormat)
    {
        CoTaskMemFree(_MixFormat);
        _MixFormat = NULL;
    }

    SafeRelease(_InputBuffer);
    SafeRelease(_InputSample);
    SafeRelease(_OutputBuffer);
    SafeRelease(_OutputSample);
}

/// <summary>
/// Initialize the capturer.
/// </summary>
/// <param name="EngineLatency">
/// Number of milliseconds of acceptable lag between live sound being produced and recording operation.
/// </param>
/// <returns>
/// true if capturer was initialized successfully, false otherwise.
/// </returns>
bool CWASAPICapture::Initialize(UINT32 EngineLatency)
{
    //
    //  Create our shutdown event - we want a manual reset event that starts in the not-signaled state.
    //
    _ShutdownEvent = CreateEventEx(NULL, NULL, CREATE_EVENT_MANUAL_RESET, EVENT_MODIFY_STATE | SYNCHRONIZE);
    if (NULL == _ShutdownEvent)
    {
        printf_s("Unable to create shutdown event: %u.\n", GetLastError());
        return false;
    }    

    //
    //  Now activate an IAudioClient object on our preferred endpoint and retrieve the mix format for that endpoint.
    //
    HRESULT hr = _Endpoint->Activate(__uuidof(IAudioClient), CLSCTX_INPROC_SERVER, NULL, reinterpret_cast<void **>(&_AudioClient));
    if (FAILED(hr))
    {
        printf_s("Unable to activate audio client: %x.\n", hr);
        return false;
    }

    //
    // Load the MixFormat.  This may differ depending on the shared mode used
    //
    if (!LoadFormat())
    {
        printf_s("Failed to load the mix format \n");
        return false;
    }

    //
    //  Remember our configured latency
    //
    _EngineLatencyInMS = EngineLatency;

    if (!InitializeAudioEngine())
    {
        return false;
    }

    _InputBufferSize = _EngineLatencyInMS * _MixFormat->nAvgBytesPerSec / 1000;
    _OutputBufferSize = _EngineLatencyInMS * _OutFormat.nAvgBytesPerSec / 1000;

    hr = CreateResamplerBuffer(_InputBufferSize, &_InputSample, &_InputBuffer);
    if (FAILED(hr))
    {
        printf_s("Unable to allocate input buffer.");
        return false;
    }

    hr = CreateResamplerBuffer(_OutputBufferSize, &_OutputSample, &_OutputBuffer);
    if (FAILED(hr))
    {
        printf_s("Unable to allocate output buffer.");
        return false;
    }

    // Create resampler object
    hr = CreateResampler(_MixFormat, &_OutFormat, &_Resampler);
    if (FAILED(hr))
    {
        printf_s("Unable to create audio resampler\n");
        return false;
    }

    return true;
}

/// <summary>
///  Start capturing audio data.
/// </summary>
/// <param name="waveFile">
/// [in] Handle to wave file where audio data will be written.
/// </param>
/// <returns>
/// true if capturer has successfully started capturing audio data, false otherwise.
/// </returns>
bool CWASAPICapture::Start(HANDLE waveFile)
{
    HRESULT hr;

    _BytesCaptured = 0;
    _CaptureFile = waveFile;

    //
    //  Now create the thread which is going to drive the capture.
    //
    _CaptureThread = CreateThread(NULL, 0, WASAPICaptureThread, this, 0, NULL);
    if (NULL == _CaptureThread)
    {
        printf_s("Unable to create transport thread: %x.", GetLastError());
        return false;
    }

    //
    //  We're ready to go, start capturing!
    //
    hr = _AudioClient->Start();
    if (FAILED(hr))
    {
        printf_s("Unable to start capture client: %x.\n", hr);
        return false;
    }

    return true;
}

/// <summary>
/// Stop the capturer.
/// </summary>
void CWASAPICapture::Stop()
{
    HRESULT hr;

    //
    //  Tell the capture thread to shut down, wait for the thread to complete then clean up all the stuff we 
    //  allocated in Start().
    //
    if (NULL != _ShutdownEvent)
    {
        SetEvent(_ShutdownEvent);
    }

    hr = _AudioClient->Stop();
    if (FAILED(hr))
    {
        printf_s("Unable to stop audio client: %x\n", hr);
    }

    if (NULL != _CaptureThread)
    {
        WaitForSingleObject(_CaptureThread, INFINITE);

        CloseHandle(_CaptureThread);
        _CaptureThread = NULL;
    }
}


/// <summary>
/// Capture thread - captures audio from WASAPI, processes it with a resampler and writes it to file.
/// </summary>
/// <param name="Context">
/// [in] Thread data, representing an instance of CWASAPICapture type.
/// </param>
/// <returns>
/// Thread return value.
/// </returns>
DWORD CWASAPICapture::WASAPICaptureThread(LPVOID Context)
{
    CWASAPICapture *capturer = static_cast<CWASAPICapture *>(Context);
    return capturer->DoCaptureThread();
}

/// <summary>
/// Capture thread - captures audio from WASAPI, processes it with a resampler and writes it to file.
/// </summary>
/// <returns>
/// Thread return value.
/// </returns>
DWORD CWASAPICapture::DoCaptureThread()
{
    bool stillPlaying = true;
    HANDLE mmcssHandle = NULL;
    DWORD mmcssTaskIndex = 0;

    HRESULT hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
    if (FAILED(hr))
    {
        printf_s("Unable to initialize COM in render thread: %x\n", hr);
        return hr;
    }

    mmcssHandle = AvSetMmThreadCharacteristics(L"Audio", &mmcssTaskIndex);
    if (mmcssHandle == NULL)
    {
        printf_s("Unable to enable MMCSS on capture thread: %u\n", GetLastError());
    }

    while (stillPlaying)
    {
        //
        //  We want to wait for half the desired latency in milliseconds.
        //
        //  That way we'll wake up half way through the processing period to pull the 
        //  next set of samples from the engine.
        //
        DWORD waitResult = WaitForSingleObject(_ShutdownEvent, _EngineLatencyInMS / 2);
  
        switch (waitResult)
        {
        case WAIT_OBJECT_0 + 0:
            // If _ShutdownEvent has been set, we're done and should exit the main capture loop.
            stillPlaying = false;
            break;

        case WAIT_TIMEOUT:

            //  We need to retrieve the next buffer of samples from the audio capturer.
            BYTE *pData;
            UINT32 framesAvailable;
            DWORD  flags;
            bool isEmpty = false;

            // Keep fetching audio in a tight loop as long as audio device still has data.
            while (!isEmpty && (WAIT_OBJECT_0 != WaitForSingleObject(_ShutdownEvent, 0)))
            {
                hr = _CaptureClient->GetBuffer(&pData, &framesAvailable, &flags, NULL, NULL);
                if (SUCCEEDED(hr))
                {
                    if ( (AUDCLNT_S_BUFFER_EMPTY == hr) || (0 == framesAvailable) )
                    {
                        isEmpty = true;
                    }
                    else
                    {
                        DWORD bytesAvailable = framesAvailable * _MixFrameSize;

                        // Process input to resampler
                        hr = ProcessResamplerInput(pData, bytesAvailable, flags);
                        if (SUCCEEDED(hr))
                        {
                            DWORD bytesWritten;

                            // Process output from resampler
                            hr = ProcessResamplerOutput(&bytesWritten);
                            if (SUCCEEDED(hr))
                            {
                                //  Audio capture was successful, so bump the capture buffer pointer.
                                _BytesCaptured += bytesWritten;
                            }
                        }
                    }

                    hr = _CaptureClient->ReleaseBuffer(framesAvailable);
                    if (FAILED(hr))
                    {
                        printf_s("Unable to release capture buffer: %x!\n", hr);
                    }
                }
            }

            break;
        }
    }

    if (NULL != mmcssHandle)
    {
        AvRevertMmThreadCharacteristics(mmcssHandle);
    }

    CoUninitialize();
    return 0;
}

/// <summary>
/// Take audio data captured from WASAPI and feed it as input to audio resampler.
/// </summary>
/// <param name="pBuffer">
/// [in] Buffer holding audio data from WASAPI.
/// </param>
/// <param name="bufferSize">
/// [in] Number of bytes available in pBuffer.
/// </param>
/// <param name="flags">
/// [in] Flags returned from WASAPI capture.
/// </param>
/// <returns>
/// S_OK on success, otherwise failure code.
/// </returns>
HRESULT CWASAPICapture::ProcessResamplerInput(BYTE *pBuffer, DWORD bufferSize, DWORD flags)
{
    HRESULT hr = S_OK;
    BYTE* pLocked = NULL;
    DWORD maxLength;

    hr = _InputBuffer->Lock(&pLocked, &maxLength, NULL);
    if (SUCCEEDED(hr))
    {
        DWORD dataToCopy = min(bufferSize, maxLength);

        //
        //  The flags on capture tell us information about the data.
        //
        //  We only really care about the silent flag since we want to put frames of silence into the buffer
        //  when we receive silence.  We rely on the fact that a logical bit 0 is silence for both float and int formats.
        //
        if (flags & AUDCLNT_BUFFERFLAGS_SILENT)
        {
            //  Fill 0s from the capture buffer to the output buffer.
            ZeroMemory(pLocked, dataToCopy);
        }
        else
        {
            //  Copy data from the audio engine buffer to the output buffer.
            memcpy_s(pLocked, maxLength, pBuffer, bufferSize);

            // Apply gain on the raw Kinect buffer if needed, before the resampler.
            if (_Gain != 1.0f)
            {
                // When we capture in shared mode the capture mix format is always 32-bit IEEE
                // floating point, so we can safely assume float samples in the buffer.
                float* pFloatLocked = (float *)pLocked;
                for (DWORD i = 0; i < dataToCopy/sizeof(float); i++)
                {
                    pFloatLocked[i] *= _Gain;
                }
            }
        }

        hr = _InputBuffer->SetCurrentLength(dataToCopy);
        if (SUCCEEDED(hr))
        {
            hr = _Resampler->ProcessInput(0, _InputSample, 0);
        }

        _InputBuffer->Unlock();
    }

    return hr;
}

/// <summary>
/// Get data output from audio resampler and write it to file.
/// </summary>
/// <param name="pBytesWritten">
/// [out] On success, will receive number of bytes written to file.
/// </param>
/// <returns>
/// S_OK on success, otherwise failure code.
/// </returns>
HRESULT CWASAPICapture::ProcessResamplerOutput(DWORD *pBytesWritten)
{
    HRESULT hr = S_OK;
    MFT_OUTPUT_DATA_BUFFER outBuffer;
    DWORD outStatus;

    outBuffer.dwStreamID = 0;
    outBuffer.pSample = _OutputSample;
    outBuffer.dwStatus = 0;
    outBuffer.pEvents = 0;

    hr = _Resampler->ProcessOutput(0, 1, &outBuffer, &outStatus);
    if (SUCCEEDED(hr))
    {
        BYTE* pLocked = NULL;

        hr = _OutputBuffer->Lock(&pLocked, NULL, NULL);
        if (SUCCEEDED(hr))
        {
            DWORD lockedLength;
            hr = _OutputBuffer->GetCurrentLength(&lockedLength);
            if (SUCCEEDED(hr))
            {
                if (!WriteFile(_CaptureFile, pLocked, lockedLength, pBytesWritten, NULL))
                {
                    hr = E_FAIL;
                }
            }

            _OutputBuffer->Unlock();
        }
    }

    return hr;
}

/// <summary>
/// Initialize WASAPI in timer driven mode, and retrieve a capture client for the transport.
/// </summary>
/// <returns>
/// S_OK on success, otherwise failure code.
/// </returns>
bool CWASAPICapture::InitializeAudioEngine()
{
    HRESULT hr = _AudioClient->Initialize(AUDCLNT_SHAREMODE_SHARED, AUDCLNT_STREAMFLAGS_NOPERSIST, _EngineLatencyInMS*10000, 0, _MixFormat, NULL);

    if (FAILED(hr))
    {
        printf_s("Unable to initialize audio client: %x.\n", hr);
        return false;
    }

    hr = _AudioClient->GetService(IID_PPV_ARGS(&_CaptureClient));
    if (FAILED(hr))
    {
        printf_s("Unable to get new capture client: %x.\n", hr);
        return false;
    }

    return true;
}

/// <summary>
/// Retrieve the format we'll use to capture samples and set gain
/// </summary>
/// <returns>
/// true if format was loaded successfully, false otherwise.
/// </returns>
bool CWASAPICapture::LoadFormat()
{
    HRESULT hr = _AudioClient->GetMixFormat(&_MixFormat);
    if (FAILED(hr))
    {
        printf_s("Unable to get mix format on audio client: %x.\n", hr);
        return false;
    }

    // Frame size (nBlockAlign) of the audio we will read from WASAPI
    _MixFrameSize = (_MixFormat->wBitsPerSample / 8) * _MixFormat->nChannels;

    // Set to 0 or 1 to select example outputs
#if 1
    // Example one: Use the capture mix format. If Kinect is the only capture device
    // running, this will give us Kinect's "raw" 4 microphones (4 channels) of audio
    // at 16khz sample rate, 32-bit IEEE float format.
    // In this example we do not apply a gain. Note that since Kinect microphones have
    // a large dynamic range, audio levels may be very low unless you have a sound 
    // source close to Kinect. You can select a gain larger than 1.0 to adjust levels.
    // Also note that in this example, the Resampler is just a pass-through. It does
    // not do any format conversion.
    _OutFormat.wFormatTag = WAVE_FORMAT_IEEE_FLOAT;
    _OutFormat.nChannels = _MixFormat->nChannels;
    _OutFormat.nSamplesPerSec = _MixFormat->nSamplesPerSec;
    _OutFormat.wBitsPerSample = _MixFormat->wBitsPerSample;
    _Gain = 1.0f;
#else
    // Example two: Use the resampler to convert sample rate, number of channels
    // and sample format of the "raw" Kinect audio. In this example, the output is mono, 
    // 16-bit signed at 48khz sample rate. We also apply a gain of 10 to compensate for 
    // low level audio.
    _OutFormat.wFormatTag = WAVE_FORMAT_PCM;
    _OutFormat.nChannels = 1;
    _OutFormat.nSamplesPerSec = 48000;
    _OutFormat.wBitsPerSample = 16;
    _Gain = 10.0f;
#endif

    _OutFormat.nBlockAlign = _OutFormat.nChannels * _OutFormat.wBitsPerSample / 8;
    _OutFormat.nAvgBytesPerSec = _OutFormat.nSamplesPerSec * _OutFormat.nBlockAlign;
    _OutFormat.cbSize = 0;

    return true;
}
