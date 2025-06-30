#pragma once

namespace comfort_swim
{
    class ComfortSwim
    {
    public:
        static void init();
        void onFrameUpdate();

    private:
        bool fixDiveDeepBug(RE::PlayerCharacter* player);
        static void adjustDeltasForWandDirectionalMovement(float& dx, float& dy);

        // used to fix diving throws the player super deep underwater
        bool _isPlayerUnderwater = false;
        float _lastPlayerPositionZ = 0;
    };

    // The ONE global to rule them ALL
    inline ComfortSwim g_comfortSwim;
}
