#include "time_utils.h"

#include <sstream>
#include <iomanip>

std::string time_ms_to_short_string(int time_ms, int precision_ms, int precision_log_ms)
{
    int ms = time_ms % 1000;
    time_ms = (time_ms - ms) / 1000;
    int seconds = time_ms % 60;
    time_ms = (time_ms - seconds) / 60;
    int minutes = time_ms % 60;
    int hours = (time_ms - minutes) / 60;

    ms /= precision_ms;

    std::ostringstream string_builder;

    const char default_fill = string_builder.fill();
    if(hours > 0)
    {
        string_builder << hours << ':';
        string_builder << std::setfill('0') << std::setw(2) << minutes << ':' << seconds;
        if(ms > 0)
        {
            string_builder << std::setfill(default_fill) << std::setw(0) << '.' << ms;
        }
    }
    else if(minutes > 0)
    {
        string_builder << minutes << ':';
        string_builder << std::setfill('0') << std::setw(2) << seconds;
        if(ms > 0)
        {
            string_builder << std::setfill(default_fill) << std::setw(0) << '.' << ms;
        }
    }
    else
    {
        string_builder << seconds << '.' << ms;
    }

    return string_builder.str();
}

std::string time_ms_to_string(int time_ms)
{
    int ms = time_ms % 1000;
    time_ms = (time_ms - ms) / 1000;
    int seconds = time_ms % 60;
    time_ms = (time_ms - seconds) / 60;
    int minutes = time_ms % 60;
    int hours = (time_ms - minutes) / 60;

    std::ostringstream string_builder;
    string_builder << hours << ':' << std::setfill('0') << std::setw(2) << minutes << ':' << seconds;
    string_builder << std::setfill('0') << '.' << std::setw(3) << ms;

    return string_builder.str();
}
