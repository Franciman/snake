#ifndef subtitle_h_INCLUDED
#define subtitle_h_INCLUDED

#include <vector>

#include "item.h"

class Subtitle
{
    std::vector<Item>::const_iterator m_item;

    Subtitle(std::vector<Item>::const_iterator item):
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

    friend class SubtitleList;
};


#endif // subtitle_h_INCLUDED

