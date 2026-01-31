// Copyright (c) 2025 Rafael Valoto. All Rights Reserved.
// Project: GamepadCore
// Description: Integration test for Audio Haptics using a .wav file as input.
// Reference: Based on AudioHapticsListener implementation for USB/BT audio processing.

#ifdef BUILD_GAMEPAD_CORE_TESTS
#include <atomic>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <filesystem>
#include <iomanip>
#include <iostream>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>
namespace fs = std::filesystem;

// miniaudio for audio playback and WAV decoding
#include "GImplementations/Utils/GamepadAudio.h"
using namespace FGamepadAudio;

// Core Headers
#include "../../../Examples/Adapters/Tests/test_device_registry_policy.h"
#include "GCore/Interfaces/IPlatformHardwareInfo.h"
#include "GCore/Interfaces/Segregations/IGamepadAudioHaptics.h"
#include "GCore/Templates/TBasicDeviceRegistry.h"
#include "GCore/Types/Structs/Context/DeviceContext.h"

using TestDeviceRegistry = GamepadCore::TBasicDeviceRegistry<Ftest_device_registry_policy>;

#if _WIN32
#include "../../../Examples/Platform_Windows/test_windows_hardware_policy.h"
using TestHardwarePolicy = Ftest_windows_platform::Ftest_windows_hardware_policy;
using TestHardwareInfo = Ftest_windows_platform::Ftest_windows_hardware;
#elif __unix__
#include "../../../Examples/Platform_Linux/test_linux_hardware_policy.h"
using TestHardwarePolicy = Ftest_linux_platform::Ftest_linux_hardware_policy;
using TestHardwareInfo = Ftest_linux_platform::Ftest_linux_hardware;
#endif

// ============================================================================
// Audio Haptics Constants (Based on AudioHapticsListener)
// ============================================================================
constexpr float kLowPassAlpha = 1.0f;
constexpr float kOneMinusAlpha = 1.0f - kLowPassAlpha;

constexpr float kLowPassAlphaBt = 1.0f;
constexpr float kOneMinusAlphaBt = 1.0f - kLowPassAlphaBt;

// ============================================================================
// Thread-safe queue for audio packets
// ============================================================================
template<typename T>
class ThreadSafeQueue
{
public:
	void Push(const T& item)
	{
		std::lock_guard<std::mutex> lock(mMutex);
		mQueue.push(item);
	}

	bool Pop(T& item)
	{
		std::lock_guard<std::mutex> lock(mMutex);
		if (mQueue.empty())
		{
			return false;
		}
		item = mQueue.front();
		mQueue.pop();
		return true;
	}

	bool Empty()
	{
		std::lock_guard<std::mutex> lock(mMutex);
		return mQueue.empty();
	}

private:
	std::queue<T> mQueue;
	std::mutex mMutex;
};

// ============================================================================
// Global state for audio callback
// ============================================================================
struct AudioCallbackData
{
	ma_decoder* pDecoder = nullptr;
	bool bIsSystemAudio = false;
	float LowPassStateLeft = 0.0f;
	float LowPassStateRight = 0.0f;
	std::atomic<bool> bFinished{false};
	std::atomic<uint64_t> framesPlayed{0};
	bool bIsWireless = false;

	// Queues for haptics (like AudioHapticsListener)
	ThreadSafeQueue<std::vector<uint8_t>> btPacketQueue;
	ThreadSafeQueue<std::vector<int16_t>> usbSampleQueue;

	// Accumulator for Bluetooth - need 1024 frames to produce 64 resampled frames
	std::vector<float> btAccumulator;
	std::mutex btAccumulatorMutex;
};

