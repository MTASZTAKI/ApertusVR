//------------------------------------------------------------------------------
// <copyright file="ResamplerUtil.cpp" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//------------------------------------------------------------------------------

#include "stdafx.h"
#include "ResamplerUtil.h"
#include "wmcodecdsp.h"

/// <summary>
/// Gets the appropriate Media Foundation audio media subtype from the specified wave format.
/// </summary>
/// <param name="pwfx">
/// [in] Input wave format to convert.
/// </param>
/// <param name="pSubType">
/// [out] Media Foundation audio subtype resulting from conversion.
/// </param>
/// <returns>
/// S_OK on success, otherwise failure code.
/// </returns>
HRESULT GetMediaSubtype(const WAVEFORMATEX* pwfx, GUID *pSubType)
{
    HRESULT hr = S_OK;

    switch(pwfx->wFormatTag)
    {
    case WAVE_FORMAT_PCM:
    case WAVE_FORMAT_IEEE_FLOAT:
    case WAVE_FORMAT_DTS:
    case WAVE_FORMAT_DOLBY_AC3_SPDIF:
    case WAVE_FORMAT_DRM:
    case WAVE_FORMAT_WMAUDIO2:
    case WAVE_FORMAT_WMAUDIO3:
    case WAVE_FORMAT_WMAUDIO_LOSSLESS:
    case WAVE_FORMAT_WMASPDIF:
    case WAVE_FORMAT_WMAVOICE9:
    case WAVE_FORMAT_MPEGLAYER3:
    case WAVE_FORMAT_MPEG:
    case WAVE_FORMAT_MPEG_HEAAC:
    case WAVE_FORMAT_MPEG_ADTS_AAC:
        {
            // These format tags map 1-to-1 to Media Foundation formats.
            // The MSDN topic http://msdn.microsoft.com/en-us/library/aa372553(VS.85).aspx indicates that
            // to create an audio subtype GUID one can:
            // 1. Start with the value MFAudioFormat_Base
            // 2. Replace the first DWORD of this GUID with the format tag
            GUID guidSubType = MFAudioFormat_Base;
            guidSubType.Data1 = pwfx->wFormatTag;

            *pSubType = guidSubType;
            break;
        }

    case WAVE_FORMAT_EXTENSIBLE:
        {
            const WAVEFORMATEXTENSIBLE* pExtensible = reinterpret_cast<const WAVEFORMATEXTENSIBLE*>(pwfx);
            // We only support PCM and IEEE float subtypes for extensible wave formats
            if (pExtensible->SubFormat == KSDATAFORMAT_SUBTYPE_PCM)
            {
                *pSubType = MFAudioFormat_PCM;
            }
            else if (pExtensible->SubFormat == KSDATAFORMAT_SUBTYPE_IEEE_FLOAT)
            {
                *pSubType = MFAudioFormat_Float;
            }
            else
            {
                hr = E_INVALIDARG;
            }
            break;
        }

    default:
        {
            hr = E_INVALIDARG;
            break;
        }
    }

    return hr;
}

