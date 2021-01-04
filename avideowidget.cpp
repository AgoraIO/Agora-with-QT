#include "avideowidget.h"
#include "video_render_opengl.h"
#include "video_render_impl.h"
#include <QPainter>
#include <QQuickWindow>
#include <QMutexLocker>
AVideoWidget::AVideoWidget(QQuickItem *parent)
    : QQuickPaintedItem(parent)
    , m_render(new VideoRendererOpenGL(width(), height()))
    , m_frame(nullptr)
    , m_rotation(0)
    , m_mirrored(false)
{
    setRenderTarget(FramebufferObject);
    connect(this, SIGNAL(frameDelivered()), this, SLOT(renderFrame()));
    connect(this, SIGNAL(windowChanged(QQuickWindow*)), this, SLOT(handleWindowChanged(QQuickWindow*)));
}

AVideoWidget::~AVideoWidget()
{
    cleanup();
}

int AVideoWidget::setViewProperties(int zOrder, float left, float top, float right, float bottom)
{
	QMutexLocker lock(&m_mutex);
    if (m_render)
        return m_render->setStreamProperties(zOrder, left, top, right, bottom);
    return -1;
}

void AVideoWidget::handleWindowChanged(QQuickWindow *win)
{
    if (win)
    {
        connect(win, SIGNAL(sceneGraphInvalidated()), this, SLOT(cleanup()), Qt::DirectConnection);
    }
}

void AVideoWidget::cleanup()
{
    {
	QMutexLocker lock(&m_mutex);
   
    if (m_frame)
    {
        m_frame->release();
        m_frame = nullptr;
    }
    }
    m_render.reset();
	m_render = nullptr;
    emit widgetInvalidated();
}

void AVideoWidget::renderFrame()
{
    update();
}

int AVideoWidget::deliverFrame(const agora::media::IVideoFrame& videoFrame, int rotation, bool mirrored)
{
    if (videoFrame.IsZeroSize())
        return -1;
    int r;
    {
	QMutexLocker lock(&m_mutex);
    m_rotation = rotation;
    m_mirrored = mirrored;
    r = videoFrame.copyFrame(&m_frame);
    }

    //notify the render thread to redraw the incoming frame
    emit frameDelivered();

    return r;
}

void AVideoWidget::paint(QPainter *painter)
{
    if (!m_render)
        return;

    painter->beginNativePainting();

    if (!m_render->isInitialized())
        m_render->initialize(width(), height());

    {
	QMutexLocker lock(&m_mutex);
    m_render->setFrameInfo(m_rotation, m_mirrored);
    if (m_frame)
        m_render->renderFrame(*m_frame);
    }

    painter->endNativePainting();
}

void AVideoWidget::geometryChanged(const QRectF& newGeometry, const QRectF& oldGeometry)
{
    if (newGeometry.width() != oldGeometry.width() ||
        newGeometry.height() != oldGeometry.height())
    {
        m_render->setSize(newGeometry.width(), newGeometry.height());
        emit viewSizeChanged(newGeometry.width(), newGeometry.height());
    }
    QQuickPaintedItem::geometryChanged(newGeometry, oldGeometry);
}