// Audio callback - plays audio on speakers and queues haptics data
void AudioDataCallback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount)
{
	auto* pData = static_cast<AudioCallbackData*>(pDevice->pUserData);
	if (!pData)
	{
		return;
	}

	std::vector<float> tempBuffer(frameCount * 2);
	ma_uint64 framesRead = 0;

	if (pData->bIsSystemAudio)
	{
		// Capture from system audio (loopback)
		if (pInput == nullptr)
		{
			return;
		}

		// miniaudio already provides the captured audio in pInput
		const float* pInputFloat = static_cast<const float*>(pInput);
		std::memcpy(tempBuffer.data(), pInputFloat, frameCount * 2 * sizeof(float));
		framesRead = frameCount;

		// If we are in duplex mode or playback, we might want to copy to pOutput to hear it
		// But usually loopback capture is enough.
		if (pOutput)
		{
			std::memcpy(pOutput, pInput, frameCount * 2 * sizeof(float));
		}
	}
	else
	{
		// Read from decoder
		if (!pData->pDecoder)
		{
			if (pOutput)
			{
				std::memset(pOutput, 0, frameCount * pDevice->playback.channels * ma_get_bytes_per_sample(pDevice->playback.format));
			}
			return;
		}

		ma_result result = ma_decoder_read_pcm_frames(pData->pDecoder, tempBuffer.data(), frameCount, &framesRead);

		if (result != MA_SUCCESS || framesRead == 0)
		{
			pData->bFinished = true;
			if (pOutput)
			{
				std::memset(pOutput, 0, frameCount * pDevice->playback.channels * ma_get_bytes_per_sample(pDevice->playback.format));
			}
			return;
		}

		// Copy to output (for speakers) - audio plays at 48kHz
		if (pOutput)
		{
			auto* pOutputFloat = static_cast<float*>(pOutput);
			std::memcpy(pOutputFloat, tempBuffer.data(), framesRead * 2 * sizeof(float));

			if (framesRead < frameCount)
			{
				std::memset(&pOutputFloat[framesRead * 2], 0, (frameCount - framesRead) * 2 * sizeof(float));
			}
		}
	}

	// Process for haptics
	if (!pData->bIsWireless)
	{
		// USB: Queue 16-bit stereo samples with high-pass filter
		for (ma_uint64 i = 0; i < framesRead; ++i)
		{
			float inLeft = tempBuffer[i * 2];
			float inRight = tempBuffer[i * 2 + 1];

			pData->LowPassStateLeft = kOneMinusAlpha * inLeft + kLowPassAlpha * pData->LowPassStateLeft;
			pData->LowPassStateRight = kOneMinusAlpha * inRight + kLowPassAlpha * pData->LowPassStateRight;

			float outLeft = std::clamp(inLeft - pData->LowPassStateLeft, -1.0f, 1.0f);
			float outRight = std::clamp(inRight - pData->LowPassStateRight, -1.0f, 1.0f);

			std::vector<int16_t> stereoSample = {
			    static_cast<int16_t>(outLeft * 32767.0f),
			    static_cast<int16_t>(outRight * 32767.0f)};
			pData->usbSampleQueue.Push(stereoSample);
		}
	}
	else
	{
		// Bluetooth: Need to accumulate 1024 input frames to get 64 output frames at 3000Hz
		// 1024 frames at 48kHz * (3000/48000) = 64 frames at 3000Hz

		// Add current frames to accumulator
		for (ma_uint64 i = 0; i < framesRead; ++i)
		{
			pData->btAccumulator.push_back(tempBuffer[i * 2]);     // Left
			pData->btAccumulator.push_back(tempBuffer[i * 2 + 1]); // Right
		}

		// Process when we have at least 1024 frames (2048 samples)
		const size_t requiredSamples = 1024 * 2; // 1024 frames * 2 channels

		while (true)
		{
			std::vector<float> framesToProcess;

			{
				std::lock_guard<std::mutex> lock(pData->btAccumulatorMutex);
				if (pData->btAccumulator.size() < requiredSamples)
				{
					break; // Not enough data yet, exit loop but continue to update framesPlayed
				}

				// Extract 1024 frames from accumulator
				framesToProcess.assign(pData->btAccumulator.begin(), pData->btAccumulator.begin() + requiredSamples);
				pData->btAccumulator.erase(pData->btAccumulator.begin(), pData->btAccumulator.begin() + requiredSamples);
			}

			// Now resample 1024 frames to 64 frames
			const float ratio = 3000.0f / 48000.0f; // 0.0625
			const std::int32_t numInputFrames = 1024;

			std::vector<float> resampledData(128, 0.0f); // 64 frames * 2 channels

			for (std::int32_t outFrame = 0; outFrame < 64; ++outFrame)
			{
				float srcPos = static_cast<float>(outFrame) / ratio;
				std::int32_t srcIndex = static_cast<std::int32_t>(srcPos);
				float frac = srcPos - static_cast<float>(srcIndex);

				if (srcIndex >= numInputFrames - 1)
				{
					srcIndex = numInputFrames - 2;
					frac = 1.0f;
				}
				if (srcIndex < 0)
				{
					srcIndex = 0;
				}

				float left0 = framesToProcess[srcIndex * 2];
				float left1 = framesToProcess[(srcIndex + 1) * 2];
				float right0 = framesToProcess[srcIndex * 2 + 1];
				float right1 = framesToProcess[(srcIndex + 1) * 2 + 1];

				resampledData[outFrame * 2] = left0 + frac * (left1 - left0);
				resampledData[outFrame * 2 + 1] = right0 + frac * (right1 - right0);
			}

			// Apply high-pass filter to all 64 frames
			for (std::int32_t i = 0; i < 64; ++i)
			{
				const std::int32_t dataIndex = i * 2;

				float inLeft = resampledData[dataIndex];
				float inRight = resampledData[dataIndex + 1];

				pData->LowPassStateLeft = kOneMinusAlphaBt * inLeft + kLowPassAlphaBt * pData->LowPassStateLeft;
				pData->LowPassStateRight = kOneMinusAlphaBt * inRight + kLowPassAlphaBt * pData->LowPassStateRight;

				resampledData[dataIndex] = inLeft - pData->LowPassStateLeft;
				resampledData[dataIndex + 1] = inRight - pData->LowPassStateRight;
			}

			// Create Packet1: Frames 0-31 (64 bytes)
			std::vector<std::int8_t> packet1(64, 0);

			for (std::int32_t i = 0; i < 32; ++i)
			{
				const std::int32_t dataIndex = i * 2;

				float leftSample = resampledData[dataIndex];
				float rightSample = resampledData[dataIndex + 1];

				std::int8_t leftInt8 = static_cast<std::int8_t>(std::clamp(static_cast<int>(std::round(leftSample * 127.0f)), -128, 127));
				std::int8_t rightInt8 = static_cast<std::int8_t>(std::clamp(static_cast<int>(std::round(rightSample * 127.0f)), -128, 127));

				packet1[dataIndex] = leftInt8;
				packet1[dataIndex + 1] = rightInt8;
			}

			// Create Packet2: Frames 32-63 (64 bytes)
			std::vector<std::int8_t> packet2(64, 0);
			for (std::int32_t i = 0; i < 32; ++i)
			{
				const std::int32_t dataIndex = (i + 32) * 2;

				float leftSample = resampledData[dataIndex];
				float rightSample = resampledData[dataIndex + 1];

				std::int8_t leftInt8 = static_cast<std::int8_t>(std::clamp(static_cast<int>(std::round(leftSample * 127.0f)), -128, 127));
				std::int8_t rightInt8 = static_cast<std::int8_t>(std::clamp(static_cast<int>(std::round(rightSample * 127.0f)), -128, 127));

				const std::int32_t packetIndex = i * 2;
				packet2[packetIndex] = leftInt8;
				packet2[packetIndex + 1] = rightInt8;
			}

			// Convert to uint8 and enqueue
			std::vector<std::uint8_t> packet1Unsigned(packet1.begin(), packet1.end());
			std::vector<std::uint8_t> packet2Unsigned(packet2.begin(), packet2.end());

			pData->btPacketQueue.Push(packet1Unsigned);
			pData->btPacketQueue.Push(packet2Unsigned);
		}
	}

	pData->framesPlayed += framesRead;
}

