#pragma once

#include "ComfortSwim.h"
#include "common/Logger.h"

namespace comfort_swim::hooks
{
    // Hook into a random function call inside the game loop function (FUN_140d83de0)
    // By overriding the address of one of the calls we can inject our own code to run
    // on every frame.
    using SomeHookFunc = void (*)(uint32_t* arg1);
    inline REL::Relocation SomeHookFuncCallSite{ REL::Offset(0xD83EBC) };
    inline SomeHookFunc _someHookFuncOriginal = nullptr;
    inline uintptr_t _someHookFuncThisHook = 0;

    /**
     * Hook handler inside game frame update tick.
     */
    inline void onGameFrameUpdateHook(std::uint32_t* arg1)
    {
        g_comfortSwim.onFrameUpdate();
        _someHookFuncOriginal(arg1);
    }

    /**
     * Read the destination of a CALL at address
     */
    inline std::uintptr_t getCallTarget(const std::uintptr_t address)
    {
        const std::int32_t rel = *reinterpret_cast<std::int32_t*>(address + 1);
        return address + 5 + rel;
    }

    /**
     * Init a game look hook at a specific call site.
     */
    inline void init()
    {
        common::logger::info("Hook into game loop at (0x{:X})...", SomeHookFuncCallSite.address());
        F4SE::AllocTrampoline(64);
        auto& trampoline = F4SE::GetTrampoline();
        const auto someHookFuncOriginal = trampoline.write_call<5>(SomeHookFuncCallSite.address(), &onGameFrameUpdateHook);

        _someHookFuncThisHook = getCallTarget(SomeHookFuncCallSite.address());
        common::logger::info("Hook into game loop successful, original: (0x{:X}), new: (0x{:X})", someHookFuncOriginal, _someHookFuncThisHook);
        _someHookFuncOriginal = reinterpret_cast<SomeHookFunc>(someHookFuncOriginal);
    }

    /**
     * Validate that the hook was NOT overridden by another mod.
     */
    inline void validate()
    {
        const auto current = getCallTarget(SomeHookFuncCallSite.address());
        if (current != _someHookFuncThisHook) {
            common::logger::error("Hook validation failed! Expected: (0x{:X}), Current: (0x{:X})", _someHookFuncThisHook, current);
        }
    }
}
