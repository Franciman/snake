#include "main_window.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QTableView>
#include <subtitle_formats/srt_parser.h>

MainWindow::MainWindow(QWidget *parent):
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_state(),
    m_model(m_state)
{
    ui->setupUi(this);

    connect(&m_state, &ApplicationState::subtitles_loaded, &m_model, &SubtitleListViewModel::reload_data);
    connect(&m_state, &ApplicationState::inserted_subtitle, &m_model, &SubtitleListViewModel::insert_subtitle);
    connect(&m_state, &ApplicationState::removed_subtitle, &m_model, &SubtitleListViewModel::erase_subtitle);
    connect(&m_state, &ApplicationState::subtitle_changed, &m_model, &SubtitleListViewModel::update_subtitle);
    connect(&m_state, &ApplicationState::subtitles_reordered, &m_model, &SubtitleListViewModel::reorder_subtitles);
    ui->subtitleList->setModel(&m_model);

    QItemSelectionModel *selectionModel = ui->subtitleList->selectionModel();
    selectionModel->clearSelection();

    connect(ui->subtitleList, &QTableView::doubleClicked, this, [this](const QModelIndex &index)
    {
        ui->subtitleList->selectRow(index.row());
    });

    ui->waveform->set_application_state(m_state);

    connect(selectionModel, &QItemSelectionModel::selectionChanged, this, &MainWindow::select_subtitle);
    connect(&m_state, &ApplicationState::selection_changed, this, &MainWindow::update_selection);

    connect(ui->subtitleDialogEdit, &QPlainTextEdit::textChanged, this, &MainWindow::update_subtitle_text);

    connect(ui->actionOpen_subtitles, &QAction::triggered, this, &MainWindow::open_subtitles);
    connect(ui->actionRemove_selected_subtitle, &QAction::triggered, this, &MainWindow::remove_selected_subtitle);

    connect(ui->actionExit, &QAction::triggered, this, &QMainWindow::close);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::open_subtitles()
{
    // std::string filename{QFileDialog::getOpenFileName(this).toStdString()};
    std::string filename = "/home/francesco/Download/Homeland.s07e08.italiansubs.srt";
    std::vector<SrtSubtitle> subs{parse_srt_file(filename.c_str())};
    SubtitleList list;
    for(auto const &sub: subs)
    {
        list.create_subtitle({sub.start_time, sub.end_time}, std::move(sub.dialog));
    }

    m_state.load_subtitles(std::move(list));
}

void MainWindow::select_subtitle(const QItemSelection &selected, const QItemSelection &deselected)
{
    if(selected.empty())
    {
        m_state.unset_selection();
    }
    else
    {
        m_state.set_selection(m_state.subtitles()[selected[0].top()]);
    }
}

void MainWindow::update_selection()
{
    if(m_state.selection())
    {
        ui->subtitleDialogEdit->setPlainText(m_state.selection()->dialog().c_str());
        ui->subtitleDialogEdit->setEnabled(true);
        ui->subtitleList->selectRow(m_state.selection()->index());
    }
    else
    {
        ui->subtitleDialogEdit->setPlainText("");
        ui->subtitleDialogEdit->setEnabled(false);
        ui->subtitleList->selectionModel()->clearSelection();
    }
}

void MainWindow::update_subtitle_text()
{
    if(m_state.selection())
    {
        std::string dialog{ui->subtitleDialogEdit->toPlainText().toStdString()};
        m_state.selection()->set_dialog(std::move(dialog));
    }
}

void MainWindow::remove_selected_subtitle()
{
    if(m_state.selection())
    {
        m_state.remove_subtitle(m_state.selection()->index());
    }
}