// Consume haptics queue and send to controller
void ConsumeHapticsQueue(IGamepadAudioHaptics* AudioHaptics, AudioCallbackData& callbackData)
{

	if (callbackData.bIsWireless)
	{
		std::vector<std::uint8_t> packet;
		while (callbackData.btPacketQueue.Pop(packet))
		{
			AudioHaptics->AudioHapticUpdate(packet);
		}
	}
	else
	{
		std::vector<std::int16_t> allSamples;
		allSamples.reserve(2048 * 2);

		std::vector<std::int16_t> stereoSample;
		while (callbackData.usbSampleQueue.Pop(stereoSample))
		{
			if (stereoSample.size() >= 2)
			{
				allSamples.push_back(stereoSample[0]);
				allSamples.push_back(stereoSample[1]);
			}
		}

		if (!allSamples.empty())
		{
			AudioHaptics->AudioHapticUpdate(allSamples);
		}
	}
}

// ============================================================================
// Gamepad Audio Worker - Manages audio/haptics for a single controller
// ============================================================================
class GamepadAudioWorker
{
public:
	GamepadAudioWorker(ISonyGamepad* InGamepad, const std::string& InWavPath, bool InUseSystemAudio)
	    : Gamepad(InGamepad)
	    , WavFilePath(InWavPath)
	    , bUseSystemAudio(InUseSystemAudio)
	{
		bFinished.store(false);
	}

