#include "application_state.h"

SubtitleSelectionModel::SubtitleSelectionModel(ApplicationState *model):
    QItemSelectionModel(model)
{ }

QVariant ApplicationState::data(const QModelIndex &index, int role) const
{
    int row = index.row();
    int column = index.column();

    if(role == Qt::DisplayRole)
    {
        Subtitle sub = m_list[row];
        switch(column)
        {
            case 0:
                return sub.start_time();
            case 1:
                return sub.end_time();
            case 2:
                return sub.dialog().c_str();
            default:
                return {};
        }
    }
    return {};
}

QVariant ApplicationState::headerData(int section, Qt::Orientation orientation, int role) const
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

void ApplicationState::remove_subtitle(size_t index)
{
    beginRemoveRows(QModelIndex(), index, index);
    m_list.delete_subtitle(index);
    endRemoveRows();
}

void ApplicationState::insert_subtitle(TimeInterval i, const std::string &text)
{
    InsertPos pos = m_list.get_insert_pos(i);
    beginInsertRows(QModelIndex(), pos.index(), pos.index());
    m_list.insert_dialog_at(pos, text);
    endInsertRows();
}

void ApplicationState::load_subtitles(SubtitleList &&list)
{
    beginResetModel();
    m_list = std::move(list);
    endResetModel();
}

void ApplicationState::update_subtitle_dialog(size_t index, const std::string &dialog)
{
    if(m_list[index].dialog() != dialog)
    {
        m_list.update_dialog(m_list[index], dialog);
        emit dataChanged(createIndex(index, 0), createIndex(index, 2));
    }
}

void ApplicationState::update_subtitle_dialog(size_t index, std::string &&dialog)
{
    if(m_list[index].dialog() != dialog)
    {
        m_list.update_dialog(m_list[index], std::move(dialog));
        emit dataChanged(createIndex(index, 0), createIndex(index, 2));
    }
}
