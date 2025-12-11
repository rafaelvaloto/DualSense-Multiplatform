// Copyright (c) 2025 Rafael Valoto/Publisher. All rights reserved.
// Created for: GamepadCore - Plugin to support DualSense controller on Windows.
// Planned Release Year: 2025
#pragma once
#include "Core/Interfaces/IDeviceRegistry.h"
#include "Core/Interfaces/IPlatformHardwareInfo.h"
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
			t.DisconnectDevice(T::EngineIdType)
		} -> std::same_as<void>;
		{
			t.DispathNewGamepad(T::EngineIdType)
		} -> std::same_as<void>;
		{t.Get(T::EngineIdType)};
	};

	template<typename DeviceRegistryPolicy>
	class TBasicDeviceRegistry : public IDeviceRegistry
	{
		DeviceRegistryPolicy Policy;
		std::unordered_map<std::string, std::int32_t> HistoryDevices;
		std::unordered_map<std::string, std::int32_t> KnownDevicePaths;
		std::unordered_map<std::int32_t, std::shared_ptr<ISonyGamepad>> LibraryInstances;

		float TimeAccumulator = 0.0f;
		const float DetectionInterval = 2.0f;

	public:
		~TBasicDeviceRegistry() override
		{
			std::vector<std::int32_t> WatcherKeys;
			WatcherKeys.reserve(LibraryInstances.size());

			for (const auto& [Key, _] : LibraryInstances)
			{
				WatcherKeys.push_back(Key);
			}

			for (const auto& ControllerId : WatcherKeys)
			{
				TBasicDeviceRegistry<DeviceRegistryPolicy>::RemoveLibraryInstance(ControllerId);
			}
		}

		void PlugPlay(float DeltaTime) override
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
						CreateLibraryInstance(Context);
					}
				}
			}
		}

		void CreateLibraryInstance(FDeviceContext& Context) override
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
				HistoryDevices.insert(Context.Path, NewDevice);
			}

			if (!Gamepad->Initialize(Context))
			{
				return;
			}

			auto DeviceId = HistoryDevices[Context.Path];
			KnownDevicePaths[Context.Path] = DeviceId;
			LibraryInstances[DeviceId] = Gamepad;
			Policy.DispathNewGamepad(DeviceId);
		}

		void RemoveLibraryInstance(const CoreDeviceId& DeviceId) override
		{
			Policy.DisconnectDevice(DeviceId);
			if (LibraryInstances.contains(DeviceId))
			{
				LibraryInstances[DeviceId]->ShutdownLibrary();
				LibraryInstances.erase(DeviceId);
			}
		}

		ISonyGamepad* GetLibraryInstance(const CoreDeviceId& DeviceId) override
		{
			return LibraryInstances[DeviceId].get();
		}

		// Acesso para o Adapter pegar o objeto real
		auto GetGamepad(typename DeviceRegistryPolicy::EngineIdType Id)
		{
			return Policy.Get(Id);
		}

		DeviceRegistryPolicy::EngineIdType& GetPolicy() { return Policy; }
	};
} // namespace GamepadCore