	~GamepadAudioWorker()
	{
		Stop();
	}

	void Start()
	{
		WorkerThread = std::thread(&GamepadAudioWorker::Run, this);
	}

	void Stop()
	{
		bFinished.store(true);
		if (WorkerThread.joinable())
		{
			WorkerThread.join();
		}
	}

	bool IsFinished() const { return bFinished.load(); }

private:
	void Run()
	{
		if (!Gamepad)
		{
			return;
		}

		int32_t DeviceId = -1; // We don't have the engine ID here easily, but we have the gamepad pointer

		std::cout << "[Worker] Starting audio worker for controller..." << std::endl;

		bool bIsWireless = Gamepad->GetConnectionType() == EDSDeviceConnection::Bluetooth;

		// Get Audio Haptics interface
		IGamepadAudioHaptics* AudioHaptics = Gamepad->GetIGamepadHaptics();
		if (!AudioHaptics)
		{
			std::cerr << "[Worker Error] Audio haptics interface not available." << std::endl;
			return;
		}

		// Initialize AudioContext for USB haptics
		FDeviceContext* Context = Gamepad->GetMutableDeviceContext();
		if (!bIsWireless && Context)
		{
			if (!Context->AudioContext || !Context->AudioContext->IsValid())
			{
				IPlatformHardwareInfo::Get().InitializeAudioDevice(Context);
			}
		}

		ma_decoder decoder;
		ma_uint64 totalFrames = 0;
		bool bDecoderInitialized = false;

		if (!bUseSystemAudio)
		{
			fs::path p(WavFilePath);
			if (!fs::exists(p))
			{
				fs::path alternativePath = fs::path(GAMEPAD_CORE_PROJECT_ROOT) / WavFilePath;
				if (fs::exists(alternativePath))
				{
					WavFilePath = alternativePath.string();
					std::cout << "[Worker] Resolved path to: " << WavFilePath << std::endl;
				}
			}

			ma_decoder_config decoderConfig = ma_decoder_config_init(ma_format_f32, 2, 48000);
			if (ma_decoder_init_file(WavFilePath.c_str(), &decoderConfig, &decoder) == MA_SUCCESS)
			{
				ma_decoder_get_length_in_pcm_frames(&decoder, &totalFrames);
				bDecoderInitialized = true;
			}
			else
			{
				std::cerr << "[Worker Error] Failed to load WAV file: " << WavFilePath << std::endl;
				return;
			}
		}

		// Setup callback data
		AudioCallbackData callbackData;
		callbackData.pDecoder = bDecoderInitialized ? &decoder : nullptr;
		callbackData.bIsSystemAudio = bUseSystemAudio;
		callbackData.bIsWireless = bIsWireless;

		// Initialize playback device
		ma_device_config deviceConfig;
		if (bUseSystemAudio)
		{
			deviceConfig = ma_device_config_init(ma_device_type_loopback);
			deviceConfig.capture.format = ma_format_f32;
			deviceConfig.capture.channels = 2;
			deviceConfig.wasapi.loopbackProcessID = 0;
		}
		else
		{
			deviceConfig = ma_device_config_init(ma_device_type_playback);
			deviceConfig.playback.format = ma_format_f32;
			deviceConfig.playback.channels = 2;
		}

		deviceConfig.sampleRate = 48000;
		deviceConfig.dataCallback = AudioDataCallback;
		deviceConfig.pUserData = &callbackData;

		ma_device device;
		if (ma_device_init(nullptr, &deviceConfig, &device) != MA_SUCCESS)
		{
			std::cerr << "[Worker Error] Failed to initialize audio device." << std::endl;
			if (bDecoderInitialized)
			{
				ma_decoder_uninit(&decoder);
			}
			return;
		}

		if (ma_device_start(&device) != MA_SUCCESS)
		{
			std::cerr << "[Worker Error] Failed to start audio device." << std::endl;
			ma_device_uninit(&device);
			if (bDecoderInitialized)
			{
				ma_decoder_uninit(&decoder);
			}
			return;
		}

		// Main loop for this controller
		while (!callbackData.bFinished && !bFinished.load() && Gamepad->IsConnected())
		{
			ConsumeHapticsQueue(AudioHaptics, callbackData);
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
		}

		// Cleanup
		ma_device_uninit(&device);
		if (bDecoderInitialized)
		{
			ma_decoder_uninit(&decoder);
		}

		if (Gamepad->IsConnected())
		{
			Gamepad->SetLightbar({0, 255, 0});
			Gamepad->UpdateOutput();
		}

		std::cout << "[Worker] Audio worker finished." << std::endl;
		bFinished.store(true);
	}

