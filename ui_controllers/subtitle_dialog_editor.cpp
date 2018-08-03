#include "subtitle_dialog_editor.h"

void SubtitleDialogEditor::setModel(SubtitleManager *model)
{
    if(m_model)
    {
        m_model->disconnect(m_data_changed_connection);
    }
    m_model = model;
    if(m_model)
    {
        setEnabled(true);
        m_data_changed_connection = connect(m_model, &SubtitleManager::dataChanged, this, &SubtitleDialogEditor::update_after_data_changed);
    }
    else
    {
        setEnabled(false);
    }
}

void SubtitleDialogEditor::setSelectionModel(SubtitleSelectionModel *model)
{
    if(m_selection_model)
    {
        m_selection_model->disconnect(m_current_changed_connection);
    }
    m_selection_model = model;
    if(m_selection_model)
    {
        m_current_changed_connection = connect(m_selection_model, &SubtitleSelectionModel::currentChanged, this, &SubtitleDialogEditor::update_after_current_changed);
        setEnabled(m_selection_model->currentIndex().isValid());
    }
}

void SubtitleDialogEditor::update_after_data_changed(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles)
{
    const QModelIndex &currentIndex = m_selection_model->currentIndex();
    // If selection was updated too, update text, otherwise no need to update
    if(topLeft < currentIndex && currentIndex < bottomRight && m_model->subtitle(currentIndex).dialog().c_str() != toPlainText())
    {
        setPlainText(currentIndex.data().toString());
    }
}

void SubtitleDialogEditor::update_after_current_changed(const QModelIndex &current, const QModelIndex &previous)
{
    if(current.isValid())
    {
        setEnabled(true);
        setPlainText(m_model->subtitle(current).dialog().c_str());
    }
    else
    {
        setEnabled(false);
        setPlainText("");
    }
}

void SubtitleDialogEditor::update_subtitle_after_text_changed()
{
    if(m_selection_model->currentIndex().isValid())
    {
        m_model->update_subtitle_dialog(m_selection_model->currentIndex().row(),
                                        toPlainText().toStdString());
    }
}
