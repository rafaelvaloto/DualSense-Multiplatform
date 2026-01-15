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

std::vector<uint8_t> g_BufferTrigger(10);

struct FSkaterConfig {
    float LowPassAlpha = 0.99f;
    float LowPassAlphaBt = 0.98f;
    uint8_t LightbarR = 200;
    uint8_t LightbarG = 160;
    uint8_t LightbarB = 80;

    void LoadFromIni(const std::wstring& filePath) {
        wchar_t buffer[256];
        
        // Read TruckTightness
        GetPrivateProfileStringW(L"SkaterSettings", L"TruckTightness", L"Medium", buffer, 256, filePath.c_str());
        UpdateTriggerBuffer(buffer);

        // Read Haptics Settings USB
        GetPrivateProfileStringW(L"HapticsSettings", L"LowPassAlpha", L"0.99", buffer, 256, filePath.c_str());
        LowPassAlpha = std::wcstof(buffer, nullptr);

    	// Read Haptics Settings Bluetooth
        GetPrivateProfileStringW(L"HapticsSettings", L"LowPassAlphaBt", L"0.98", buffer, 256, filePath.c_str());
        LowPassAlphaBt = std::wcstof(buffer, nullptr);

        // Read Lightbar Color
        GetPrivateProfileStringW(L"VisualSettings", L"LightbarR", L"200", buffer, 256, filePath.c_str());
        LightbarR = static_cast<uint8_t>(std::wcstol(buffer, nullptr, 10));
        GetPrivateProfileStringW(L"VisualSettings", L"LightbarG", L"160", buffer, 256, filePath.c_str());
        LightbarG = static_cast<uint8_t>(std::wcstol(buffer, nullptr, 10));
        GetPrivateProfileStringW(L"VisualSettings", L"LightbarB", L"80", buffer, 256, filePath.c_str());
        LightbarB = static_cast<uint8_t>(std::wcstol(buffer, nullptr, 10));
    }

private:
    void UpdateTriggerBuffer(const std::wstring& str) {
        g_BufferTrigger[0] = 0x21;
        g_BufferTrigger[1] = 0xfe;
        g_BufferTrigger[2] = 0x03;
        g_BufferTrigger[3] = 0xf8;
        g_BufferTrigger[4] = 0xff;
        g_BufferTrigger[5] = 0xff;
        g_BufferTrigger[6] = 0x1f;

        if (str == L"Light") {
        	g_BufferTrigger[1] = 0xfe;
        	g_BufferTrigger[2] = 0x03;
        	g_BufferTrigger[3] = 0xf8;
        	g_BufferTrigger[4] = 0xff;
        	g_BufferTrigger[5] = 0xff;
        	g_BufferTrigger[6] = 0x0a;
        } else if (str == L"Heavy") {
        	g_BufferTrigger[1] = 0xfe;
        	g_BufferTrigger[2] = 0x03;
        	g_BufferTrigger[3] = 0xf8;
        	g_BufferTrigger[4] = 0xff;
        	g_BufferTrigger[5] = 0xff;
        	g_BufferTrigger[6] = 0x3f;
        }

    	g_BufferTrigger[7] = 0x00;
    	g_BufferTrigger[8] = 0x00;
    	g_BufferTrigger[9] = 0x00;
    }
};

FSkaterConfig g_SkaterConfig;

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

		auto pInputFloat = static_cast<const float*>(pInput);
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

			pData->LowPassStateLeft = (1.0f - g_SkaterConfig.LowPassAlpha) * inLeft + g_SkaterConfig.LowPassAlpha * pData->LowPassStateLeft;
			pData->LowPassStateRight = (1.0f - g_SkaterConfig.LowPassAlpha) * inRight + g_SkaterConfig.LowPassAlpha * pData->LowPassStateRight;

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
		{
			std::lock_guard<std::mutex> lock(pData->btAccumulatorMutex);
			
			const size_t maxAccumulatorFrames = 24000 * 2; 
			if (pData->btAccumulator.size() > maxAccumulatorFrames)
			{
				pData->btAccumulator.clear(); 
			}

			for (ma_uint64 i = 0; i < framesRead; ++i)
			{
				pData->btAccumulator.push_back(tempBuffer[i * 2]);     
				pData->btAccumulator.push_back(tempBuffer[i * 2 + 1]); 
			}
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

				pData->LowPassStateLeft = (1.0f - g_SkaterConfig.LowPassAlphaBt) * inLeft + g_SkaterConfig.LowPassAlphaBt * pData->LowPassStateLeft;
				pData->LowPassStateRight = (1.0f - g_SkaterConfig.LowPassAlphaBt) * inRight + g_SkaterConfig.LowPassAlphaBt * pData->LowPassStateRight;

				resampledData[dataIndex] = inLeft - pData->LowPassStateLeft;
				resampledData[dataIndex + 1] = inRight - pData->LowPassStateRight;
			}

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

