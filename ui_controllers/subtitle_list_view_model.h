#ifndef subtitle_list_view_model_h_INCLUDED
#define subtitle_list_view_model_h_INCLUDED

#include <QAbstractTableModel>

#include <subtitle_list/subtitle_list.h>

class ApplicationState;

class SubtitleListViewModel: public QAbstractTableModel
{
    Q_OBJECT

    ApplicationState &m_state;

public:
    SubtitleListViewModel(ApplicationState &state, QObject *parent = nullptr):
        QAbstractTableModel(parent),
        m_state(state)
    { }

    virtual ~SubtitleListViewModel() { }

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override
    {
        return 3;
    }

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

public slots:
    void reload_data()
    {
        beginResetModel();
        endResetModel();
    }

    void update_subtitle(Subtitle s);
    void insert_subtitle(Subtitle s);
    void erase_subtitle(size_t index);
};

#endif // subtitle_list_view_model_h_INCLUDED

