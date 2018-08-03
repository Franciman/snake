#include "main_window.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QTableView>
#include <subtitle_formats/srt_parser.h>

#include <iostream>

MainWindow::MainWindow(QWidget *parent):
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_model(),
    m_selection_model(&m_model)
{
    ui->setupUi(this);

    ui->subtitleList->setModel(&m_model);
    ui->subtitleList->set_selection_model(&m_selection_model);

    
    ui->subtitleDialogEdit->setModel(&m_model);
    ui->subtitleDialogEdit->setSelectionModel(&m_selection_model);

    ui->waveform->setModel(&m_model);
    ui->waveform->setSelectionModel(&m_selection_model);

    connect(ui->actionOpen_subtitles, &QAction::triggered, this, &MainWindow::open_subtitles);

    connect(ui->actionRemove_selected_subtitle, &QAction::triggered, this, [this]()
    {
        if(m_selection_model.has_selection())
        {
            m_model.remove_subtitle(m_selection_model.selection());
        }
    });

    connect(ui->actionExit, &QAction::triggered, this, &MainWindow::close);

    test_open_subtitles();
}

MainWindow::~MainWindow()
{
    delete ui;
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
