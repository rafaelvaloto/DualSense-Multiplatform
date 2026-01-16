#pragma once
#ifdef BUILD_GAMEPAD_CORE_TESTS
#if defined(_WIN32) && defined(USE_VIGEM)

#include "GCore/Types/Structs/Context/InputContext.h"
#include <ViGEm/Client.h>
#include <windows.h>

namespace GamepadCore
{

	class ViGEmAdapter
	{
	public:
		ViGEmAdapter();
		~ViGEmAdapter();

		bool Initialize();
		void Shutdown();

		void Update(const FInputContext& context);

	private:
		PVIGEM_CLIENT m_Client = nullptr;
		PVIGEM_TARGET m_Target = nullptr;
		bool m_Initialized = false;
	};

} // namespace GamepadCore

#endif // _WIN32 && USE_VIGEM
#endif // BUILD_GAMEPAD_CORE_TESTS
