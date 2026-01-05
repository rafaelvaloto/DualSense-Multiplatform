// Copyright (c) 2025 Rafael Valoto/Publisher. All rights reserved.
// Created for: GamepadCore - Plugin to support DualSense controller on Windows.
// Planned Release Year: 2025
#pragma once
#include "GCore/Interfaces/IDeviceRegistry.h"
#include "GCore/Interfaces/IPlatformHardwareInfo.h"
#include "GCore/Types/ECoreGamepad.h"
#include "GImplementations/Libraries/DualSense/DualSenseLibrary.h"
#include "GImplementations/Libraries/DualShock/DualShockLibrary.h"
#include <ranges>
#include <vector>

namespace GamepadCore
{
	template<typename T>
	concept DeviceRegistryPolicy = requires(T t, typename T::EngineIdType id) {
		typename T::EngineIdType;

		{ t.AllocEngineDevice() } -> std::same_as<typename T::EngineIdType>;

		{ t.DisconnectDevice(id) } -> std::same_as<void>;

		{ t.DispatchNewGamepad(id) } -> std::same_as<void>;
	};

	template<typename DeviceRegistryPolicy>
	class TBasicDeviceRegistry : public IDeviceRegistry
	{
		using EngineIdType = typename DeviceRegistryPolicy::EngineIdType;
		std::unordered_map<std::string, typename DeviceRegistryPolicy::EngineIdType> KnownDevicePaths;
		std::unordered_map<std::string, typename DeviceRegistryPolicy::EngineIdType> HistoryDevices;
		std::unordered_map<typename DeviceRegistryPolicy::EngineIdType, std::shared_ptr<ISonyGamepad>, typename DeviceRegistryPolicy::Hasher> LibraryInstances;

		float TimeAccumulator = 0.0f;
		const float DetectionInterval = 2.0f;

	public:
		DeviceRegistryPolicy Policy;

		virtual ~TBasicDeviceRegistry() override = default;

		virtual void PlugAndPlay(float DeltaTime) override
		{
			TimeAccumulator += DeltaTime;
			if (TimeAccumulator < DetectionInterval)
			{
				return;
			}
			TimeAccumulator = 0.0f;

			std::unordered_set<std::string> OrphanPaths;
			OrphanPaths.clear();
			for (const auto& [Path, Key] : KnownDevicePaths)
			{
				OrphanPaths.insert(Path);
			}

			std::vector<FDeviceContext> DetectedDevices;
			DetectedDevices.clear();
			IPlatformHardwareInfo::Get().Detect(DetectedDevices);

			for (const auto& Context : DetectedDevices)
			{
				OrphanPaths.erase(Context.Path);
			}

			for (const std::string& Path : OrphanPaths)
			{
				auto It = KnownDevicePaths.find(Path);
				if (It != KnownDevicePaths.end())
				{
					EngineIdType DeviceId = It->second;
					RemoveLibraryInstance(DeviceId);
					KnownDevicePaths.erase(It);
				}
			}

			for (auto Context : DetectedDevices)
			{
				Context.Output = FOutputContext();
				if (bool IsCreateHandle = IPlatformHardwareInfo::Get().CreateHandle(&Context))
				{
					CreateLibrary(Context);
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

		void RemoveLibraryInstance(EngineIdType DeviceId)
		{
			Policy.DisconnectDevice(DeviceId);
			if (LibraryInstances.contains(DeviceId))
			{
				LibraryInstances[DeviceId]->ShutdownLibrary();
				LibraryInstances.erase(DeviceId);
			}
		}

		void RequestImmediateDetection()
		{
			TimeAccumulator = DetectionInterval;
		}

	private:
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
				HistoryDevices[Context.Path] = Policy.AllocEngineDevice();
			}

			auto DeviceId = HistoryDevices[Context.Path];
			if (!LibraryInstances.contains(DeviceId))
			{
				Gamepad->Initialize(Context);
				LibraryInstances[DeviceId] = Gamepad;
				KnownDevicePaths[Context.Path] = DeviceId;
				Policy.DispatchNewGamepad(DeviceId);
			}
		}
	};
} // namespace GamepadCore
