#include "media_renderer.h"

#include <iostream>

#include <mpv/client.h>

static void wakeup(void *mr)
{
    MediaRenderer *media_renderer = reinterpret_cast<MediaRenderer*>(mr);
    emit media_renderer->on_mpv_events();
}

MediaRenderer::MediaRenderer(QObject *parent):
    QObject(parent),
    m_mpv_client(mpv_create())
{
    std::cout << "I haz media rendeer?: " << (m_mpv_client == nullptr) << std::endl;
    if(!m_mpv_client)
    {
        throw std::runtime_error("Couldn't create mpv client");
    }

    int res = mpv_initialize(m_mpv_client);
    if(res < 0)
    {
        throw std::runtime_error(mpv_error_string(res));
    }

    mpv_set_property_string(m_mpv_client, "pause", "yes");

    mpv_set_option_string(m_mpv_client, "terminal", "yes");
    mpv_set_option_string(m_mpv_client, "msg-level", "all=v");

    mpv_observe_property(m_mpv_client, 0, "time-pos", MPV_FORMAT_DOUBLE);
    mpv_observe_property(m_mpv_client, 0, "video-aspect", MPV_FORMAT_DOUBLE);


    mpv_set_wakeup_callback(m_mpv_client, wakeup, this);

    connect(this, &MediaRenderer::on_mpv_events, this, &MediaRenderer::process_mpv_events, Qt::QueuedConnection);
}

void MediaRenderer::process_mpv_events()
{
    while(true)
    {
        mpv_event *event = mpv_wait_event(m_mpv_client, 0);
        if(event->event_id == MPV_EVENT_NONE)
        {
            break;
        }
        handle_mpv_event(event);
    }
}

void MediaRenderer::handle_mpv_event(mpv_event *event)
{
    switch(event->event_id)
    {
    case MPV_EVENT_PROPERTY_CHANGE:
        {
            mpv_event_property *prop = reinterpret_cast<mpv_event_property*>(event->data);
            std::cout << "A change" << std::endl;
            std::cout << "What changed: " << prop->name << std::endl;
            int res = strcmp(prop->name, "time-pos");
            std::cout << "La res: " << res << std::endl;
            if(strcmp(prop->name, "time-pos"))
            {
                m_time_pos = *reinterpret_cast<double*>(prop->data);
                std::cout << "La timepos: " << m_time_pos << std::endl;
                emit time_pos_changed(m_time_pos);
            }
            else if(strcmp(prop->name, "video-aspect"))
            {
                m_video_aspect = *reinterpret_cast<double*>(prop->data);
            }
        }
        break;

    default:
        break;
    }
}

void MediaRenderer::load_media_file(const char *filename)
{
    const char *args[] = { "loadfile", filename, nullptr };
    mpv_command_async(m_mpv_client, 0, args);
}

void MediaRenderer::play()
{
    int pause = 0;
    mpv_set_property_async(m_mpv_client, 0, "pause", MPV_FORMAT_FLAG, &pause);
}

void MediaRenderer::pause()
{
    int pause = 1;
    mpv_set_property_async(m_mpv_client, 0, "pause", MPV_FORMAT_FLAG, &pause);
}

void MediaRenderer::toggle_play_pause()
{
    const char *args[] = { "cycle", "pause", nullptr };
    mpv_command_async(m_mpv_client, 0, args);
}

void MediaRenderer::stop()
{
    const char *args[] = { "stop", nullptr };
    mpv_command_async(m_mpv_client, 0, args);
}
