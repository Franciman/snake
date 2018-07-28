#include "subtitle_list.h"

#include <algorithm>
#include <iterator>

Subtitle SubtitleList::insert_item(Item &&i)
{
    auto insert_pos = std::upper_bound(m_subtitles.begin(), m_subtitles.end(), i);
    insert_pos = m_subtitles.emplace(insert_pos, std::move(i));
    update_max_end_points();
    return {insert_pos};
}

void SubtitleList::update_dialog(Subtitle s, const std::string &d)
{
    size_t index = std::distance(m_subtitles.cbegin(), s.m_item);
    m_subtitles[index].set_dialog(d);
}

Subtitle SubtitleList::update_timing(Subtitle s, TimeInterval t)
{
    size_t index = std::distance(m_subtitles.cbegin(), s.m_item);
    Item i{std::move(m_subtitles[index])};
    m_subtitles.erase(s.m_item);
    i.set_time_interval(t);
    i.set_max_end_time(t.end_time());

    return insert_item(std::move(i));
}

int SubtitleList::update_max_end_points_impl(SubtitleList::VectorIterator begin,
                                             SubtitleList::VectorIterator end)
{
    if(begin == end)
    {
        return std::numeric_limits<int>::min();
    }

    size_t half_count = std::distance(begin, end) / 2;
    VectorIterator half = begin + half_count;

    int left_max_end_point = update_max_end_points_impl(begin, half);
    int right_max_end_point = update_max_end_points_impl(half + 1, end);

    int new_max_end_point = std::max(left_max_end_point, right_max_end_point);
    half->set_max_end_time(std::max(half->max_end_time(), new_max_end_point));

    return half->max_end_time();
}
