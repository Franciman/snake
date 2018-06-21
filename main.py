import sys
from intervaltree import Interval, IntervalTree
import subtitles
import srtparser
import srt


def main(argv):
    if len(argv) < 2:
        print("Usage python main.py filename")
        sys.exit()

    _, filename = argv

    try:
        subs = srtparser.parse_srt(filename)
    except srt.SRTParseError:
        print("Invalid Srt file")
        sys.exit()

    subs_set = subtitles.SubtitleSet(subs)

    for s in subs_set.get_overlapping_subtitles(1000, 5000):
        print(s.start_time())
        print(s.end_time())
        print(s.dialog())
        s.set_dialog("Russ is a Genius")
        s.update_interval(6000, 6100)


    for s in subs_set.get_overlapping_subtitles(6100, 7000):
        print(s.start_time())
        print(s.end_time())
        print(s.dialog())

if __name__ == "__main__":
    main(sys.argv)
