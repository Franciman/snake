#ifndef subtitle_list_view_h_INCLUDED
#define subtitle_list_view_h_INCLUDED

#include <QTableView>

class SubtitleSelectionModel;

class SubtitleListView: public QTableView
{
    Q_OBJECT

    SubtitleSelectionModel *m_selection_model;
    QMetaObject::Connection m_selection_changed_connection;
public:
    SubtitleListView(QWidget *parent = nullptr);

    void set_selection_model(SubtitleSelectionModel *model);

protected:
    void currentChanged(const QModelIndex &current, const QModelIndex &previous) override;

private slots:
    void update_after_selection_changed(int current, int previous);
};

#endif // subtitle_list_view_h_INCLUDED