ma_device g_AudioDevice;
bool g_AudioDeviceInitialized = false;
AudioCallbackData g_AudioCallbackData;

void AudioLoop()
{
    std::cout << "[AppDLL] Audio Loop Started." << std::endl;

	ISonyGamepad* Gamepad = g_Registry->GetLibrary(TargetDeviceId);
	Gamepad->SetLightbar({g_SkaterConfig.LightbarR, g_SkaterConfig.LightbarG, g_SkaterConfig.LightbarB});
	Gamepad->DualSenseSettings(0x10, 0x01, 0x7C, 0x7C, 0x7C, 0xFC, 0x00, 0x00);

	auto Trigger = Gamepad->GetIGamepadTrigger();
	if (Trigger)
	{
		Trigger->SetCustomTrigger(EDSGamepadHand::AnyHand, g_BufferTrigger);
	}
	Gamepad->UpdateOutput();

    while (g_Running)
    {
        if (Gamepad && Gamepad->IsConnected())
        {
            bool bIsWireless = Gamepad->GetConnectionType() == EDSDeviceConnection::Bluetooth;
            IGamepadAudioHaptics* AudioHaptics = Gamepad->GetIGamepadHaptics();
            if (AudioHaptics)
            {


                if (!g_AudioDeviceInitialized || g_AudioCallbackData.bIsWireless != bIsWireless || (g_AudioDeviceInitialized && ma_device_get_state(&g_AudioDevice) == ma_device_state_stopped))
                {
                    if (g_AudioDeviceInitialized)
                    {
                        ma_device_uninit(&g_AudioDevice);
                        g_AudioDeviceInitialized = false;
                        
                        {
                            std::lock_guard<std::mutex> lock(g_AudioCallbackData.btAccumulatorMutex);
                            g_AudioCallbackData.btAccumulator.clear();
                        }
                        std::vector<uint8_t> dummy;
                        while(g_AudioCallbackData.btPacketQueue.Pop(dummy));
                        std::vector<int16_t> dummy2;
                        while(g_AudioCallbackData.usbSampleQueue.Pop(dummy2));
                    }

                    std::cout << "[AppDLL] Initializing Audio Loopback for Haptics (" << (bIsWireless ? "Bluetooth" : "USB") << ")..." << std::endl;

                    FDeviceContext* Context = Gamepad->GetMutableDeviceContext();
                    if (!bIsWireless && Context)
                    {
                        if (!Context->AudioContext || !Context->AudioContext->IsValid())
                        {
                            IPlatformHardwareInfo::Get().InitializeAudioDevice(Context);
                        }
                    }

                    g_AudioCallbackData.bIsSystemAudio = true;
                    g_AudioCallbackData.bIsWireless = bIsWireless;
                    g_AudioCallbackData.bFinished = false;

                    ma_device_config deviceConfig = ma_device_config_init(ma_device_type_loopback);
                    deviceConfig.capture.format = ma_format_f32;
                    deviceConfig.capture.channels = 2;
                    deviceConfig.sampleRate = 48000;
                    deviceConfig.dataCallback = AudioDataCallback;
                    deviceConfig.pUserData = &g_AudioCallbackData;
                    deviceConfig.wasapi.loopbackProcessID = 0; 

                    ma_result result = ma_device_init(nullptr, &deviceConfig, &g_AudioDevice);
                    if (result == MA_SUCCESS)
                    {
                        if (ma_device_start(&g_AudioDevice) == MA_SUCCESS)
                        {
                            g_AudioDeviceInitialized = true;
                            std::cout << "[AppDLL] Audio Loopback Started." << std::endl;
                        }
                        else
                        {
                            ma_device_uninit(&g_AudioDevice);
                            std::cerr << "[AppDLL] Failed to start audio device." << std::endl;
                        }
                    }
                    else
                    {
                        std::cerr << "[AppDLL] Failed to initialize audio device (Error: " << result << ")." << std::endl;
                    }
                }
                ConsumeHapticsQueue(AudioHaptics, g_AudioCallbackData);
            }
        }
        else
        {
            if (g_AudioDeviceInitialized)
            {
                ma_device_uninit(&g_AudioDevice);
                g_AudioDeviceInitialized = false;
                std::cout << "[AppDLL] Audio Loopback Stopped (Controller Disconnected)." << std::endl;
            }
        }

    }

    if (g_AudioDeviceInitialized)
    {
        ma_device_uninit(&g_AudioDevice);
        g_AudioDeviceInitialized = false;
    }

    std::cout << "[AppDLL] Audio Loop Stopped." << std::endl;
}


