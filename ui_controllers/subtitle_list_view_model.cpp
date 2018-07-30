#include "subtitle_list_view_model.h"

#include <application_state.h>
#include <subtitle_list/subtitle_list.h>

int SubtitleListViewModel::rowCount(const QModelIndex &parent) const
{
    return m_state.subtitles().size();
}

QVariant SubtitleListViewModel::data(const QModelIndex &index, int role) const
{
    int row = index.row();
    int column = index.column();

    if(role == Qt::DisplayRole)
    {
        switch(column)
        {
        case 0:
            return QVariant{m_state.subtitles()[row].start_time()};
        case 1:
            return QVariant{m_state.subtitles()[row].end_time()};
        case 2:
            return QVariant{m_state.subtitles()[row].dialog().c_str()};
        default:
            return QVariant{};
        }
    }

    return QVariant{};
}

void SubtitleListViewModel::update_subtitle(Subtitle s)
{
    size_t index = m_state.subtitles().index(s);
    // Update line of the given subtitle
    emit dataChanged(createIndex(index, 0), createIndex(index, 2));
}

void SubtitleListViewModel::insert_subtitle(Subtitle s)
{
    size_t index = m_state.subtitles().index(s);
    beginInsertRows(QModelIndex(), index, index);
    endInsertRows();
}

void SubtitleListViewModel::erase_subtitle(size_t index)
{
    beginRemoveRows(QModelIndex(), index, index);
    endRemoveRows();
}
