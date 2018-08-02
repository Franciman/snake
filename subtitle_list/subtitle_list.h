#ifndef subtitle_list_h_INCLUDED
#define subtitle_list_h_INCLUDED

#include "item.h"
#include <deque>

class InsertPos
{
    TimeInterval m_interval;
    size_t m_index;

    friend class SubtitleList;

    InsertPos(const TimeInterval &interval, size_t index):
        m_interval(interval),
        m_index(index)
    { }

public:
    const TimeInterval &time_interval() const
    {
        return m_interval;
    }

    size_t index() const
    {
        return m_index;
    }
};

class SubtitleList
{
    using Container = std::deque<Item>;
    using ContainerIterator = Container::iterator;
    using ContainerConstIterator = Container::const_iterator;
    Container m_subtitles;
public:
    class Subtitle
    {
        ContainerConstIterator m_item;

        Subtitle(ContainerConstIterator item):
            m_item(item)
        { }
    public:
        int start_time() const
        {
            return m_item->start_time();
        }

        int end_time() const
        {
            return m_item->end_time();
        }

        const std::string &dialog() const
        {
            return m_item->dialog();
        }

        bool operator==(Subtitle other) const
        {
            return m_item == other.m_item;
        }

        bool operator!=(Subtitle other) const
        {
            return !(*this == other);
        }

        friend class SubtitleList;
    };

    SubtitleList() = default;
    SubtitleList(std::initializer_list<std::pair<TimeInterval, std::string>> arguments)
    {
        for(auto const &element: arguments)
        {
            create_subtitle(element.first, element.second);
        }
    }

    InsertPos get_insert_pos(const TimeInterval &i) const;

    Subtitle insert_dialog_at(InsertPos pos, const std::string &dialog);
    Subtitle insert_dialog_at(InsertPos pos, std::string &&dialog);

    Subtitle create_subtitle(TimeInterval timing, const std::string &text)
    {
        Item new_element{timing, text};
        return insert_item(std::move(new_element));
    }

    Subtitle create_subtitle(TimeInterval timing, std::string &&text)
    {
        Item new_element{timing, std::move(text)};
        return insert_item(std::move(new_element));
    }

    // Returns the Subtitle right after the one deleted
    Subtitle delete_subtitle(Subtitle s)
    {
        ContainerIterator next_item = m_subtitles.erase(s.m_item);
        update_max_end_points();
        return {next_item};
    }

    Subtitle delete_subtitle(size_t index)
    {
        return delete_subtitle((*this)[index]);
    }

    Subtitle operator[](size_t index) const
    {
        return {m_subtitles.begin() + index};
    }

    size_t index(Subtitle s) const
    {
        return std::distance(m_subtitles.begin(), s.m_item);
    }

    template <class Func>
    void for_each_overlapping(TimeInterval i, Func f) const
    {
        find_overlaps(m_subtitles.begin(), m_subtitles.end(), f);
    }

    void update_dialog(Subtitle s, const std::string &d);
    Subtitle update_timing(Subtitle s, TimeInterval t);

    size_t size() const
    {
        return m_subtitles.size();
    }

private:
    Subtitle insert_item(Item &&i);

    int update_max_end_points_impl(ContainerIterator begin, ContainerIterator end);

    void update_max_end_points()
    {
        update_max_end_points_impl(m_subtitles.begin(), m_subtitles.end());
    }

    template <class Func>
    void report_overlaps(ContainerConstIterator begin, ContainerConstIterator end, TimeInterval i, Func f) const;
};



template <class Func>
void SubtitleList::report_overlaps(SubtitleList::ContainerConstIterator begin,
                                 SubtitleList::ContainerConstIterator end,
                                 TimeInterval i,
                                 Func f) const
{
    if(begin == end) return;

    size_t count = std::distance(begin, end);

    ContainerConstIterator half = begin + count / 2;

    if(half->max_end_time() < i.start_time()) return;

    if(i.overlaps(half->time_interval()))
    {
        f({half});
    }

    report_overlaps(begin, half, i, f);

    if(half->start_time() <= i.end_time())
    {
        report_overlaps(half + 1, end, i, f);
    }
}

using Subtitle = SubtitleList::Subtitle;

#endif // subtitle_list_h_INCLUDED

