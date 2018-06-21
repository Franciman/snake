from intervaltree import Interval, IntervalTree

class SubtitleBase:
    """
    Base class common to all subtitle types
    A subtitle has two components:
        - an interval [start_time, end_time], with both ends included, indicating the subtitle timing in milliseconds (at what time it must appear on video, and at what time it must disappear)
        - a string with the subtitle's text

    The interval is immutable, while the dialog can be mutated
    """
    def start_time(self):
        pass

    def end_time(self):
        pass

    def dialog(self):
        pass

    def set_dialog(self, new_dialog):
        pass

    def update_interval(self, new_start, new_end):
        pass



class StoredSubtitle(SubtitleBase):
    """
    A subtitle in the SubtitleSet
    """
    def __init__(self, owner, interval):
        self.owner = owner
        self.interval = interval

    def start_time(self):
        return self.interval.begin

    def end_time(self):
        return self.interval.end - 1

    def dialog(self):
        return self.owner._dialog_for_id(self.interval.data)

    def set_dialog(self, new_dialog):
        self.owner._set_dialog_for_id(self.interval.data, new_dialog)

    def update_interval(self, new_start, new_end):
        self.owner._update_interval(self.interval, new_start, new_end)





class SubtitleSet:
    """
    Class containing a set of subtitles
    We keep a dictionary Int -> String
    and an Interval Tree

    The dictionary associates an id to each subtitle's dialog
    Each Interval in the IntervalTree contains the interval, of course, and an Int. The Int is the dialog's id
    In this way we can have subtitles with same interval and same dialog text as separate entities.

    Therefore a Subtitle with interval [a, b] and dialog D
    is stored in this way:
        generate a new id;
        add dialog D to the dictionary at key id
        add ([a, b], id) to the interval tree
    """

    # The IntervalTree acts on intervals of the type [start, end), so the endpoint is not included
    # We need it to be included.
    # In our case, since the endpoints are integers, and therefore discrete, we have: [start, end+1) = [start, end]
    # So to make IntervalTree consider the right interval we just add 1 to the endpoint
    # Note that on the outside we always consider intervals of the form [start, end]
    # so all the conversion needs to be done here

    def __init__(self, subs = []):
        self.subs_collection = IntervalTree()
        self.dialogs_dict = dict()
        self.dialogs_id_counter = 0
        for subtitle in subs:
            self.add_subtitle(subtitle)


    def get_overlapping_subtitles(self, start, end):
        overlaps = self.subs_collection[start : end + 1]
        return map(lambda res: StoredSubtitle(self, res), overlaps)

    def get_subtitles_at(self, point):
        return get_overlapping_subtitles(self, point, point)


    def add_subtitle(self, subtitle):
        new_dialog_id = self._make_new_dialog_id()
        self._set_dialog_for_id(new_dialog_id, subtitle.dialog())
        self.subs_collection.addi(subtitle.start_time(), subtitle.end_time() + 1, new_dialog_id)



    def remove_subtitle(self, subtitle):
        dialog_id = subtitle.interval.data
        self.subs_collection.remove(subtitle.interval)
        del self.dialogs_dict[dialog_id]

    # Private functions
    def _make_new_dialog_id(self):
        new_dialog_id = self.dialogs_id_counter
        self.dialogs_id_counter += 1
        if self.dialogs_id_counter == 0:
            raise MemoryError
        else:
            return new_dialog_id

    def _dialog_for_id(self, dialog_id):
        return self.dialogs_dict[dialog_id]

    def _set_dialog_for_id(self, dialog_id, new_dialog):
        self.dialogs_dict[dialog_id] = new_dialog

    def _update_interval(self, interval, new_start, new_end):
        new_interval = Interval(new_start, new_end + 1, interval.data)
        self.subs_collection.remove(interval)
        self.subs_collection.add(new_interval)

