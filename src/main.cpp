#include <spdlog/sinks/rotating_file_sink.h>

namespace
{
	void initLogger()
	{
		auto path = logger::log_directory();
		const auto gamepath = REL::Module::IsVR() ? "Fallout4VR/F4SE" : "Fallout4/F4SE";
		if (!path.value().generic_string().ends_with(gamepath)) {
			// handle bug where game directory is missing
			path = path.value().parent_path().append(gamepath);
		}

		// Use rolling log files (5 files, max 10mb each)
		*path /= fmt::format("{}.log"sv, Version::PROJECT);
		auto sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(path->string(), 1024 * 1024 * 10, 5, true);

		auto log = std::make_shared<spdlog::logger>("GLOBAL"s, std::move(sink));

		constexpr auto level = spdlog::level::info;
		log->set_level(level);
		log->flush_on(level);

		spdlog::set_default_logger(std::move(log));

		// see: https://github.com/gabime/spdlog/wiki/Custom-formatting
		spdlog::set_pattern("%H:%M:%S.%e %L: %v"s);
	}

	void logPluginGameStart()
	{
		const auto game = REL::Module::IsVR() ? "Fallout4VR" : "Fallout4";
		const auto runtimeVer = REL::Module::get().version();
		logger::info("Starting '{}' v{} ; {} v{} ; {} at {}"sv,
			Version::PROJECT, Version::NAME, game, runtimeVer.string(), __DATE__, __TIME__);
	}
}

extern "C" DLLEXPORT bool F4SEAPI F4SEPlugin_Query(const F4SE::QueryInterface* a_skse, F4SE::PluginInfo* a_info)
{
	a_info->infoVersion = F4SE::PluginInfo::kVersion;
	a_info->name = Version::PROJECT.data();
	a_info->version = Version::MAJOR;

	if (a_skse->IsEditor()) {
		logger::critical("Loaded in editor, marking as incompatible"sv);
		return false;
	}

	const auto ver = a_skse->RuntimeVersion();
	if (ver < (REL::Module::IsF4() ? F4SE::RUNTIME_LATEST : F4SE::RUNTIME_LATEST_VR)) {
		logger::critical(FMT_STRING("Unsupported runtime version {}"), ver.string());
		return false;
	}

	return true;
}

extern "C" DLLEXPORT bool F4SEAPI F4SEPlugin_Load(const F4SE::LoadInterface* a_f4se)
{
	initLogger();

	logPluginGameStart();

	F4SE::Init(a_f4se, false);

	return true;
}
