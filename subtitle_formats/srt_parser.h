#ifndef srt_parser_h_INCLUDED
#define srt_parser_h_INCLUDED

#include <string>
#include <vector>

struct SrtSubtitle
{
    int start_time;
    int end_time;

    std::string dialog;

    SrtSubtitle(int _start_time, int _end_time, std::string &&_dialog):
        start_time(_start_time),
        end_time(_end_time),
        dialog(std::move(_dialog))
    { }

    SrtSubtitle(int _start_time, int _end_time, const std::string &_dialog):
        start_time(_start_time),
        end_time(_end_time),
        dialog(_dialog)
    { }
};

std::vector<SrtSubtitle> parse_srt_file(const char *filename);

#endif // srt_parser_h_INCLUDED

