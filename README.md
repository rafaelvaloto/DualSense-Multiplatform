<h1 align="center">GamepadCore 🕹️</h1>

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

**GamepadCore** is a high-performance, platform-agnostic C++ library designed to handle raw HID communication with game controllers. It bypasses generic abstraction layers (like XInput or SDL) to unlock hardware-specific features often inaccessible in standard APIs.

> 🚀 **Battle-Tested & Engine Agnostic:**
> While currently powering the [Godot-Dualsense](https://github.com/rafaelvaloto/Godot-Dualsense) GDExtension, this core logic is built to be easily integrated into **Unreal Engine**, **Unity** (via Native Plugin), custom engines, or standalone C++ applications.

## 🔌 Integrations & Showcase

Since **GamepadCore** is decoupled from game engines, it serves as the backend logic for multiple implementations. See it in action:

| Engine | Project                                                                                                | Description |
| :--- |:-------------------------------------------------------------------------------------------------------| :--- |
| <img src="https://upload.wikimedia.org/wikipedia/commons/6/6a/Godot_icon.svg" width="40"/> **Godot** | [**Godot-Dualsense**]([LINK_DO_SEU_REPO_GODOT](https://github.com/rafaelvaloto/Godot-Dualsense))                    | A GDExtension wrapper exposing features to GDScript via Signals. |
| <img src="https://upload.wikimedia.org/wikipedia/commons/2/20/UE_Logo_Black_Centered.svg" width="40"/> **Unreal** | [**Unreal-Dualsense**](https://github.com/rafaelvaloto/WindowsDualsenseUnreal/tree/v2.0.0-development) | A UE5 Plugin implementation demonstrating how to map raw HID to Unreal's Subsystem and Input mappings. |

> 💡 **Why this matters:** This proves that the core logic (HID parsing, Haptics, Trigger math) is written once and reused everywhere.

## ✨ Key Features
This library implements the raw protocol logic to communicate directly with hardware, offering features that standard drivers miss:

* 🏗️ Extensible Multi-Platform Architecture: Flexible design using policy-based templates. Supporting new hardware or platforms is as simple as implementing a connection interface.

* 🔌 Dynamic Hot-Swap: Automatically detects controller connection and disconnection events in real-time without stalling the main loop.

* 🎯 Adaptive Triggers: Full native control over resistance, vibration, and "weapon recoil" effects on DualSense L2/R2 triggers.

* 💡 Lightbar & LED Control: Direct programmatic access to RGB Lightbar, Player LEDs, and Microphone Mute LED.

* ⚡ Optimized Performance: Zero-allocation loop architecture designed for high-performance scenarios, ideal for multiplayer environments where latency matters.

* ⚙️ Force Feedback: Low-level command generation for standard rumble motors (High/Low frequency).

* 🤝 Non-Intrusive: Designed to coexist with existing input managers (like Windows RawInput, UE Enhanced Input, or SDL), preventing device conflict.
## 🏛️ Architecture Overview

The library follows a strict separation of concerns to ensure portability. By using **Policy-Based Design**, the core logic remains pure C++, while platform-specific details (like how Windows handles USB vs. how Linux handles HID) are injected externally.

### 1. GCore (Abstract Layer)
The stable heart of the library. It defines generic interfaces (`IGamepad`, `IGamepadTrigger`) and the Device Registry logic. It knows **nothing** about the specific OS or Engine, ensuring the code is completely portable.

### 2. Platform Policy (The Bridge)
This is where the flexibility lies. You provide a "Policy" class that tells the Core *how* to identify and communicate with devices on your specific environment.
* **Example:** `WindowsHardwarePolicy` uses `setupapi.h`, while a custom `GodotRegistryPolicy` hooks into Godot's internal callbacks.

### 3. Implementations (The Drivers)
Contains the specific HID protocol logic (byte arrays) for the hardware itself.
* **SonyGamepadAbstract:** Handles logic shared between PS4/PS5.
* **DualSenseLibrary:** Handles specific DualSense features (Adaptive Triggers, Haptics, Lightbar) by interpreting raw input/output reports.

## 💻 Integration Example
1. Basic Setup (C++ Standalone)
   To use GamepadCore in your project, you instantiate the registry with a specific policy.
2. Continuous Device Discovery (Game Loop)
   The registry periodically scans for new devices and updates their connection states.
   #include "GCore/Templates/TBasicDeviceRegistry.h"
   #include "Platforms/Windows/WindowsHardwarePolicy.h"
```cpp
// Define a registry using the Windows Policy
using MyDeviceRegistry = GamepadCore::TBasicDeviceRegistry<WindowsHardwarePolicy>;

int main() {
auto registry = std::make_unique<MyDeviceRegistry>();

    // Main Loop
    while (true) {
        // 1. Detect devices
        registry->PlugAndPlay(0.016f); // DeltaTime
        
        // 2. Access a connected gamepad
        auto* gamepad = registry->GetLibrary(0); // ID 0
        
        if (gamepad) {
            // Send specific HID command (e.g., Set Lightbar to Red)
            gamepad->SetLightbarColor(255, 0, 0); 
            
            // Trigger Effect: Resistance
            gamepad->GetIGamepadTrigger()->SetTriggerEffect(
                TriggerId::Right, 
                TriggerMode::Resistance, 
                0, 255
            );
        }
    }
}
```

## 🧑‍💻 Contributing (Build & sanity checks)

GamepadCore is meant to be consumed **from source** (e.g., compiled inside an engine/plugin build).  
This repository still provides a CMake project so contributors can quickly validate changes locally.

### Requirements
- CMake >= 3.20
- C++20 compiler (Clang/GCC/MSVC)
- Ninja (recommended) or Make

### 1) Configure (one time per build type)

```bash
cmake -S . -B cmake-build-debug -DCMAKE_BUILD_TYPE=Debug
cmake -S . -B cmake-build-release -DCMAKE_BUILD_TYPE=Release
```

### 2) Format (before pushing)
```bash
cmake --build cmake-build-debug --target GamepadCoreFormat -j
```
### 3) Compile (after any change)

```bash
cmake --build cmake-build-debug --target GamepadCore -j
cmake --build cmake-build-release --target GamepadCore -j
````

### Notes
- Build artifacts (like `libGamepadCore.a`) are generated inside the build folder (e.g. `cmake-build-release/Source/`).
- Contributors generally should **not** commit build directories (`cmake-build-*`).


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
