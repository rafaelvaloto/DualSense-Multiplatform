#pragma once
#include "GCore/Templates/TBasicDeviceRegistry.h"
#include "GodotRegistryPolicy.h"

using FGodotDeviceRegistryLogic = GamepadCore::TBasicDeviceRegistry<FGodotRegistryPolicy>;

class FGodotDeviceRegistry {
public:
    static void Initialize()
    {
        if (!RegistryInstance)
        {
            RegistryInstance = std::make_unique<FGodotDeviceRegistryLogic>();
        }
    }

    static void Shutdown()
    {
        RegistryInstance.reset();
    }

    static void DiscoverDevices(double DeltaTime)
    {
        if (RegistryInstance)
        {
            RegistryInstance->PlugAndPlay(static_cast<float>(DeltaTime));
        }
    }

    static ISonyGamepad* GetGamepad(int32_t DeviceId)
    {
        if (RegistryInstance)
        {
            return RegistryInstance->GetLibrary(DeviceId);
        }
        return nullptr;
    }

    static IGamepadTrigger* GetTriggerGamepad(int32_t DeviceId)
    {
        if (RegistryInstance)
        {
            ISonyGamepad* Gamepad = GetGamepad( DeviceId);
            if (Gamepad) {
                return Gamepad->GetIGamepadTrigger();
            }
        }
        return nullptr;
    }

private:
    static inline std::unique_ptr<FGodotDeviceRegistryLogic> RegistryInstance = nullptr;
};
