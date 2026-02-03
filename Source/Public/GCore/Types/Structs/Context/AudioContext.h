// Copyright (c) 2025 Rafael Valoto. All Rights Reserved.
// Project: GamepadCore
// Description: Cross-platform library for DualSense and generic gamepad input support.
// Targets: Windows, Linux, macOS.
#pragma once

#include <algorithm>
#include <cstring>
#include <vector>

#if GAMEPAD_CORE_HAS_AUDIO
#include "miniaudio.h"
#endif

#include "GImplementations/Utils/GamepadAudio.h"

using namespace FGamepadAudio;

#if !GAMEPAD_CORE_HAS_AUDIO
// Minimal stubs when audio is disabled to allow compilation of unused fields/methods
typedef struct
{
	void* pUserData;
	struct
	{
		int channels;
	} playback;
} ma_device;
typedef struct
{
	int dummy;
} ma_device_id;
typedef unsigned int ma_uint32;
typedef struct
{
	int dummy;
} ma_pcm_rb;
typedef struct
{
	struct
	{
		int format;
		int channels;
		const ma_device_id* pDeviceID;
	} playback;
	int sampleRate;
	void (*dataCallback)(ma_device*, void*, const void*, ma_uint32);
	void* pUserData;
} ma_device_config;

#define MA_SUCCESS 0
#define ma_format_f32 0
#define ma_device_type_playback 0

inline int ma_pcm_rb_available_read(void*)
{
	return 0;
}
inline int ma_pcm_rb_acquire_read(void*, void*, void**)
{
	return 0;
}
inline int ma_pcm_rb_commit_read(void*, int)
{
	return 0;
}
inline int ma_pcm_rb_init(int, int, int, void*, void*, void*)
{
	return 0;
}
inline void ma_pcm_rb_uninit(void*)
{}
inline ma_device_config ma_device_config_init(int)
{
	ma_device_config c = {0};
	return c;
}
inline int ma_device_init(void*, void*, void*)
{
	return 0;
}
inline void ma_device_uninit(void*)
{}
inline int ma_device_start(void*)
{
	return 0;
}
inline int ma_device_stop(void*)
{
	return 0;
}
inline int ma_pcm_rb_available_write(void*)
{
	return 0;
}
inline int ma_pcm_rb_acquire_write(void*, void*, void**)
{
	return 0;
}
inline int ma_pcm_rb_commit_write(void*, int)
{
	return 0;
}
#endif

/**
 * @brief Audio device context using miniaudio for cross-platform audio playback.
 *
 * This replaces the previous WASAPI-specific implementation to support
 * Windows, Linux, and macOS platforms.
 */
struct FAudioDeviceContext
{
	FAudioDeviceContext() = default;

	~FAudioDeviceContext()
	{
		Close();
	}

	static void DataCallback(ma_device* pDevice, void* pOutput, const void* /*pInput*/, ma_uint32 frameCount)
	{
		auto pContext = static_cast<FAudioDeviceContext*>(pDevice->pUserData);
		if (!pContext || !pContext->bInitialized)
		{
			std::memset(pOutput, 0, frameCount * pDevice->playback.channels * sizeof(float));
			return;
		}

		ma_uint32 framesAvailable = ma_pcm_rb_available_read(&pContext->RingBuffer);
		ma_uint32 framesToRead = frameCount;

		if (framesAvailable < framesToRead)
		{
			framesToRead = framesAvailable;
		}

		if (framesToRead > 0)
		{
			void* pReadBuffer;
			ma_uint32 readSize = framesToRead;
			ma_pcm_rb_acquire_read(&pContext->RingBuffer, &readSize, &pReadBuffer);

			std::memcpy(pOutput, pReadBuffer, readSize * pContext->NumChannels * sizeof(float));

			ma_pcm_rb_commit_read(&pContext->RingBuffer, readSize);
		}

		if (framesToRead < frameCount)
		{
			float* pOutputFloat = static_cast<float*>(pOutput);
			ma_uint32 framesMissing = frameCount - framesToRead;

			std::memset(&pOutputFloat[framesToRead * pContext->NumChannels], 0,
			            framesMissing * pContext->NumChannels * sizeof(float));
		}
	}

	bool Initialize(int InSampleRate = 48000, int InNumChannels = 4)
	{
		return InitializeWithDeviceId(nullptr, InSampleRate, InNumChannels);
	}

