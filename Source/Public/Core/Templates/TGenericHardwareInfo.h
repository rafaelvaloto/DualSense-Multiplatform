// Copyright (c) 2025 Rafael Valoto. All Rights Reserved.
// Project: GamepadCore
// Description: Cross-platform library for DualSense and generic gamepad input support.
// Targets: Windows, Linux, macOS.
#pragma once
#include "Core/Interfaces/IPlatformHardwareInfo.h"
#include "Core/Types/Structs/Context/DeviceContext.h"

namespace GamepadCore
{
	template<typename T>
	concept IsHardwarePolicy = requires(T t, FDeviceContext* ctx, std::vector<FDeviceContext>& devs)
	{
		{
			t.Read(ctx)
		} -> std::same_as<void>;
		{
			t.Write(ctx)
		} -> std::same_as<void>;
		{
			t.Detect(devs)
		} -> std::same_as<void>;
		{
			t.CreateHandle(ctx)
		} -> std::same_as<bool>;
		{
			t.InvalidateHandle(ctx)
		} -> std::same_as<void>;
		{
			t.ProcessAudioHaptic(ctx)
		} -> std::same_as<void>;
	}

	template<typename THardwarePolicy>
	class TGenericHardwareInfo : public IPlatformHardwareInfo
	{
		THardwarePolicy Policy;

	public:
		~TGenericHardwareInfo() override = default;

		void Read(FDeviceContext* Context) override
		{
			Policy.Read(Context);
		}

		void Write(FDeviceContext* Context) override
		{
			Policy.Write(Context);
		}

		void Detect(std::vector<FDeviceContext>& Devices) override
		{
			Policy.Detect(Devices);
		}

		bool CreateHandle(FDeviceContext* Context) override
		{
			return Policy.CreateHandle(Context);
		}

		void InvalidateHandle(FDeviceContext* Context) override
		{
			Policy.InvalidateHandle(Context);
		}

		void ProcessAudioHapitc(FDeviceContext* Context) override
		{
			Policy.ProcessAudioHaptic(Context);
		}

		THardwarePolicy& GetPolicy() { return Policy; }
	};
} // namespace GamepadCore
