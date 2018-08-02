#include "application_state.h"

void ApplicationState::insert_subtitle(const TimeInterval &i, const std::string &dialog)
{
    InsertPosHint pos = m_list.insert_pos(i);
    beginInsertRows(QModelIndex(), pos.index(), pos.index());
    m_list.insert_subtitle_at(pos, dialog);
    endInsertRows();
}

void ApplicationState::remove_subtitle(size_t index)
{
    beginRemoveRows(QModelIndex(), index, index);
    m_list.delete_subtitle(index);
    endRemoveRows();
}
