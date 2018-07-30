#include <iostream>

#include <subtitle_formats/srt_parser.h>
#include <subtitle_list/subtitle_list.h>
#include <utils/time_it.h>

int main(int argc, char *argv[])
{
    if(argc != 2) return 0;

    auto res = time_it([argv]() {
        std::vector<SrtSubtitle> subs = parse_srt_file(argv[1]);
        std::cout << "Size: " << subs.size() << std::endl;
        SubtitleList list;
        for(const auto &s: subs)
        {
            list.create_subtitle({s.start_time, s.end_time}, s.dialog);
        }
    });

    std::cout << "Time elapsed: " << res.count() << " ms" << std::endl;
    return 0;
}
