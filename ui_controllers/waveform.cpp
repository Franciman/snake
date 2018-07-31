#include "waveform.h"

#include <QPainter>

#include <cmath>
#include <cstring>

#include <iostream>

std::string time_ms_to_short_string(int time_ms, int precision_ms, int precision_log_ms)
{
    int ms = time_ms % 1000;
    time_ms = (time_ms - ms) / 1000;
    int seconds = time_ms % 60;
    time_ms = (time_ms - seconds) / 60;
    int minutes = time_ms % 60;
    int hours = (time_ms - minutes) / 60;

    ms /= precision_ms;

    char buff[1024];

    if(hours > 0)
    {
        if(ms > 0)
        {
            std::snprintf(buff, sizeof(buff), "%d:%02d:%02d.%d", hours, minutes, seconds, ms);
        }
        else
        {
            std::snprintf(buff, sizeof(buff), "%d:%02d:%02d", hours, minutes, seconds);
        }
    }
    else if(minutes > 0)
    {
        if(ms > 0)
        {
            std::snprintf(buff, sizeof(buff), "%d:%02d.%d", minutes, seconds, ms);
        }
        else
        {
            std::snprintf(buff, sizeof(buff), "%d:%02d", minutes, seconds);
        }
    }
    else
    {
        std::snprintf(buff, sizeof(buff), "%d.%d", seconds, ms);
    }

    return {buff};
}

void WaveformView::load_subtitles()
{
    redraw(UpdateCategory::All);
}

void WaveformView::change_selection()
{
    if(m_state->selection())
    {
        m_selection = {m_state->selection()->start_time(), m_state->selection()->end_time()};
    }
    else
    {
        m_selection.reset();
    }
    redraw(UpdateCategory::Selection);
}

void WaveformView::insert_subtitle(Subtitle s)
{
    redraw(UpdateCategory::Subtitles);
}

void WaveformView::remove_subtitle(size_t index)
{
    redraw(UpdateCategory::Subtitles);
}

void WaveformView::update_subtitle(Subtitle s)
{
    redraw(UpdateCategory::Subtitles);
}

void WaveformView::paintEvent(QPaintEvent *ev)
{
    paint_ruler();
    paint_subtitles();
    paint_selection();
}

void WaveformView::paint_ruler()
{
    if(m_ruler_height <= 0) return;

    QPainter painter(this);

    QRect ruler_bounding_rect{0, height() - m_ruler_height - 1, width(), m_ruler_height};

    painter.fillRect(ruler_bounding_rect, Qt::gray);

    // Paint frame around ruler
    painter.setPen(Qt::black);
    painter.drawLine(ruler_bounding_rect.topLeft(), ruler_bounding_rect.topRight());
    painter.drawLine(ruler_bounding_rect.bottomLeft(), ruler_bounding_rect.bottomRight());

    int start_time = m_position;
    int end_time   = (m_position + m_pagesize);

    QFont font{"Time New Roman", 8};
    QFontMetrics font_metrics{font};

    painter.setFont(font);

    QSize text_max_size = font_metrics.size(0, "0:00:00.0");
    text_max_size.setWidth(text_max_size.width() * 2);

    // Calculations to show round time
    int max_pos_step = std::round(width() / text_max_size.width());
    int step_ms = std::round(m_pagesize / max_pos_step);
    if(step_ms == 0) step_ms = 1;

    // Find the power of 10 that best approximates from below step_ms
    double exponent = std::trunc(std::log10(step_ms));
    double step_approx = std::trunc(std::pow(10, exponent));

    step_ms = std::trunc(step_ms / step_approx) * step_approx;

    int p = (start_time / step_ms) * step_ms;

    while(p < end_time)
    {
        int x = scaled_x(p);

        // Draw main division
        painter.drawLine(x, ruler_bounding_rect.top() + 1, x, ruler_bounding_rect.top() + 5);


        std::string time_string {time_ms_to_short_string(p, step_approx, exponent)};
        int x1 = x - font_metrics.size(0, time_string.c_str()).width() / 2;

        int y1 = ruler_bounding_rect.top() + font_metrics.size(0, time_string.c_str()).height();

        // Draw text shadow
        painter.setPen(Qt::black);
        painter.drawText(x1 + 2, y1 + 4 + 2, time_string.c_str());

        // Draw text
        painter.setPen(Qt::white);
        painter.drawText(x1, y1 + 4, time_string.c_str());

        // Draw subdivision
        p += step_ms;
        int x2 = (scaled_x(p) + x) / 2;

        painter.drawLine(x2, ruler_bounding_rect.top() + 1, x2, ruler_bounding_rect.top() + 3);
    }
}

void WaveformView::paint_subtitles()
{
    if(!m_state) return;

    QPainter painter(this);

    QColor colors[] = {
        Qt::red,
        Qt::blue,
    };

    QRect bounding_rect{rect()};
    bounding_rect.setHeight(bounding_rect.height() - m_ruler_height);

    int height_div_10 = bounding_rect.height() / 10;

    int y1 = bounding_rect.top() + height_div_10;
    int y2 = bounding_rect.bottom() - height_div_10;

    int start_time = m_position;
    int end_time = m_position + m_pagesize;

    constexpr int text_margins = 5;
    constexpr int min_space = 25;

    m_state->subtitles().for_each_overlapping({start_time, end_time}, [&](Subtitle s)
    {
        size_t index = m_state->subtitles().index(s);
        QColor color = colors[index % 2];

        painter.setPen(color);

        int sub_start_pixel = scaled_x(s.start_time());
        int sub_end_pixel   = scaled_x(s.end_time());

        if(s.start_time() < m_position)
        {
            sub_start_pixel = 0;
        }
        else
        {
            painter.drawLine(sub_start_pixel, y1, sub_start_pixel, y2);
        }

        if(s.end_time() > m_position + m_pagesize)
        {
            sub_end_pixel = width() - 1;
        }
        else
        {
            painter.drawLine(sub_end_pixel, y1, sub_end_pixel, y2);
        }

        painter.drawLine(sub_start_pixel, y1, sub_end_pixel, y1);
        painter.drawLine(sub_start_pixel, y2, sub_end_pixel, y2);

        QRect text_rect{QPoint(sub_start_pixel, y1), QPoint(sub_end_pixel, y2)};
        text_rect.adjust(text_margins, 0, -text_margins, 0);

        if(text_rect.width() > min_space)
        {
            painter.drawText(text_rect, Qt::TextWordWrap, s.dialog().c_str());
        }
    });
}

void WaveformView::paint_selection()
{
    if(!m_selection) return;
    QPainter painter(this);

    QRect bounding_rect{rect()};

    bounding_rect.setHeight(bounding_rect.height() - m_ruler_height);
    int left_pixel = scaled_x(std::max(m_position, m_selection->start_time()));
    int right_pixel = scaled_x(std::min(m_selection->end_time(), m_position + m_pagesize));

    bounding_rect.setLeft(left_pixel);
    bounding_rect.setRight(right_pixel);

    painter.fillRect(bounding_rect, Qt::black);
}
