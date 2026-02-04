#pragma once

#if defined(GAMEPAD_CORE_EXTERNAL_SO_DEFINES)
#include GAMEPAD_CORE_EXTERNAL_SO_DEFINES
#endif

#if !defined(GAMEPAD_CORE_EMBEDDED)
#include <chrono>
#include <thread>
#include <mutex>
#endif

// =====================
// Tempo (tipo e helpers)
// =====================
namespace gc_time {
#if defined(GAMEPAD_CORE_EMBEDDED)
    using ms = unsigned int; // milissegundos nativo (evita <chrono>)
#else
    using ms = std::chrono::milliseconds; // desktop/host
#endif
}

// =====================
// Sincronização (locks)
// =====================
#if defined(GAMEPAD_CORE_EMBEDDED)
namespace gc_lock {
    // No-op por padrão em embarcados (single-core/sync externo)
    struct mutex { void lock(){} void unlock(){} };
    template <class M>
    struct lock_guard { explicit lock_guard(M&) {} };
} // namespace gc_lock
#else
    namespace gc_lock {
        using mutex = std::mutex;
        template <class M>
        using lock_guard = std::lock_guard<M>;
    } // namespace gc_lock
#endif

// =====================
// Sleep helpers portáveis
// =====================

#if defined(GAMEPAD_CORE_EMBEDDED)
#define gc_sleep_ms ::sleep_ms
#endif

namespace gc_sync {
    inline void sleep_ms(unsigned int ms_val) {
    #if defined(GAMEPAD_CORE_EMBEDDED)
        gc_sleep_ms(ms_val);
    #else
        std::this_thread::sleep_for(std::chrono::milliseconds(ms_val));
    #endif
    }

    inline void sleep_for(gc_time::ms duration) {
    #if defined(GAMEPAD_CORE_EMBEDDED)
        // Embarcado: gc_time::ms == unsigned int
        sleep_ms(static_cast<unsigned int>(duration));
    #else
        std::this_thread::sleep_for(duration);
    #endif
    }
}
