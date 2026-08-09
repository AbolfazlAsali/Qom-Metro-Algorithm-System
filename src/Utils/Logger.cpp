#include "Utils/Logger.hpp"

#include <iostream>
using namespace std;

namespace QomMetro::Utils {

string Logger::level_to_String(Log_Level level){

    switch(level){

        case Log_Level::Info:    return "INFO";
        case Log_Level::Warning: return "WARN";
        case Log_Level::Error:   return "ERROR";
    }
    return "UNKNOWN";
}

void Logger::log(Log_Level level, const string& message){

    ostream& out = (level == Log_Level::Error) ? cerr : cout;
    out << "[" << level_to_String(level) << "] " << message << endl;
}

void Logger::info(const string& message){
    log(Log_Level::Info, message);
}

void Logger::warning(const string& message) {
    log(Log_Level::Warning, message);
}

void Logger::error(const string& message) {
    log(Log_Level::Error, message);
}

} // namespace QomMetro::Utils