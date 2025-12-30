// Copyright (c) 2025 Rafael Valoto. All Rights Reserved.
// Project: GamepadCore
// Description: Cross-platform library for DualSense and generic gamepad input support.
// Targets: Windows, Linux, macOS.
#pragma once

#include <cstring>
#include <mutex>
#include <vector>

#include "GImplementations/Utils/GamepadAudio.h"
using namespace FGamepadAudio;

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

    /**
     * @brief Data callback for miniaudio - called when device needs more audio data.
     */
    static void DataCallback(ma_device* pDevice, void* pOutput, const void* /*pInput*/, ma_uint32 frameCount)
    {
        auto pContext = static_cast<FAudioDeviceContext*>(pDevice->pUserData);
        if (!pContext || !pContext->bInitialized)
        {
            std::memset(pOutput, 0, frameCount * pDevice->playback.channels * sizeof(float));
            return;
        }

        std::lock_guard<std::mutex> Lock(pContext->BufferMutex);

        ma_uint32 framesToRead = frameCount;
        ma_uint32 framesAvailable = ma_pcm_rb_available_read(&pContext->RingBuffer) / pContext->NumChannels;

        if (framesAvailable < framesToRead)
        {
            framesToRead = framesAvailable;
        }

        if (framesToRead > 0)
        {
            void* pReadBuffer;
            ma_uint32 readSize = framesToRead * pContext->NumChannels;
            ma_pcm_rb_acquire_read(&pContext->RingBuffer, &readSize, &pReadBuffer);
            std::memcpy(pOutput, pReadBuffer, readSize * sizeof(float));
            ma_pcm_rb_commit_read(&pContext->RingBuffer, readSize);
        }

        // Fill remaining with silence
        if (framesToRead < frameCount)
        {
            auto pOutputFloat = static_cast<float*>(pOutput);
            std::memset(&pOutputFloat[framesToRead * pContext->NumChannels], 0,
                       (frameCount - framesToRead) * pContext->NumChannels * sizeof(float));
        }
    }

    /**
     * @brief Initialize the audio device with specified parameters.
     * @param InSampleRate Sample rate in Hz (default: 48000)
     * @param InNumChannels Number of audio channels (default: 4 for DualSense haptics)
     * @return true if initialization succeeded
     */
    bool Initialize(int InSampleRate = 48000, int InNumChannels = 4)
    {
        return InitializeWithDeviceId(nullptr, InSampleRate, InNumChannels);
    }

    /**
     * @brief Initialize the audio device with a specific device ID.
     * @param pDeviceId Pointer to device ID (nullptr for default device)
     * @param InSampleRate Sample rate in Hz (default: 48000)
     * @param InNumChannels Number of audio channels (default: 4 for DualSense haptics)
     * @return true if initialization succeeded
     */
    bool InitializeWithDeviceId(const ma_device_id* pDeviceId, int InSampleRate = 48000, int InNumChannels = 4)
    {
        SampleRate = InSampleRate;
        NumChannels = InNumChannels;

        // Initialize ring buffer for audio data (1 second buffer)
        ma_uint32 bufferSizeInFrames = static_cast<ma_uint32>(SampleRate);
        if (ma_pcm_rb_init(ma_format_f32, NumChannels, bufferSizeInFrames, nullptr, nullptr, &RingBuffer) != MA_SUCCESS)
        {
            return false;
        }
        bRingBufferInitialized = true;

        // Configure device
        ma_device_config Config = ma_device_config_init(ma_device_type_playback);
        Config.playback.format = ma_format_f32;
        Config.playback.channels = NumChannels;
        Config.playback.pDeviceID = pDeviceId;  // Use specific device if provided
        Config.sampleRate = SampleRate;
        Config.dataCallback = DataCallback;
        Config.pUserData = this;
        Config.periodSizeInMilliseconds = 10; // Low latency for haptics

        // Initialize device
        if (ma_device_init(nullptr, &Config, &Device) != MA_SUCCESS)
        {
            ma_pcm_rb_uninit(&RingBuffer);
            bRingBufferInitialized = false;
            return false;
        }

        // Start playback
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

    /**
     * @brief Close and cleanup the audio device.
     */
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

    /**
     * @brief Check if the audio context is valid and ready.
     */
    bool IsValid()
    {
        return bInitialized && ma_device_is_started(&Device);
    }

    /**
     * @brief Get the buffer size in frames.
     */
    ma_uint32 GetBufferSize() const
    {
        return static_cast<ma_uint32>(SampleRate); // 1 second buffer
    }

    /**
     * @brief Get the current padding (frames written but not yet played).
     */
    ma_uint32 GetCurrentPadding()
    {
        if (!bRingBufferInitialized)
            return 0;
        return ma_pcm_rb_available_read(&RingBuffer) / NumChannels;
    }

    /**
     * @brief Get the number of frames available for writing.
     */
    ma_uint32 GetAvailableFrames()
    {
        return GetBufferSize() - GetCurrentPadding();
    }

    /**
     * @brief Write interleaved float audio data to the playback buffer.
     * @param AudioData Pointer to interleaved float samples
     * @param FrameCount Number of frames to write
     * @return true if write succeeded
     */
    bool WriteAudioData(const float* AudioData, ma_uint32 FrameCount)
    {
        if (!IsValid() || !AudioData || FrameCount == 0)
            return false;

        std::lock_guard<std::mutex> Lock(BufferMutex);

        void* pWriteBuffer;
        ma_uint32 writeSize = FrameCount * NumChannels;

        if (ma_pcm_rb_acquire_write(&RingBuffer, &writeSize, &pWriteBuffer) != MA_SUCCESS)
            return false;

        ma_uint32 actualFrames = writeSize / NumChannels;
        std::memcpy(pWriteBuffer, AudioData, actualFrames * NumChannels * sizeof(float));

        ma_pcm_rb_commit_write(&RingBuffer, actualFrames * NumChannels);

        return true;
    }

    /**
     * @brief Write stereo int16 audio data to haptic channels (channels 2 and 3).
     *
     * This method is designed for DualSense haptic feedback where:
     * - Channel 0, 1: Main audio (left/right speakers, set to 0)
     * - Channel 2: Haptic Left actuator
     * - Channel 3: Haptic Right actuator
     *
     * @param AudioData Vector of stereo int16 samples [left, right]
     * @return true if write succeeded
     */
    bool WriteHapticData(const std::vector<std::vector<std::int16_t>>& AudioData)
    {
        if (!IsValid() || AudioData.empty())
            return false;

        ma_uint32 FrameCount = static_cast<ma_uint32>(AudioData.size());
        ma_uint32 AvailableFrames = GetAvailableFrames();

        if (AvailableFrames == 0)
            return false;

        ma_uint32 FramesToWrite = (FrameCount < AvailableFrames) ? FrameCount : AvailableFrames;

        // Prepare interleaved buffer
        std::vector<float> InterleavedBuffer(FramesToWrite * NumChannels, 0.0f);

        for (ma_uint32 n = 0; n < FramesToWrite; n++)
        {
            float LeftFloat = static_cast<float>(AudioData[n][0]) / 32768.0f;
            float RightFloat = static_cast<float>(AudioData[n][1]) / 32768.0f;

            if (NumChannels >= 4)
            {
                // Haptic channels are typically 2 and 3
                InterleavedBuffer[n * NumChannels + 2] = LeftFloat;  // Haptic L
                InterleavedBuffer[n * NumChannels + 3] = RightFloat; // Haptic R
            }
            else if (NumChannels >= 2)
            {
                // Fallback to stereo
                InterleavedBuffer[n * NumChannels + 0] = LeftFloat;
                InterleavedBuffer[n * NumChannels + 1] = RightFloat;
            }
        }

        return WriteAudioData(InterleavedBuffer.data(), FramesToWrite);
    }

public:
    ma_device Device;
    ma_pcm_rb RingBuffer;
    std::mutex BufferMutex;
    int SampleRate = 48000;
    int NumChannels = 4;  // 4 channels for DualSense (stereo + haptic L/R)
    bool bInitialized = false;
    bool bRingBufferInitialized = false;
};