#ifndef subtitle_dialog_editor_h_INCLUDED
#define subtitle_dialog_editor_h_INCLUDED

#include <QPlainTextEdit>

#include <application_state.h>

class SubtitleDialogEditor: public QPlainTextEdit
{
    Q_OBJECT

    SubtitleManager *m_model;
    SubtitleSelectionModel *m_selection_model;

    QMetaObject::Connection m_data_changed_connection;
    QMetaObject::Connection m_current_changed_connection;

public:
    SubtitleDialogEditor(QWidget *parent = nullptr):
        QPlainTextEdit(parent)
    { 
        setEnabled(false);
        connect(this, &SubtitleDialogEditor::textChanged, this, &SubtitleDialogEditor::update_subtitle_after_text_changed);
    }

    void setModel(SubtitleManager *model);
    void setSelectionModel(SubtitleSelectionModel *model);

private slots:
    void update_after_data_changed(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles);
    void update_after_current_changed(int current, int previous);
    void update_subtitle_after_text_changed();
};

#endif // subtitle_dialog_editor_h_INCLUDED

