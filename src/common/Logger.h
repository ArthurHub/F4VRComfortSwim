#pragma once

#include <chrono>
#include <SimpleIni.h>
#include <string>
#include <unordered_map>
#include <spdlog/sinks/rotating_file_sink.h>

#include "CommonUtils.h"

namespace fs = std::filesystem;

namespace common
{
    class Log
    {
    public:
        /**
         * Init logging using a log with the given name put in "My Games" folder.
         */
        static void init(const std::string_view& logFileName)
        {
            auto path = F4SE::log::log_directory();
            const auto gamepath = REL::Module::IsVR() ? "Fallout4VR/F4SE" : "Fallout4/F4SE";
            if (!path.value().generic_string().ends_with(gamepath))
            {
                // handle bug where game directory is missing
                path = path.value().parent_path().append(gamepath);
            }

            // Use rolling log files (5 files, max 10mb each)
            *path /= fmt::format("{}.log"sv, logFileName);
            auto sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(path->string(), 1024 * 1024 * 10, 5, true);

            _logger = std::make_shared<spdlog::logger>("GLOBAL"s, std::move(sink));

            constexpr auto level = spdlog::level::info;
            _logger->set_level(level);
            _logger->flush_on(level);

            spdlog::set_default_logger(_logger);

            // see: https://github.com/gabime/spdlog/wiki/Custom-formatting
            spdlog::set_pattern("%H:%M:%S.%e %L: %v"s);
        }

        /**
         * Update the global logger log level based on the config setting.
         */
        static void setLogLevel(int logLevel)
        {
            if (_logLevel == logLevel)
                return;

            info("Set log level = %d", logLevel);
            _logLevel = logLevel;
            const auto levelEnum = static_cast<spdlog::level::level_enum>(logLevel);
            _logger->set_level(levelEnum);
            _logger->flush_on(levelEnum);
        }

        static void trace(const char* fmt, ...)
        {
            va_list args;
            va_start(args, fmt);
            char buffer[1024];
            auto const ret = vsnprintf(buffer, sizeof(buffer), fmt, args);
            // Ensure null termination
            buffer[(ret >= 0 && static_cast<size_t>(ret) < sizeof(buffer) ? ret : sizeof(buffer) - 1)] = '\0';
            va_end(args);

            _logger->trace(buffer);
        }

        static void debug(const char* fmt, ...)
        {
            va_list args;
            va_start(args, fmt);
            char buffer[1024];
            auto const ret = vsnprintf(buffer, sizeof(buffer), fmt, args);
            // Ensure null termination
            buffer[(ret >= 0 && static_cast<size_t>(ret) < sizeof(buffer) ? ret : sizeof(buffer) - 1)] = '\0';
            va_end(args);

            _logger->trace(buffer);
        }

        static void verbose(const char* fmt, ...)
        {
            va_list args;
            va_start(args, fmt);
            char buffer[1024];
            auto const ret = vsnprintf(buffer, sizeof(buffer), fmt, args);
            // Ensure null termination
            buffer[(ret >= 0 && static_cast<size_t>(ret) < sizeof(buffer) ? ret : sizeof(buffer) - 1)] = '\0';
            va_end(args);

            _logger->debug(buffer);
        }

        static void info(const char* fmt, ...)
        {
            va_list args;
            va_start(args, fmt);
            char buffer[1024];
            auto const ret = vsnprintf(buffer, sizeof(buffer), fmt, args);
            // Ensure null termination
            buffer[(ret >= 0 && static_cast<size_t>(ret) < sizeof(buffer) ? ret : sizeof(buffer) - 1)] = '\0';
            va_end(args);

            _logger->info(buffer);
        }

        // don't add the extra prefix stuff. Useful for multiline debug dump logs
        static void infoRaw(const char* fmt, ...)
        {
            va_list args;
            va_start(args, fmt);
            char buffer[1024];
            auto const ret = vsnprintf(buffer, sizeof(buffer), fmt, args);
            // Ensure null termination
            buffer[(ret >= 0 && static_cast<size_t>(ret) < sizeof(buffer) ? ret : sizeof(buffer) - 1)] = '\0';
            va_end(args);

            _logger->info(buffer);
        }

