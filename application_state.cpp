#include "application_state.h"

#include <utils/time_utils.h>

SubtitleSelectionModel::SubtitleSelectionModel(SubtitleManager *model, QObject *parent):
    QObject(parent),
    m_model(model),
    m_selection(-1)
{
    if(m_model)
    {
        connect(m_model, &SubtitleManager::rowsInserted,
                this, &SubtitleSelectionModel::update_after_rows_inserted);
        connect(m_model, &SubtitleManager::rowsRemoved,
                this, &SubtitleSelectionModel::update_after_rows_removed);

    }
}

void SubtitleSelectionModel::set_current_subtitle(Subtitle s)
{
    if(m_model)
    {
        int old_index = m_selection;
        int new_index = m_model->subtitles().index(s);
        if(old_index != new_index)
        {
            m_selection = new_index;
            emit selection_changed(new_index, old_index);
        }
    }
}

void SubtitleSelectionModel::set_current_subtitle(int new_index)
{
    if(m_model)
    {
        int old_index = m_selection;
        if(old_index != new_index)
        {
            m_selection = new_index;
            emit selection_changed(new_index, old_index);
        }
    }
}

void SubtitleSelectionModel::clear_selection()
{
    if(m_model)
    {
        if(m_selection != -1)
        {
            int old_index = m_selection;
            m_selection = -1;
            emit selection_changed(-1, old_index);
        }
    }
}

Subtitle SubtitleSelectionModel::selected_subtitle() const
{
    return m_model->subtitles()[m_selection];
}

void SubtitleSelectionModel::update_after_rows_inserted(const QModelIndex &parent, int first, int last)
{
    if(m_selection >= first)
    {
        int count = last - first + 1;
        int old_selection = m_selection;
        m_selection += count;
        emit selection_changed(m_selection, old_selection);
    }
}

void SubtitleSelectionModel::update_after_rows_removed(const QModelIndex &parent, int first, int last)
{
    int old_selection;
    if(m_selection > last)
    {
        int count = last - first + 1;
        old_selection = m_selection;
        m_selection -= count;
    }
    else if(first <= m_selection && m_selection <= last)
    {
        old_selection = m_selection;
        if(m_model->rowCount(QModelIndex()) == 0)
        {
            m_selection = -1;
        }
        else
        {
            m_selection = first < m_model->rowCount(QModelIndex()) ? first : m_model->rowCount(QModelIndex());
        }
    }
    emit selection_changed(m_selection, old_selection);
}

QVariant SubtitleManager::data(const QModelIndex &index, int role) const
{
    int row = index.row();
    int column = index.column();

    if(role == Qt::DisplayRole)
    {
        Subtitle sub = m_list[row];
        switch(column)
        {
            case 0:
                return time_ms_to_string(sub.start_time()).c_str();
            case 1:
                return time_ms_to_string(sub.end_time()).c_str();
            case 2:
                return sub.dialog().c_str();
            default:
                return {};
        }
    }
    return {};
}

QVariant SubtitleManager::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(role != Qt::DisplayRole) return {};

    switch(orientation)
    {
    case Qt::Horizontal:
        switch(section)
        {
            case 0:
                return "Start time";
            case 1:
                return "End time";
            case 2:
                return "Dialog";
            default:
                return {};
        }

    case Qt::Vertical:
        return section;
    }

    return {};
}

void SubtitleManager::remove_subtitle(size_t index)
{
    beginRemoveRows(QModelIndex(), index, index);
    m_list.delete_subtitle(index);
    endRemoveRows();
}

Subtitle SubtitleManager::insert_subtitle(const TimeInterval &i, const std::string &text)
{
    InsertPos pos = m_list.get_insert_pos(i);
    beginInsertRows(QModelIndex(), pos.index(), pos.index());
    Subtitle res = m_list.insert_dialog_at(pos, text);
    endInsertRows();
    return res;
}

void SubtitleManager::load_subtitles(SubtitleList &&list)
{
    beginResetModel();
    m_list = std::move(list);
    endResetModel();
}

void SubtitleManager::update_subtitle_dialog(size_t index, const std::string &dialog)
{
    if(m_list[index].dialog() != dialog)
    {
        m_list.update_dialog(m_list[index], dialog);
        emit dataChanged(createIndex(index, 0), createIndex(index, 2));
    }
}

void SubtitleManager::update_subtitle_dialog(size_t index, std::string &&dialog)
{
    if(m_list[index].dialog() != dialog)
    {
        m_list.update_dialog(m_list[index], std::move(dialog));
        emit dataChanged(createIndex(index, 0), createIndex(index, 2));
    }
}

void SubtitleManager::update_subtitle_timing(Subtitle s, const TimeInterval &new_interval)
{
    if(new_interval != s.time_interval())
    {
        size_t index = m_list.index(s);
        m_list.update_timing(s, new_interval);
        emit dataChanged(createIndex(index, 0), createIndex(index, 2));
    }
}
