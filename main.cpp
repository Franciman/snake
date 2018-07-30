#include <iostream>

#include <subtitle_formats/srt_parser.h>

int main(int argc, char *argv[])
{
    if(argc != 2) return 0;

    std::vector<SrtSubtitle> subs = parse_srt_file(argv[1]);
    std::cout << "Size: " << subs.size() << std::endl;
    return 0;
}
