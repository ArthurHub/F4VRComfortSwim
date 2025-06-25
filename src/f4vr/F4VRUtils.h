#pragma once

#include "F4VROffsets.h"

namespace f4vr
{
    /**
     * Get the "bLeftHandedMode:VR" setting from the INI file.
     * Direct memory access is A LOT faster than "RE::INIPrefSettingCollection::GetSingleton()->GetSetting("bLeftHandedMode:VR")->GetBinary();"
     */
    inline bool isLeftHandedMode()
    {
        return *iniLeftHandedMode;
    }

    /**
     * Is the player swimming either on the surface or underwater.
     */
    inline bool isSwimming(const RE::PlayerCharacter* player = RE::PlayerCharacter::GetSingleton())
    {
        return player && static_cast<int>(player->DoGetCharacterState()) == 5;
    }

    /**
     * Is the player is currently underwater as detected by underwater timer being non-zero.
     */
    inline bool isUnderwater(const RE::PlayerCharacter* player = RE::PlayerCharacter::GetSingleton())
    {
        return player && player->underWaterTimer > 0;
    }
}
