#ifndef media_renderer_h_INCLUDED
#define media_renderer_h_INCLUDED

#include <QObject>
#include <QTimer>
#include <stdexcept>

struct mpv_handle;
struct mpv_event;

class MediaRenderer: public QObject
{
    Q_OBJECT
public:
    MediaRenderer(QObject *parent = nullptr);

    mpv_handle *mpv_client()
    {
        return m_mpv_client;
    }

    double video_aspect() const
    {
        return m_video_aspect;
    }

    double time_pos() const
    {
        return m_time_pos;
    }

    void load_media_file(const char *filename);

    void play();
    void pause();
    void toggle_play_pause();
    void stop();

signals:
    void on_mpv_events();
    void time_pos_changed(double time_pos);

private slots:
    void process_mpv_events();

private:
    void handle_mpv_event(mpv_event *event);

private:
    mpv_handle *m_mpv_client;

    double m_time_pos = 0;
    double m_old_time_pos = 0;

    double m_video_aspect = -1;
};

#endif // media_renderer_h_INCLUDED

