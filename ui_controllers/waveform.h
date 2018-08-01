#ifndef waveform_h_INCLUDED
#define waveform_h_INCLUDED

#include <QWidget>
#include <QAction>

#include <application_state.h>
#include <chrono>
#include <cmath>
#include <optional>

using namespace std::chrono;

class WaveformView: public QWidget
{
    Q_OBJECT

    ApplicationState *m_state;

public:
    WaveformView(QWidget *parent = nullptr):
        QWidget(parent),
        m_state(nullptr),
        m_pagesize(15000),
        m_position(0),
        m_ruler_height(20),
        m_selection(),
        m_selection_origin(-1),
        m_focused_subtitle(),
        m_focus_position(IntervalBoundary::Start),
        m_mouse_down(false),
        m_add_subtitle_action(new QAction("Add subtitle", this)),
        m_remove_subtitle_action(new QAction("Remove subtitle", this))
    {
        setMouseTracking(true);

        connect(m_add_subtitle_action, &QAction::triggered, this, &WaveformView::create_subtitle_from_selection);
        connect(m_remove_subtitle_action, &QAction::triggered, this, &WaveformView::remove_selected_subtitle);
    }

    void set_application_state(ApplicationState &state)
    {
        m_state = &state;
        connect(m_state, &ApplicationState::subtitles_loaded, this, &WaveformView::load_subtitles);
        connect(m_state, &ApplicationState::selection_changed, this, &WaveformView::change_selection);
        connect(m_state, &ApplicationState::inserted_subtitle, this, &WaveformView::insert_subtitle);
        connect(m_state, &ApplicationState::removed_subtitle, this, &WaveformView::remove_subtitle);
        connect(m_state, &ApplicationState::subtitle_changed, this, &WaveformView::update_subtitle);
        connect(m_state, &ApplicationState::subtitles_reordered, this, &WaveformView::reorder_subtitles);
    }

protected:
    virtual void paintEvent(QPaintEvent *ev) override;
    virtual void mouseDoubleClickEvent(QMouseEvent *ev) override;
    virtual void mousePressEvent(QMouseEvent *ev) override;
    virtual void mouseMoveEvent(QMouseEvent *ev) override;
    virtual void mouseReleaseEvent(QMouseEvent *ev) override;
    virtual void contextMenuEvent(QContextMenuEvent *ev) override;

    void paint_wave();
    void paint_ruler();
    void paint_subtitles();
    void paint_selection();

public slots:
    void load_subtitles();
    void change_selection();
    void insert_subtitle(Subtitle s);
    void remove_subtitle(size_t index);
    void update_subtitle(Subtitle s);
    void reorder_subtitles();

private slots:
    void create_subtitle_from_selection();
    void remove_selected_subtitle();

private:
    enum class UpdateCategory: unsigned char
    {
        Selection = 2,
        Subtitles = 4,
        All = 255,
    };

    void redraw(UpdateCategory uc)
    {
        update();
    }

    int scaled_x(int ms) const
    {
        double x_scale_factor = width() / (double)m_pagesize;
        return std::round((ms - m_position) * x_scale_factor);
    }

    int time_from_pos(int x) const
    {
        double x_scale_factor = width() / (double)m_pagesize;
        return std::round(x / x_scale_factor) + m_position;
    }

    int m_pagesize;
    int m_position;

    int m_ruler_height;

    std::optional<TimeInterval> m_selection;
    int m_selection_origin;

    // std::optional<TimeInterval> m_focused_range;
    std::optional<Subtitle> m_focused_subtitle;
    IntervalBoundary m_focus_position;


    bool m_mouse_down;

    QAction *m_add_subtitle_action;
    QAction *m_remove_subtitle_action;
};

#endif // waveform_h_INCLUDED

