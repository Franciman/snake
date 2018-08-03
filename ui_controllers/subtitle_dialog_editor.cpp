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
        m_current_changed_connection = connect(m_selection_model, &SubtitleSelectionModel::selection_changed, this, &SubtitleDialogEditor::update_after_current_changed);
        setEnabled(m_selection_model->has_selection());
    }
}

void SubtitleDialogEditor::update_after_data_changed(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles)
{
    int currentIndex = m_selection_model->selection();
    // If selection was updated too, update text, otherwise no need to update
    if(topLeft.row() < currentIndex && currentIndex < bottomRight.row() && m_model->subtitle(currentIndex).dialog().c_str() != toPlainText())
    {
        setPlainText(m_selection_model->selected_subtitle().dialog().c_str());
    }
}

void SubtitleDialogEditor::update_after_current_changed(int current, int previous)
{
    if(current != -1)
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
    if(m_selection_model->has_selection())
    {
        m_model->update_subtitle_dialog(m_selection_model->selection(),
                                        toPlainText().toStdString());
    }
}
