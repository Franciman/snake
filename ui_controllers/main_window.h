#ifndef main_window_h_INCLUDED
#define main_window_h_INCLUDED

#include <QMainWindow>
#include <application_state.h>

class QItemSelection;

namespace Ui
{
    class MainWindow;
}

class MediaRenderer;

class MainWindow: public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void test_open_subtitles();

private slots:
    void open_subtitles();

    void open_video();
    void toggle_detach_video();

    void play();
    void pause();

private:
    Ui::MainWindow *ui;
    SubtitleManager m_model;
    SubtitleSelectionModel m_selection_model;
    MediaRenderer *m_renderer;
};

#endif // main_window_h_INCLUDED

