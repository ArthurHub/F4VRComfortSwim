#include "ComfortSwim.h"

#include "Config.h"
#include "Hooks.h"
#include "f4vr/VRControllersManager.h"

using namespace common;

namespace comfort_swim
{
    void F4SEAPI onF4SEMessageHandler(F4SE::MessagingInterface::Message* message)
    {
        switch (message->type) {
        case F4SE::MessagingInterface::kGameLoaded:
            logger::info("Game loaded");
            break;
        default: ;
        }
    }

    void ComfortSwim::init()
    {
        logger::info("ComfortSwim init...");
        g_config.load();

        const auto messaging = F4SE::GetMessagingInterface();
        if (!messaging || !messaging->RegisterListener(onF4SEMessageHandler)) {
            throw std::runtime_error("Failed to register message handler.");
        }

        hooks::init();
    }

    void ComfortSwim::onFrameUpdate()
    {
        f4vr::VRControllers.update();

        const auto player = RE::PlayerCharacter::GetSingleton();
        if (!player || !player->loadedData) {
            logger::sample(3000, "no player data - noop");
            return;
        }

        // logger::sample("player sneak?({}) - jumping?({}) - Location?({})", player->IsSneaking(), player->IsJumping(), player->DoGetMovementHeight());
        logger::sample("Underwater?: CharacterState({}) underWaterTimer({}) leftHanded?({})",
            static_cast<int>(player->DoGetCharacterState()), player->underWaterTimer, f4vr::isLeftHandedMode());

        logger::sample("Player Location: ({}, {}, {}) ({}, {})",
            player->data.location.x, player->data.location.y, player->data.location.z,
            player->GetPosition().x, player->GetPosition().y);

        const auto offhandAxisValue = f4vr::VRControllers.getAxisValue(f4vr::Hand::Primary);
        const auto primaryAxisValue = f4vr::VRControllers.getAxisValue(f4vr::Hand::Offhand);

        if (fEqual(player->underWaterTimer, 0)) {
            logger::sample("not underwater - noop");
            return;
        }

        if (fEqual(offhandAxisValue.x, 0) && fEqual(offhandAxisValue.y, 0) && fEqual(primaryAxisValue.y, 0)) {
            logger::sample("no movement - noop");
            return;
        }

        // moving?
        auto pos = player->GetPosition();
        pos.x += 3 * offhandAxisValue.x;
        pos.y += 3 * offhandAxisValue.y;
        pos.z += 2 * primaryAxisValue.y;
        player->SetPosition(pos, true);

        // crashes:
        // player->Move(0.1, RE::NiPoint3(1, 1, 1), 0.1);
    }
}
