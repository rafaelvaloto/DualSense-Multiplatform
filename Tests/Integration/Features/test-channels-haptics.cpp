// Copyright (c) 2025 Rafael Valoto. All Rights Reserved.
// Project: GamepadCore
// Description: Integration test for Audio Haptics using different .wav files for different controllers.

#ifdef BUILD_GAMEPAD_CORE_TESTS
#include <atomic>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <filesystem>
#include <iomanip>
#include <iostream>
#include <map>
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
		if (pInput == nullptr)
		{
			return;
		}

		const float* pInputFloat = static_cast<const float*>(pInput);
		std::memcpy(tempBuffer.data(), pInputFloat, frameCount * 2 * sizeof(float));
		framesRead = frameCount;

		if (pOutput)
		{
			std::memcpy(pOutput, pInput, frameCount * 2 * sizeof(float));
		}
	}
	else
	{
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

	if (!pData->bIsWireless)
	{
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
		for (ma_uint64 i = 0; i < framesRead; ++i)
		{
			pData->btAccumulator.push_back(tempBuffer[i * 2]);     // Left
			pData->btAccumulator.push_back(tempBuffer[i * 2 + 1]); // Right
		}

		const size_t requiredSamples = 1024 * 2;

		while (true)
		{
			std::vector<float> framesToProcess;

			{
				std::lock_guard<std::mutex> lock(pData->btAccumulatorMutex);
				if (pData->btAccumulator.size() < requiredSamples)
				{
					break;
				}

				framesToProcess.assign(pData->btAccumulator.begin(), pData->btAccumulator.begin() + requiredSamples);
				pData->btAccumulator.erase(pData->btAccumulator.begin(), pData->btAccumulator.begin() + requiredSamples);
			}

			const float ratio = 3000.0f / 48000.0f;
			const std::int32_t numInputFrames = 1024;

			std::vector<float> resampledData(128, 0.0f);

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

			std::vector<std::int8_t> packet1(64, 0);
			for (std::int32_t i = 0; i < 32; ++i)
			{
				const std::int32_t dataIndex = i * 2;
				float leftSample = resampledData[dataIndex];
				float rightSample = resampledData[dataIndex + 1];
				packet1[dataIndex] = static_cast<std::int8_t>(std::clamp(static_cast<int>(std::round(leftSample * 127.0f)), -128, 127));
				packet1[dataIndex + 1] = static_cast<std::int8_t>(std::clamp(static_cast<int>(std::round(rightSample * 127.0f)), -128, 127));
			}

			std::vector<std::int8_t> packet2(64, 0);
			for (std::int32_t i = 0; i < 32; ++i)
			{
				const std::int32_t dataIndex = (i + 32) * 2;
				float leftSample = resampledData[dataIndex];
				float rightSample = resampledData[dataIndex + 1];
				packet2[i * 2] = static_cast<std::int8_t>(std::clamp(static_cast<int>(std::round(leftSample * 127.0f)), -128, 127));
				packet2[i * 2 + 1] = static_cast<std::int8_t>(std::clamp(static_cast<int>(std::round(rightSample * 127.0f)), -128, 127));
			}

			std::vector<std::uint8_t> packet1Unsigned(packet1.begin(), packet1.end());
			std::vector<std::uint8_t> packet2Unsigned(packet2.begin(), packet2.end());

			pData->btPacketQueue.Push(packet1Unsigned);
			pData->btPacketQueue.Push(packet2Unsigned);
		}
	}

	pData->framesPlayed += framesRead;
}

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

		std::vector<int16_t> stereoSample;
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

	~GamepadAudioWorker() { Stop(); }

	void Start() { WorkerThread = std::thread(&GamepadAudioWorker::Run, this); }

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

		std::cout << "[Worker] Starting audio worker for controller (File: " << (bUseSystemAudio ? "System Audio" : WavFilePath) << ")..." << std::endl;

		bool bIsWireless = Gamepad->GetConnectionType() == EDSDeviceConnection::Bluetooth;
		IGamepadAudioHaptics* AudioHaptics = Gamepad->GetIGamepadHaptics();
		if (!AudioHaptics)
		{
			return;
		}

		FDeviceContext* Context = Gamepad->GetMutableDeviceContext();
		if (!bIsWireless && Context)
		{
			if (!Context->AudioContext || !Context->AudioContext->IsValid())
			{
				IPlatformHardwareInfo::Get().InitializeAudioDevice(Context);
			}
		}

		ma_decoder decoder;
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
				bDecoderInitialized = true;
			}
			else
			{
				std::cerr << "[Worker Error] Failed to load WAV file: " << WavFilePath << std::endl;
				return;
			}
		}

		AudioCallbackData callbackData;
		callbackData.pDecoder = bDecoderInitialized ? &decoder : nullptr;
		callbackData.bIsSystemAudio = bUseSystemAudio;
		callbackData.bIsWireless = bIsWireless;

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
			if (bDecoderInitialized)
			{
				ma_decoder_uninit(&decoder);
			}
			return;
		}

		if (ma_device_start(&device) != MA_SUCCESS)
		{
			ma_device_uninit(&device);
			if (bDecoderInitialized)
			{
				ma_decoder_uninit(&decoder);
			}
			return;
		}

		while (!callbackData.bFinished && !bFinished.load() && Gamepad->IsConnected())
		{
			ConsumeHapticsQueue(AudioHaptics, callbackData);
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
		}

		ma_device_uninit(&device);
		if (bDecoderInitialized)
		{
			ma_decoder_uninit(&decoder);
		}
		bFinished.store(true);
	}

	ISonyGamepad* Gamepad;
	std::string WavFilePath;
	bool bUseSystemAudio;
	std::atomic<bool> bFinished;
	std::thread WorkerThread;
};

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

