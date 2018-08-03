#include "subtitle_list_view.h"

#include <application_state.h>

SubtitleListView::SubtitleListView(QWidget *parent):
    QTableView(parent),
    m_selection_model(nullptr)
{
    setSelectionBehavior(QAbstractItemView::SelectRows);
    setSelectionMode(QAbstractItemView::SingleSelection);
}

void SubtitleListView::set_selection_model(SubtitleSelectionModel *model)
{
    if(m_selection_model)
    {
        m_selection_model->disconnect(m_selection_changed_connection);
    }
    m_selection_model = model;
    if(m_selection_model)
    {
        m_selection_changed_connection = connect(m_selection_model, &SubtitleSelectionModel::selection_changed,
                                                 this, &SubtitleListView::update_after_selection_changed);
    }
}

void SubtitleListView::currentChanged(const QModelIndex &current, const QModelIndex &previous)
{
    if(m_selection_model)
    {
        if(current.isValid())
        {
            m_selection_model->set_current_subtitle(current.row());
        }
        else
        {
            m_selection_model->clear_selection();
        }
    }
}

void SubtitleListView::update_after_selection_changed(int current, int previous)
{
    if(current == -1)
    {
        clearSelection();
    }
    else
    {
        setCurrentIndex(model()->index(current, 0));
    }
}
