#pragma once
#include <openvr.h>

namespace comfort_swim
{
    class ComfortSwim
    {
    public:
        static void init();
        static void onFrameUpdate();

    private:
        static void doSwimming(RE::PlayerCharacter* player, vr::VRControllerAxis_t primaryAxisValue, vr::VRControllerAxis_t offhandAxisValue);
    };

    // The ONE global to rule them ALL
    inline ComfortSwim g_comfortSwim;
}
