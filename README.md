<h1 align="center"><i>GamepadCore_</i></h1>

<p align="center">
Modern, policy-based C++ library for advanced gamepad features (DualSense/DS4). Engine-agnostic architecture designed for Unreal, Unity, Godot, and O3DE.
<br />
<br />

<div align="center">

![Windows](https://img.shields.io/badge/Windows-0078D6?style=for-the-badge&logo=windows&logoColor=white)
![Linux](https://img.shields.io/badge/Linux-FCC624?style=for-the-badge&logo=linux&logoColor=black)
![macOS](https://img.shields.io/badge/macOS-000000?style=for-the-badge&logo=apple&logoColor=white)
![PlayStation](https://img.shields.io/badge/PlayStation-003791?style=for-the-badge&logo=playstation&logoColor=white)

![Godot](https://img.shields.io/badge/Godot-478CBF?style=for-the-badge&logo=godotengine&logoColor=white)
![O3DE](https://img.shields.io/badge/O3DE-FF6D00?style=for-the-badge&logo=op3n&logoColor=white)
![Unreal Engine](https://img.shields.io/badge/Unreal_Engine-313131?style=for-the-badge&logo=unrealengine&logoColor=white)
![Unity](https://img.shields.io/badge/Unity-000000?style=for-the-badge&logo=unity&logoColor=white)


</div>

---

**GamepadCore** is a modern, **policy-based C++ library** designed to provide unified access to advanced gamepad features (DualSense, DualShock 4) across multiple platforms and game engines.

It decouples **Hardware Logic** from **Engine Logic**, allowing you to write your input code once and run it anywhere.

## 🚀 Key Features

* **⚡ Engine Agnostic:** Zero dependencies on specific game engines in the Core logic.
* **🧩 Policy-Based Design:**
    * **Registry Policy:** Adapt container types (e.g., `TMap`, `std::map`) and ID types (e.g., `FInputDeviceId`, `int`) to fit your engine's ecosystem.
    * **Hardware Policy:** Inject platform-specific drivers (Windows HID, Linux udev, PS5 SDK) without modifying the core code.
* **🎮 Advanced Support:** Native support for DualSense Adaptive Triggers, Haptics, and Lightbar.
* **🏎️ Zero-Cost Abstraction:** Heavy use of templates ensures logic is resolved at compile-time for maximum performance.

## 📂 Project Structure

The project is structured to strictly separate the **Public API** (Contracts & Templates) from the **Private Implementation** (Logic & Drivers).

```text
GamepadCore/
├── Source/
│   ├── Private/                            # Internal Implementation (Hidden from API consumers)
│   │   ├── Core/
│   │   │   ├── Algorithms/                 # Sensor fusion (e.g., Madgwick IMU)
│   │   │   └── Interfaces/                 # Hardware abstraction implementation
│   │   └── Implementations/
│   │       ├── Libraries/                  # Logic for specific controllers
│   │       │   ├── Base/                   # Abstract Sony Gamepad logic
│   │       │   ├── DualSense/              # DualSense specific driver logic
│   │       │   └── DualShock/              # DualShock 4 specific driver logic
│   │       └── Utils/                      # Output report construction
│   │
│   └── Public/                             # The API you include in your project
│       ├── Core/
│       │   ├── Algorithms/                 # Header definitions for algorithms
│       │   ├── Interfaces/                 # Pure virtual contracts (The "What")
│       │   │   ├── Segregations/           # Granular feature interfaces (Audio, Haptics, Lightbar...)
│       │   │   └── ISonyGamepad.h          # Main controller interface
│       │   ├── Templates/                  # Policy-based templates (The "How")
│       │   │   ├── TBasicDeviceRegistry.h  # Engine-agnostic device container
│       │   │   └── TGenericHardwareInfo.h  # Hardware injection points
│       │   └── Types/                      # Data structures and Enums
│       │       ├── ECoreGamepad.h          # Supported device types
│       │       └── Structs/
│       │           ├── Config/             # Feature configuration structs (Triggers, LEDs, etc.)
│       │           └── Context/            # Input/Output state contexts
│       └── Implementations/                # Concrete Classes exposed to the user
│           ├── Libraries/                  # Public headers for controller libraries
│           └── Utils/                      # Utility helpers
│
└── Adapters/                               # Reference Implementations (Not part of Core)
    ├── Unreal/                             # Example: How to wrap Core for Unreal
    ├── Unity/                              # Example: C++ to C# Marshaling
    └── Godot/                              # Example: GDExtension binding
```

## 🛠️ Usage Example (Concept)

1. Initialize with Injection
GamepadCore uses dependency injection to link the Core with your specific platform/engine environment.
```cpp
   // In your Engine's Startup Module (e.g., Unreal Engine)
   void StartupModule()
   {
   // 1. Inject Hardware Driver (Windows, Linux, or Console)
   IPlatformHardwareInfo::SetInstance(new FWindowsHardwareDriver());

   // 2. Initialize Registry with Engine Policy
   // Here we tell the core to use Unreal's TMap and FInputDeviceId
   Registry = std::make_unique<TBasicDeviceRegistry<FUnrealRegistryPolicy>>();
   }
```

2. The Policy (Your Engine Adapter)
   You only need to define how to store the device. The Core handles when to connect it.
```cpp
// Example implementation of a Policy method
EngineIdType AllocEngineDevice(CoreDeviceId CoreId) 
{
    // The Core detected a physical device! Now we tell the engine.
    int32_t NewId = EngineInputSystem::AddDevice();
    return NewId;
}

void DispatchNewGamepad(CoreDeviceId CoreId) 
{
    // Notify the game via Delegate/Event
    OnControllerConnected.Broadcast();
}

void FreeEngineDevice(CoreDeviceId CoreId, EngineIdType EngineId) 
{
    // Cleanup when unplugged
    EngineInputSystem::RemoveDevice(EngineId);
}
```

## 🤝 Integration & Adapters

The `Adapters/` directory contains **reference implementations** and examples. These files are **not** compiled with the core library but serve as a guide on how to bridge `GamepadCore` to your target engine.

* **Unreal Engine:** Examples showing how to map `FInputDeviceId` and `CoreDelegates`.
* **Unity:** Examples of a Native Plugin Interface with C# callbacks.
* **Godot:** GDExtension reference code.

You are encouraged to copy these adapters into your project and modify them to suit your specific architectural needs.

## 📄 License

![License](https://img.shields.io/badge/License-MIT-green?style=for-the-badge)

This project is licensed under the **MIT License**. See the `LICENSE` file for details.

Copyright (c) 2025 **Rafael Valoto**

---

## ⚖️ Disclaimer and Trademarks

This software is an independent and unofficial project. It is **not** affiliated, associated, authorized, endorsed by, or in any way officially connected with Sony Interactive Entertainment Inc., Microsoft Corporation, Apple Inc., Epic Games, Unity Technologies, the Godot Engine project, or the Open 3D Foundation.

**Trademarks belong to their respective owners:**

* **Sony:** "PlayStation", "PlayStation Family Mark", "PS5 logo", "PS5", "DualSense", and "DualShock" are registered trademarks or trademarks of Sony Interactive Entertainment Inc. "SONY" is a registered trademark of Sony Corporation.
* **Microsoft:** "Windows" and "Xbox" are registered trademarks of Microsoft Corporation.
* **Apple:** "Mac" and "macOS" are registered trademarks of Apple Inc.
* **Linux:** "Linux" is the registered trademark of Linus Torvalds in the U.S. and other countries.
* **Epic Games:** "Unreal" and "Unreal Engine" are trademarks or registered trademarks of Epic Games, Inc. in the United States of America and elsewhere.
* **Unity:** "Unity", Unity logos, and other Unity trademarks are trademarks or registered trademarks of Unity Technologies or its affiliates in the U.S. and elsewhere.
* **Godot:** "Godot" and the Godot logo are trademarks of the Godot Engine project.
* **O3DE:** "O3DE" and the O3DE logo are trademarks of the Open 3D Foundation.