void InputLoop()
{
    std::cout << "[AppDLL] Input Loop Started." << std::endl;

    while (g_Running)
    {
        float DeltaTime = 0.006f;
        g_Registry->PlugAndPlay(DeltaTime);

        ISonyGamepad* Gamepad = g_Registry->GetLibrary(TargetDeviceId);

        if (Gamepad && Gamepad->IsConnected())
        {
            Gamepad->UpdateInput(DeltaTime);

            FDeviceContext* DeviceContext = Gamepad->GetMutableDeviceContext();
            if (DeviceContext)
            {
                FInputContext* CurrentState = DeviceContext->GetInputState();
                if (CurrentState)
                {
#ifdef USE_VIGEM
                    if (g_ViGEmAdapter) {
                        g_ViGEmAdapter->Update(*CurrentState);
                    }
#endif
                    static int HeartbeatCounter = 0;
                    if (++HeartbeatCounter % 600 == 0) {
                        std::cout << "[AppDLL] Input Loop Alive" << std::endl;
                    }
                }
            }
        }
        else if (Gamepad)
        {
             static int ConnCounter = 0;
             if (++ConnCounter % 60 == 0) {
                 std::cout << "[AppDLL] Gamepad Object exists but IsConnected() is FALSE" << std::endl;
             }
        }
        else
        {
             static int NullCounter = 0;
             if (++NullCounter % 60 == 0) {
                 std::cout << "[AppDLL] Gamepad Object is NULL for ID " << TargetDeviceId << std::endl;
             }
        }
    }

    std::cout << "[AppDLL] Input Loop Stopped." << std::endl;
}

void CreateConsole() {
    if (GetConsoleWindow() != NULL) {
        return;
    }
    if (AllocConsole()) {
        FILE* fp;
        freopen_s(&fp, "CONOUT$", "w", stdout);
        freopen_s(&fp, "CONOUT$", "w", stderr);
        freopen_s(&fp, "CONIN$", "r", stdin);
        
        setvbuf(stdout, NULL, _IONBF, 0);
        setvbuf(stderr, NULL, _IONBF, 0);

        std::cout << "[Console] Debug Console Attached!" << std::endl;
    }
}

void StartServiceThread() {
    if (g_ServiceInitialized.exchange(true)) return;

    CreateConsole();

    std::cout << "[AppDLL] Service Thread Starting..." << std::endl;
    std::cout.flush();

    g_LastInputState = FInputContext();

    
    std::cout << "[System] Initializing Hardware Layer..." << std::endl;
    std::cout.flush();

    auto HardwareImpl = std::make_unique<TestHardwareInfo>();
    IPlatformHardwareInfo::SetInstance(std::move(HardwareImpl));

    g_Registry = std::make_unique<TestDeviceRegistry>();
    g_Registry->Policy.deviceId = 0; 
    
    std::cout << "[System] Requesting Immediate Detection..." << std::endl;
    std::cout.flush();
    
    g_Registry->RequestImmediateDetection();
    g_Registry->PlugAndPlay(0.5);

#ifdef USE_VIGEM
    std::cout << "[System] Initializing ViGEm Adapter..." << std::endl;
    g_ViGEmAdapter = std::make_unique<ViGEmAdapter>();
    if (!g_ViGEmAdapter->Initialize()) {
        std::cerr << "[System] ViGEm Adapter failed to initialize. Xbox Emulation will not be available." << std::endl;
    }
#endif

    std::cout << "[System] Waiting for controller connection via USB/BT..." << std::endl;
    std::cout.flush();
    
    g_Running = true;
    
    std::cout << "[AppDLL] Gamepad Service Started." << std::endl;
    std::cout.flush();
    
    g_AudioThread = std::thread(AudioLoop);

    InputLoop();
    
    if (g_AudioThread.joinable())
    {
        g_AudioThread.join();
    }

#ifdef USE_VIGEM
    if (g_ViGEmAdapter) {
        g_ViGEmAdapter->Shutdown();
        g_ViGEmAdapter.reset();
    }
#endif

    if (g_Registry) g_Registry.reset();
    
    std::cout << "[AppDLL] Gamepad Service Stopped." << std::endl;
    std::cout.flush();
    g_ServiceInitialized = false;
}

