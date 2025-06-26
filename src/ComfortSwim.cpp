#include "ComfortSwim.h"

#include "Config.h"
#include "Hooks.h"
#include "f4vr/VRControllersManager.h"

using namespace common;

namespace comfort_swim
{
    void ComfortSwim::init()
    {
        logger::info("ComfortSwim init...");
        g_config.load();
        hooks::init();
    }

    /**
     * On every frame if player is initialized, swimming, and has input, do comfort swimming.
     */
    void ComfortSwim::onFrameUpdate()
    {
        f4vr::VRControllers.update();

        const auto player = RE::PlayerCharacter::GetSingleton();
        if (!player || !player->loadedData) {
            logger::sample(3000, "no player data - noop");
            return;
        }

        if (!f4vr::isSwimming(player)) {
            logger::trace("not swimming - noop");
            _isPlayerUnderwater = false;
            return;
        }

        if (fixDiveDeepBug(player)) {
            logger::trace("not underwater or dive fix - noop");
            return;
        }

        const auto primaryAxisValue = f4vr::VRControllers.getAxisValue(f4vr::Hand::Primary);
        const auto offhandAxisValue = f4vr::VRControllers.getAxisValue(f4vr::Hand::Offhand);

        if (!fNotEqual(offhandAxisValue.x, 0) && !fNotEqual(offhandAxisValue.y, 0) && !fNotEqual(primaryAxisValue.y, 0)) {
            logger::trace("no user controllers input - noop");
            return;
        }

        const float dx = offhandAxisValue.x * g_config.strafeSwimmingSpeedMultiplier;
        const float dy = offhandAxisValue.y * (offhandAxisValue.y > 0 ? g_config.forwardSwimmingSpeedMultiplier : g_config.backwardSwimmingSpeedMultiplier);
        const float dz = primaryAxisValue.y * (primaryAxisValue.y > 0 ? g_config.upSwimmingSpeedMultiplier : g_config.downSwimmingSpeedMultiplier);

        logger::debug("Underwater movement by: ({:.4f}, {:.4f}, {:.4f})", dx, dy, dz);
        player->Move(0.1f, { dx, dy, dz }, false);
    }

    /**
     * Diving for first time has a bug that throws the player super deep underwater.
     * This trying to fix it by checking the last player position before underwater is detected and moving the
     * player back up until the game stops moving it down.
     */
    bool ComfortSwim::fixDiveDeepBug(RE::PlayerCharacter* const player)
    {
        if (!f4vr::isUnderwater(player)) {
            _isPlayerUnderwater = false;
            _lastPlayerPositionZ = player->GetPositionZ();
            return true;
        }

        if (_isPlayerUnderwater) {
            return false;
        }

        auto pos = player->GetPosition();
        if (_lastPlayerPositionZ - pos.z <= 3) {
            _isPlayerUnderwater = true;
            return false;
        }

        logger::info("Diving fix: moving player back from {:.3f} to {:.3f}", pos.z, _lastPlayerPositionZ);
        pos.z = _lastPlayerPositionZ;
        player->SetPosition(pos, true);
        _lastPlayerPositionZ -= 0.5f;
        return true;
    }
}
