#pragma once

#include <string>

namespace QomMetro::Utils {

enum class Log_Level { Info, Warning, Error };


class Logger {

public:

    static void log(Log_Level level, const std::string& message);

    static void info(const std::string& message);
    static void warning(const std::string& message);
    static void error(const std::string& message);

private:
    static std::string level_to_String(Log_Level level);
};

} // namespace QomMetro::Utils