extern "C" {

    __declspec(dllexport) void StartGamepadService()
    {
        if (g_Running || g_ServiceInitialized)
        {
            return;
        }

        // Carregar configurações do INI
        g_SkaterConfig.LoadFromIni(L".\\config.ini");
        std::cout << "[AppDLL] Config Loaded"
                  << " | Alpha=" << g_SkaterConfig.LowPassAlpha 
                  << " | AlphaBt=" << g_SkaterConfig.LowPassAlphaBt << std::endl;

        if (g_ServiceThread.joinable())
        {
            g_ServiceThread.detach();
        }

        g_ServiceThread = std::thread(StartServiceThread);
    }

    __declspec(dllexport) void StopGamepadService()
    {
        g_Running = false;
    }

	__declspec(dllexport) bool GetGamepadStateSafe(int ControllerId, FInputContext* OutState)
    {
        if (!OutState) return false;
        if (!g_Registry) return false;
    	
    	ISonyGamepad* Gamepad = g_Registry->GetLibrary(ControllerId);
    	if (Gamepad && Gamepad->IsConnected())
    	{
    		FDeviceContext* DeviceContext = Gamepad->GetMutableDeviceContext();
    		if (DeviceContext)
    		{
    			FInputContext* InState = DeviceContext->GetInputState();
                if (!InState) return false;
                
    			OutState->bCross = InState->bCross;
    			OutState->bCircle = InState->bCircle;
    			OutState->bTriangle = InState->bTriangle;
    			OutState->bSquare = InState->bSquare;
    			OutState->bDpadDown = InState->bDpadDown;
    			OutState->bDpadLeft = InState->bDpadLeft;
    			OutState->bDpadRight = InState->bDpadRight;
    			OutState->bDpadUp = InState->bDpadUp;
    			OutState->RightAnalog = InState->RightAnalog;
    			OutState->LeftAnalog = InState->LeftAnalog;
    			OutState->bLeftAnalogDown = InState->bLeftAnalogDown;
    			OutState->bLeftAnalogLeft = InState->bLeftAnalogLeft;
    			OutState->bLeftAnalogRight = InState->bLeftAnalogRight;
    			OutState->bLeftAnalogUp = InState->bLeftAnalogUp;
                OutState->bRightAnalogDown = InState->bRightAnalogDown;
                OutState->bRightAnalogLeft = InState->bRightAnalogLeft;
                OutState->bRightAnalogRight = InState->bRightAnalogRight;
                OutState->bRightAnalogUp = InState->bRightAnalogUp;
    			OutState->bLeftShoulder = InState->bLeftShoulder;
    			OutState->bRightShoulder = InState->bRightShoulder;
    			OutState->LeftTriggerAnalog = InState->LeftTriggerAnalog;
    			OutState->RightTriggerAnalog = InState->RightTriggerAnalog;
    			OutState->bLeftTriggerThreshold = InState->bLeftTriggerThreshold;
    			OutState->bRightTriggerThreshold = InState->bRightTriggerThreshold;
    			OutState->bStart = InState->bStart;
    			OutState->bShare = InState->bShare;
    			OutState->bMute = InState->bMute;
    			OutState->bPSButton = InState->bPSButton;
    			OutState->bTouch = InState->bTouch;
                OutState->bLeftStick = InState->bLeftStick;
                OutState->bRightStick = InState->bRightStick;
                OutState->BatteryLevel = InState->BatteryLevel;
    			return true;
    		}
    	}
    	return false;
    }
}

#ifndef BUILDING_PROXY_DLL
BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    	StartGamepadService();
        break;

    case DLL_PROCESS_DETACH:
        g_Running = false;
        
        if (lpReserved == nullptr) { 
            if (g_AudioThread.joinable()) g_AudioThread.detach();
            if (g_ServiceThread.joinable()) g_ServiceThread.detach();
        }

        g_ServiceInitialized = false;
        break;
    }
    return TRUE;
}

#endif // _WIN32 && USE_VIGEM
