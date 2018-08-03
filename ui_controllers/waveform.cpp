#include "waveform.h"

#include <cassert>

#include <QPainter>
#include <QMenu>

#include <cmath>
#include <cstring>

#include <iostream>
#include <QMouseEvent>

#include <utils/time_utils.h>


void WaveformView::setModel(SubtitleManager *model)
{
    if(m_model)
    {
        m_model->disconnect(m_data_changed_connection);
    }
    m_model = model;
    if(m_model)
    {
        m_data_changed_connection = connect(m_model,
                                            &SubtitleManager::dataChanged,
                                            this,
                                            &WaveformView::update_after_data_changed);
    }
}

void WaveformView::setSelectionModel(SubtitleSelectionModel *model)
{
    if(m_selection_model)
    {
        m_selection_model->disconnect(m_current_changed_connection);
    }
    m_selection_model = model;
    if(m_selection_model)
    {
        m_current_changed_connection = connect(m_selection_model,
                                               &SubtitleSelectionModel::currentChanged,
                                               this,
                                               &WaveformView::update_after_current_changed);
    }
}

void WaveformView::paintEvent(QPaintEvent *ev)
{
    paint_wave();
    paint_ruler();

    if(is_active())
    {
        paint_subtitles();
        paint_selection();
    }
}

void WaveformView::paint_wave()
{
    QPainter painter(this);
    QRect waveform_rect{m_drawing_context.get_waveform_rect(size())};

    painter.fillRect(waveform_rect, Qt::black);
}

void WaveformView::paint_ruler()
{
    if(!m_drawing_context.ruler_is_visible()) return;

    QPainter painter(this);

    QRect ruler_rect{m_drawing_context.get_ruler_rect(size())};

    painter.fillRect(ruler_rect, Qt::gray);

    // Paint frame around ruler
    painter.setPen(Qt::black);
    painter.drawLine(ruler_rect.topLeft(), ruler_rect.topRight());
    painter.drawLine(ruler_rect.bottomLeft(), ruler_rect.bottomRight());

    int start_time = m_drawing_context.start_time();
    int end_time   = m_drawing_context.end_time();

    QFont font{"Time New Roman", 8};
    QFontMetrics font_metrics{font};

    painter.setFont(font);

    QSize text_max_size = font_metrics.size(0, "0:00:00.0");
    text_max_size.setWidth(text_max_size.width() * 2);

    // Calculations to show round time
    int max_pos_step = std::round(width() / text_max_size.width());
    int step_ms = std::round(m_drawing_context.page_size() / max_pos_step);
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
        painter.drawLine(x, ruler_rect.top() + 1, x, ruler_rect.top() + 5);


        std::string time_string {time_ms_to_short_string(p, step_approx, exponent)};
        int x1 = x - font_metrics.size(0, time_string.c_str()).width() / 2;

        int y1 = ruler_rect.top() + font_metrics.size(0, time_string.c_str()).height();

        // Draw text shadow
        painter.setPen(Qt::black);
        painter.drawText(x1 + 2, y1 + 4 + 2, time_string.c_str());

        // Draw text
        painter.setPen(Qt::white);
        painter.drawText(x1, y1 + 4, time_string.c_str());

        // Draw subdivision
        p += step_ms;
        int x2 = (scaled_x(p) + x) / 2;

        painter.drawLine(x2, ruler_rect.top() + 1, x2, ruler_rect.top() + 3);
    }
}

