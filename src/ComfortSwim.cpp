#include "ComfortSwim.h"

#include "Config.h"

using namespace common;

namespace comfort_swim
{
    void F4SEAPI onF4SEMessageHandler(F4SE::MessagingInterface::Message* message)
    {
        switch (message->type) {
        case F4SE::MessagingInterface::kGameLoaded:
            logger::info("Game loaded - starting swimming state monitoring");
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
    }

    void ComfortSwim::testing()
    {
        auto player = RE::PlayerCharacter::GetSingleton();
        if (!player || !player->loadedData) {
            logger::sample(3000, "no player data yet");
            return;
        }

        // logger::sample("player sneak?({}) - jumping?({}) - Location?({})", player->IsSneaking(), player->IsJumping(), player->DoGetMovementHeight());
        logger::sample("Underwater?: CharacterState({}) underWaterTimer({})", static_cast<int>(player->DoGetCharacterState()), player->underWaterTimer);

        logger::sample("Player Location: ({}, {}, {}) ({}, {})",
            player->data.location.x, player->data.location.y, player->data.location.z,
            player->GetPosition().x, player->GetPosition().y);

        // moving?
        auto pos = player->GetPosition();
        pos.x += 5; // just to test
        pos.y += 5; // just to test
        player->SetPosition(pos, true);

        // crashes:
        // player->Move(0.1, RE::NiPoint3(1, 1, 1), 0.1);
    }
}