void PrintHelp()
{
	std::cout << "\n=======================================================" << std::endl;
	std::cout << "        CHANNELS HAPTICS INTEGRATION TEST              " << std::endl;
	std::cout << "=======================================================" << std::endl;
	std::cout << " Usage: test-channels-haptics <wav1> <wav2> ... <wavN>" << std::endl;
	std::cout << "" << std::endl;
	std::cout << " Each argument is assigned to a controller based on its order" << std::endl;
	std::cout << " Example: test-channels-haptics drum.wav bass.wav" << std::endl;
	std::cout << "   - Controller 0: drum.wav" << std::endl;
	std::cout << "   - Controller 1: bass.wav" << std::endl;
	std::cout << "=======================================================" << std::endl;
}

int main(int argc, char* argv[])
{
	std::vector<std::string> WavFiles;
	bool bUseSystemAudio = false;

	if (argc < 2)
	{
#ifdef AUTOMATED_TESTS
		WavFiles.push_back(std::string(GAMEPAD_CORE_PROJECT_ROOT) + "/Tests/Integration/Datasets/ES_Touch_SCENE.wav");
		std::cout << "[Test] Automated mode: Using default file." << std::endl;
#else
		PrintHelp();
		bUseSystemAudio = true;
		std::cout << "[System] No WAV files provided. Using System Audio Loopback for all." << std::endl;
#endif
	}
	else
	{
		for (int i = 1; i < argc; ++i)
		{
			WavFiles.push_back(argv[i]);
		}
	}

	std::cout << "[System] Initializing Hardware..." << std::endl;
	IPlatformHardwareInfo::SetInstance(std::make_unique<TestHardwareInfo>());
	auto Registry = std::make_unique<AudioTestDeviceRegistry>();

	std::unordered_map<int32_t, std::unique_ptr<GamepadAudioWorker>> ActiveWorkers;

	while (true)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(16));
		Registry->PlugAndPlay(0.016f);

		{
			std::lock_guard<std::mutex> Lock(Registry->Policy.NewGamepadsMutex);
			for (int32_t GamepadId : Registry->Policy.NewGamepads)
			{
				ISonyGamepad* Gamepad = Registry->GetLibrary(GamepadId);
				if (GamepadId == 1 && Gamepad)
				{
					Gamepad->SetLightbar({200, 255, 0});
					Gamepad->SetPlayerLed(EDSPlayer::Two, 0xff);
					Gamepad->DualSenseSettings(0, 0, 1, 0, 0xff, 0xFC, 0, 0);
					Gamepad->UpdateOutput();
					std::this_thread::sleep_for(std::chrono::seconds(1));
				}
				else if (GamepadId == 0 && Gamepad)
				{
					Gamepad->SetLightbar({0, 255, 255});
					Gamepad->SetPlayerLed(EDSPlayer::One, 0xff);
					Gamepad->DualSenseSettings(0, 0, 1, 0, 0xff, 0xFC, 0, 0);
					Gamepad->UpdateOutput();
					std::this_thread::sleep_for(std::chrono::seconds(1));
				}

				if (Gamepad)
				{
					std::string SelectedWav;
					bool bLocalUseSystem = bUseSystemAudio;

					if (!bUseSystemAudio)
					{
						if (GamepadId < WavFiles.size())
						{
							SelectedWav = WavFiles[GamepadId];
						}
						else
						{
							SelectedWav = WavFiles.back();
							std::cout << "[Warning] No specific WAV for GamepadId " << GamepadId << ". Using last: " << SelectedWav << std::endl;
						}
					}

					auto Worker = std::make_unique<GamepadAudioWorker>(Gamepad, SelectedWav, bLocalUseSystem);
					Worker->Start();
					ActiveWorkers[GamepadId] = std::move(Worker);
				}
			}
			Registry->Policy.NewGamepads.clear();
		}

		for (auto it = ActiveWorkers.begin(); it != ActiveWorkers.end();)
		{
			ISonyGamepad* Gamepad = Registry->GetLibrary(it->first);
			if (it->second->IsFinished() || !Gamepad || !Gamepad->IsConnected())
			{
				it = ActiveWorkers.erase(it);
			}
			else
			{
				++it;
			}
		}

#ifdef AUTOMATED_TESTS
		static auto StartTime = std::chrono::steady_clock::now();
		if (std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() - StartTime).count() >= 10)
		{
			break;
		}
#endif
	}

	return 0;
}
#endif
