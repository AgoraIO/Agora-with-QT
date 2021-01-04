#include "video_render_impl.h"
#include "avideowidget.h"
#include <QDebug>
#include <QMutexLocker>
using namespace agora::media;

VideoRenderImpl::VideoRenderImpl(const agora::media::ExternalVideoRenerContext& context)
    :m_view(reinterpret_cast<AVideoWidget*>(context.view))
    ,m_renderCallback(context.renderCallback)
{
//    qDebug() << "create video render " << this << "for view " << m_view;

    m_view->setViewProperties(context.zOrder, context.left, context.top, context.right, context.bottom);
    connect(m_view, SIGNAL(widgetInvalidated()), this, SLOT(handleWidgetInvalidated()), Qt::DirectConnection);
    connect(m_view, SIGNAL(viewSizeChanged(int,int)), this, SLOT(handleViewSizeChanged(int,int)), Qt::DirectConnection);
}

VideoRenderImpl::~VideoRenderImpl()
{
//    qDebug() << "video render " << this << " destroyed";
}

void VideoRenderImpl::handleWidgetInvalidated()
{
    {
		QMutexLocker lock(&m_mutex);
    m_view = nullptr;
    }
    if (m_renderCallback)
        m_renderCallback->onViewDestroyed();
}

void VideoRenderImpl::handleViewSizeChanged(int width, int height)
{
    if (m_renderCallback)
        m_renderCallback->onViewSizeChanged(width, height);
}

int VideoRenderImpl::deliverFrame(const agora::media::IVideoFrame& videoFrame, int rotation, bool mirrored)
{
	QMutexLocker lock(&m_mutex);
    if (m_view)
        return m_view->deliverFrame(videoFrame, rotation, mirrored);
    return -1;
}
