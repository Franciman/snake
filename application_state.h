#ifndef application_state_h_INCLUDED
#define application_state_h_INCLUDED

#include <subtitle_list/subtitle_list.h>

#include <optional>
#include <QObject>

class ApplicationState;

class SubtitleSelection: public Subtitle
{
    ApplicationState *m_owner;
public:
    SubtitleSelection(ApplicationState &owner, Subtitle subtitle):
        Subtitle(subtitle),
        m_owner(&owner)
    { }

    inline size_t index() const;

    inline void set_dialog(const std::string &d);

    inline void set_dialog(std::string &&d);

    inline void update_interval(const TimeInterval &t);

    bool operator==(Subtitle s) const
    {
        return Subtitle::operator==(s);
    }

    bool operator!=(Subtitle s) const
    {
        return !(*this == s);
    }

    void set_subtitle(Subtitle s)
    {
        Subtitle::operator=(s);
    }

};

class ApplicationState: public QObject
{
    Q_OBJECT

    friend class SubtitleSelection;

    SubtitleList m_list;
    std::optional<SubtitleSelection> m_selection; 

public:
    ApplicationState(QObject *parent = nullptr):
        QObject(parent)
    { }

    void load_subtitles(SubtitleList &&list)
    {
        m_list = std::move(list);
        m_selection.reset();
        emit subtitles_loaded();
    }

    const std::optional<SubtitleSelection> &selection() const
    {
        return m_selection;
    }

    std::optional<SubtitleSelection> &selection()
    {
        return m_selection;
    }

    void set_selection(Subtitle s)
    {
        if(s != m_selection)
        {
            m_selection = {*this, s};
            emit selection_changed();
        }
    }

    void unset_selection()
    {
        if(m_selection)
        {
            m_selection.reset();
            emit selection_changed();
        }
    }

    const SubtitleList &subtitles() const
    {
        return m_list;
    }


    void remove_subtitle(size_t index);

    Subtitle insert_subtitle(TimeInterval i, const std::string &text);

signals:
    void subtitles_loaded();
    void selection_changed();
    void inserted_subtitle(Subtitle s);
    void removed_subtitle(size_t index);
    void subtitle_changed(Subtitle s);
    void subtitles_reordered();
};

size_t SubtitleSelection::index() const
{
    return m_owner->m_list.index(*this);
}

void SubtitleSelection::set_dialog(const std::string &d)
{
    m_owner->m_list.update_dialog(*this, d);
    emit m_owner->subtitle_changed(*this);
}

void SubtitleSelection::set_dialog(std::string &&d)
{
    m_owner->m_list.update_dialog(*this, std::move(d));
    emit m_owner->subtitle_changed(*this);
}

void SubtitleSelection::update_interval(const TimeInterval &t)
{
    Subtitle self = m_owner->m_list.update_timing(*this, t);
    set_subtitle(self);
    emit m_owner->subtitles_reordered();
}

#endif // application_state_h_INCLUDED

