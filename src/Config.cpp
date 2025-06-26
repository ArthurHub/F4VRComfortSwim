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
        forwardSwimmingSpeedMultiplier = static_cast<float>(ini.GetDoubleValue(DEFAULT_SECTION, "forwardSwimmingSpeedMultiplier", 1.0));
        backwardSwimmingSpeedMultiplier = static_cast<float>(ini.GetDoubleValue(DEFAULT_SECTION, "backwardSwimmingSpeedMultiplier", 1.0));
        strafeSwimmingSpeedMultiplier = static_cast<float>(ini.GetDoubleValue(DEFAULT_SECTION, "strafeSwimmingSpeedMultiplier", 1.0));
        upSwimmingSpeedMultiplier = static_cast<float>(ini.GetDoubleValue(DEFAULT_SECTION, "upSwimmingSpeedMultiplier", 1.0));
        downSwimmingSpeedMultiplier = static_cast<float>(ini.GetDoubleValue(DEFAULT_SECTION, "downSwimmingSpeedMultiplier", 1.0));
    }

    void Config::saveIniConfigInternal(CSimpleIniA& ini) const {}
}
