// Copyright (c) 2025 Rafael Valoto. All Rights Reserved.
#ifdef BUILD_GAMEPAD_CORE_TESTS
#include "../../../Examples/Adapters/Tests/test_device_registry_policy.h"
#include "GCore/Interfaces/IPlatformHardwareInfo.h"
#include "GCore/Templates/TBasicDeviceRegistry.h"
#include "GCore/Types/Structs/Context/DeviceContext.h"
#include "GCore/Types/Structs/Context/InputContext.h"
#include <chrono>
#include <iomanip>
#include <iostream>
#include <memory>
#include <thread>
#include <vector>

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

int main()
{
	std::cout << "--- DualShock Analog Buffer Test ---" << std::endl;

	auto HardwareImpl = std::make_unique<TestHardwareInfo>();
	IPlatformHardwareInfo::SetInstance(std::move(HardwareImpl));

	auto Registry = std::make_unique<TestDeviceRegistry>();

	const int32_t TargetDeviceId = 0;
	bool bWasConnected = false;

	std::cout << "Reading analog buffers. Press Ctrl+C to stop." << std::endl;
	std::cout << std::fixed << std::setprecision(3);

	while (true)
	{
		float DeltaTime = 0.016f;
		Registry->PlugAndPlay(DeltaTime);

		ISonyGamepad* Gamepad = Registry->GetLibrary(TargetDeviceId);

		if (Gamepad && Gamepad->IsConnected())
		{
			if (!bWasConnected)
			{
				bWasConnected = true;
				std::cout << "\n>>> CONTROLLER CONNECTED! <<<" << std::endl;

				Gamepad->SetLightbar({0, 255, 0}); // Green on connect
				Gamepad->UpdateOutput();
			}

			Gamepad->UpdateInput(DeltaTime);
			FDeviceContext* Context = Gamepad->GetMutableDeviceContext();
			FInputContext* Input = Context->GetInputState();

			if (Input)
			{
				std::cout << "\rLeft Stick: [" << std::setw(6) << Input->LeftAnalog.X << ", " << std::setw(6) << Input->LeftAnalog.Y << "] | "
				          << "Right Stick: [" << std::setw(6) << Input->RightAnalog.X << ", " << std::setw(6) << Input->RightAnalog.Y << "]    " << std::flush;

				if (Input->bCross)
				{
					Gamepad->SetVibration(0, 200);
					Gamepad->SetLightbar({255, 0, 0});
					Gamepad->UpdateOutput();
				}
				else if (Input->bCircle)
				{
					Gamepad->SetVibration(100, 0);
					Gamepad->SetLightbar({0, 0, 255});
					Gamepad->UpdateOutput();
				}
				else if (Input->bTriangle)
				{
					Gamepad->SetVibration(0, 0);
					Gamepad->SetLightbar({0, 0, 0});
					Gamepad->UpdateOutput();
				}
			}
		}
		else
		{
			if (bWasConnected)
			{
				bWasConnected = false;
				std::cout << "\n>>> CONTROLLER DISCONNECTED! <<<" << std::endl;
			}
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(16));
	}

	return 0;
}
#endif