	bool InitializeWithDeviceId(const ma_device_id* pDeviceId, int InSampleRate = 48000, int InNumChannels = 4)
	{
		if (bInitialized)
		{
			Close();
		}

		if (pDeviceId)
		{
			DeviceId = *pDeviceId;
			bHasDeviceId = true;
		}
		else
		{
			bHasDeviceId = false;
		}

		SampleRate = InSampleRate;
		NumChannels = InNumChannels;
		ma_uint32 bufferSizeInFrames = static_cast<ma_uint32>(SampleRate);

		if (ma_pcm_rb_init(ma_format_f32, NumChannels, bufferSizeInFrames, nullptr, nullptr, &RingBuffer) != MA_SUCCESS)
		{
			return false;
		}
		bRingBufferInitialized = true;

		ma_device_config Config = ma_device_config_init(ma_device_type_playback);
		Config.playback.format = ma_format_f32;
		Config.playback.channels = NumChannels;
		Config.playback.pDeviceID = pDeviceId;
		Config.sampleRate = SampleRate;
		Config.dataCallback = DataCallback;
		Config.pUserData = this;
		// Config.periodSizeInMilliseconds = 10;

		if (ma_device_init(nullptr, &Config, &Device) != MA_SUCCESS)
		{
			ma_pcm_rb_uninit(&RingBuffer);
			bRingBufferInitialized = false;
			return false;
		}

		if (ma_device_start(&Device) != MA_SUCCESS)
		{
			ma_device_uninit(&Device);
			ma_pcm_rb_uninit(&RingBuffer);
			bRingBufferInitialized = false;
			return false;
		}

		bInitialized = true;
		return true;
	}

	void Close()
	{
		if (bInitialized)
		{
			ma_device_uninit(&Device);
			bInitialized = false;
		}
		if (bRingBufferInitialized)
		{
			ma_pcm_rb_uninit(&RingBuffer);
			bRingBufferInitialized = false;
		}
	}

	bool IsValid() const
	{
		return bInitialized && bRingBufferInitialized;
	}

	ma_uint32 GetAvailableWriteFrames()
	{
		if (!bRingBufferInitialized)
		{
			return 0;
		}
		return ma_pcm_rb_available_write(&RingBuffer);
	}

	bool WriteHapticData(const std::vector<std::int16_t>& InterleavedData)
	{
		if (!IsValid() || InterleavedData.empty())
		{
			return false;
		}
		ma_uint32 framesInput = static_cast<ma_uint32>(InterleavedData.size() / 2);

		ma_uint32 framesAvailable = ma_pcm_rb_available_write(&RingBuffer);
		ma_uint32 framesToWrite = (framesInput > framesAvailable) ? framesAvailable : framesInput;

		if (framesToWrite == 0)
		{
			return true;
		}

		void* pWriteBufferPtr;
		if (ma_pcm_rb_acquire_write(&RingBuffer, &framesToWrite, &pWriteBufferPtr) != MA_SUCCESS)
		{
			return false;
		}

		float* pOutputBuffer = static_cast<float*>(pWriteBufferPtr);
		constexpr float kNormalization = 1.0f / 32768.0f;
		for (ma_uint32 i = 0; i < framesToWrite; i++)
		{
			float LeftFloat = static_cast<float>(InterleavedData[i * 2]) * kNormalization;
			float RightFloat = static_cast<float>(InterleavedData[(i * 2) + 1]) * kNormalization;
			ma_uint32 baseIndex = i * NumChannels;

			if (NumChannels >= 4)
			{
				pOutputBuffer[baseIndex + 0] = 0.f;
				pOutputBuffer[baseIndex + 1] = 0.f;
				pOutputBuffer[baseIndex + 2] = LeftFloat;
				pOutputBuffer[baseIndex + 3] = RightFloat;
			}
			else
			{
				pOutputBuffer[baseIndex + 0] = LeftFloat;
				pOutputBuffer[baseIndex + 1] = RightFloat;
			}
		}

		ma_pcm_rb_commit_write(&RingBuffer, framesToWrite);
		return true;
	}

public:
	ma_device Device;
	ma_pcm_rb RingBuffer;
	ma_device_id DeviceId;
	int SampleRate = 48000;
	int NumChannels = 4;
	bool bInitialized = false;
	bool bRingBufferInitialized = false;
	bool bHasDeviceId = false;
};
