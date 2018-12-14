#ifndef AVIDEOCANVAS_H
#define AVIDEOCANVAS_H

#include <QQuickPaintedItem>
#include <mutex>
#include <memory>

namespace agora { namespace media {
    class IVideoFrame;
}}

class VideoRendererOpenGL;

class AVideoWidget : public QQuickPaintedItem
{
    Q_OBJECT
public:
    AVideoWidget(QQuickItem *parent = 0);
    ~AVideoWidget();
    int setViewProperties(int zOrder, float left, float top, float right, float bottom);
    int deliverFrame(const agora::media::IVideoFrame& videoFrame, int rotation, bool mirrored);
    virtual void paint(QPainter *painter) Q_DECL_OVERRIDE;
protected:
    virtual void geometryChanged(const QRectF& newGeometry, const QRectF& oldGeometry) Q_DECL_OVERRIDE;
signals:
    void frameDelivered();
    void widgetInvalidated();
    void viewSizeChanged(int width, int height);

public slots:
    void renderFrame();
    void handleWindowChanged(QQuickWindow *win);
    void cleanup();
private:
    std::unique_ptr<VideoRendererOpenGL> m_render;
    std::mutex m_mutex;
    //usage of m_frame should be guarded by m_mutex
    agora::media::IVideoFrame* m_frame;
    int m_rotation;
    bool m_mirrored;
};

#endif // AVIDEOCANVAS_H
