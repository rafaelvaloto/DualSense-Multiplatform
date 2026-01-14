#pragma once
#if defined(_WIN32) && defined(USE_VIGEM)

#include <windows.h>
#include <ViGEm/Client.h>
#include "GCore/Types/Structs/Context/InputContext.h"

namespace GamepadCore {

class ViGEmAdapter {
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
