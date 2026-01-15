#if defined(_WIN32) && defined(USE_VIGEM)

#include <windows.h>
#include <iostream>
#include <thread>
#include <atomic>
#include <vector>
#include <memory>
#include <string>
#include <mutex>
#include <queue>
#include <map>

#include "GImplementations/Utils/GamepadAudio.h"
using namespace FGamepadAudio;

#include "GCore/Interfaces/IPlatformHardwareInfo.h"
#include "GCore/Templates/TGenericHardwareInfo.h"
#include "GCore/Templates/TBasicDeviceRegistry.h"
#include "GCore/Types/Structs/Context/DeviceContext.h"
#include "../Adapters/Tests/test_device_registry_policy.h"

#ifdef USE_VIGEM
#include "../Platform_Windows/ViGEmAdapter/ViGEmAdapter.h"
#endif

#if _WIN32
#include "../Platform_Windows/test_windows_hardware_policy.h"
using TestHardwarePolicy = Ftest_windows_platform::Ftest_windows_hardware_policy;
using TestHardwareInfo = Ftest_windows_platform::Ftest_windows_hardware;
#endif

using namespace GamepadCore;
using TestDeviceRegistry = GamepadCore::TBasicDeviceRegistry<Ftest_device_registry_policy>;

std::atomic<bool> g_Running(false);
std::atomic<bool> g_ServiceInitialized(false);
std::thread g_ServiceThread;
std::thread g_AudioThread;
std::unique_ptr<TestDeviceRegistry> g_Registry;
#ifdef USE_VIGEM
std::unique_ptr<ViGEmAdapter> g_ViGEmAdapter;
#endif
FInputContext g_LastInputState;
const int32_t TargetDeviceId = 0;

constexpr float kLowPassAlpha = 0.99f;
constexpr float kOneMinusAlpha = 1.0f - kLowPassAlpha;

constexpr float kLowPassAlphaBt = 0.98f;
constexpr float kOneMinusAlphaBt = 1.0f - kLowPassAlphaBt;

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

struct AudioCallbackData
{
	ma_decoder* pDecoder = nullptr;
	bool bIsSystemAudio = false;
	float LowPassStateLeft = 0.0f;
	float LowPassStateRight = 0.0f;
	std::atomic<bool> bFinished{false};
	std::atomic<uint64_t> framesPlayed{0};
	bool bIsWireless = false;

	ThreadSafeQueue<std::vector<uint8_t>> btPacketQueue;
	ThreadSafeQueue<std::vector<int16_t>> usbSampleQueue;

	std::vector<float> btAccumulator;
	std::mutex btAccumulatorMutex;
};

