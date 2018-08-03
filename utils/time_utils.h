#ifndef time_utils_h_INCLUDED
#define time_utils_h_INCLUDED

#include <string>

std::string time_ms_to_short_string(int time_ms, int precision_ms, int precision_log_ms);
std::string time_ms_to_string(int time_ms);

#endif // time_utils_h_INCLUDED