	ISonyGamepad* Gamepad;
	std::string WavFilePath;
	bool bUseSystemAudio;
	std::atomic<bool> bFinished;
	std::thread WorkerThread;
};

// Custom policy to notify about new gamepads
struct FAudioTestRegistryPolicy : public Ftest_device_registry_policy
{
	std::vector<int32_t> NewGamepads;
	std::mutex NewGamepadsMutex;

	void DispatchNewGamepad(EngineIdType GamepadId)
	{
		std::lock_guard<std::mutex> Lock(NewGamepadsMutex);
		NewGamepads.push_back(GamepadId);
		std::cout << "[Policy] New Gamepad Registered: " << GamepadId << std::endl;
	}
};

using AudioTestDeviceRegistry = GamepadCore::TBasicDeviceRegistry<FAudioTestRegistryPolicy>;

// ============================================================================
// Helper Functions
// ============================================================================
void PrintHelp()
{
	std::cout << "\n=======================================================" << std::endl;
	std::cout << "        AUDIO HAPTICS INTEGRATION TEST                 " << std::endl;
	std::cout << "=======================================================" << std::endl;
	std::cout << " Usage: AudioHapticsTest <wav_file_path>" << std::endl;
	std::cout << "" << std::endl;
	std::cout << " This test plays a WAV file on your speakers" << std::endl;
	std::cout << " and simultaneously sends haptic feedback to" << std::endl;
	std::cout << " your DualSense controller." << std::endl;
	std::cout << "" << std::endl;
	std::cout << " Supports both USB and Bluetooth!" << std::endl;
	std::cout << " - USB: 48kHz haptics via audio device" << std::endl;
	std::cout << " - Bluetooth: 3000Hz haptics via HID" << std::endl;
	std::cout << "=======================================================" << std::endl;
}

