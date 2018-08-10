#include "video_player.h"

#include <media_renderer.h>

#include <mpv/client.h>
#include <mpv/opengl_cb.h>

#include <QOpenGLContext>
#include <QLayout>

static void *get_proc_address(void *, const char *name)
{
    QOpenGLContext *gl_ctx = QOpenGLContext::currentContext();
    if(!gl_ctx)
    {
        return nullptr;
    }
    else
    {
        return reinterpret_cast<void*>(gl_ctx->getProcAddress(QByteArray(name)));
    }
}

static void issue_redraw(void *vp)
{
    VideoPlayer *self = reinterpret_cast<VideoPlayer*>(vp);
    emit self->redraw();
}

VideoPlayer::VideoPlayer(QWidget *parent):
    QOpenGLWidget(parent),
    m_renderer(nullptr),
    m_mpv_render_context(nullptr)
{
    connect(this, &VideoPlayer::redraw, this, &VideoPlayer::do_redraw, Qt::QueuedConnection);
    connect(this, &VideoPlayer::frameSwapped, this, &VideoPlayer::buffersSwapped);
}

VideoPlayer::~VideoPlayer()
{
    makeCurrent();
    deinitialize_render_context();
    doneCurrent();
}

void VideoPlayer::set_media_renderer(MediaRenderer &renderer)
{
    if(m_renderer) return;

    m_renderer = &renderer;

    m_mpv_render_context = (mpv_opengl_cb_context*)mpv_get_sub_api(m_renderer->mpv_client(), MPV_SUB_API_OPENGL_CB);

    if(!m_mpv_render_context)
    {
        throw std::runtime_error("OpenGL support not compiled in");
    }
}

void VideoPlayer::initialize_render_context()
{
    mpv_opengl_cb_init_gl(m_mpv_render_context, nullptr, get_proc_address, nullptr);
    mpv_opengl_cb_set_update_callback(m_mpv_render_context, issue_redraw, this);
}

void VideoPlayer::deinitialize_render_context()
{
    if(m_mpv_render_context)
    {
        mpv_opengl_cb_set_update_callback(m_mpv_render_context, nullptr, nullptr);
    }
    mpv_opengl_cb_uninit_gl(m_mpv_render_context);

    QOpenGLContext::currentContext()->disconnect(m_cleaner_connection);
}

void VideoPlayer::clean_context()
{
    makeCurrent();
    deinitialize_render_context();
    doneCurrent();
    m_cleaned_up = true;
}

void VideoPlayer::initializeGL()
{
    if(!m_renderer)
    {
        m_cleaned_up = false;
        return;
    }

    m_cleaner_connection = connect(QOpenGLContext::currentContext(), &QOpenGLContext::aboutToBeDestroyed, this, &VideoPlayer::clean_context);

    initialize_render_context();

    if(m_cleaned_up)
    {
        const char *args[] = { "cycle", "video", nullptr };
        mpv_command_async(m_renderer->mpv_client(), 0, args);
    }
    m_cleaned_up = false;
}

void VideoPlayer::paintGL()
{
    if(m_renderer)
    {
        mpv_opengl_cb_draw(m_mpv_render_context, (int)defaultFramebufferObject(), width(), -height());
    }
}

void VideoPlayer::buffersSwapped()
{
    if(m_renderer)
    {
        mpv_opengl_cb_report_flip(m_mpv_render_context, 0);
    }
}

void VideoPlayer::reattach(QLayout *l)
{
    l->addWidget(this);
    show();
}
