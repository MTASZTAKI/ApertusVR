//------------------------------------------------------------------------------
// <copyright file="ResamplerUtil.h" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//------------------------------------------------------------------------------

#pragma once

#include <mfapi.h>
#include <mfidl.h>

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
HRESULT CreateResampler(const WAVEFORMATEX* pwfxIn, const WAVEFORMATEX* pwfxOut, IMFTransform **ppResampler);

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
HRESULT CreateResamplerBuffer(size_t bufferSize, IMFSample** ppSample, IMFMediaBuffer** ppBuffer);