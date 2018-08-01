#include "application_state.h"

void ApplicationState::remove_subtitle(size_t index)
{
    if(!m_selection)
    {
        m_list.delete_subtitle(index);
    }
    else
    {
        size_t selection_index = m_list.index(*m_selection);

        // The subtitle after the one that got deleted
        Subtitle following_index = m_list.delete_subtitle(index);

        // Update selection
        if(selection_index == index)
        {
            m_selection->set_subtitle(following_index);
        }
        else if(selection_index > index)
        {
            m_selection->set_subtitle(m_list[selection_index - 1]);
        }
        else
        {
            m_selection->set_subtitle(m_list[selection_index]);
        }
    }
    emit removed_subtitle(index);
}

Subtitle ApplicationState::insert_subtitle(TimeInterval i, const std::string &text)
{
    if(!m_selection)
    {
        Subtitle new_sub = m_list.create_subtitle(i, text);
        emit inserted_subtitle(new_sub);
        return new_sub;
    }
    else
    {
        size_t selection_index = m_list.index(*m_selection);
        Subtitle new_sub = m_list.create_subtitle(i, text);
        size_t new_subtitle_index = m_list.index(new_sub);

        // Update selection
        if(selection_index < new_subtitle_index)
        {
            m_selection->set_subtitle(m_list[selection_index]);
        }
        else
        {
            m_selection->set_subtitle(m_list[selection_index + 1]);
        }
        emit inserted_subtitle(new_sub);
        return new_sub;
    }
}
