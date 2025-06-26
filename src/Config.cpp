#include "Config.h"

using namespace common;

namespace comfort_swim
{
    static const auto DEFAULT_SECTION = "F4VRComfortSwim";

    void Config::load()
    {
        logger::info("Load ini config...");
        createDirDeep(INI_PATH);
        initIniConfig();
    }

    void Config::loadIniConfigInternal(const CSimpleIniA& ini)
    {
        forwardSwimmingSpeedMultiplier = static_cast<float>(ini.GetDoubleValue(DEFAULT_SECTION, "ForwardSwimmingSpeedMultiplier", 3.0));
        backwardSwimmingSpeedMultiplier = static_cast<float>(ini.GetDoubleValue(DEFAULT_SECTION, "BackwardSwimmingSpeedMultiplier", 1.5));
        strafeSwimmingSpeedMultiplier = static_cast<float>(ini.GetDoubleValue(DEFAULT_SECTION, "StrafeSwimmingSpeedMultiplier", 1.5));
        upSwimmingSpeedMultiplier = static_cast<float>(ini.GetDoubleValue(DEFAULT_SECTION, "UpSwimmingSpeedMultiplier", 1.6));
        downSwimmingSpeedMultiplier = static_cast<float>(ini.GetDoubleValue(DEFAULT_SECTION, "DownSwimmingSpeedMultiplier", 1.2));

        divingBugFixEnabled = ini.GetBoolValue(DEFAULT_SECTION, "DivingBugFixEnabled", true);
    }
}
