// Copyright (c) 2025 Rafael Valoto. All Rights Reserved.
// Project: GamepadCore - Adapter Example
// Description: Example implementation of DeviceRegistry Policy for Unreal Engine.
#pragma once
#include "CoreMinimal.h"
#include "Misc/CoreDelegates.h"

struct FDeviceRegistryPolicy
{
public:
	using EngineIdType = FInputDeviceId;

	struct Hasher
	{
		std::size_t operator()(const FInputDeviceId& Id) const
		{
			return GetTypeHash(Id);
		}
	};

	FInputDeviceId AllocEngineDevice()
	{
		// We should never call into IPlatformInputDeviceMapper from non-game thread because it is not thread-safe
		check(IsInGameThread());
		return IPlatformInputDeviceMapper::Get().AllocateNewInputDeviceId();
	}

	void DispatchNewGamepad(EngineIdType GamepadId)
	{
		check(IsInGameThread());
		if (IPlatformInputDeviceMapper::Get().GetInputDeviceConnectionState(GamepadId) != EInputDeviceConnectionState::Connected)
		{
			FPlatformUserId UserId = IPlatformInputDeviceMapper::Get().GetUserForInputDevice(GamepadId);
			if (!UserId.IsValid())
			{
#if ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION < 6
				TArray<FInputDeviceId> Devices;
				Devices.Reset();

				IPlatformInputDeviceMapper::Get().GetAllInputDevicesForUser(
					IPlatformInputDeviceMapper::Get().GetPrimaryPlatformUser(), Devices);

				bool AllocateDeviceToDefaultUser = false;
				if (Devices.Num() <= 1)
				{
					AllocateDeviceToDefaultUser = true;
				}

				UserId = AllocateDeviceToDefaultUser
					         ? IPlatformInputDeviceMapper::Get().GetPrimaryPlatformUser()
					         : IPlatformInputDeviceMapper::Get().AllocateNewUserId();

#else
				UserId = IPlatformInputDeviceMapper::Get().GetPlatformUserForNewlyConnectedDevice();
#endif
			}

			IPlatformInputDeviceMapper::Get().Internal_MapInputDeviceToUser(GamepadId, UserId, EInputDeviceConnectionState::Connected);
		}
	}

	static void DisconnectDevice(EngineIdType GamepadId)
	{
		IPlatformInputDeviceMapper::Get().Internal_SetInputDeviceConnectionState(GamepadId, EInputDeviceConnectionState::Disconnected);
	}

	static FInputDeviceId ConvertFromInt(int32 Id)
	{
		return FInputDeviceId::CreateFromInternalId(Id);
	}

	static int32 ConvertToInt(FInputDeviceId Id)
	{
		return Id.GetId();
	}
};
