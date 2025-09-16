#include "Config.h"

using namespace common;

namespace comfort_swim
{
    static constexpr auto DEFAULT_SECTION = std::string(Version::PROJECT).c_str();

    void Config::loadIniConfigInternal(const CSimpleIniA& ini)
    {
        forwardSwimmingSpeedMultiplier = static_cast<float>(ini.GetDoubleValue(DEFAULT_SECTION, "fForwardSwimmingSpeedMultiplier", 3.0));
        backwardSwimmingSpeedMultiplier = static_cast<float>(ini.GetDoubleValue(DEFAULT_SECTION, "fBackwardSwimmingSpeedMultiplier", 1.5));
        strafeSwimmingSpeedMultiplier = static_cast<float>(ini.GetDoubleValue(DEFAULT_SECTION, "fStrafeSwimmingSpeedMultiplier", 1.5));
        upSwimmingSpeedMultiplier = static_cast<float>(ini.GetDoubleValue(DEFAULT_SECTION, "fUpSwimmingSpeedMultiplier", 1.6));
        downSwimmingSpeedMultiplier = static_cast<float>(ini.GetDoubleValue(DEFAULT_SECTION, "fDownSwimmingSpeedMultiplier", 1.2));

        divingBugFixEnabled = ini.GetBoolValue(DEFAULT_SECTION, "bDivingBugFixEnabled", true);
    }
}
