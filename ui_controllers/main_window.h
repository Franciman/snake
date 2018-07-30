#ifndef main_window_h_INCLUDED
#define main_window_h_INCLUDED

#include <QMainWindow>
#include <application_state.h>
#include <ui_controllers/subtitle_list_view_model.h>

class QItemSelection;

namespace Ui
{
    class MainWindow;
}

class MainWindow: public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void open_subtitles();
    void select_subtitle(const QItemSelection &selected, const QItemSelection &deselected);
    void update_selection();
    void update_subtitle_text();
    void remove_selected_subtitle();

private:
    Ui::MainWindow *ui;
    ApplicationState m_state;
    SubtitleListViewModel m_model;
};

#endif // main_window_h_INCLUDED

