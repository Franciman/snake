#ifndef time_interval_h_INCLUDED
#define time_interval_h_INCLUDED

#include <utility>

struct TimeInterval: public std::pair<int, int>
{
    using std::pair<int, int>::pair;
    int start_time() const
    {
        return first;
    }

    int end_time() const
    {
        return second;
    }

    void set_start_time(int t)
    {
        first = t;
    }

    void set_end_time(int t)
    {
        second = t;
    }

    bool operator<(TimeInterval other) const
    {
        return start_time() < other.start_time() ||
               (start_time() == other.start_time() && end_time() < other.end_time());
    }

    bool overlaps(TimeInterval other) const
    {
        return start_time() <= other.end_time() && other.start_time() <= end_time();
    }
};


#endif // time_interval_h_INCLUDED

