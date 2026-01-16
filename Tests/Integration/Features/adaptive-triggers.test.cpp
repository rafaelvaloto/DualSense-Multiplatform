#ifdef BUILD_GAMEPAD_CORE_TESTS
#include "../../../Examples/Adapters/Tests/test_device_registry_policy.h"
#include "GCore/Interfaces/IPlatformHardwareInfo.h"
#include "GCore/Templates/TBasicDeviceRegistry.h"
#include "GCore/Types/Structs/Context/DeviceContext.h"
#include <chrono>
#include <cmath>
#include <format>
#include <functional>
#include <iomanip>
#include <iostream>
#include <memory>
#include <thread>
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

void PrintControlsHelper()
{
	std::cout << "\n=======================================================" << std::endl;
	std::cout << "           DUALSENSE INTEGRATION TEST                  " << std::endl;
	std::cout << "=======================================================" << std::endl;
	std::cout << " [ FACE BUTTONS ]" << std::endl;
	std::cout << "   (X) Cross    : Heavy Rumble + RED Light" << std::endl;
	std::cout << "   (O) Circle   : Soft Rumble  + BLUE Light" << std::endl;
	std::cout << "   [ ] Square   : Trigger Effect: GAMECUBE (R2)" << std::endl;
	std::cout << "   /_\\ Triangle : Stop All" << std::endl;
	std::cout << "-------------------------------------------------------" << std::endl;
	std::cout << " [ D-PADS & SHOULDERS ]" << std::endl;
	std::cout << "   [L1]    : Trigger Effect: Gallop (L2)" << std::endl;
	std::cout << "   [R1]    : Trigger Effect: Machine (R2)" << std::endl;
	std::cout << "   [UP]    : Trigger Effect: Feedback (Rigid)" << std::endl;
	std::cout << "   [DOWN]  : Trigger Effect: Bow (Tension)" << std::endl;
	std::cout << "   [LEFT]  : Trigger Effect: Weapon (Semi)" << std::endl;
	std::cout << "   [RIGHT] : Trigger Effect: Automatic Gun (Buzz)" << std::endl;
	std::cout << "=======================================================" << std::endl;
	std::cout << " Waiting for input..." << std::endl
	          << std::endl;
}

