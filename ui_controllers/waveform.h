#ifndef waveform_h_INCLUDED
#define waveform_h_INCLUDED

#include <QWidget>

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
        m_ruler_height(20)
    { }

    void set_application_state(ApplicationState &state)
    {
        m_state = &state;
        connect(m_state, &ApplicationState::subtitles_loaded, this, &WaveformView::load_subtitles);
        connect(m_state, &ApplicationState::selection_changed, this, &WaveformView::change_selection);
        connect(m_state, &ApplicationState::inserted_subtitle, this, &WaveformView::insert_subtitle);
        connect(m_state, &ApplicationState::removed_subtitle, this, &WaveformView::remove_subtitle);
        connect(m_state, &ApplicationState::subtitle_changed, this, &WaveformView::update_subtitle);
    }

protected:
    virtual void paintEvent(QPaintEvent *ev) override;
    virtual void mouseDoubleClickEvent(QMouseEvent *ev) override;

    void paint_ruler();
    void paint_subtitles();
    void paint_selection();

public slots:
    void load_subtitles();
    void change_selection();
    void insert_subtitle(Subtitle s);
    void remove_subtitle(size_t index);
    void update_subtitle(Subtitle s);

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
};

#endif // waveform_h_INCLUDED
