// Copyright (c) 2025 Rafael Valoto. All Rights Reserved.
// Project: GamepadCore - Adapter Example
// Description: Example implementation of DeviceRegistry Policy for Unreal Engine.

#pragma once

#include "CoreMinimal.h"
#include "Misc/CoreDelegates.h"

// Assume que seu Core define CoreDeviceId como uint64
using CoreDeviceId = uint64_t;

struct FUnrealRegistryPolicy
{
  // Unreal 5.1+ usa FInputDeviceId, versões antigas usam int32 ControllerId
  using EngineIdType = FInputDeviceId;

  // Mapa reverso para tradução rápida se a engine chamar de volta
  TMap<FInputDeviceId, CoreDeviceId> EngineToCoreMap;

  FInputDeviceId AllocEngineDevice(CoreDeviceId CoreId)
  {
    // No Unreal, o Hardware cria o ID, mas aqui simulamos a alocação
    // Na prática, você pegaria o próximo ID disponível ou usaria o IInputInterface
    static int32 NextId = 0;
    FInputDeviceId NewId = FInputDeviceId::CreateFromInt(NextId++);

    EngineToCoreMap.Add(NewId, CoreId);

    UE_LOG(LogTemp, Log, TEXT("[GamepadCore] Device Connected: CoreID=%llu -> UE_ID=%d"), CoreId, NewId.GetId());

    // Dispara evento global do Unreal
    FCoreDelegates::OnControllerConnectionChange.Broadcast(true, -1, NewId.GetId());

    return NewId;
  }

  void DispatchNewGamepad(CoreDeviceId CoreId)
  {
    // Example: Notify Engine Delegate
  }

  void DisconnectDevice(CoreDeviceId CoreId, FInputDeviceId EngineId)
  {
    UE_LOG(LogTemp, Log, TEXT("[GamepadCore] Device Disconnected: UE_ID=%d"), EngineId.GetId());

    FCoreDelegates::OnControllerConnectionChange.Broadcast(false, -1, EngineId.GetId());

    EngineToCoreMap.Remove(EngineId);
  }
};