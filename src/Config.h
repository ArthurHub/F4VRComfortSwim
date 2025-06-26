#pragma once

#include "Resources.h"
#include "common/ConfigBase.h"

namespace comfort_swim
{
    static const auto BASE_PATH = common::BASE_PATH + R"(\F4VRComfortSwim)";
    static const auto INI_PATH = BASE_PATH + R"(\F4VRComfortSwim.ini)";

    constexpr int INI_VERSION = 1;

    class Config : public common::ConfigBase
    {
    public:
        explicit Config() :
            ConfigBase(INI_PATH, IDR_CONFIG_INI, INI_VERSION) {}

        void load();

        // swimming speed multipliers for different directions
        float forwardSwimmingSpeedMultiplier = 0;
        float backwardSwimmingSpeedMultiplier = 0;
        float strafeSwimmingSpeedMultiplier = 0;
        float upSwimmingSpeedMultiplier = 0;
        float downSwimmingSpeedMultiplier = 0;

    protected:
        virtual void loadIniConfigInternal(const CSimpleIniA& ini) override;
        virtual void saveIniConfigInternal(CSimpleIniA& ini) const override;
    };

    // Global singleton for easy access
    inline Config g_config;
}
