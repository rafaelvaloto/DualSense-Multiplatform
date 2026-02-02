<div align="center">

# 🎮 Gamepad-Core

### The Ultimate Cross-Platform DualSense & DualShock API
**Pure C++ • Zero Dependencies • Engine Agnostic**

[![Build Status](https://img.shields.io/github/actions/workflow/status/rafaelvaloto/Gamepad-Core/build.yml?style=for-the-badge&logo=github&label=Build&color=2ea44f)](https://github.com/rafaelvaloto/Gamepad-Core/actions)
[![CodeQL](https://img.shields.io/github/actions/workflow/status/rafaelvaloto/Gamepad-Core/build.yml?style=for-the-badge&logo=github&label=CodeQL&color=2ea44f)](https://github.com/rafaelvaloto/Gamepad-Core/security/code-scanning)
[![License: MIT](https://img.shields.io/badge/License-MIT-green.svg?style=for-the-badge)](https://github.com/rafaelvaloto/Gamepad-Core/blob/main/LICENSE)

---

[Report Bug](https://github.com/rafaelvaloto/Gamepad-Core/issues) · [Suggest a Feature](https://github.com/rafaelvaloto/Gamepad-Core/pulls) · [Documentation](https://github.com/rafaelvaloto/Gamepad-Core/wiki) · [Security Policy](https://github.com/rafaelvaloto/Gamepad-Core/security/policy)

**Core Tech Stack**
<br>
[![C++20](https://img.shields.io/badge/C++-20-00599C.svg?style=for-the-badge&logo=c%2B%2B)](https://isocpp.org/)
[![CMake](https://img.shields.io/badge/CMake-3.20+-064F8C.svg?style=for-the-badge&logo=cmake)](https://cmake.org/)

**Supported Platforms**
<br>
![Windows](https://img.shields.io/badge/Windows-0078D6?style=for-the-badge&logo=windows&logoColor=white)
![Linux](https://img.shields.io/badge/Linux-FCC624?style=for-the-badge&logo=linux&logoColor=black)
![macOS](https://img.shields.io/badge/macOS-000000?style=for-the-badge&logo=apple&logoColor=white)
![PlayStation](https://img.shields.io/badge/PlayStation-003791?style=for-the-badge&logo=playstation&logoColor=white)

**Game Engine Ready**
<br>
![Unreal Engine](https://img.shields.io/badge/Unreal_Engine-313131?style=for-the-badge&logo=unrealengine&logoColor=white)
![Godot](https://img.shields.io/badge/Godot-478CBF?style=for-the-badge&logo=godotengine&logoColor=white)
![Unity](https://img.shields.io/badge/Unity-000000?style=for-the-badge&logo=unity&logoColor=white)
![O3DE](https://img.shields.io/badge/O3DE-FF6D00?style=for-the-badge&logo=op3n&logoColor=white)

**Works with any C++ project — Game Engines, Emulators, Desktop Apps, and more**

[Features](#-features) • [Quick Start](#-quick-start) • [Integration](#-integration) • [Examples](#-real-world-projects) • [Architecture](#design-philosophy)


---

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
- 🎚️ **Factory Calibration** — Automatic calibration on connection, no setup required
- 🎤 **Smart Mute Detection** — Automatic mute LED control, no coding required

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

### 🧱 Policy-Based Architecture
The library leverages **policy-based design** to abstract platform-specific details. This zero-cost abstraction makes it trivial to extend support to new platforms or custom hardware without touching the core logic.

---

## ✨ Features

### 🎮 Advanced Hardware Control
Unlock the full potential of the DualSense controller with direct access to hardware features.

* **Adaptive Triggers:** Full control over resistance profiles including Feedback, Weapon, Bow, Galloping, and raw HEX commands.
* **Audio-to-Haptic:** Process real-time audio buffers and convert them into high-fidelity haptic feedback (USB & Bluetooth).
* **RGB & LEDs:** Programmatic control over the Lightbar, player indicator LEDs, and microphone mute status.
* **IMU Sensors:** Access raw Gyroscope & Accelerometer data for motion controls and precision aiming.
* **Power Management:** Query battery level, charging status, and connection type.

### ⚡ Performance & Reliability
Built for high-performance engines where every millisecond counts.

* **Zero-Allocation Hot Loop:** No memory allocation during the update loop to prevent GC spikes or latency.
* **Non-Blocking I/O:** Asynchronous device discovery and state updates.
* **Thread-Safe:** Designed from the ground up for multi-threaded game engines.
* **Plug-and-Play:** Hot-swap support with automatic device detection and reconnection.

### 🛠️ Developer Experience
* **Modern C++20:** Built with concepts, templates, and smart pointers for type safety.
* **Extensible:** Implement simple platform policies to add new support.
* **Test Suite:** Includes a standalone integration test target (CMake) to verify features on physical hardware.
* **MIT Licensed:** Free for commercial and open-source use.

---

## 🎯 Real-World Projects
**Gamepad-Core** is production-ready and currently powers projects used by the gaming community.

### 🛹 [Session: Skate Sim - Native DualSense Mod](https://github.com/rafaelvaloto/Gaming-Mods-Dualsense)
A specialized implementation tailored for *Session: Skate Sim*, demonstrating the library's ability to enhance gameplay physics.

* **Truck Physics on Triggers:** Adaptive triggers stiffen dynamically to simulate truck resistance while turning.
* **Haptic Texture:** Audio-based haptics allow players to "feel" the pop, landing, and grind friction on different surfaces.

### 🚀 Build Mods for Your Favorite Games
Want to create a similar mod for another game?

The **Session Mod** repository above serves as a perfect **Template**. You can fork it or review the source code to learn how to:
1.  Initialize the GamepadCore library within a DLL.
2.  Hook into game events (like Unreal Engine physics).
3.  Translate game audio/physics into Haptics and Trigger effects.

**[👉 Start by cloning the Session Mod Repo](https://github.com/rafaelvaloto/Gaming-Mods-Dualsense)**

### 🏆 Unreal Engine — Primary Integration
**[Unreal-Dualsense](https://github.com/rafaelvaloto/Unreal-Dualsense/tree/v2.0.0-pre-release)** (v2.0.0-pre-release)

The flagship plugin bringing native DualSense support to UE5 via Blueprint and C++.
* ✅ Live Adaptive Trigger prototyping via Data Tables.
* ✅ Real-time Audio-to-Haptics submix processing.
* ✅ Native Input System integration for Force Feedback & Motion.

### ⚙️ O3DE (Open 3D Engine) — Gem Integration

**[O3DE-Dualsense](https://github.com/rafaelvaloto/o3de-dualsense)** (O3DE Gem)

A native O3DE Gem that brings DualSense and DualShock 4 support to the Open 3D Engine. Demonstrates Gamepad-Core's flexibility with O3DE's modular Gem architecture.

**Features:**
- ✅ Full adaptive trigger support
- ✅ RGB lightbar and player LED control
- ✅ Haptic feedback integration
- ✅ Native O3DE EBus system integration
- ✅ Blueprint-compatible scripting bindings

### 🧩 Godot Engine — Prototype Integration

**[Godot-Dualsense](https://github.com/rafaelvaloto/Godot-Dualsense)** (GDExtension for Godot 4.x)

A prototype demonstrating Gamepad-Core's portability through native GDExtension bindings.  Shows how the library can be wrapped for any scripting environment.

---


## 🚀 Quick Start

### Try It in 5 Minutes

The fastest way to experience Gamepad-Core is through the **Integration Test**:

```bash
# 1. Clone the repository
git clone https://github.com/rafaelvaloto/Gamepad-Core.git
cd Gamepad-Core

# 2. Configure and build
cmake -S . -B cmake-build-release -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTS=ON
cmake --build cmake-build-release --target test-gamepad-outputs -j

# 3. Run (make sure your DualSense/DualShock is connected)
./cmake-build-release/Tests/Integration/test-gamepad-outputs
```

### 🎮 Test Controls
Once the console application is running, use your DualSense to test the features:

### Input Testing (test-gamepad-inputs)
The `test-gamepad-inputs` executable allows you to monitor controller data in real-time. To avoid log misalignment due to terminal width limits, **it is highly recommended to test one parameter at a time**.

**Usage:**
```bash
./test-gamepad-inputs [flags]
```

**Available Flags:**

| Flag | Description |
| :--- | :--- |
| `--buttons` | Displays digital button states (Cross, Circle, etc.) |
| `--analogs` | Displays stick and trigger positions (Default if no flags) |
| `--touch` | Displays detailed touchpad data (ID, Fingers, Position, Velocity) |
| `--sensors` | Displays Motion Sensor data (Gyroscope and Accelerometer) |

*Note: When `--touch` or `--sensors` are passed, the respective hardware features are automatically enabled on the controller.*

---

### Output Testing (test-gamepad-outputs)
The `test-gamepad-outputs` executable allows you to test various controller feedback mechanisms, including vibrations, lightbar colors, and adaptive triggers.

**Usage:**
```bash
./test-gamepad-outputs
```

#### [ FACE BUTTONS ]
| Button | Action | Effect |
| :--- | :--- | :--- |
| **❌ Cross** | Vibration/LED | Heavy Rumble + 🔴 Red Light |
| **⭕ Circle** | Vibration/LED | Soft Rumble + 🔵 Blue Light |
| **🟥 Square** | Trigger Effect | Activates **GameCube-style** trigger snap on **R2** |
| **🔺 Triangle** | Reset | **Stops all effects** (Panic Button) |

#### [ D-PAD & SHOULDERS (Trigger Mods) ]
| Button | Hand | Effect |
| :--- | :--- | :--- |
| **L1** | **L2** | **Gallop** Effect (Vibration on trigger) |
| **R1** | **R2** | **Machine Gun** Effect (Fast vibration) |
| **⬆️ Up** | **L2** | **Feedback** (Rigid Resistance) |
| **⬇️ Down** | **R2** | **Bow** (String Tension) |
| **⬅️ Left** | **R2** | **Weapon** (Semi-Automatic) |
| **➡️ Right** | **R2** | **Automatic Gun** (Buzzing) |

---

## 🎧 Audio Haptics Integration Test (test-audio-haptics)
The `test-audio-haptics` demonstrates the high-fidelity Audio Haptics feature. It can play a WAV file or capture system audio, converting it into tactile feedback for the DualSense.

**Usage:**
```bash
# Play a specific WAV file (using relative path)
./test-audio-haptics "Tests/Integration/Datasets/ES_Touch_SCENE.wav"

# Capture system audio (Loopback mode)
./test-audio-haptics
```

**Requirements:**
- DualSense controller connected via USB or Bluetooth.
- **USB:** Provides 48kHz high-fidelity haptics.
- **Bluetooth:** Provides 3000Hz haptics via HID reports.

**Supported Modes:**
- **WAV Playback:** Reads a file and plays it on your default speakers while sending haptics to the controller.
- **System Audio:** Captures whatever is playing on your computer and converts it to haptics in real-time.

---

## 🎛️ Multi-Channel Haptics Test (test-channels-haptics)
The `test-channels-haptics` allows testing multiple controllers simultaneously with independent audio sources for haptics.

**Usage:**
```bash
# Assign different WAV files to different gamepads (using relative paths)
./test-channels-haptics "Tests/Integration/Datasets/ES_Touch_SCENE.wav" "Tests/Integration/Datasets/ES_Replay_Lawd_Ito.wav"

# If more controllers are connected than files provided, the last file is repeated.
# If no file is provided, it defaults to System Audio Loopback for all controllers.
```

**Features:**
- **Independent Channels:** Gamepad 1 gets the first WAV, Gamepad 2 gets the second, and so on.
- **Automatic Assignment:** Automatically detects connected gamepads and starts a dedicated audio worker for each.
- **Hot-Swap Support:** New controllers connected during the test will automatically start receiving haptic feedback.

### 🎵 Music Credits
Special thanks to **Epidemic Sound** for providing high-quality royalty-free music for testing:

1. **Track:** *Touch*  
   **Artist:** *SCENE*  
   **Source:** [Epidemic Sound](https://www.epidemicsound.com/)

2. **Track:** *Replay*  
   **Artist:** *Lawd Ito*  
   **Source:** [Epidemic Sound](https://www.epidemicsound.com/)

---

## 💻 Integration

### Minimal Example (Standalone C++)

```cpp
#include "GCore/Templates/TBasicDeviceRegistry.h"

// 1. Choose your platform policy
#ifdef _WIN32
    #include "Examples/Platform_Windows/test_windows_hardware_policy.h"
    using HardwarePolicy = Ftest_windows_platform::Ftest_windows_hardware_policy;
    using HardwareInfo = Ftest_windows_platform::Ftest_windows_hardware;
#elif __unix__
    #include "Examples/Platform_Linux/test_linux_hardware_policy.h"
    using HardwarePolicy = Ftest_linux_platform::Ftest_linux_hardware_policy;
    using HardwareInfo = Ftest_linux_platform::Ftest_linux_hardware;
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
                
                if (Input.bCross) {
                    // Trigger haptic feedback
                    Gamepad->SetLightbar({255, 0, 0});
                    Gamepad->SetRumble(255, 128);
                }
                
                // Control adaptive triggers
                if (auto* Trigger = Gamepad->GetIGamepadTrigger()) {
                    
                    Trigger->SetGameCube(EDSGamepadHand::AnyHand);
                    
                    // Example Custom Trigger Bow(0x22)
                    std::vector<uint8_t> BufferTrigger(10);
                    BufferTrigger[0] = 0x22;
                    BufferTrigger[1] = 0x02;
                    BufferTrigger[2] = 0x01;
                    BufferTrigger[3] = 0x3f;
                    BufferTrigger[4] = 0x00;
                    BufferTrigger[5] = 0x00;
                    BufferTrigger[6] = 0x00;
                    BufferTrigger[7] = 0x00;
                    BufferTrigger[8] = 0x00;
                    BufferTrigger[9] = 0x00;

                    if (Trigger) {
                        // Trigger->SetBow22();
                        Trigger->SetCustomTrigger(EDSGamepadHand::Right, BufferTrigger);
                    }
                }
                
                // Audio Haptic Interface
                if (auto* Haptic = Gamepad->IGamepadAudioHaptics()) {
                    // Convert audio buffer into haptic feedback
                    // Haptic->AudioHapticUpdate(<AudioData>);
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
      		void Read(FDeviceContext* Context)
		{
			Ftest_windows_platform::Ftest_windows_device_info::Read(Context);
		}

		void Write(FDeviceContext* Context)
		{
			Ftest_windows_platform::Ftest_windows_device_info::Write(Context);
		}

		void Detect(std::vector<FDeviceContext>& Devices)
		{
			Ftest_windows_platform::Ftest_windows_device_info::Detect(Devices);
		}

		bool CreateHandle(FDeviceContext* Context)
		{
			return Ftest_windows_platform::Ftest_windows_device_info::CreateHandle(Context);
		}

		void InvalidateHandle(FDeviceContext* Context)
		{
			Ftest_windows_platform::Ftest_windows_device_info::InvalidateHandle(Context);
		}

		void ProcessAudioHaptic(FDeviceContext* Context)
		{
			Ftest_windows_platform::Ftest_windows_device_info::ProcessAudioHaptic(Context);
		}

		void InitializeAudioDevice (FDeviceContext* Context)
		{
		}
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
## 🎵 Audio Pipeline — How It Works

Gamepad-Core provides a **complete audio-to-haptics and audio-to-speaker pipeline**.  Here's how the data flows from your application to the DualSense hardware:

```aiignore
┌─────────────────────────────────────────────────────────────┐
│                    YOUR APPLICATION                         │
│  • Captures audio (game sounds, music, etc.)                │
│  • Mixes audio channels                                     │
│  • Applies effects/filters                                  │
└──────────────────────┬──────────────────────────────────────┘
                       │ (sends audio buffer)
                       ▼
┌─────────────────────────────────────────────────────────────┐
│                   GAMEPAD-CORE LIB                          │
│  • Receives audio buffer                                    │
│  • Converts to haptic commands (for haptics)                │
│  • Encodes for speaker output (for speaker)                 │
│  • Sends via HID (USB/Bluetooth)                            │
└──────────────────────┬──────────────────────────────────────┘
                       │ (HID commands)
                       ▼
┌─────────────────────────────────────────────────────────────┐
│                   DUALSENSE HARDWARE                        │
│  • Vibration motors (haptics)                               │
│  • Built-in speaker                                         │
└─────────────────────────────────────────────────────────────┘
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

> [!IMPORTANT]
> **API Change:** The `BufferOutput` attribute in the `FDeviceContext` struct is now **private**.
> To access the write buffer, use the new method: `Context->GetRawOutputBuffer()`.

> [!IMPORTANT]
> **After calling any effect event on the controller (lights, triggers, vibrations, etc.), it is always necessary to call `Gamepad->UpdateOutput()` to apply the changes.**

Example:
```cpp
// Set LED color
gamepad->SetLightbar(255, 0, 0);

// Set trigger effect
gamepad->SetResistance(TriggerEffect:: Resistance, ... );

// Apply vibration
gamepad->SetVibration(0.5f, 0.5f);

// ⚠️ REQUIRED: Update output to apply all changes
gamepad->UpdateOutput();
```

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
cmake --build build/debug --target test-gamepad-outputs -j
./build/debug/Tests/Integration/test-gamepad-outputs
```


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

## ⭐ Credits and Acknowledgments

The foundation of this plugin was built upon the research and code from several amazing projects in the community:

* [DualSense on Windows API](https://github.com/Ohjurot/DualSense-Windows) - Initial DS5 implementation logic.
* [Nielk1 on GIST](https://gist.github.com/Nielk1/6d54cc2c00d2201ccb8c2720ad7538db) - HID report structures.
* [DualSenseAPI](https://github.com/BadMagic100/DualSenseAPI/tree/master) - Hardware communication references.
* [flok pydualsense](https://github.com/flok/pydualsense) - Feature report research.
* [SAxense](https://github.com/egormanga/SAxense) - Base for Bluetooth Audio Haptics.
* [miniaudio](https://github.com/mackron/miniaudio) - Audio playback and conversion library.
* [Ryochan7/DS4Windows](https://github.com/Ryochan7/DS4Windows) - Industry standard for DualShock/DualSense on Windows.
* [linux/drivers/hid/hid-playstation.c](https://github.com/torvalds/linux/blob/master/drivers/hid/hid-playstation.c#L1709) - Reference for calibration, gyroscope, and Linux driver standards.

Special thanks to the community members who helped improve this plugin:

* **[yncat](https://github.com/yncat)**: For the extensive research and implementation logic regarding **USB Audio Haptics**, which was crucial for supporting high-fidelity haptics via USB ([Issue #105](https://github.com/rafaelvaloto/Unreal-Dualsense/issues/105)).


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

[⬆ Back to Top](#-gamepad-core)

</div>
