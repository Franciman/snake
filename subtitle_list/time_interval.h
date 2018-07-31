#ifndef time_interval_h_INCLUDED
#define time_interval_h_INCLUDED

#include <utility>

// This enum indicates a side of the interval
// e.g. IntervalBoundary::Start means that a number is the start point of the interval
enum class IntervalBoundary: bool
{
    Start,
    End,
};

IntervalBoundary flip_boundary(IntervalBoundary b);

struct TimeInterval: public std::pair<int, int>
{
    TimeInterval(int start_time, int end_time):
        std::pair<int, int>(start_time, end_time)
    {
        keep_consistency();
    }

    int start_time() const
    {
        return first;
    }

    int end_time() const
    {
        return second;
    }

    int get_boundary(IntervalBoundary b) const
    {
        switch(b)
        {
        case IntervalBoundary::Start:
            return start_time();

        case IntervalBoundary::End:
            return end_time();
        }
    }

    bool set_start_time(int t)
    {
        first = t;
        return keep_consistency();
    }

    bool set_end_time(int t)
    {
        second = t;
        return keep_consistency();
    }

    bool update_boundary(int t, IntervalBoundary b)
    {
        switch(b)
        {
        case IntervalBoundary::Start:
            return set_start_time(t);

        case IntervalBoundary::End:
            return set_end_time(t);
        }
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

private:
    // Flip interval if start_time > end_time
    bool keep_consistency()
    {
        if(start_time() > end_time())
        {
            std::swap(first, second);
            return true;
        }
        return false;
    }
};


#endif // time_interval_h_INCLUDED

