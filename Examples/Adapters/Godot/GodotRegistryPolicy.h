#pragma once

#include <godot_cpp/variant/utility_functions.hpp>
#include <functional>
#include <concepts>

struct FGodotRegistryPolicy
{
    using EngineIdType = int32_t;

    // Hasher required for TBasicDeviceRegistry to use IDs as keys in Maps
    struct Hasher
    {
        std::size_t operator()(const EngineIdType& Id) const
        {
            return std::hash<EngineIdType>{}(Id);
        }
    };

    static inline EngineIdType NextDeviceId = 0;

    // Function called by the library when a new physical control is found.
    EngineIdType AllocEngineDevice()
    {
        return ++NextDeviceId;
    }

    // Called when the connection is confirmed.
    void DispatchNewGamepad(EngineIdType GamepadId)
    {
        godot::UtilityFunctions::print("[GamepadCore] DualSense Is Connected! ID: ", GamepadId);
        DualSenseManager::get_singleton()->test_lightbar();
        DualSenseManager::get_singleton()->test_rumble();
        DualSenseManager::get_singleton()->test_weapon();
        DualSenseManager::get_singleton()->test_custom_trigger();
        // DualSenseManager::get_singleton()->emit_signal("device_connected", GamepadId);
    }

    // Chamado quando o controle é desconectado
    void DisconnectDevice(EngineIdType GamepadId)
    {
        godot::UtilityFunctions::print("[GamepadCore] DualSense Disconnected! ID: ", GamepadId);
        // DualSenseManager::get_singleton()->emit_signal("device_disconnected", GamepadId);
    }
};
