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

    /**
     * Hook handler inside game frame update tick.
     */
    inline void onGameFrameUpdateHook(std::uint32_t* arg1)
    {
        g_comfortSwim.onFrameUpdate();
        _someHookFuncOriginal(arg1);
    }

    inline void init()
    {
        common::logger::info("Hook into game loop at (0x{:X})...", SomeHookFuncCallSite.address());
        F4SE::AllocTrampoline(64);
        auto& trampoline = F4SE::GetTrampoline();
        _someHookFuncOriginal = reinterpret_cast<SomeHookFunc>(trampoline.write_call<5>(SomeHookFuncCallSite.address(), &onGameFrameUpdateHook));
    }
}