// ============================================================================
// Main Entry Point
// ============================================================================
int main(int argc, char* argv[])
{
	std::string WavFilePath;
	bool bUseSystemAudio = false;

	if (argc < 2)
	{
#ifdef AUTOMATED_TESTS
		WavFilePath = std::string(GAMEPAD_CORE_PROJECT_ROOT) + "/Tests/Integration/Datasets/ES_Touch_SCENE.wav";
		bUseSystemAudio = false;
		std::cout << "[Test] Automated mode: Forcing audio file: " << WavFilePath << std::endl;
#else
		bUseSystemAudio = true;
		std::cout << "[System] No WAV file provided. Using System Audio Loopback." << std::endl;
#endif
	}
	else
	{
		WavFilePath = argv[1];
	}

	std::cout << "[System] Audio Haptics Integration Test" << std::endl;

	ma_decoder decoder;
	ma_uint64 totalFrames = 0;

	if (!bUseSystemAudio)
	{
		fs::path p(WavFilePath);
		if (!fs::exists(p))
		{
			fs::path alternativePath = fs::path(GAMEPAD_CORE_PROJECT_ROOT) / WavFilePath;
			if (fs::exists(alternativePath))
			{
				WavFilePath = alternativePath.string();
				std::cout << "[System] Resolved path to: " << WavFilePath << std::endl;
			}
		}

		std::cout << "[System] Loading WAV file: " << WavFilePath << std::endl;

		// Initialize decoder (output as float, stereo, 48kHz)
		ma_decoder_config decoderConfig = ma_decoder_config_init(ma_format_f32, 2, 48000);

		if (ma_decoder_init_file(WavFilePath.c_str(), &decoderConfig, &decoder) != MA_SUCCESS)
		{
			std::cerr << "[Error] Failed to load WAV file: " << WavFilePath << std::endl;
			return 1;
		}

		ma_decoder_get_length_in_pcm_frames(&decoder, &totalFrames);

		std::cout << "[WavReader] Loaded WAV file successfully:" << std::endl;
		std::cout << "  - Sample Rate: " << decoder.outputSampleRate << " Hz" << std::endl;
		std::cout << "  - Channels: " << decoder.outputChannels << std::endl;
		std::cout << "  - Total Frames: " << totalFrames << std::endl;
		std::cout << "  - Duration: " << (static_cast<float>(totalFrames) / decoder.outputSampleRate) << " seconds" << std::endl;
	}
	else
	{
		std::cout << "[System] Mode: System Audio Capture (Press Ctrl+C to stop)" << std::endl;
	}

	// Initialize Hardware Layer
	std::cout << "[System] Initializing Hardware Layer..." << std::endl;
	auto HardwareImpl = std::make_unique<TestHardwareInfo>();
	IPlatformHardwareInfo::SetInstance(std::move(HardwareImpl));

	// Initialize Registry
	auto Registry = std::make_unique<AudioTestDeviceRegistry>();

	std::cout << "[System] Waiting for controller connection via USB/BT..." << std::endl;
	std::cout << "[System] Press Ctrl+C to stop." << std::endl;

	std::unordered_map<int32_t, std::unique_ptr<GamepadAudioWorker>> ActiveWorkers;

	while (true)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(16));
		float DeltaTime = 0.016f;

		Registry->PlugAndPlay(DeltaTime);

		// Check for new gamepads from policy
		{
			std::lock_guard<std::mutex> Lock(Registry->Policy.NewGamepadsMutex);
			for (int32_t GamepadId : Registry->Policy.NewGamepads)
			{
				ISonyGamepad* Gamepad = Registry->GetLibrary(GamepadId);
				if (Gamepad)
				{
					if (GamepadId == 1)
					{
						Gamepad->SetLightbar({200, 255, 0});
						Gamepad->SetPlayerLed(EDSPlayer::Two, 0xff);
						Gamepad->DualSenseSettings(0, 0, 0, 0, 0, 0xFC, 0, 0);
						Gamepad->UpdateOutput();
						std::this_thread::sleep_for(std::chrono::seconds(1));
					}
					else if (GamepadId == 0)
					{
						Gamepad->SetLightbar({0, 255, 255});
						Gamepad->SetPlayerLed(EDSPlayer::One, 0xff);
						Gamepad->DualSenseSettings(0, 0, 0, 0, 0, 0xFC, 0, 0);
						Gamepad->UpdateOutput();
						std::this_thread::sleep_for(std::chrono::seconds(1));
					}

					std::cout << "[System] Creating worker for GamepadId: " << GamepadId << std::endl;
					auto Worker = std::make_unique<GamepadAudioWorker>(Gamepad, WavFilePath, bUseSystemAudio);
					Worker->Start();
					ActiveWorkers[GamepadId] = std::move(Worker);
				}
			}
			Registry->Policy.NewGamepads.clear();
		}

		// Cleanup finished or disconnected workers
		for (auto it = ActiveWorkers.begin(); it != ActiveWorkers.end();)
		{
			bool bIsConnected = false;
			ISonyGamepad* Gamepad = Registry->GetLibrary(it->first);
			if (Gamepad && Gamepad->IsConnected())
			{
				bIsConnected = true;
			}

			if (it->second->IsFinished() || !bIsConnected)
			{
				std::cout << "[System] Removing worker for GamepadId: " << it->first << std::endl;
				it = ActiveWorkers.erase(it);
			}
			else
			{
				++it;
			}
		}

#ifdef AUTOMATED_TESTS
		static auto StartTime = std::chrono::steady_clock::now();
		auto Now = std::chrono::steady_clock::now();
		if (std::chrono::duration_cast<std::chrono::seconds>(Now - StartTime).count() >= 10)
		{
			std::cout << "[Test] Automated timeout reached. Exiting." << std::endl;
			break;
		}
#endif
	}

	return 0;
}
#endif
