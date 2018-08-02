#ifndef application_state_h_INCLUDED
#define application_state_h_INCLUDED

#include <subtitle_list/subtitle_list.h>

#include <optional>
#include <QAbstractTableModel>
#include <QItemSelectionModel>

class ApplicationState;

class SubtitleSelectionModel: public QItemSelectionModel
{
    Q_OBJECT
public:
    SubtitleSelectionModel(ApplicationState *model);
};

class ApplicationState: public QAbstractTableModel
{
    Q_OBJECT

    SubtitleList m_list;
public:
    ApplicationState(QObject *parent = nullptr):
        QAbstractTableModel(parent)
    { }

    int columnCount(const QModelIndex &) const override
    {
        return 3;
    }

    int rowCount(const QModelIndex &) const override
    {
        return m_list.size();
    }

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    void remove_subtitle(size_t index);
    void remove_subtitle(const QModelIndex &index)
    {
        remove_subtitle(index.row());
    }

    void insert_subtitle(TimeInterval i, const std::string &text);

    void update_subtitle_dialog(size_t index, const std::string &text);
    void update_subtitle_dialog(size_t index, std::string &&text);

    Subtitle subtitle(size_t index) const
    {
        return m_list[index];
    }

    Subtitle subtitle(const QModelIndex &index) const
    {
        return m_list[index.row()];
    }

    // Reload all subtitles
    void load_subtitles(SubtitleList &&list);
};

#endif // application_state_h_INCLUDED