void AudioDataCallback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount)
{
	auto* pData = static_cast<AudioCallbackData*>(pDevice->pUserData);
	if (!pData) return;

	std::vector<float> tempBuffer(frameCount * 2);
	ma_uint64 framesRead = 0;

	if (pData->bIsSystemAudio)
	{
		if (pInput == nullptr) return;
		auto pInputFloat = static_cast<const float*>(pInput);
		std::memcpy(tempBuffer.data(), pInputFloat, frameCount * 2 * sizeof(float));
		framesRead = frameCount;
		if (pOutput) std::memcpy(pOutput, pInput, frameCount * 2 * sizeof(float));
	}
	else
	{
		if (!pData->pDecoder)
		{
			if (pOutput) std::memset(pOutput, 0, frameCount * pDevice->playback.channels * ma_get_bytes_per_sample(pDevice->playback.format));
			return;
		}
		ma_result result = ma_decoder_read_pcm_frames(pData->pDecoder, tempBuffer.data(), frameCount, &framesRead);
		if (result != MA_SUCCESS || framesRead == 0)
		{
			pData->bFinished = true;
			if (pOutput) std::memset(pOutput, 0, frameCount * pDevice->playback.channels * ma_get_bytes_per_sample(pDevice->playback.format));
			return;
		}
		if (pOutput)
		{
			auto* pOutputFloat = static_cast<float*>(pOutput);
			std::memcpy(pOutputFloat, tempBuffer.data(), framesRead * 2 * sizeof(float));
			if (framesRead < frameCount) std::memset(&pOutputFloat[framesRead * 2], 0, (frameCount - framesRead) * 2 * sizeof(float));
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
			std::vector<int16_t> stereoSample = {static_cast<int16_t>(outLeft * 32767.0f), static_cast<int16_t>(outRight * 32767.0f)};
			pData->usbSampleQueue.Push(stereoSample);
		}
	}
	else
	{
		{
			std::lock_guard<std::mutex> lock(pData->btAccumulatorMutex);
			if (pData->btAccumulator.size() > 48000) pData->btAccumulator.clear();
			for (ma_uint64 i = 0; i < framesRead; ++i)
			{
				pData->btAccumulator.push_back(tempBuffer[i * 2]);
				pData->btAccumulator.push_back(tempBuffer[i * 2 + 1]);
			}
		}

		while (true)
		{
			std::vector<float> framesToProcess;
			{
				std::lock_guard<std::mutex> lock(pData->btAccumulatorMutex);
				if (pData->btAccumulator.size() < 2048) break;
				framesToProcess.assign(pData->btAccumulator.begin(), pData->btAccumulator.begin() + 2048);
				pData->btAccumulator.erase(pData->btAccumulator.begin(), pData->btAccumulator.begin() + 2048);
			}

			const float ratio = 3000.0f / 48000.0f;
			std::vector<float> resampledData(128, 0.0f);
			for (std::int32_t outFrame = 0; outFrame < 64; ++outFrame)
			{
				float srcPos = static_cast<float>(outFrame) / ratio;
				std::int32_t srcIndex = static_cast<std::int32_t>(srcPos);
				float frac = srcPos - static_cast<float>(srcIndex);
				if (srcIndex >= 1023) { srcIndex = 1022; frac = 1.0f; }
				float left0 = framesToProcess[srcIndex * 2];
				float left1 = framesToProcess[(srcIndex + 1) * 2];
				float right0 = framesToProcess[srcIndex * 2 + 1];
				float right1 = framesToProcess[(srcIndex + 1) * 2 + 1];
				resampledData[outFrame * 2] = left0 + frac * (left1 - left0);
				resampledData[outFrame * 2 + 1] = right0 + frac * (right1 - right0);
			}

			for (std::int32_t i = 0; i < 64; ++i)
			{
				pData->LowPassStateLeft = kOneMinusAlphaBt * resampledData[i*2] + kLowPassAlphaBt * pData->LowPassStateLeft;
				pData->LowPassStateRight = kOneMinusAlphaBt * resampledData[i*2+1] + kLowPassAlphaBt * pData->LowPassStateRight;
				resampledData[i*2] -= pData->LowPassStateLeft;
				resampledData[i*2+1] -= pData->LowPassStateRight;
			}

			for (int p = 0; p < 2; ++p) {
				std::vector<std::uint8_t> packet(64, 0);
				for (int i = 0; i < 32; ++i) {
					int idx = (p * 32 + i) * 2;
					packet[i*2] = static_cast<std::uint8_t>(std::clamp(static_cast<int>(resampledData[idx] * 127.0f), -128, 127));
					packet[i*2+1] = static_cast<std::uint8_t>(std::clamp(static_cast<int>(resampledData[idx+1] * 127.0f), -128, 127));
				}
				pData->btPacketQueue.Push(packet);
			}
		}
	}
	pData->framesPlayed += framesRead;
}

void ConsumeHapticsQueue(IGamepadAudioHaptics* AudioHaptics, AudioCallbackData& callbackData)
{
	if (callbackData.bIsWireless) {
		std::vector<std::uint8_t> packet;
		while (callbackData.btPacketQueue.Pop(packet)) AudioHaptics->AudioHapticUpdate(packet);
	} else {
		std::vector<std::int16_t> allSamples, stereoSample;
		while (callbackData.usbSampleQueue.Pop(stereoSample)) {
			allSamples.push_back(stereoSample[0]);
			allSamples.push_back(stereoSample[1]);
		}
		if (!allSamples.empty()) AudioHaptics->AudioHapticUpdate(allSamples);
	}
}

