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
            return;
        }

        const auto primaryAxisValue = f4vr::VRControllers.getAxisValue(f4vr::Hand::Primary);
        const auto offhandAxisValue = f4vr::VRControllers.getAxisValue(f4vr::Hand::Offhand);

        if (!fNotEqual(offhandAxisValue.x, 0) && !fNotEqual(offhandAxisValue.y, 0) && !fNotEqual(primaryAxisValue.y, 0)) {
            logger::trace("no user controllers input - noop");
            return;
        }

        if (f4vr::isUnderwater(player)) {
            const RE::NiPoint3 deltaPos = { 3 * offhandAxisValue.x, 3 * offhandAxisValue.y, 1.5f * primaryAxisValue.y };
            logger::debug("Underwater movement by: ({}, {}, {})", deltaPos.x, deltaPos.y, deltaPos.z);
            player->Move(0.1f, deltaPos, false);
        } else if (primaryAxisValue.y < 0) {
            // TODO: handle diving better
        }
    }
}
