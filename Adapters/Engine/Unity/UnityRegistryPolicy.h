// Copyright (c) 2025 Rafael Valoto. All Rights Reserved.
// Project: GamepadCore - Adapter Example
// Description: Example implementation of DeviceRegistry Policy for Unity Native Plugin.

#pragma once

#include <unordered_map>
#include <iostream>

using CoreDeviceId = uint64_t;

// Tipos de callback que o C# deve fornecer
typedef void (*UnityOnConnectCallback)(int32_t EngineId);
typedef void (*UnityOnDisconnectCallback)(int32_t EngineId);

struct UnityRegistryPolicy
{
  using EngineIdType = int32_t;

  // Callbacks estáticos configurados pelo C# na inicialização
  static UnityOnConnectCallback OnConnect;
  static UnityOnDisconnectCallback OnDisconnect;

  // Contador simples para gerar IDs únicos para o C#
  static int32_t NextFreeId;

  int32_t AllocEngineDevice(CoreDeviceId CoreId)
  {
    int32_t NewId = NextFreeId++;

    // Avisa o C# (Unity Input System)
    if (OnConnect) {
      OnConnect(NewId);
    } else {
      std::cout << "[GamepadCore] Warning: Unity callback not set!" << std::endl;
    }

    return NewId;
  }

  void DispatchNewGamepad(CoreDeviceId CoreId)
  {
    // Example: Notify Engine Delegate
  }

  void DisconnectDevice(CoreDeviceId CoreId, int32_t EngineId)
  {
    if (OnDisconnect) {
      OnDisconnect(EngineId);
    }
  }
};

// Inicialização dos estáticos (normalmente num .cpp separado)
// int32_t UnityRegistryPolicy::NextFreeId = 0;
// UnityOnConnectCallback UnityRegistryPolicy::OnConnect = nullptr;
// UnityOnDisconnectCallback UnityRegistryPolicy::OnDisconnect = nullptr;