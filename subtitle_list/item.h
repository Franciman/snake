#ifndef item_h_INCLUDED
#define item_h_INCLUDED

#include <string>

#include "time_interval.h"

class Item
{
    int m_max_end_time;
    TimeInterval m_timing;
    std::string m_dialog;

public:
    Item(TimeInterval timing, const std::string &dialog):
        m_max_end_time(timing.end_time()),
        m_timing(timing),
        m_dialog(dialog)
    { }

    Item(TimeInterval timing, std::string &&dialog):
        m_max_end_time(timing.end_time()),
        m_timing(timing),
        m_dialog(std::move(dialog))
    { }

    Item(const Item &) = default;
    Item(Item &&) = default;

    Item &operator=(const Item &) = default;
    Item &operator=(Item &&) = default;

    int start_time() const
    {
        return time_interval().start_time();
    }

    int end_time() const
    {
        return time_interval().end_time();
    }

    int get_boundary(IntervalBoundary b) const
    {
        return time_interval().get_boundary(b);
    }

    const TimeInterval &time_interval() const
    {
        return m_timing;
    }

    const std::string &dialog() const
    {
        return m_dialog;
    }

    std::string &dialog()
    {
        return m_dialog;
    }

    void set_time_interval(const TimeInterval &t)
    {
        m_timing = t;
    }

    void set_time_interval(TimeInterval &&t)
    {
        m_timing = std::move(t);
    }

    bool set_start_time(int t)
    {
        return m_timing.set_start_time(t);
    }

    bool set_end_time(int t)
    {
        return m_timing.set_end_time(t);
    }

    bool update_boundary(int t, IntervalBoundary b)
    {
        return m_timing.update_boundary(t, b);
    }

    void set_dialog(const std::string &s)
    {
        m_dialog = s;
    }

    void set_dialog(std::string &&s)
    {
        m_dialog = std::move(s);
    }

    int max_end_time() const
    {
        return m_max_end_time;
    }

    void set_max_end_time(int t)
    {
        m_max_end_time = t;
    }

    bool operator<(const Item &other) const
    {
        return m_timing < other.m_timing;
    }
};
#endif // item_h_INCLUDED