        static void warn(const char* fmt, ...)
        {
            va_list args;
            va_start(args, fmt);
            char buffer[1024];
            auto const ret = vsnprintf(buffer, sizeof(buffer), fmt, args);
            // Ensure null termination
            buffer[(ret >= 0 && static_cast<size_t>(ret) < sizeof(buffer) ? ret : sizeof(buffer) - 1)] = '\0';
            va_end(args);

            _logger->warn(buffer);
        }

        static void error(const char* fmt, ...)
        {
            va_list args;
            va_start(args, fmt);
            char buffer[1024];
            auto const ret = vsnprintf(buffer, sizeof(buffer), fmt, args);
            // Ensure null termination
            buffer[(ret >= 0 && static_cast<size_t>(ret) < sizeof(buffer) ? ret : sizeof(buffer) - 1)] = '\0';
            va_end(args);

            _logger->error(buffer);
        }

        static void fatal(const char* fmt, ...)
        {
            va_list args;
            va_start(args, fmt);
            char buffer[1024];
            auto const ret = vsnprintf(buffer, sizeof(buffer), fmt, args);
            // Ensure null termination
            buffer[(ret >= 0 && static_cast<size_t>(ret) < sizeof(buffer) ? ret : sizeof(buffer) - 1)] = '\0';
            va_end(args);

            _logger->critical(buffer);
        }

        static void critical(const char* fmt, ...)
        {
            va_list args;
            va_start(args, fmt);
            char buffer[1024];
            auto const ret = vsnprintf(buffer, sizeof(buffer), fmt, args);
            // Ensure null termination
            buffer[(ret >= 0 && static_cast<size_t>(ret) < sizeof(buffer) ? ret : sizeof(buffer) - 1)] = '\0';
            va_end(args);

            _logger->critical(buffer);
        }

        /**
         * Same as calling info() but only one message log per "time" in milliseconds, other logs are dropped.
         * Use the message format as a key to identify the log messages that should be sampled.
         */
        static void sample(const int time, const char* fmt, ...)
        {
            va_list args;
            va_start(args, fmt);
            sampleImpl(time, fmt, args);
            va_end(args);
        }

        /**
         * Same as calling info() but only one message log per second, other logs are dropped.
         * Use the message format as a key to identify the log messages that should be sampled.
         */
        static void sample(const char* fmt, ...)
        {
            va_list args;
            va_start(args, fmt);
            sampleImpl(1000, fmt, args);
            va_end(args);
        }

    private:
        /**
         * Same as calling _MESSAGE but only one message log per "time" second, other logs are dropped.
         */
        static void sampleImpl(const int time, const char* fmt, const va_list args)
        {
            const auto key = fmt;
            const auto now = std::chrono::steady_clock::now();
            if (_sampleMessagesTtl.contains(key) && now - _sampleMessagesTtl[key] <= std::chrono::milliseconds(time))
            {
                return;
            }
            _sampleMessagesTtl[key] = now;
            char buffer[1024];
            auto const ret = vsnprintf(buffer, sizeof(buffer), fmt, args);
            // Ensure null termination
            buffer[(ret >= 0 && static_cast<size_t>(ret) < sizeof(buffer) ? ret : sizeof(buffer) - 1)] = '\0';

            _logger->info(" [SAMPLE] {}", buffer);
        }

        /**
         * Global logger instance
         */
        inline static std::shared_ptr<spdlog::logger> _logger;

        /**
         * Current log level
         */
        inline static int _logLevel = -1;

        /**
         * Holds the last time of a log message per key.
         */
        inline static std::unordered_map<std::string, std::chrono::steady_clock::time_point> _sampleMessagesTtl;
    };
}
