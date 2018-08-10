#include "main_window.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QTableView>
#include <subtitle_formats/srt_parser.h>
#include <media_renderer.h>

#include <iostream>

#include <utils/time_utils.h>

MainWindow::MainWindow(QWidget *parent):
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_model(),
    m_selection_model(&m_model),
    m_renderer(new MediaRenderer(this))
{
    ui->setupUi(this);

    ui->subtitleList->setModel(&m_model);
    ui->subtitleList->set_selection_model(&m_selection_model);

    
    ui->subtitleDialogEdit->setModel(&m_model);
    ui->subtitleDialogEdit->setSelectionModel(&m_selection_model);

    ui->waveform->setModel(&m_model);
    ui->waveform->setSelectionModel(&m_selection_model);

    ui->videoPlayer->set_media_renderer(*m_renderer);

    connect(ui->actionOpen_subtitles, &QAction::triggered, this, &MainWindow::open_subtitles);

    connect(ui->actionRemove_selected_subtitle, &QAction::triggered, this, [this]()
    {
        if(m_selection_model.has_selection())
        {
            m_model.remove_subtitle(m_selection_model.selection());
        }
    });

    connect(m_renderer, &MediaRenderer::time_pos_changed, this, [&](double time_pos)
    {
        std::cout << "Yo" << std::endl;
        int milliseconds = time_pos * 1000.0;
        ui->timer->setText(time_ms_to_string(milliseconds).c_str());
    });

    connect(ui->play, &QPushButton::clicked, this, &MainWindow::play);
    connect(ui->pause, &QPushButton::clicked, this, &MainWindow::pause);

    connect(ui->actionOpen_video, &QAction::triggered, this, &MainWindow::open_video);
    connect(ui->actionDetach_video, &QAction::triggered, this, &MainWindow::toggle_detach_video);

    connect(ui->actionExit, &QAction::triggered, this, &MainWindow::close);

    test_open_subtitles();
}

MainWindow::~MainWindow()
{
    // TODO: make further tests, maybe this is unnecessary
    ui->videoPlayer->terminate();
    delete ui;
    std::cout << "Finished here" << std::endl;
}

void MainWindow::open_subtitles()
{
    std::string filename{QFileDialog::getOpenFileName(this).toStdString()};
    std::vector<SrtSubtitle> subs{parse_srt_file(filename.c_str())};
    SubtitleList list;
    for(auto const &sub: subs)
    {
        list.create_subtitle({sub.start_time, sub.end_time}, std::move(sub.dialog));
    }

    m_model.load_subtitles(std::move(list));
}

void MainWindow::test_open_subtitles()
{    
    std::string filename = "/home/francesco/Downloads/Silicon Valley - 05x05 - Facial Recognition.AMZN-web-ntb.English.C.orig.Addic7ed.com.srt";
    std::vector<SrtSubtitle> subs{parse_srt_file(filename.c_str())};
    SubtitleList list;
    for(auto const &sub: subs)
    {
        list.create_subtitle({sub.start_time, sub.end_time}, std::move(sub.dialog));
    }

    m_model.load_subtitles(std::move(list));
}

void MainWindow::open_video()
{
    std::string filename{QFileDialog::getOpenFileName(this).toStdString()};
    m_renderer->load_media_file(filename.c_str());
}

void MainWindow::toggle_detach_video()
{
    if(ui->actionDetach_video->isChecked())
    {
        ui->videoPlayer->detach();
    }
    else
    {
        ui->videoPlayer->reattach(ui->multimedialPanel->layout());
    }
}

void MainWindow::play()
{
    m_renderer->play();
}

void MainWindow::pause()
{
    m_renderer->pause();
}