/// <summary>
/// Converts the specified wave format into the appropriate Media Foundation audio media type.
/// </summary>
/// <param name="pwfx">
/// [in] Input wave format to convert.
/// </param>
/// <param name="ppType">
/// [out] Media Foundation type resulting from conversion.
/// </param>
/// <returns>
/// S_OK on success, otherwise failure code.
/// </returns>
HRESULT CreateMediaType(const WAVEFORMATEX* pwfx, IMFMediaType** ppType)
{
    HRESULT hr = S_OK;
    IMFMediaType* pType = NULL;
    GUID guidSubType;

    // Create the empty media type.
    hr = MFCreateMediaType(&pType);
    if (FAILED(hr))
    {
        return hr;
    }

    hr = GetMediaSubtype(pwfx, &guidSubType);
    if (SUCCEEDED(hr))
    {
        // Calculate derived values.
        UINT32 blockAlign = pwfx->nChannels * (pwfx->wBitsPerSample / 8);
        UINT32 bytesPerSecond = blockAlign * pwfx->nSamplesPerSec;

        // Set attributes on the type.
        pType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Audio);
        pType->SetGUID(MF_MT_SUBTYPE, guidSubType);
        pType->SetUINT32(MF_MT_AUDIO_NUM_CHANNELS, pwfx->nChannels);
        pType->SetUINT32(MF_MT_AUDIO_SAMPLES_PER_SECOND, pwfx->nSamplesPerSec);
        pType->SetUINT32(MF_MT_AUDIO_BLOCK_ALIGNMENT, blockAlign);
        pType->SetUINT32(MF_MT_AUDIO_AVG_BYTES_PER_SECOND, bytesPerSecond);
        pType->SetUINT32(MF_MT_AUDIO_BITS_PER_SAMPLE, pwfx->wBitsPerSample);
        pType->SetUINT32(MF_MT_ALL_SAMPLES_INDEPENDENT, TRUE);

        // Return the type to the caller.
        *ppType = pType;
        pType = NULL;
    }

    SafeRelease(pType);
    return hr;
}

/// <summary>
/// Create Media Foundation transform that resamples audio in specified input format
/// into specified output format.
/// </summary>
/// <param name="pwfxIn">
/// [in] Wave format input to resampling operation.
/// </param>
/// <param name="pwfxOut">
/// [in] Wave format output from resampling operation.
/// </param>
/// <param name="ppResampler">
/// [out] Media transform object that will resample audio
/// </param>
/// <returns>
/// S_OK on success, otherwise failure code.
/// </returns>
HRESULT CreateResampler(const WAVEFORMATEX* pwfxIn, const WAVEFORMATEX* pwfxOut, IMFTransform **ppResampler)
{
    HRESULT hr = S_OK;
    IMFMediaType* pInputType = NULL;
    IMFMediaType* pOutputType = NULL;
    IMFTransform* pResampler = NULL;

    hr = CoCreateInstance(CLSID_CResamplerMediaObject, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pResampler));
    if (FAILED(hr))
    {
        return hr;
    }

    hr = CreateMediaType(pwfxIn, &pInputType);
    if (SUCCEEDED(hr))
    {
        hr = pResampler->SetInputType(0, pInputType, 0);
        if (SUCCEEDED(hr))
        {
            hr = CreateMediaType(pwfxOut, &pOutputType);
            if (SUCCEEDED(hr))
            {
                hr = pResampler->SetOutputType(0, pOutputType, 0);
                if (SUCCEEDED(hr))
                {
                    *ppResampler = pResampler;
                    pResampler = NULL;
                }
            }
        }
    }

    SafeRelease(pInputType);
    SafeRelease(pOutputType);
    SafeRelease(pResampler);
    return hr;
}

/// <summary>
/// Create a media buffer to be used as input or output for resampler.
/// </summary>
/// <param name="bufferSize">
/// [in] Size of buffer to create.
/// </param>
/// <param name="ppSample">
/// [out] Media Foundation sample created.
/// </param>
/// <param name="ppBuffer">
/// [out] Media buffer created.
/// </param>
/// <returns>
/// S_OK on success, otherwise failure code.
/// </returns>
HRESULT CreateResamplerBuffer(size_t bufferSize, IMFSample** ppSample, IMFMediaBuffer** ppBuffer)
{
    IMFSample* pSample = NULL;
    IMFMediaBuffer* pBuffer = NULL;
    HRESULT hr = S_OK;

    hr = MFCreateSample(&pSample);
    if (FAILED(hr))
    {
        return hr;
    }

    hr = MFCreateMemoryBuffer(static_cast<DWORD>(bufferSize), &pBuffer);
    if (SUCCEEDED(hr))
    {
        hr = pSample->AddBuffer(pBuffer);
        if (SUCCEEDED(hr))
        {
            *ppSample = pSample;
            pSample = NULL;
            *ppBuffer = pBuffer;
            pBuffer = NULL;
        }
    }

    SafeRelease(pSample);
    SafeRelease(pBuffer);

    return hr;
}