void WaveformView::paint_subtitles()
{
    QPainter painter(this);

    QColor colors[] = {
        Qt::red,
        Qt::blue,
    };

    QRect waveform_rect{m_drawing_context.get_waveform_rect(size())};

    int height_div_10 = waveform_rect.height() / 10;

    int y1 = waveform_rect.top() + height_div_10;
    int y2 = waveform_rect.bottom() - height_div_10;

    int start_time = m_drawing_context.start_time();
    int end_time = m_drawing_context.end_time();

    constexpr int text_margins = 5;
    constexpr int min_space = 25;

    m_model->subtitles().for_each_overlapping({start_time, end_time}, [&](Subtitle s)
    {
        size_t index = m_model->subtitles().index(s);
        QColor color = colors[index % 2];

        painter.setPen(color);

        TimeInterval interval = s.time_interval();

        int sub_start_pixel = scaled_x(interval.start_time());
        int sub_end_pixel   = scaled_x(interval.end_time());


        if(!m_drawing_context.is_visible(interval.start_time()))
        {
            sub_start_pixel = 0;
        }
        else
        {
            QPen oldPen = painter.pen();
            QPen newPen{oldPen};
            /*if(m_focused_subtitle == s && m_focus_position == IntervalBoundary::Start)
            {
                newPen.setStyle(Qt::SolidLine);
            }
            else
            {
                newPen.setStyle(Qt::DotLine);
            }*/

            painter.setPen(newPen);
            painter.drawLine(sub_start_pixel, y1, sub_start_pixel, y2);
            painter.setPen(oldPen);
        }

        if(!m_drawing_context.is_visible(interval.end_time()))
        {
            sub_end_pixel = width() - 1;
        }
        else
        {
            QPen oldPen = painter.pen();
            QPen newPen{oldPen};

            /*if(m_focused_subtitle == s && m_focus_position == IntervalBoundary::End)
            {
                newPen.setStyle(Qt::SolidLine);
            }
            else
            {
                newPen.setStyle(Qt::DotLine);
            }*/

            painter.setPen(newPen);
            painter.drawLine(sub_end_pixel, y1, sub_end_pixel, y2);
            painter.setPen(oldPen);
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
    if(m_selection_ctx)
    {
        QPainter painter(this);

        QRect selection_rect{m_drawing_context.get_waveform_rect(size())};

        TimeInterval selection = m_selection_ctx->m_selection;

        int left_pixel = scaled_x(selection.start_time());
        int right_pixel = scaled_x(selection.end_time());

        selection_rect.setLeft(left_pixel);
        selection_rect.setRight(right_pixel);

        painter.fillRect(selection_rect, QColor(255, 255, 255, 50));
    }
}

void WaveformView::mouseDoubleClickEvent(QMouseEvent *ev)
{
    if(!is_active()) return;
    if(ev->button() != Qt::LeftButton) return;

    int pos_ms = pixel_to_time_ms(ev->pos().x());

    std::optional<Subtitle> sub = m_model->subtitles().first_overlapping(pos_ms);
    if(sub)
    {
        m_selection_model->set_current_subtitle(*sub);
    }
    else
    {
        m_selection_model->clearCurrentIndex();
    }
}

void WaveformView::mousePressEvent(QMouseEvent *ev)
{
    m_mouse_down = true;
    if(!is_active()) return;
    if(ev->button() != Qt::LeftButton) return;

    if(m_focus_ctx)
    {
        if(m_focus_ctx->m_subtitle)
        {
            m_selection_model->set_current_subtitle(*m_focus_ctx->m_subtitle);
        }
        else
        {
            // Do nothing
        }
    }
    else
    {
        // Create selection
        int pos_ms = pixel_to_time_ms(ev->pos().x());
        TimeInterval selection = {pos_ms, pos_ms};
        m_selection_ctx = {selection, pos_ms};

        m_selection_model->clearCurrentIndex();

        redraw(UpdateCategory::Selection);
    }
}


void WaveformView::resize_focused_object(QMouseEvent *ev)
{
    int new_pos_ms = pixel_to_time_ms(ev->pos().x());
    if(m_focus_ctx)
    {
        std::cout << "I haz focus" << std::endl;
        assert((bool)m_selection_ctx);
        m_selection_ctx->m_selection.update_boundary(new_pos_ms, m_focus_ctx->m_position);

        // If the thing selected is a subtitle, update it
        if(m_focus_ctx->m_subtitle)
        {
            m_model->update_subtitle_timing(*m_focus_ctx->m_subtitle, m_selection_ctx->m_selection);
        }
        else
        {
            redraw(UpdateCategory::Selection);
        }
    }
    else if(m_selection_ctx)
    {
        std::cout << "I haz selection" << std::endl;
        if(new_pos_ms > m_selection_ctx->m_selection_origin)
        {
            m_selection_ctx->m_selection.set_end_time(new_pos_ms);
        }
        else
        {
            m_selection_ctx->m_selection.set_start_time(new_pos_ms);
        }
        redraw(UpdateCategory::Selection);
    }
    else
    {
        assert(false && "Strange things happen, this should be unreachable");
    }
}

void WaveformView::focus_object_near_cursor(QMouseEvent *ev)
{
    int pos_ms = pixel_to_time_ms(ev->pos().x());

    int focus_interval_width = std::max(1, m_drawing_context.duration_to_pixel_interval(2, size()));
    int tolerance_ms = std::max(1, m_drawing_context.duration_to_pixel_interval(4,size()));

    // A list of nearby subtitles and the side focused
    std::vector<std::pair<Subtitle, IntervalBoundary>> nearby_subtitles;

    auto focus_detector = [&nearby_subtitles, tolerance_ms, pos_ms](Subtitle s)
    {
        if(std::abs(s.start_time() - pos_ms) < tolerance_ms)
        {
            nearby_subtitles.push_back(std::make_pair(s, IntervalBoundary::Start));
        }
        else if(std::abs(s.end_time() - pos_ms) < tolerance_ms)
        {
            nearby_subtitles.push_back(std::make_pair(s, IntervalBoundary::End));
        }
    };

    TimeInterval focus_interval{pos_ms - focus_interval_width, pos_ms + focus_interval_width};

    m_model->subtitles().for_each_overlapping({pos_ms - tolerance_ms, pos_ms + tolerance_ms},
                                              focus_detector);

    auto nearest_subtitle = std::min_element(nearby_subtitles.begin(), nearby_subtitles.end(),
                                             [pos_ms](const auto &s1, const auto &s2)
                                             {
                                                 int time1 = s1.first.get_boundary(s1.second);
                                                 int time2 = s2.first.get_boundary(s2.second);

                                                 return std::abs(time1 - pos_ms) < std::abs(time2 - pos_ms);
                                             });

    if(nearest_subtitle != nearby_subtitles.end())
    {
        std::cout << "Found sub to focus" << std::endl;
        // We found the focused subtitle
        set_focus(nearest_subtitle->second, nearest_subtitle->first);
    }
    else
    {
        // Check if we're focusing selection
        if(m_selection_ctx)
        {
            const TimeInterval &interval = m_selection_ctx->m_selection;

            std::cout << "Informations: " << std::endl;
            std::cout << "Tolerance: " << tolerance_ms << std::endl;
            std::cout << "Start eta: " << std::abs(interval.start_time() - pos_ms) << std::endl;
            std::cout << "End eta: " << std::abs(interval.end_time() - pos_ms) << std::endl;

            if(std::abs(interval.start_time() - pos_ms) < tolerance_ms)
            {
                std::cout << "Focus on selection dawg" << std::endl;
                set_focus(IntervalBoundary::Start);
            }
            else if(std::abs(interval.end_time() - pos_ms) < tolerance_ms)
            {
                std::cout << "Focus on selection dawg" << std::endl;
                set_focus(IntervalBoundary::End);
            }
            else
            {
                std::cout << "No way dawg" << std::endl;
                remove_focus();
            }
        }
        else
        {
            std::cout << "Uncle " << std::endl;
            remove_focus();
        }
    }
}


void WaveformView::mouseMoveEvent(QMouseEvent *ev)
{
    if(!is_active()) return;

    if(m_mouse_down)
    {
        resize_focused_object(ev);
    }
    else
    {
        focus_object_near_cursor(ev);
    }
}

void WaveformView::mouseReleaseEvent(QMouseEvent *ev)
{
    if(m_mouse_down)
    {
        m_mouse_down = false;
    }
}

void WaveformView::contextMenuEvent(QContextMenuEvent *ev)
{
    if(!is_active()) return;

    QMenu menu(this);

    if(m_selection_model->currentIndex().isValid())
    {
        menu.addAction(m_remove_subtitle_action);
        menu.exec(ev->globalPos());
    }
    if(m_selection_ctx)
    {
        menu.addAction(m_add_subtitle_action);
        menu.exec(ev->globalPos());
    }
}

void WaveformView::create_subtitle_from_selection()
{
/*    Subtitle new_sub = m_model->insert_subtitle(m_selection_ctx->m_selection, "");
    m_selection_ctx->set_current_subtitle(new_sub);*/
}

void WaveformView::remove_selected_subtitle()
{
    m_model->remove_subtitle(m_selection_model->currentIndex());
}

void WaveformView::update_after_data_changed(const QModelIndex &topLeft,
                                             const QModelIndex &bottomRight,
                                             const QVector<int> &roles)
{
    redraw(UpdateCategory::Subtitles);
}

void WaveformView::update_after_current_changed(const QModelIndex &current, const QModelIndex &previos)
{
    if(current.isValid())
    {
        Subtitle selected_subtitle = m_model->subtitle(current);
        m_selection_ctx = { selected_subtitle.time_interval(), selected_subtitle.start_time() };
    }
    else
    {
        m_selection_ctx.reset();
    }
    redraw(UpdateCategory::Selection);
}

