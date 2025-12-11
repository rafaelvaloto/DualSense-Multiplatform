// Copyright (c) 2025 Rafael Valoto. All Rights Reserved.
// Project: GamepadCore - Adapter Example
// Description: Example implementation of DeviceRegistry Policy for Godot GDExtension.

#pragma once

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/input.hpp>

using CoreDeviceId = uint64_t;

struct GodotRegistryPolicy
{
  using EngineIdType = int32_t; // Godot Input IDs são int

  // Singleton do Input do Godot
  godot::Input* InputServer = nullptr;

  GodotRegistryPolicy() {
    InputServer = godot::Input::get_singleton();
  }

  void DispatchNewGamepad(CoreDeviceId CoreId)
  {
    // Example: Notify Engine Delegate
  }

  int32_t AllocEngineDevice(CoreDeviceId CoreId)
  {
    // No Godot, IDs de Joypads são gerenciados internamente,
    // mas se estamos criando um Joypad virtual customizado, geramos um ID.
    static int32_t VirtualIdCounter = 100;
    int32_t NewId = VirtualIdCounter++;

    godot::UtilityFunctions::print(godot::String("[GamepadCore] New Device: ") + godot::String::num_int64(NewId));

    // Aqui você chamaria métodos do GDExtension para registrar input
    // InputServer->joy_connection_changed(NewId, true, "DualSense", "");

    return NewId;
  }

  void DisconnectDevice(CoreDeviceId CoreId, int32_t EngineId)
  {
    godot::UtilityFunctions::print("[GamepadCore] Device Disconnected");
    // InputServer->joy_connection_changed(EngineId, false, "", "");
  }
};