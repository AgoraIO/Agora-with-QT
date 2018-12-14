#ifndef VIDEORENDERER_OPENGL_H
#define VIDEORENDERER_OPENGL_H
#include <QOpenGLFunctions>

namespace agora { namespace media {
    class IVideoFrame;
}}

class AVideoWidget;
class QOpenGLShaderProgram;

class VideoRendererOpenGL
{
public:
    VideoRendererOpenGL(int width, int height);
    ~VideoRendererOpenGL();
    bool isInitialized() const { return m_program != nullptr; }
    int initialize(int width, int height);
    void setSize(int width, int height);
    int setStreamProperties(int zOrder, float left, float top, float right, float bottom);
    int getStreamProperties(int& zOrder, float& left, float& top, float& right, float& bottom);
    int renderFrame(const agora::media::IVideoFrame& videoFrame);
    int width() const { return m_targetWidth; }
    int height() const { return m_targetHeight; }
    void setFrameInfo(int rotation, bool mirrored);
private:
    int prepare();
    int frameSizeChange(int width, int height);
    QOpenGLShaderProgram* createProgram();
    int applyVertices();
    void setupTextures(const agora::media::IVideoFrame& frameToRender);
    void initializeTexture(int name, int id, int width, int height);
    void updateTextures(const agora::media::IVideoFrame& frameToRender);
    void glTexSubImage2D(GLsizei width, GLsizei height, int stride, const uint8_t* plane);
    int ajustVertices();
    int adjustCoordinates(int frWidth, int frHeight, int targetWidth, int targetHeight, int renderMode);
    static QOpenGLFunctions* renderer();
    void cleanup();
private:
    QOpenGLShaderProgram *m_program;
    GLfloat m_vertices[20];
    GLuint m_textureIds[3]; // Texture id of Y,U and V texture.

    int m_zOrder;
    float m_left;
    float m_top;
    float m_right;
    float m_bottom;
    int m_renderMode;

    int m_textureWidth;
    int m_textureHeight;
    int m_targetWidth;
    int m_targetHeight;
    bool m_resetGlVert;
    int m_rotation;
    bool m_mirrored;
};

#endif // VIDEORENDERER_OPENGL_H