ma_device g_AudioDevice;
bool g_AudioDeviceInitialized = false;
AudioCallbackData g_AudioCallbackData;

void AudioLoop()
{
	ISonyGamepad* Gamepad = g_Registry->GetLibrary(TargetDeviceId);
    while (g_Running)
    {
        if (Gamepad && Gamepad->IsConnected())
        {
            bool bIsWireless = Gamepad->GetConnectionType() == EDSDeviceConnection::Bluetooth;
            IGamepadAudioHaptics* AudioHaptics = Gamepad->GetIGamepadHaptics();
            if (AudioHaptics)
            {
                Gamepad->DualSenseSettings(0x10, 0x7C, 0x7C, 0x7C, 0x7C, 0xFC, 0x00, 0x00);
                if (!g_AudioDeviceInitialized || g_AudioCallbackData.bIsWireless != bIsWireless || (g_AudioDeviceInitialized && ma_device_get_state(&g_AudioDevice) == ma_device_state_stopped))
                {
                    if (g_AudioDeviceInitialized) ma_device_uninit(&g_AudioDevice);
                    g_AudioCallbackData.bIsSystemAudio = true;
                    g_AudioCallbackData.bIsWireless = bIsWireless;
                    ma_device_config deviceConfig = ma_device_config_init(ma_device_type_loopback);
                    deviceConfig.capture.format = ma_format_f32;
                    deviceConfig.capture.channels = 2;
                    deviceConfig.sampleRate = 48000;
                    deviceConfig.dataCallback = AudioDataCallback;
                    deviceConfig.pUserData = &g_AudioCallbackData;
                    if (ma_device_init(nullptr, &deviceConfig, &g_AudioDevice) == MA_SUCCESS) {
                        if (ma_device_start(&g_AudioDevice) == MA_SUCCESS) g_AudioDeviceInitialized = true;
                    }
                }
                ConsumeHapticsQueue(AudioHaptics, g_AudioCallbackData);
            }
        } else if (g_AudioDeviceInitialized) {
            ma_device_uninit(&g_AudioDevice);
            g_AudioDeviceInitialized = false;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}

void InputLoop()
{
    while (g_Running)
    {
        g_Registry->PlugAndPlay(0.006f);
        ISonyGamepad* Gamepad = g_Registry->GetLibrary(TargetDeviceId);
        if (Gamepad && Gamepad->IsConnected())
        {
            Gamepad->UpdateInput(0.006f);
            FDeviceContext* DeviceContext = Gamepad->GetMutableDeviceContext();
            if (DeviceContext && DeviceContext->GetInputState())
            {
#ifdef USE_VIGEM
                if (g_ViGEmAdapter) g_ViGEmAdapter->Update(*DeviceContext->GetInputState());
#endif
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(6));
    }
}

void StartServiceThread() {
    if (g_ServiceInitialized.exchange(true)) return;
    IPlatformHardwareInfo::SetInstance(std::make_unique<TestHardwareInfo>());
    g_Registry = std::make_unique<TestDeviceRegistry>();
    g_Registry->RequestImmediateDetection();
#ifdef USE_VIGEM
    g_ViGEmAdapter = std::make_unique<ViGEmAdapter>();
    g_ViGEmAdapter->Initialize();
#endif
    g_Running = true;
    g_AudioThread = std::thread(AudioLoop);
    InputLoop();
    if (g_AudioThread.joinable()) g_AudioThread.join();
    g_ServiceInitialized = false;
}

extern "C" {
    __declspec(dllexport) void StartGamepadService() {
        if (!g_Running && !g_ServiceInitialized) g_ServiceThread = std::thread(StartServiceThread);
    }
    __declspec(dllexport) void StopGamepadService() { g_Running = false; }
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    if (ul_reason_for_call == DLL_PROCESS_ATTACH) StartGamepadService();
    else if (ul_reason_for_call == DLL_PROCESS_DETACH) g_Running = false;
    return TRUE;
}
#endif
