<div align="center">

# 🎮 Gamepad-Core

### The Ultimate Cross-Platform DualSense & DualShock API
**Pure C++ • Zero Dependencies • Engine Agnostic**

[![License:  MIT](https://img.shields.io/badge/License-MIT-green.svg?style=for-the-badge)](https://github.com/rafaelvaloto/Gamepad-Core?tab=MIT-1-ov-file)
[![C++20](https://img.shields.io/badge/C++-20-00599C.svg?style=for-the-badge&logo=c%2B%2B)](https://isocpp.org/)
[![CMake](https://img.shields.io/badge/CMake-3.20+-064F8C.svg?style=for-the-badge&logo=cmake)](https://cmake.org/)
![Windows](https://img.shields.io/badge/Windows-0078D6?style=for-the-badge&logo=windows&logoColor=white)
![Linux](https://img.shields.io/badge/Linux-FCC624?style=for-the-badge&logo=linux&logoColor=black)
![macOS](https://img.shields.io/badge/macOS-000000?style=for-the-badge&logo=apple&logoColor=white)
![PlayStation](https://img.shields.io/badge/PlayStation-003791?style=for-the-badge&logo=playstation&logoColor=white)

![Godot](https://img.shields.io/badge/Godot-478CBF?style=for-the-badge&logo=godotengine&logoColor=white)
![O3DE](https://img.shields.io/badge/O3DE-FF6D00?style=for-the-badge&logo=op3n&logoColor=white)
![Unreal Engine](https://img.shields.io/badge/Unreal_Engine-313131?style=for-the-badge&logo=unrealengine&logoColor=white)
![Unity](https://img.shields.io/badge/Unity-000000?style=for-the-badge&logo=unity&logoColor=white)

**Works with any C++ project — Game Engines, Emulators, Desktop Apps, and more**

[Features](#-features) • [Quick Start](#-quick-start) • [Integration](#-integration) • [Examples](#-real-world-projects) • [Architecture](https://github.com/rafaelvaloto/Gamepad-Core/wiki/%F0%9F%8F%9B%EF%B8%8F-Architecture-Overview)


</div>

## 🚀 What is Gamepad-Core? 

**Gamepad-Core** is a high-performance, policy-based C++ library that unlocks the **full potential** of Sony's DualSense and DualShock 4 controllers through direct HID communication. 

Unlike generic gamepad APIs (XInput, SDL, etc.), Gamepad-Core gives you **raw, low-level access** to advanced hardware features that standard drivers can't touch: 

- 🎯 **Adaptive Triggers** — Dynamic resistance and haptic feedback on L2/R2
- 💡 **RGB Lightbar** — Full LED control with per-device customization
- 🎧 **Audio Haptics** — Convert game audio into tactile feedback (USB & Wireless)
- ⚡ **High-Frequency Rumble** — Precise motor control for immersive vibration
- 🔄 **Hot-Swap Detection** — Real-time plug-and-play support
- 🎮 **Multi-Device** — Handle up to 4 controllers simultaneously

### 🌍 True Cross-Platform Architecture

Gamepad-Core is **engine-agnostic by design**.  It's a pure C++ library that works anywhere C++20 is supported: 

<div align="center">

**🎨 Game Engines**  
Unreal • Unity • Godot • O3DE • Custom Engines

**🖥️ Desktop Applications**  
Qt • wxWidgets • Electron • Native Win32/Cocoa

**🕹️ Emulators & Tools**  
RetroArch • Dolphin • RPCS3 • Controller Testers

**⚙️ Embedded Systems**  
Raspberry Pi • Steam Deck • Custom Linux Devices

</div>

The library uses **policy-based design** to abstract platform-specific details, making it trivial to add support for new platforms or hardware. 



## ✨ Features

### Advanced Hardware Control

- **Adaptive Triggers (DualSense)**  
  Full control over trigger resistance profiles:  Feedback, Weapon, Bow, Galloping, and custom effects via raw HEX commands
  
- **Audio-to-Haptic Conversion**  
  Process real-time audio buffers and convert them into haptic feedback over USB or Bluetooth
  
- **RGB Lightbar & Player LEDs**  
  Programmatically control the RGB lightbar, player indicator LEDs, and microphone mute LED
  
- **Gyroscope & Accelerometer**  
  Access raw IMU data for motion controls and gyro aiming
  
- **Battery & Connection Status**  
  Query battery level, charging state, and connection type (USB/Bluetooth)

### Performance & Reliability

- **Zero-Allocation Hot Loop**  
  Designed for high-performance scenarios where every millisecond counts
  
- **Non-Blocking I/O**  
  Asynchronous device discovery and state updates
  
- **Thread-Safe by Design**  
  Safe for use in multi-threaded game engines
  
- **Plug-and-Play**  
  Automatic device detection and reconnection without blocking the main thread

### Developer Experience

- **Policy-Based Architecture**  
  Extend the library by implementing simple platform policies — no need to modify core logic
  
- **Standalone Integration Test**  
  Pre-configured CMake target to test all features with your physical controller
  
- **Clean, Modern C++20**  
  Uses concepts, templates, and smart pointers for type-safe, maintainable code
  
- **MIT Licensed**  
  Use it freely in commercial and open-source projects



## 🎯 Real-World Projects

Gamepad-Core isn't just a proof-of-concept — it's **production-ready** and powers real projects used by thousands of developers:

### 🏆 Unreal Engine — Primary Integration

**[Unreal-Dualsense](https://github.com/rafaelvaloto/Unreal-Dualsense/tree/v2.0.0-pre-release)** (v2.0.0-pre-release)

The flagship Unreal Engine plugin that brings **every feature** of Gamepad-Core to UE5 through native Blueprint and C++ APIs.

**What's Powered by Gamepad-Core:**
- ✅ Adaptive Triggers with live console prototyping and Data Table workflows
- ✅ Audio-to-Haptics engine for real-time submix processing
- ✅ Gyroscope aiming for precision FPS controls
- ✅ Multi-controller support (up to 4 players)
- ✅ Force Feedback integration with UE's native system
- ✅ Hot-swap detection for seamless controller switching

**Example projects in Unreal Engine 5.6**
- [Arena Shooter Sample](https://drive.google.com/file/d/11iUQuWwA4zkFI_eP0roYbTDh0ss8614m/view?usp=drive_link) — Weapon recoil, damage feedback, LED effects
- [Audio Haptics Demo](https://drive.google.com/file/d/1lGUBfz42gnh2bSIJTptC_LqOdFHsPuSi/view?usp=sharing) — Bluetooth & USB audio conversion
- [Live Demo: Gyro Aiming YouTube](https://youtu.be/r_NhoODeELU?si=Ca2IOUzEDT7UFls3) — Precision ADS mechanics

> **All v2.x features are 100% powered by Gamepad-Core's API.**


### 🧩 Godot Engine — Prototype Integration

**[Godot-Dualsense](https://github.com/rafaelvaloto/Godot-Dualsense)** (GDExtension for Godot 4.x)

A prototype demonstrating Gamepad-Core's portability through native GDExtension bindings.  Shows how the library can be wrapped for any scripting environment.

---

### 💡 Your Project Could Be Next

Gamepad-Core is designed to be **drop-in ready** for any C++ codebase. Whether you're building: 

- A custom game engine
- A flight simulator with haptic feedback
- A VR application with motion controls
- A rhythm game with precise audio haptics
- A controller testing tool

**You can integrate Gamepad-Core in under 100 lines of code.**



## 🚀 Quick Start

### Try It in 5 Minutes

The fastest way to experience Gamepad-Core is through the **Integration Test**:

```bash
# 1. Clone the repository
git clone https://github.com/rafaelvaloto/Gamepad-Core.git
cd Gamepad-Core

# 2. Configure and build
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTS=ON
cmake --build build --target IntegrationTest -j

# 3. Run (make sure your DualSense/DualShock is connected)
./build/Tests/Integration/IntegrationTest
```

### 🎮 Test Controls
Once the console application is running, use your DualSense to test the features:

#### [ FACE BUTTONS ]
| Button | Action | Effect |
| :--- | :--- | :--- |
| **❌ Cross** | Rumble | Heavy Vibration + 🔴 Red Light |
| **⭕ Circle** | Lightbar | Soft Vibration + 🔵 Blue Light |
| **🟥 Square** | GameCube | Activates "GameCube-style" trigger snap on **R2** |
| **🔺 Triangle** | Reset | **Stops all effects** (Panic Button) |
| **L1 / R1** | Effects | Gallop Effect (L2) / Machine Gun (R2) |

#### [ D-PAD (Trigger Mods on R2) ]
| Button | Action | Effect |
| :--- | :--- | :--- |
| **⬆️ Up** | Trigger Effect | **Feedback** (Rigid Resistance) |
| **⬇️ Down** | Trigger Effect | **Bow** (String Tension) |
| **⬅️ Left** | Trigger Effect | **Weapon** (Semi-Automatic) |
| **➡️ Right** | Trigger Effect | **Automatic Gun** (Buzzing) |

## 💻 Integration

### Minimal Example (Standalone C++)

```cpp
#include "GCore/Templates/TBasicDeviceRegistry.h"

// 1. Choose your platform policy
#ifdef _WIN32
    #include "Examples/Platform_Windows/test_windows_hardware_policy.h"
    using HardwarePolicy = FWindowsPlatform::FWindowsHardwarePolicy;
    using HardwareInfo = FWindowsPlatform::FWindowsHardware;
#elif __linux__
    #include "Examples/Platform_Linux/LinuxHardwarePolicy.h"
    using HardwarePolicy = FLinuxPlatform::FLinuxHardwarePolicy;
    using HardwareInfo = FLinuxPlatform::FLinuxHardware;
#endif

// 2. Define your registry policy
#include "Examples/Adapters/Tests/test_device_registry_policy.h"
using DeviceRegistry = GamepadCore::TBasicDeviceRegistry<Test_DeviceRegistryPolicy>;

int main() {
    // Initialize hardware layer
    auto Hardware = std::make_unique<HardwareInfo>();
    IPlatformHardwareInfo:: SetInstance(std::move(Hardware));

    // Create device registry
    auto Registry = std::make_unique<DeviceRegistry>();

    // Game loop
    while (true) {
        float DeltaTime = 0.016f; // 60 FPS
        
        // Scan for connected devices
        Registry->PlugAndPlay(DeltaTime);

        // Get first connected gamepad
        if (auto* Gamepad = Registry->GetLibrary(0)) {
            if (Gamepad->IsConnected()) {
                // Update input state
                Gamepad->UpdateInput(DeltaTime);
                
                // Read button state
                auto Context = Gamepad->GetMutableDeviceContext();
                auto Input = Context->GetInputState();
                
                if (Input. bCross) {
                    // Trigger haptic feedback
                    Gamepad->SetLightbar({255, 0, 0});
                    Gamepad->SetRumble(255, 128);
                }
                
                // Control adaptive triggers
                if (auto* Trigger = Gamepad->GetIGamepadTrigger()) {
                    Trigger->SetWeapon(EDSGamepadHand::Right, 2, 200, 80);
                }
            }
        }
        
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }
}
```

### Platform Policy Structure

Gamepad-Core uses **policies** to abstract OS-specific code:

```cpp
struct MyCustomHardwarePolicy {
    // Enumerate connected devices
    static void EnumerateDevices(TArray<FDeviceInfo>& OutDevices);
    
    // Open HID connection
    static void* OpenDevice(const FDeviceInfo& Device);
    
    // Read/Write HID reports
    static bool ReadReport(void* Handle, uint8_t* Buffer, size_t Size);
    static bool WriteReport(void* Handle, const uint8_t* Buffer, size_t Size);
};
```

This design makes it trivial to support **custom platforms** (e.g., PlayStation SDK, proprietary embedded systems) without touching core logic.


### Design Philosophy

Gamepad-Core follows **strict separation of concerns** to ensure portability and extensibility:

```
┌─────────────────────────────────────────────────────────────┐
│                    Your Application                         │
│              (Game Engine, Desktop App, Tool)               │
└────────────────────────┬────────────────────────────────────┘
                         │
         ┌───────────────┴───────────────┐
         │   Adapter Layer (Policy)      │  ◄── You implement this
         │  (Engine-specific bindings)   │      (or use examples)
         └───────────────┬───────────────┘
                         │
         ┌───────────────┴───────────────┐
         │      GCore (Abstract)         │
         │  • Device Registry            │  ◄── Pure C++, stable API
         │  • ISonyGamepad Interface     │
         │  • IGamepadTrigger Interface  │
         └───────────────┬───────────────┘
                         │
         ┌───────────────┴───────────────┐
         │   GImplementations (Drivers)  │
         │  • DualSense HID Protocol     │  ◄── Hardware-specific
         │  • DualShock 4 HID Protocol   │
         └───────────────┬───────────────┘
                         │
         ┌───────────────┴───────────────┐
         │   Platform Policy (OS/HAL)    │  ◄── OS-specific I/O
         │  • Windows (SetupAPI + HID)   │
         │  • Linux (HIDAPI)             │
         │  • macOS (IOKit)              │
         │  • Custom (PS5 SDK, etc.)     │
         └───────────────────────────────┘
```

### Key Components

#### 1. **GCore** — The Stable Abstraction
Pure C++ interfaces and templates.  **Completely OS and engine-agnostic.**

```cpp
class ISonyGamepad {
    virtual void UpdateInput(float DeltaTime) = 0;
    virtual void SetLightbar(FColor RGB) = 0;
    virtual void SetRumble(uint8_t Low, uint8_t High) = 0;
    virtual IGamepadTrigger* GetIGamepadTrigger() = 0;
    // ... 
};
```

#### 2. **GImplementations** — The Hardware Drivers
Contains the raw HID protocol logic for each controller: 
- **SonyGamepadAbstract** — Shared logic between DualShock 4 and DualSense
- **DualSenseLibrary** — Adaptive Triggers, Haptics, LED control
- **DualShock4Library** — Classic force feedback and lightbar

#### 3. **Platform Policy** — The OS Bridge
Tells the library **how** to discover and communicate with devices on your platform:
- **Windows:** Uses `SetupAPI` and `hid.dll`
- **Linux:** Uses `hidapi` or `libusb`
- **macOS:** Uses `IOKit`
- **Custom:** Implement your own for proprietary SDKs

---

## 🧪 Building from Source

### Prerequisites

- **CMake** 3.20 or higher
- **C++20** compatible compiler (MSVC, GCC, Clang)
- **Ninja** (recommended) or Make

### Build Commands

```bash
# Configure (Debug)
cmake -S . -B build/debug -DCMAKE_BUILD_TYPE=Debug

# Configure (Release)
cmake -S . -B build/release -DCMAKE_BUILD_TYPE=Release

# Compile the library
cmake --build build/release --target GamepadCore -j

# Run integration tests (hardware required)
cmake --build build/debug --target IntegrationTest -j
./build/debug/Tests/Integration/IntegrationTest
```

### Format Code (Contributors)

```bash
cmake --build build/debug --target GamepadCoreFormat -j
```

---

## 🤝 Contributing

Contributions are welcome! Whether you want to:

- Add support for a new platform (e.g., FreeBSD, Android)
- Improve documentation or examples
- Optimize HID communication
- Report bugs or suggest features

Feel free to open an **Issue** or submit a **Pull Request**. 

### Guidelines

1. Follow the existing code style (use `clang-format`)
2. Test your changes with a physical controller
3. Update documentation if you add new features
4. Keep commits focused and well-described

---

## 📄 License

![License](https://img.shields.io/badge/License-MIT-green?style=for-the-badge)

This project is licensed under the **MIT License**. See the `LICENSE` file for details.

Copyright (c) 2025 **Rafael Valoto**


---

## ⚖️ Legal & Trademarks

This software is an **independent project** and is **not affiliated** with Sony Interactive Entertainment Inc., Epic Games, Unity Technologies, Godot Engine, or any of their subsidiaries.

**Trademarks belong to their respective owners:**

- **Sony:** PlayStation, DualSense, DualShock are trademarks of Sony Interactive Entertainment Inc.
- **Microsoft:** Windows, Xbox are trademarks of Microsoft Corporation
- **Apple:** macOS is a trademark of Apple Inc.
- **Epic Games:** Unreal Engine is a trademark of Epic Games, Inc. 
- **Unity:** Unity is a trademark of Unity Technologies
- **Godot:** Godot and the Godot logo are trademarks of the Godot Engine project

---

<div align="center">

### 🌟 If Gamepad-Core powers your project, consider giving it a star! 

[⬆ Back to Top](#-gamepad-core)

</div>
