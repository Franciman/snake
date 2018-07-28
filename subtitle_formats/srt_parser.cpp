#include "srt_parser.h"

#include <regex>
#include <iomanip>
#include <fstream>
#include <string>

int parse_int(const std::ssub_match &time)
{
    int res = 0;
    for(auto it = time.first; it != time.second; ++it)
    {
        res *= 10;
        res += *it - '0';
    }
    return res;
}

int parse_time_ms(const std::ssub_match &hours,
                  const std::ssub_match &minutes,
                  const std::ssub_match &seconds,
                  const std::ssub_match &milliseconds)
{
    int h  = parse_int(hours);
    int m  = parse_int(minutes) + h * 60;
    int s  = parse_int(seconds) + m * 60;
    int ms = parse_int(milliseconds) + s * 1000;

    return ms;
}

std::vector<SrtSubtitle> parse_srt_file(const char *filename)
{
    std::regex header_matcher(R"!(\d+\r\n(\d\d):(\d\d):(\d\d),(\d\d\d) --> (\d\d):(\d\d):(\d\d),(\d\d\d)\r\n)!");

    std::ifstream file{filename};

    file >> std::noskipws;

    std::istream_iterator<char> begin{file}, end{};

    std::string content{begin, end};

    std::vector<SrtSubtitle> result;

    std::regex_iterator<std::string::const_iterator> match_iterator{content.begin(), content.end(), header_matcher};
    std::regex_iterator<std::string::const_iterator> match_end_iterator{};

    while(match_iterator != match_end_iterator)
    {
        std::string::const_iterator dialog_start = (*match_iterator)[0].second;
        std::string::const_iterator dialog_end   = content.end();

        int start_time = parse_time_ms((*match_iterator)[1],
                                       (*match_iterator)[2],
                                       (*match_iterator)[3],
                                       (*match_iterator)[4]);

        int end_time = parse_time_ms((*match_iterator)[5],
                                     (*match_iterator)[6],
                                     (*match_iterator)[7],
                                     (*match_iterator)[8]);

        ++match_iterator;
        if(match_iterator != match_end_iterator)
        {
            dialog_end = (*match_iterator)[0].first;
        }

        result.emplace_back(start_time, end_time, std::string{dialog_start, dialog_end});
    }

    return result;
}
