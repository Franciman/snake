import srt
import subtitles

class SrtSubtitle(subtitles.SubtitleBase):

    def __init__(self, sub):
        self.sub = sub


    def start_time(self):
        return self.sub.start.total_seconds() * 1000

    def end_time(self):
        return self.sub.end.total_seconds() * 1000

    def dialog(self):
        return self.sub.content


def parse_srt(filename):
    with open(filename) as f:
        subs = list(srt.parse(f.read()))
        return map(lambda s: SrtSubtitle(s), subs)
