#include "ComfortSwim.h"
#include "Config.h"
#include "common/Logger.h"

using namespace common;

namespace
{
    void logPluginGameStart()
    {
        const auto game = REL::Module::IsVR() ? "Fallout4VR" : "Fallout4";
        const auto runtimeVer = REL::Module::get().version();
        logger::info("Starting '{}' v{} ; {} v{} ; {} at {} ; BaseAddress: 0x{:X}",
            Version::PROJECT, Version::NAME, game, runtimeVer.string(), __DATE__, __TIME__, REL::Module::get().base());
    }
}

extern "C" DLLEXPORT bool F4SEAPI F4SEPlugin_Query(const F4SE::QueryInterface* a_skse, F4SE::PluginInfo* a_info)
{
    a_info->infoVersion = F4SE::PluginInfo::kVersion;
    a_info->name = Version::PROJECT.data();
    a_info->version = Version::MAJOR;

    if (a_skse->IsEditor()) {
        logger::critical("Loaded in editor, marking as incompatible");
        return false;
    }

    const auto ver = a_skse->RuntimeVersion();
    if (ver < (REL::Module::IsF4() ? F4SE::RUNTIME_LATEST : F4SE::RUNTIME_LATEST_VR)) {
        logger::critical("Unsupported runtime version {}", ver.string());
        return false;
    }

    return true;
}

extern "C" DLLEXPORT bool F4SEAPI F4SEPlugin_Load(const F4SE::LoadInterface* a_f4se)
{
    try {
        logger::init(Version::PROJECT);
        logPluginGameStart();

        F4SE::Init(a_f4se, false);

        comfort_swim::ComfortSwim::init();

        return true;
    } catch (const std::exception& ex) {
        logger::error("Unhandled exception: {}", ex.what());
        return false;
    }
}
