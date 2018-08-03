#ifndef waveform_h_INCLUDED
#define waveform_h_INCLUDED

#include <QWidget>
#include <QAction>

#include <application_state.h>
#include <chrono>
#include <cmath>
#include <optional>

using namespace std::chrono;

struct WaveformSelectionContext
{
    TimeInterval m_selection;
    int m_selection_origin;
};

struct FocusContext
{
    std::optional<Subtitle> m_subtitle;
    IntervalBoundary m_position;
};


struct DrawingContext
{
    int m_position_ms;
    int m_ruler_height;

    // Zoom settings
    int m_pagesize_ms;

    // Horizontal coordinate conversion
    int scaled_x(int time_ms, const QSize &size) const
    {
        double x_scale_factor = size.width() / static_cast<double>(m_pagesize_ms);
        return std::round((time_ms - m_position_ms) * x_scale_factor);
    }

    int pixel_to_time_ms(int pixel, const QSize &size) const
    {
        double ms_per_pixel = m_pagesize_ms / static_cast<double>(size.width());

        return std::round(pixel * ms_per_pixel) + m_position_ms;
    }

    int distance_to_duration(int x, const QSize &size) const
    {
        return pixel_to_time_ms(x, size) - m_position_ms;
    }

    QRect get_waveform_rect(const QSize &size) const
    {
        return QRect{0, 0, size.width(), size.height() - m_ruler_height};
    }

    QRect get_ruler_rect(const QSize &size) const
    {
        return QRect{0, size.height() - m_ruler_height, size.width(), m_ruler_height};
    }

    bool ruler_is_visible() const
    {
        return m_ruler_height > 0;
    }

    int start_time() const
    {
        return m_position_ms;
    }

    int end_time() const
    {
        return start_time() + m_pagesize_ms;
        
    }

    int page_size() const
    {
        return m_pagesize_ms;
    }

    bool is_visible(int pos_ms) const
    {
        return start_time() <= pos_ms && pos_ms <= end_time();
    }
};

class WaveformView: public QWidget
{
    Q_OBJECT

    SubtitleManager *m_model;
    SubtitleSelectionModel *m_selection_model;

    QMetaObject::Connection m_data_changed_connection;
    QMetaObject::Connection m_current_changed_connection;

    std::optional<WaveformSelectionContext> m_selection_ctx;
    std::optional<FocusContext> m_focus_ctx;
    DrawingContext m_drawing_context;

    bool m_mouse_down;
    QAction *m_add_subtitle_action;
    QAction *m_remove_subtitle_action;

public:
    WaveformView(QWidget *parent = nullptr):
        QWidget(parent),
        m_model(nullptr),
        m_selection_model(nullptr),
        m_selection_ctx(),
        m_focus_ctx(),
        m_drawing_context({0, 20, 15000}),
        m_mouse_down(false),
        m_add_subtitle_action(new QAction("Add subtitle", this)),
        m_remove_subtitle_action(new QAction("Remove subtitle", this))
    {
        setMouseTracking(true);

        connect(m_add_subtitle_action, &QAction::triggered, this, &WaveformView::create_subtitle_from_selection);
        connect(m_remove_subtitle_action, &QAction::triggered, this, &WaveformView::remove_selected_subtitle);
    }

    void setModel(SubtitleManager *model);
    void setSelectionModel(SubtitleSelectionModel *model);

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

private slots:
    void update_after_data_changed(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles);
    void update_after_current_changed(int current, int previous);

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

    int scaled_x(int time_ms) const
    {
        return m_drawing_context.scaled_x(time_ms, size());
    }

    int pixel_to_time_ms(int pixel) const
    {
        return m_drawing_context.pixel_to_time_ms(pixel, size());
    }

    bool is_active() const
    {
        return m_model && m_selection_model;
    }

    void resize_focused_object(QMouseEvent *ev);

    void focus_object_near_cursor(QMouseEvent *ev);

    void set_focus(IntervalBoundary position, std::optional<Subtitle> sub = std::optional<Subtitle>())
    {
        m_focus_ctx = {sub, position};
        setCursor(Qt::SplitHCursor);
        redraw(UpdateCategory::Subtitles);
    }

    void remove_focus()
    {
        m_focus_ctx.reset();
        setCursor(Qt::ArrowCursor);
        redraw(UpdateCategory::Subtitles);
    }
};

#endif // waveform_h_INCLUDED