int main()
{
	std::cout << "[System] Initializing Hardware Layer..." << std::endl;

	auto HardwareImpl = std::make_unique<TestHardwareInfo>();
	IPlatformHardwareInfo::SetInstance(std::move(HardwareImpl));

	auto Registry = std::make_unique<TestDeviceRegistry>();

	std::cout << "[System] Waiting for controller connection via USB/BT..." << std::endl;

	bool bWasDebugAnalog = false;
	bool bWasConnected = false;
	const int32_t TargetDeviceId = 0;

#ifdef AUTOMATED_TESTS
	std::cout << "[Test] Automated mode active. The test will end in 5s." << std::endl;
	auto startTime = std::chrono::steady_clock::now();
#endif

	std::vector<uint8_t> BufferTrigger;
	BufferTrigger.resize(10);

	bool bControllerFound = false;
	int MaxWaitIterations = 300;

	while (true)
	{
#ifdef AUTOMATED_TESTS
		auto now = std::chrono::steady_clock::now();
		if (std::chrono::duration_cast<std::chrono::seconds>(now - startTime).count() >= 5)
		{
			if (bControllerFound)
			{
				std::cout << "\n[Test] Timeout reached (5s). Finishing..." << std::endl;
			}
			else
			{
				std::cout << "\n[Test] No controller found in automated mode. Skipping test." << std::endl;
			}
			break;
		}
#endif
		std::this_thread::sleep_for(std::chrono::milliseconds(16));
		float DeltaTime = 0.016f;

		Registry->PlugAndPlay(DeltaTime);
		ISonyGamepad* Gamepad = Registry->GetLibrary(TargetDeviceId);

		if (Gamepad && Gamepad->IsConnected())
		{
			bControllerFound = true;
			if (!bWasConnected)
			{
				bWasConnected = true;
				std::cout << ">>> CONTROLLER CONNECTED! <<<" << std::endl;

				Gamepad->SetLightbar({0, 255, 0});
				Gamepad->SetPlayerLed(EDSPlayer::One, 255);
				PrintControlsHelper();
			}

			Gamepad->UpdateInput(DeltaTime);
			FDeviceContext* Context = Gamepad->GetMutableDeviceContext();
			FInputContext* InputState = Context->GetInputState();

			auto Trigger = Gamepad->GetIGamepadTrigger();

			std::string StatusText;

			if (InputState->bCross)
			{
				StatusText = "Cross";
				Gamepad->SetVibration(0, 200);
				Gamepad->SetLightbar({255, 0, 0});
			}
			else if (InputState->bCircle)
			{
				StatusText = "Circle";
				Gamepad->SetVibration(100, 0);
				Gamepad->SetLightbar({0, 0, 255});
			}
			else if (InputState->bSquare)
			{
				StatusText = "Trigger R: GameCube (0x02)";
				if (Trigger)
				{
					Trigger->SetGameCube(EDSGamepadHand::Right);
				}
			}
			else if (InputState->bDpadUp)
			{
				BufferTrigger[0] = 0x21;
				BufferTrigger[1] = 0xfe;
				BufferTrigger[2] = 0x03;
				BufferTrigger[3] = 0xf8;
				BufferTrigger[4] = 0xff;
				BufferTrigger[5] = 0xff;
				BufferTrigger[6] = 0x3f;
				BufferTrigger[7] = 0x00;
				BufferTrigger[8] = 0x00;
				BufferTrigger[9] = 0x00;

				StatusText = "Trigger L: feedback (0x21)";
				if (Trigger)
				{
					Trigger->SetCustomTrigger(EDSGamepadHand::Left, BufferTrigger);
				}
			}
			else if (InputState->bDpadDown)
			{
				BufferTrigger[0] = 0x22;
				BufferTrigger[1] = 0x02;
				BufferTrigger[2] = 0x01;
				BufferTrigger[3] = 0x3f;
				BufferTrigger[4] = 0x00;
				BufferTrigger[5] = 0x00;
				BufferTrigger[6] = 0x00;
				BufferTrigger[7] = 0x00;
				BufferTrigger[8] = 0x00;
				BufferTrigger[9] = 0x00;

				StatusText = "Trigger R: Bow (0x22)";
				if (Trigger)
				{
					Trigger->SetCustomTrigger(EDSGamepadHand::Right, BufferTrigger);
				}
			}
			else if (InputState->bLeftShoulder)
			{
				BufferTrigger[0] = 0x23;
				BufferTrigger[1] = 0x82;
				BufferTrigger[2] = 0x00;
				BufferTrigger[3] = 0xf7;
				BufferTrigger[4] = 0x02;
				BufferTrigger[5] = 0x00;
				BufferTrigger[6] = 0x00;
				BufferTrigger[7] = 0x00;
				BufferTrigger[8] = 0x00;
				BufferTrigger[9] = 0x00;

				StatusText = "Trigger L: Gallop (0x23)";
				if (Trigger)
				{
					Trigger->SetCustomTrigger(EDSGamepadHand::Left, BufferTrigger);
				}
			}
			else if (InputState->bRightShoulder)
			{
				BufferTrigger[0] = 0x27;
				BufferTrigger[1] = 0x80;
				BufferTrigger[2] = 0x02;
				BufferTrigger[3] = 0x3a;
				BufferTrigger[4] = 0x0a;
				BufferTrigger[5] = 0x04;
				BufferTrigger[6] = 0x00;
				BufferTrigger[7] = 0x00;
				BufferTrigger[8] = 0x00;
				BufferTrigger[9] = 0x00;

				StatusText = "Trigger R: Machine (0x27)";
				if (Trigger)
				{
					Trigger->SetCustomTrigger(EDSGamepadHand::Right, BufferTrigger);
				}
			}
			else if (InputState->bDpadLeft)
			{
				BufferTrigger[0] = 0x25;
				BufferTrigger[1] = 0x08;
				BufferTrigger[2] = 0x01;
				BufferTrigger[3] = 0x07;
				BufferTrigger[4] = 0x00;
				BufferTrigger[5] = 0x00;
				BufferTrigger[6] = 0x00;
				BufferTrigger[7] = 0x00;
				BufferTrigger[8] = 0x00;
				BufferTrigger[9] = 0x00;

				StatusText = "Trigger R: Weapon (0x25)";
				if (Trigger)
				{
					Trigger->SetCustomTrigger(EDSGamepadHand::Right, BufferTrigger);
				}
			}
			else if (InputState->bDpadRight)
			{
				StatusText = "Trigger R: AutomaticGun (0x26)";
				if (Trigger)
				{
					Trigger->SetMachineGun26(0xed, 0x03, 0x02, 0x09, EDSGamepadHand::Right);
				}
			}
			else if (InputState->bTriangle)
			{
				StatusText = "Triangle";
				bWasDebugAnalog = !bWasDebugAnalog;

				Gamepad->SetVibration(0, 0);
				Gamepad->SetLightbar({0, 255, 0});

				Trigger->StopTrigger(EDSGamepadHand::Left);
				Trigger->StopTrigger(EDSGamepadHand::Right);
			}
			else
			{
				Gamepad->SetVibration(0, 0);
			}

			printf("\r[%s]", StatusText.c_str());
			fflush(stdout);
		}
		else
		{
			if (bWasConnected)
			{
				std::cout << "\n\n<<< CONTROLLER DISCONNECTED >>>" << std::endl;
				std::cout << "[System] Waiting for reconnection..." << std::endl;
				bWasConnected = false;
			}
		}
	}
	return 0;
}
#endif
