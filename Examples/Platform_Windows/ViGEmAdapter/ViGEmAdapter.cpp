#ifdef BUILD_GAMEPAD_CORE_TESTS
#if defined(_WIN32) && defined(USE_VIGEM)
#include "ViGEmAdapter.h"
#include <algorithm>
#include <iostream>

namespace GamepadCore
{

	ViGEmAdapter::ViGEmAdapter()
	{}

	ViGEmAdapter::~ViGEmAdapter()
	{
		Shutdown();
	}

	bool ViGEmAdapter::Initialize()
	{
		if (m_Initialized)
		{
			return true;
		}

		m_Client = vigem_alloc();
		if (m_Client == nullptr)
		{
			std::cerr << "[ViGEm] Failed to allocate ViGEm Client." << std::endl;
			return false;
		}

		const VIGEM_ERROR err = vigem_connect(m_Client);
		if (!VIGEM_SUCCESS(err))
		{
			std::cerr << "[ViGEm] Failed to connect to ViGEm Bus (Error: 0x" << std::hex << err << "). Is the driver installed?" << std::endl;
			vigem_free(m_Client);
			m_Client = nullptr;
			return false;
		}

		m_Target = vigem_target_x360_alloc();
		if (m_Target == nullptr)
		{
			std::cerr << "[ViGEm] Failed to allocate X360 Target." << std::endl;
			vigem_disconnect(m_Client);
			vigem_free(m_Client);
			m_Client = nullptr;
			return false;
		}

		const VIGEM_ERROR addErr = vigem_target_add(m_Client, m_Target);
		if (!VIGEM_SUCCESS(addErr))
		{
			std::cerr << "[ViGEm] Failed to add X360 Target (Error: 0x" << std::hex << addErr << ")." << std::endl;
			vigem_target_free(m_Target);
			vigem_disconnect(m_Client);
			vigem_free(m_Client);
			m_Client = nullptr;
			m_Target = nullptr;
			return false;
		}

		std::cout << "[ViGEm] Virtual Xbox 360 Controller connected!" << std::endl;
		m_Initialized = true;
		return true;
	}

	void ViGEmAdapter::Shutdown()
	{
		if (!m_Initialized)
		{
			return;
		}

		if (m_Target)
		{
			vigem_target_remove(m_Client, m_Target);
			vigem_target_free(m_Target);
			m_Target = nullptr;
		}

		if (m_Client)
		{
			vigem_disconnect(m_Client);
			vigem_free(m_Client);
			m_Client = nullptr;
		}

		m_Initialized = false;
		std::cout << "[ViGEm] Virtual Xbox 360 Controller disconnected." << std::endl;
	}

	void ViGEmAdapter::Update(const FInputContext& context)
	{
		if (!m_Initialized)
		{
			return;
		}

		XUSB_REPORT report;
		XUSB_REPORT_INIT(&report);

		// Buttons mapping
		if (context.bCross)
		{
			report.wButtons |= XUSB_GAMEPAD_A;
		}
		if (context.bCircle)
		{
			report.wButtons |= XUSB_GAMEPAD_B;
		}
		if (context.bSquare)
		{
			report.wButtons |= XUSB_GAMEPAD_X;
		}
		if (context.bTriangle)
		{
			report.wButtons |= XUSB_GAMEPAD_Y;
		}

		if (context.bDpadUp)
		{
			report.wButtons |= XUSB_GAMEPAD_DPAD_UP;
		}
		if (context.bDpadDown)
		{
			report.wButtons |= XUSB_GAMEPAD_DPAD_DOWN;
		}
		if (context.bDpadLeft)
		{
			report.wButtons |= XUSB_GAMEPAD_DPAD_LEFT;
		}
		if (context.bDpadRight)
		{
			report.wButtons |= XUSB_GAMEPAD_DPAD_RIGHT;
		}

		if (context.bStart)
		{
			report.wButtons |= XUSB_GAMEPAD_START;
		}
		if (context.bShare)
		{
			report.wButtons |= XUSB_GAMEPAD_BACK;
		}

		if (context.bLeftShoulder)
		{
			report.wButtons |= XUSB_GAMEPAD_LEFT_SHOULDER;
		}
		if (context.bRightShoulder)
		{
			report.wButtons |= XUSB_GAMEPAD_RIGHT_SHOULDER;
		}

		if (context.bLeftStick)
		{
			report.wButtons |= XUSB_GAMEPAD_LEFT_THUMB;
		}
		if (context.bRightStick)
		{
			report.wButtons |= XUSB_GAMEPAD_RIGHT_THUMB;
		}

		if (context.bPSButton)
		{
			report.wButtons |= XUSB_GAMEPAD_GUIDE;
		}

		// Triggers (0-255)
		report.bLeftTrigger = static_cast<BYTE>(context.LeftTriggerAnalog * 255.0f);
		report.bRightTrigger = static_cast<BYTE>(context.RightTriggerAnalog * 255.0f);

		// Analogs (-32768 to 32767)
		// Note: Y axis is inverted in DS compared to XInput (or vice versa depending on perspective)
		// GamepadCore: Y is up (+1.0), down (-1.0)
		// XInput: Y is up (+32767), down (-32768)
		report.sThumbLX = static_cast<SHORT>(std::clamp(context.LeftAnalog.X * 32767.0f, -32768.0f, 32767.0f));
		report.sThumbLY = static_cast<SHORT>(std::clamp(context.LeftAnalog.Y * 32767.0f, -32768.0f, 32767.0f));
		report.sThumbRX = static_cast<SHORT>(std::clamp(context.RightAnalog.X * 32767.0f, -32768.0f, 32767.0f));
		report.sThumbRY = static_cast<SHORT>(std::clamp(context.RightAnalog.Y * 32767.0f, -32768.0f, 32767.0f));

		vigem_target_x360_update(m_Client, m_Target, report);
	}

} // namespace GamepadCore
#endif // _WIN32 && USE_VIGEM
#endif // BUILD_GAMEPAD_CORE_TESTS
