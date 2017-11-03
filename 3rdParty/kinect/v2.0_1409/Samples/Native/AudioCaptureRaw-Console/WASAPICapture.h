//------------------------------------------------------------------------------
// <copyright file="WASAPICapture.h" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//------------------------------------------------------------------------------

// Common includes used in AudioCaptureRaw sample. *

#pragma once
#include <MMDeviceAPI.h>
#include <AudioClient.h>
#include <AudioPolicy.h>
#include "ResamplerUtil.h"

//
//  WASAPI Capture class.
class CWASAPICapture
{
public:
    /// <summary>
    /// Initializes an instance of CWASAPICapture type.
    /// </summary>
    CWASAPICapture(IMMDevice *Endpoint);

    /// <summary>
    /// Uninitialize an instance of CWASAPICapture type.
    /// </summary>
    /// <remarks>
    /// Shuts down the capture code and frees all the resources.
    /// </remarks>
	~CWASAPICapture(void);

    /// <summary>
    /// Initialize the capturer.
    /// </summary>
    /// <param name="EngineLatency">
    /// Number of milliseconds of acceptable lag between live sound being produced and recording operation.
    /// </param>
    /// <returns>
    /// true if capturer was initialized successfully, false otherwise.
    /// </returns>  
    bool Initialize(UINT32 EngineLatency);

    /// <summary>
    ///  Start capturing audio data.
    /// </summary>
    /// <param name="waveFile">
    /// [in] Handle to wave file where audio data will be written.
    /// </param>
    /// <returns>
    /// true if capturer has successfully started capturing audio data, false otherwise.
    /// </returns>
    bool Start(HANDLE waveFile);

    /// <summary>
    /// Stop the capturer.
    /// </summary>
    void Stop();

    /// <summary>
    /// Get format of audio written to file.
    /// </summary>
    /// <returns>
    /// WAVEFORMATEX representing audio format.
    /// </returns>
    WAVEFORMATEX *GetOutputFormat() { return &_OutFormat; }

    /// <summary>
    /// Get number of bytes of audio data captured so far into wave file.
    /// </summary>
    /// <returns>
    /// Number of bytes of audio data captured so far into wave file.
    /// </returns>
    DWORD BytesCaptured() { return _BytesCaptured; }

private:
    //
    //  Core Audio Capture member variables.
    //
    IMMDevice *             _Endpoint;
    IAudioClient *          _AudioClient;
    IAudioCaptureClient *   _CaptureClient;
    IMFTransform *          _Resampler;

    HANDLE                  _CaptureThread;
    HANDLE                  _CaptureFile;
    HANDLE                  _ShutdownEvent;
    LONG                    _EngineLatencyInMS;
    WAVEFORMATEX *          _MixFormat;
    DWORD                   _MixFrameSize;
    WAVEFORMATEX            _OutFormat; 
    float                   _Gain;

    //
    //  Capture buffer member variables
    //
    size_t                  _InputBufferSize;
    IMFMediaBuffer *        _InputBuffer;
    IMFSample *             _InputSample;
    size_t                  _OutputBufferSize;
    IMFMediaBuffer *        _OutputBuffer;
    IMFSample *             _OutputSample;
    DWORD                   _BytesCaptured;
    
    /// <summary>
    /// Capture thread - captures audio from WASAPI, processes it with a resampler and writes it to file.
    /// </summary>
    /// <param name="Context">
    /// [in] Thread data, representing an instance of CWASAPICapture type.
    /// </param>
    /// <returns>
    /// Thread return value.
    /// </returns>
    static DWORD __stdcall WASAPICaptureThread(LPVOID Context);

    /// <summary>
    /// Capture thread - captures audio from WASAPI, processes it with a resampler and writes it to file.
    /// </summary>
    /// <returns>
    /// Thread return value.
    /// </returns>
    DWORD DoCaptureThread();

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
    HRESULT ProcessResamplerInput(BYTE *pBuffer, DWORD bufferSize, DWORD flags);

    /// <summary>
    /// Get data output from audio resampler and write it to file.
    /// </summary>
    /// <param name="pBytesWritten">
    /// [out] On success, will receive number of bytes written to file.
    /// </param>
    /// <returns>
    /// S_OK on success, otherwise failure code.
    /// </returns>
    HRESULT ProcessResamplerOutput(DWORD *pBytesWritten);

    /// <summary>
    /// Initialize WASAPI in timer driven mode, and retrieve a capture client for the transport.
    /// </summary>
    /// <returns>
    /// S_OK on success, otherwise failure code.
    /// </returns>
    bool InitializeAudioEngine();

    /// <summary>
    /// Retrieve the format we'll use to capture samples.
    ///  We use the Mix format since we're capturing in shared mode.
    /// </summary>
    /// <returns>
    /// true if format was loaded successfully, false otherwise.
    /// </returns>
    bool LoadFormat();
};
