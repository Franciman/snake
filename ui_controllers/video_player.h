#ifndef video_player_h_INCLUDED
#define video_player_h_INCLUDED

#include <QOpenGLWidget>

struct mpv_opengl_cb_context;

class MediaRenderer;

class VideoPlayer: public QOpenGLWidget
{
    Q_OBJECT

public:
    VideoPlayer(QWidget *parent = nullptr);
    ~VideoPlayer();

    // This function must be called only once
    // Successive calls will do nothing
    void set_media_renderer(MediaRenderer &renderer);

    void detach()
    {
        setParent(nullptr);
        show();
    }

    void reattach(QLayout *l);

    void terminate()
    {
        makeCurrent();
        deinitialize_render_context();
        doneCurrent();
    }

protected:
    void initializeGL() override;
    void paintGL() override;

signals:
    void redraw();

private slots:
    void do_redraw()
    {
        update();
    }

    void buffersSwapped();

    void clean_context();

private:
    void initialize_render_context();
    void deinitialize_render_context();

    MediaRenderer *m_renderer;
    
    mpv_opengl_cb_context *m_mpv_render_context;

    bool m_cleaned_up = false;
    QMetaObject::Connection m_cleaner_connection;
};

#endif // video_player_h_INCLUDED

