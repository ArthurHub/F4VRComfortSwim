#pragma once

#include "Config.h"
#include "ModBase.h"

namespace comfort_swim
{
    class ComfortSwim : public f4cf::ModBase
    {
    public:
        ComfortSwim() :
            ModBase(Version::PROJECT, Version::NAME, &g_config, 32) {}

    protected:
        virtual void onModLoaded(const F4SE::LoadInterface* f4SE) override;
        virtual void onGameLoaded() override;
        virtual void onGameSessionLoaded() override;
        virtual void onFrameUpdate() override;

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
