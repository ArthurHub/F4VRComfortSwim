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
            return;
        }

        const auto primaryAxisValue = f4vr::VRControllers.getAxisValue(f4vr::Hand::Primary);
        const auto offhandAxisValue = f4vr::VRControllers.getAxisValue(f4vr::Hand::Offhand);

        if (fNotEqual(offhandAxisValue.x, 0) || fNotEqual(offhandAxisValue.y, 0) || fNotEqual(primaryAxisValue.y, 0)) {
            doSwimming(player, primaryAxisValue, offhandAxisValue);
        }
    }

    /**
     * Calculate the new player position based on swimming input and heading.
     */
    void ComfortSwim::doSwimming(RE::PlayerCharacter* const player, const vr::VRControllerAxis_t primaryAxisValue, const vr::VRControllerAxis_t offhandAxisValue)
    {
        auto const heading = player->GetHeading();

        // Calculate movement in the player's local space and rotate by heading
        const float cosAngle = std::cos(heading);
        const float sinAngle = std::sin(heading);

        // Offhand axis controls X (strafe) and Y (forward/back) in controller space
        const float controllerX = offhandAxisValue.x;
        const float controllerY = offhandAxisValue.y;
        const float deltaX = controllerX * cosAngle + controllerY * sinAngle;
        const float deltaY = -controllerX * sinAngle + controllerY * cosAngle;

        auto pos = player->GetPosition();
        if (f4vr::isUnderwater(player)) {
            pos.x += 2 * deltaX;
            pos.y += 2 * deltaY;
            pos.z += 1.5f * primaryAxisValue.y;
        } else if (primaryAxisValue.y < 0) {
            // TODO: handle diving better
        }

        player->SetPosition(pos, true);
    }
}
