#ifndef application_state_h_INCLUDED
#define application_state_h_INCLUDED

#include <subtitle_list/subtitle_list.h>

#include <optional>
#include <QObject>
#include <QItemSelectionModel>

class ApplicationState;

class SubtitleSelectionModel: public QItemSelectionModel
{
    Q_OBJECT

public:
    SubtitleSelectionModel(ApplicationState *model = nullptr);
};

class ApplicationState: public QAbstractTableModel
{
    Q_OBJECT

    SubtitleList m_list;
public:
    ApplicationState(QObject *parent = nullptr):
        QAbstractTableModel(parent)
    { }

    virtual int columnCount(const QModelIndex &) const override
    {
        return 3;
    }

    virtual int rowCount(const QModelIndex &) const override
    {
        return m_list.size();
    }

    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override
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

    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override
    {
        if(orientation == Qt::Horizontal && role == Qt::DisplayRole)
        {
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
        }
        return {};
    }

    void insert_subtitle(const TimeInterval &i, const std::string &dialog);
    // void insert_subtitle(const TimeInterval &i, std::string &&dialog);

    void remove_subtitle(size_t index);
};

#endif // application_state_h_INCLUDED

