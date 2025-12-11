// Copyright (c) 2025 Rafael Valoto. All Rights Reserved.
// Project: GamepadCore - Adapter Example
// Description: Example implementation of DeviceRegistry Policy for O3DE (Open 3D Engine).

#pragma once

#include <AzCore/EBus/EBus.h>
#include <AzFramework/Input/Buses/Requests/InputDeviceRequestBus.h>

using CoreDeviceId = uint64_t;

struct O3DERegistryPolicy
{
  // O3DE usa AzFramework::InputDeviceId
  using EngineIdType = AzFramework::InputDeviceId;

  AzFramework::InputDeviceId AllocEngineDevice(CoreDeviceId CoreId)
  {
    // Gera um ID baseado no nome ou hash
    AzFramework::InputDeviceId NewId(static_cast<uint32_t>(CoreId));

    AZ_Printf("GamepadCore", "Device Connected: %u", NewId.GetIndex());

    // Notifica o sistema de Input do O3DE
    // AzFramework::InputDeviceRequestBus::Broadcast(&AzFramework::InputDeviceRequests::AddInputDevice, ...);

    return NewId;
  }

  void DispatchNewGamepad(CoreDeviceId CoreId)
  {
    // Example: Notify Engine Delegate
  }

  void DisconnectDevice(CoreDeviceId CoreId, AzFramework::InputDeviceId EngineId)
  {
    AZ_Printf("GamepadCore", "Device Disconnected");
    // AzFramework::InputDeviceRequestBus::Broadcast(&AzFramework::InputDeviceRequests::RemoveInputDevice, EngineId);
  }
};