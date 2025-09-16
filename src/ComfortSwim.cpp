#include "ComfortSwim.h"

#include "Config.h"
#include "f4vr/F4VRUtils.h"
#include "f4vr/VRControllersManager.h"

using namespace common;

// This is the entry point to the mod.
extern "C" DLLEXPORT bool F4SEAPI F4SEPlugin_Query(const F4SE::QueryInterface* a_skse, F4SE::PluginInfo* a_info)
{
    return f4cf::g_mod->onF4SEPluginQuery(a_skse, a_info);
}

// This is the entry point to the mod.
extern "C" DLLEXPORT bool F4SEAPI F4SEPlugin_Load(const F4SE::LoadInterface* a_f4se)
{
    return f4cf::g_mod->onF4SEPluginLoad(a_f4se);
}

namespace comfort_swim
{
    void ComfortSwim::onModLoaded(const F4SE::LoadInterface*)
    {
        //noop
    }

    void ComfortSwim::onGameLoaded()
    {
        //noop
    }

    void ComfortSwim::onGameSessionLoaded()
    {
        //noop
    }

    /**
     * On every frame if player is initialized, swimming, and has input, do comfort swimming.
     */
    void ComfortSwim::onFrameUpdate()
    {
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

        if (g_config.divingBugFixEnabled && fixDiveDeepBug(player)) {
            logger::trace("dive fix applied - noop");
            return;
        }

        if (!f4vr::isUnderwater(player)) {
            logger::trace("not underwater or dive fix - noop");
            return;
        }

        const auto primaryAxisValue = f4vr::VRControllers.getThumbstickValue(f4vr::Hand::Primary);
        const auto offhandAxisValue = f4vr::VRControllers.getThumbstickValue(f4vr::Hand::Offhand);

        if (!fNotEqual(offhandAxisValue.x, 0) && !fNotEqual(offhandAxisValue.y, 0) && !fNotEqual(primaryAxisValue.y, 0)) {
            logger::trace("no user controllers input - noop");
            return;
        }

        // Get raw controller input and apply speed multipliers
        float dx = offhandAxisValue.x * g_config.strafeSwimmingSpeedMultiplier;
        float dy = offhandAxisValue.y * (offhandAxisValue.y > 0 ? g_config.forwardSwimmingSpeedMultiplier : g_config.backwardSwimmingSpeedMultiplier);
        const float dz = primaryAxisValue.y * (primaryAxisValue.y > 0 ? g_config.upSwimmingSpeedMultiplier : g_config.downSwimmingSpeedMultiplier);

        const bool useWandDirecMove = f4vr::useWandDirectionalMovement();
        if (useWandDirecMove) {
            // Transform controller input based on controller heading relative to player
            adjustDeltasForWandDirectionalMovement(dx, dy);
        }

        logger::debug("Underwater movement by wandDirection?({}): ({:.4f}, {:.4f}, {:.4f})", useWandDirecMove, dx, dy, dz);
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
            return false;
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
        _lastPlayerPositionZ -= 0.4f;
        return true;
    }

    /**
     * Transforms raw controller input based on controller orientation relative to HMD.
     * Uses the relative angle between controller and HMD to modify movement deltas.
     * Details:
     * "player->Move" function adjusts the movement deltas by the heading of the HMD.
     * If the game is configured to use wand directional movement, we need to do the manual work to adjust
     * the deltas based on the controller's relative heading to the HMD. So when the game applies
     * it's adjustment it will be correct.
     * P.S. I didn't find in game RE where to do movement based on wand movement so Had to implement it here.
     */
    void ComfortSwim::adjustDeltasForWandDirectionalMovement(float& dx, float& dy)
    {
        const float controllerRelativeHeading = f4vr::VRControllers.getControllerRelativeHeading(f4vr::Hand::Offhand);
        const float cosRelative = std::cos(controllerRelativeHeading);
        const float sinRelative = std::sin(controllerRelativeHeading);

        const float orgDx = dx;
        const float orgDy = dy;
        dx = orgDx * cosRelative - orgDy * sinRelative;
        dy = orgDx * sinRelative + orgDy * cosRelative;
    }
}
