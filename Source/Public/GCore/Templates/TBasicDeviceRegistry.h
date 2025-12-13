// Copyright (c) 2025 Rafael Valoto/Publisher. All rights reserved.
// Created for: GamepadCore - Plugin to support DualSense controller on Windows.
// Planned Release Year: 2025
#pragma once
#include "../GCore/Interfaces/IDeviceRegistry.h"
#include "GCore/Interfaces/IPlatformHardwareInfo.h"
#include "GCore/Types/ECoreGamepad.h"
#include "Implementations/Libraries/DualSense/DualSenseLibrary.h"
#include "Implementations/Libraries/DualShock/DualShockLibrary.h"
#include <ranges>
#include <vector>

namespace GamepadCore
{
	template<typename T>
	concept DeviceRegistryPolicy = requires(T t)
	{
		typename T::EngineIdType;

		{
			t.AllocEngineDevice()
		} -> std::same_as<typename T::EngineIdType>;

		{
			t.DisconnectDevice(std::declval<typename T::EngineIdType>())
		} -> std::same_as<void>;

		{
			t.DispatchNewGamepad(std::declval<typename T::EngineIdType>())
		} -> std::same_as<void>;
	};

	template<typename DeviceRegistryPolicy>
	class TBasicDeviceRegistry : public IDeviceRegistry
	{
		DeviceRegistryPolicy Policy;
		using EngineIdType = typename DeviceRegistryPolicy::EngineIdType;
		std::unordered_map<std::string, typename DeviceRegistryPolicy::EngineIdType> KnownDevicePaths;
		std::unordered_map<std::string, typename DeviceRegistryPolicy::EngineIdType> HistoryDevices;
		std::unordered_map<typename DeviceRegistryPolicy::EngineIdType, std::shared_ptr<ISonyGamepad>, typename DeviceRegistryPolicy::Hasher> LibraryInstances;

		float TimeAccumulator = 0.0f;
		const float DetectionInterval = 2.0f;

	public:
		virtual ~TBasicDeviceRegistry() override = default;

		virtual void PlugAndPlay(float DeltaTime) override
		{
			TimeAccumulator += DeltaTime;
			if (TimeAccumulator < DetectionInterval)
			{
				return;
			}
			TimeAccumulator = 0.0f;

			std::vector<FDeviceContext> DetectedDevices;
			IPlatformHardwareInfo::Get().Detect(DetectedDevices);

			std::unordered_set<std::string> CurrentlyConnectedPaths;
			for (const auto& Context : DetectedDevices)
			{
				CurrentlyConnectedPaths.insert(Context.Path);
			}

			std::vector<std::string> DisconnectedPaths;
			for (const auto& [Path, Key] : KnownDevicePaths)
			{
				if (!CurrentlyConnectedPaths.contains(Path))
				{
					if (LibraryInstances.contains(Key))
					{
						Policy.DisconnectDevice(Key);
						DisconnectedPaths.push_back(Path);
					}
				}
			}

			for (const auto& Path : DisconnectedPaths)
			{
				if (KnownDevicePaths.contains(Path))
				{
					KnownDevicePaths.erase(Path);
				}
			}

			for (FDeviceContext& Context : DetectedDevices)
			{
				if (!KnownDevicePaths.contains(Context.Path))
				{
					Context.Output = FOutputContext();
					if (IPlatformHardwareInfo::Get().CreateHandle(&Context))
					{
						CreateLibrary(Context);
					}
				}
			}
		}

		ISonyGamepad* GetLibrary(EngineIdType DeviceId)
		{
			if (LibraryInstances.contains(DeviceId))
			{
				return LibraryInstances[DeviceId].get();
			}
			return nullptr;
		}

	private:
		void RemoveLibraryInstance(EngineIdType DeviceId)
		{
			Policy.DisconnectDevice(DeviceId);
			if (LibraryInstances.contains(DeviceId))
			{
				LibraryInstances[DeviceId]->ShutdownLibrary();
				LibraryInstances.erase(DeviceId);
			}
		}

		void CreateLibrary(FDeviceContext& Context)
		{
			std::shared_ptr<ISonyGamepad> Gamepad = nullptr;
			if (Context.DeviceType == EDSDeviceType::DualSense || Context.DeviceType == EDSDeviceType::DualSenseEdge)
			{
				Gamepad = std::make_shared<FDualSenseLibrary>();
			}

			if (Context.DeviceType == EDSDeviceType::DualShock4)
			{
				Gamepad = std::make_shared<FDualShockLibrary>();
			}

			if (!Gamepad)
			{
				return;
			}

			if (!HistoryDevices.contains(Context.Path))
			{
				auto NewDevice = Policy.AllocEngineDevice();
				HistoryDevices[Context.Path] = NewDevice;
			}

			if (!Gamepad->Initialize(Context))
			{
				return;
			}

			auto DeviceId = HistoryDevices[Context.Path];
			KnownDevicePaths[Context.Path] = DeviceId;
			LibraryInstances[DeviceId] = Gamepad;

			Policy.DispatchNewGamepad(DeviceId);
		}
	};
} // namespace GamepadCore
