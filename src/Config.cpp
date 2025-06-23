#include "Config.h"

using namespace common;

namespace comfort_swim
{
    void Config::loadAllConfig()
    {
        logger::info("Load ini config...");
        createDirDeep(INI_PATH);
        initIniConfig();
    }

    void Config::loadIniConfigInternal(const CSimpleIniA& ini) {}

    void Config::saveIniConfigInternal(CSimpleIniA& ini) const {}
}
