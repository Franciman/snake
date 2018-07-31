#include "time_interval.h"

IntervalBoundary flip_boundary(IntervalBoundary b)
{
    switch(b)
    {
    case IntervalBoundary::Start:
        return IntervalBoundary::End;

    case IntervalBoundary::End:
        return IntervalBoundary::Start;
    }
}
