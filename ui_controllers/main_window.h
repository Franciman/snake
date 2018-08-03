#ifndef main_window_h_INCLUDED
#define main_window_h_INCLUDED

#include <QMainWindow>
#include <application_state.h>

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

private:
    Ui::MainWindow *ui;
    SubtitleManager m_model;
    SubtitleSelectionModel m_selection_model;
};

#endif // main_window_h_INCLUDED

