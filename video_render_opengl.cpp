#include "video_render_opengl.h"
#include "video_render_impl.h"
#include <QOpenGLContext>
#include <QOpenGLShaderProgram>
#include <QOpenGLFunctions>
#include <QDebug>

using namespace agora::media;

static const char g_indices[] = { 0, 3, 2, 0, 2, 1 };

VideoRendererOpenGL::VideoRendererOpenGL(int width, int height)
    :m_program(nullptr)
    ,m_zOrder(0)
    ,m_left(1)
    ,m_top(0)
    ,m_right(0)
    ,m_bottom(1)
    ,m_renderMode(1)
    ,m_textureWidth(-1)
    ,m_textureHeight(-1)
    ,m_targetWidth(width)
    ,m_targetHeight(height)
    ,m_resetGlVert(true)
    ,m_rotation(0)
    ,m_mirrored(false)
{
    static const GLfloat vertices[20] = {
      // X, Y, Z, U, V
      -1, -1, 0, 1, 0, // Bottom Left
      1, -1, 0, 0, 0, //Bottom Right
      1, 1, 0, 0, 1, //Top Right
      -1, 1, 0, 1, 1 }; //Top Left

    memset(m_textureIds, 0, sizeof(m_textureIds));
    memcpy(m_vertices, vertices, sizeof(m_vertices));
}

VideoRendererOpenGL::~VideoRendererOpenGL()
{
//    qDebug() << "video renderer " << this << " destroyed";
    if (m_textureIds[0] != 0) {
        QOpenGLFunctions *f = renderer();
        f->glDeleteTextures(3, m_textureIds);
    }
}

void VideoRendererOpenGL::cleanup()
{
    if (m_program)
    {
        delete m_program;
        m_program = nullptr;
    }
}

int VideoRendererOpenGL::setStreamProperties(int zOrder, float left, float top, float right, float bottom)
{
    m_zOrder = zOrder;
    m_left = left;
    m_top = top;
    m_right = right;
    m_bottom = bottom;
    return 0;
}

int VideoRendererOpenGL::getStreamProperties(int& zOrder, float& left, float& top, float& right, float& bottom)
{
    zOrder = m_zOrder;
    left = m_left;
    top = m_top;
    right = m_right;
    bottom = m_bottom;
    return 0;
}

void VideoRendererOpenGL::setFrameInfo(int rotation, bool mirrored)
{
    if (m_rotation != rotation || m_mirrored != mirrored)
    {
        m_resetGlVert = true;
        m_rotation = rotation;
        m_mirrored = mirrored;
    }
}

inline QOpenGLFunctions* VideoRendererOpenGL::renderer()
{
    return QOpenGLContext::currentContext()->functions();
}

QOpenGLShaderProgram* VideoRendererOpenGL::createProgram()
{
    static const char vertextShader[] = {
      "attribute vec4 aPosition;\n"
      "attribute vec2 aTextureCoord;\n"
      "varying vec2 vTextureCoord;\n"
      "void main() {\n"
      "  gl_Position = aPosition;\n"
      "  vTextureCoord = aTextureCoord;\n"
      "}\n" };

    // The fragment shader.
    // Do YUV to RGB565 conversion.
    static const char fragmentShader[] = {
      "mediump float;\n"
      "uniform sampler2D Ytex;\n"
      "uniform sampler2D Utex,Vtex;\n"
      "varying vec2 vTextureCoord;\n"
      "void main(void) {\n"
      "  float nx,ny,r,g,b,y,u,v;\n"
      "  mediump vec4 txl,ux,vx;"
      "  nx=vTextureCoord[0];\n"
      "  ny=vTextureCoord[1];\n"
      "  y=texture2D(Ytex,vec2(nx,ny)).r;\n"
      "  u=texture2D(Utex,vec2(nx,ny)).r;\n"
      "  v=texture2D(Vtex,vec2(nx,ny)).r;\n"

      //"  y = v;\n"+
      "  y=1.1643*(y-0.0625);\n"
      "  u=u-0.5;\n"
      "  v=v-0.5;\n"

      "  r=y+1.5958*v;\n"
      "  g=y-0.39173*u-0.81290*v;\n"
      "  b=y+2.017*u;\n"
      "  gl_FragColor=vec4(r,g,b,1.0);\n"
      "}\n" };

    QOpenGLShaderProgram* program = new QOpenGLShaderProgram;
    bool r = program->addShaderFromSourceCode(QOpenGLShader::Vertex, vertextShader);
    r = program->addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShader);
    r = program->link();
    return program;
}

int VideoRendererOpenGL::initialize(int width, int height)
{
    if (m_program)
        return 0;
    m_program = createProgram();
    setSize(width, height);
    return 0;
}

void VideoRendererOpenGL::setSize(int width, int height)
{
    m_targetWidth = width;
    m_targetHeight = height;
}

int VideoRendererOpenGL::prepare()
{
    if (!m_program)
        return -1;

    int positionHandle = m_program->attributeLocation("aPosition");
    if (positionHandle == -1)
        return -1;

    int textureHandle = m_program->attributeLocation("aTextureCoord");
    if (textureHandle == -1) {
        return -1;
    }

    QOpenGLFunctions *f = renderer();
    // set the vertices array in the shader
    // _vertices contains 4 vertices with 5 coordinates.
    // 3 for (xyz) for the vertices and 2 for the texture
    f->glVertexAttribPointer(positionHandle, 3, GL_FLOAT, false,
                          5 * sizeof(GLfloat), m_vertices);

    f->glEnableVertexAttribArray(positionHandle);

    // set the texture coordinate array in the shader
    // _vertices contains 4 vertices with 5 coordinates.
    // 3 for (xyz) for the vertices and 2 for the texture
    f->glVertexAttribPointer(textureHandle, 2, GL_FLOAT, false, 5
                          * sizeof(GLfloat), &m_vertices[3]);
    f->glEnableVertexAttribArray(textureHandle);

    m_program->bind();

    int i = m_program->uniformLocation("Ytex");
    f->glUniform1i(i, 0); /* Bind Ytex to texture unit 0 */

    i = m_program->uniformLocation("Utex");
    f->glUniform1i(i, 1); /* Bind Utex to texture unit 1 */

    i = m_program->uniformLocation("Vtex");
    f->glUniform1i(i, 2); /* Bind Vtex to texture unit 2 */

    f->glViewport(0, 0, m_targetWidth, m_targetHeight);

    return 0;
}

int VideoRendererOpenGL::applyVertices()
    {
    if (!m_program)
        return -1;

    int positionHandle = m_program->attributeLocation("aPosition");
    if (positionHandle == -1) {
        return -1;
    }

    int textureHandle = m_program->attributeLocation("aTextureCoord");
    if (textureHandle == -1) {
        return -1;
    }

    QOpenGLFunctions *f = renderer();

    // set the vertices array in the shader
    // _vertices contains 4 vertices with 5 coordinates.
    // 3 for (xyz) for the vertices and 2 for the texture
    f->glVertexAttribPointer(positionHandle, 3, GL_FLOAT, false,
                        5 * sizeof(GLfloat), m_vertices);

    f->glEnableVertexAttribArray(positionHandle);

    // set the texture coordinate array in the shader
    // _vertices contains 4 vertices with 5 coordinates.
    // 3 for (xyz) for the vertices and 2 for the texture
    f->glVertexAttribPointer(textureHandle, 2, GL_FLOAT, false, 5
                        * sizeof(GLfloat), &m_vertices[3]);

    f->glEnableVertexAttribArray(textureHandle);

    return 0;
}

int VideoRendererOpenGL::renderFrame(const agora::media::IVideoFrame& videoFrame)
{
    if (videoFrame.IsZeroSize())
        return -1;

    int r = prepare();
    if (r)
        return r;

    QOpenGLFunctions *f = renderer();

    f->glClear(GL_COLOR_BUFFER_BIT);

    if (m_textureWidth != (GLsizei) videoFrame.width() ||
        m_textureHeight != (GLsizei) videoFrame.height())
    {
        setupTextures(videoFrame);
        m_resetGlVert = true;
    }

    if (m_resetGlVert)
    {
        if (!ajustVertices())
            m_resetGlVert = false;
    }

    updateTextures(videoFrame);

    f->glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, g_indices);

    return 0;
}

void VideoRendererOpenGL::initializeTexture(int name, int id, int width, int height)
{
    QOpenGLFunctions *f = renderer();
    f->glActiveTexture(name);
    f->glBindTexture(GL_TEXTURE_2D, id);
    f->glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    f->glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    f->glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    f->glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    f->glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, width, height, 0,
               GL_LUMINANCE, GL_UNSIGNED_BYTE, NULL);
}

void VideoRendererOpenGL::setupTextures(const agora::media::IVideoFrame& frameToRender)
{
    const GLsizei width = frameToRender.width();
    const GLsizei height = frameToRender.height();

    QOpenGLFunctions *f = renderer();

    if (!m_textureIds[0])
        f->glGenTextures(3, m_textureIds); //Generate  the Y, U and V texture
    initializeTexture(GL_TEXTURE0, m_textureIds[0], width, height);
    initializeTexture(GL_TEXTURE1, m_textureIds[1], width / 2, height / 2);
    initializeTexture(GL_TEXTURE2, m_textureIds[2], width / 2, height / 2);

    m_textureWidth = width;
    m_textureHeight = height;
}

void VideoRendererOpenGL::updateTextures(const agora::media::IVideoFrame& frameToRender)
{
    const GLsizei width = frameToRender.width();
    const GLsizei height = frameToRender.height();

    QOpenGLFunctions *f = renderer();
    f->glActiveTexture(GL_TEXTURE0);
    f->glBindTexture(GL_TEXTURE_2D, m_textureIds[0]);
    glTexSubImage2D(width, height, frameToRender.stride(IVideoFrame::Y_PLANE),
                  frameToRender.buffer(IVideoFrame::Y_PLANE));

    f->glActiveTexture(GL_TEXTURE1);
    f->glBindTexture(GL_TEXTURE_2D, m_textureIds[1]);
    glTexSubImage2D(width / 2, height / 2, frameToRender.stride(IVideoFrame::U_PLANE),
                  frameToRender.buffer(IVideoFrame::U_PLANE));

    f->glActiveTexture(GL_TEXTURE2);
    f->glBindTexture(GL_TEXTURE_2D, m_textureIds[2]);
    glTexSubImage2D(width / 2, height / 2, frameToRender.stride(IVideoFrame::V_PLANE),
                  frameToRender.buffer(IVideoFrame::V_PLANE));
}

// Uploads a plane of pixel data, accounting for stride != width*bpp.
void VideoRendererOpenGL::glTexSubImage2D(GLsizei width, GLsizei height, int stride, const uint8_t* plane)
{
    QOpenGLFunctions *f = renderer();
    if (stride == width)
    {
        // Yay!  We can upload the entire plane in a single GL call.
        f->glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_LUMINANCE,
                        GL_UNSIGNED_BYTE,
                        static_cast<const GLvoid*>(plane));
    }
    else
    {
        // Boo!  Since GLES2 doesn't have GL_UNPACK_ROW_LENGTH and Android doesn't
        // have GL_EXT_unpack_subimage we have to upload a row at a time.  Ick.
        for (int row = 0; row < height; ++row)
        {
            f->glTexSubImage2D(GL_TEXTURE_2D, 0, 0, row, width, 1, GL_LUMINANCE,
                          GL_UNSIGNED_BYTE,
                          static_cast<const GLvoid*>(plane + (row * stride)));
        }
    }
}

int VideoRendererOpenGL::ajustVertices()
{
  if (m_left < -90) {
    return -1;
  }

  int r = m_rotation;
  r = r < 0 ? r+360:r;

//  qDebug() << "rotation " << r;

  //adjust U/V
  if (r == 90)
  {
    //bottom left
    m_vertices[3] = 0;
    m_vertices[4] = 0;

    //bottom right
    m_vertices[8] = 0;
    m_vertices[9] = 1;

    //top right
    m_vertices[13] = 1;
    m_vertices[14] = 1;

    //top left
    m_vertices[18] = 1;
    m_vertices[19] = 0;
  }
  else if (r == 180)
  {
    //bottom left
    m_vertices[3] = 0;
    m_vertices[4] = 1;

    //bottom right
    m_vertices[8] = 1;
    m_vertices[9] = 1;

    //top right
    m_vertices[13] = 1;
    m_vertices[14] = 0;

    //top left
    m_vertices[18] = 0;
    m_vertices[19] = 0;
  }
  else if (r == 270)
  {
    //bottom left
    m_vertices[3] = 0;
    m_vertices[4] = 0;

    //bottom right
    m_vertices[8] = 0;
    m_vertices[9] = 1;

    //top right
    m_vertices[13] = 1;
    m_vertices[14] = 1;

    //top left
    m_vertices[18] = 1;
    m_vertices[19] = 0;
  }
  else
  {
    //bottom left
    m_vertices[3] = 1;
    m_vertices[4] = 0;

    //bottom right
    m_vertices[8] = 0;
    m_vertices[9] = 0;

    //top right
    m_vertices[13] = 0;
    m_vertices[14] = 1;

    //top left
    m_vertices[18] = 1;
    m_vertices[19] = 1;
  }


  if (m_left > m_right)
  {
      std::swap(m_vertices[3], m_vertices[8]);
      std::swap(m_vertices[4], m_vertices[9]);
      std::swap(m_vertices[13], m_vertices[18]);
      std::swap(m_vertices[14], m_vertices[19]);
  }

  int renderMode = m_renderMode;

  if ((r == 0 || r == 180))
  {
    adjustCoordinates(m_textureWidth, m_textureHeight,
      m_targetWidth, m_targetHeight, renderMode);
  }
  else
  {
    adjustCoordinates(m_textureHeight, m_textureWidth,
      m_targetWidth, m_targetHeight, renderMode);
  }

  return applyVertices();
}

int VideoRendererOpenGL::adjustCoordinates(int frWidth, int frHeight, int targetWidth, int targetHeight, int renderMode)
{
  if (targetWidth <= 0 || targetHeight <= 0) {
    return 0;
  }

  frWidth = frWidth <= 0 ? 640:frWidth;
  frHeight = frHeight <= 0 ? 360:frHeight;

  int twidth = frWidth;
  int theight = frHeight;
  float orig_ratio = (theight + 0.0f) / twidth;
  float require_ratio = (targetHeight + 0.0f) / targetWidth;
  float left, right, top, bottom, zOrder;
  zOrder = m_zOrder;
  left = 0;//_left;
  right = 1;//_right;
  top = m_top;
  bottom = m_bottom;

  if (m_renderMode == 3) {
    if (require_ratio*0.9 < orig_ratio && orig_ratio < require_ratio*1.1)
      renderMode = 1;
    else
      renderMode = 2;
  }

  if (renderMode == 1) {
    if (require_ratio < orig_ratio) {
      theight = require_ratio * frWidth;

      top = m_top -
        (m_bottom - m_top) * (frHeight / (theight + 0.0f) - 1.0f) / 2;
      bottom = m_bottom +
        (m_bottom - m_top) * (frHeight / (theight + 0.0f) - 1.0f) / 2;
    } else if (require_ratio > orig_ratio) {
      twidth = frHeight / require_ratio;

      //left = _left -
      //  (_right - _left) * (frWidth / (twidth + 0.0f) - 1.0f) / 2;
      //right = _right +
      //  (_right - _left) * (frWidth / (twidth + 0.0f) - 1.0f) / 2;
      left = 0 - (frWidth / (twidth + 0.0f) - 1.0f) / 2;
      right = 1 + (frWidth / (twidth + 0.0f) - 1.0f) / 2;
    }
  } else if (renderMode == 2) {
    if (require_ratio < orig_ratio) {
      twidth = targetHeight / orig_ratio;

      left = 0 + (targetWidth - twidth) / (targetWidth + 0.0f) / 2;
      right = 1 - (targetWidth - twidth) / (targetWidth + 0.0f) / 2;
    } else if (require_ratio > orig_ratio) {
      theight = targetWidth * orig_ratio;

      bottom = m_bottom +
        (m_top - m_bottom) * (targetHeight - theight) / (targetHeight + 0.0f) / 2;
      top = m_top -
        (m_top - m_bottom) * (targetHeight - theight) / (targetHeight + 0.0f) / 2;
    }
  }

  // Bottom Left
  m_vertices[0] = (left * 2) - 1;
  m_vertices[1] = -1 * (2 * bottom) + 1;
  m_vertices[2] = zOrder;

  //Bottom Right
  m_vertices[5] = (right * 2) - 1;
  m_vertices[6] = -1 * (2 * bottom) + 1;
  m_vertices[7] = zOrder;

  //Top Right
  m_vertices[10] = (right * 2) - 1;
  m_vertices[11] = -1 * (2 * top) + 1;
  m_vertices[12] = zOrder;

  //Top Left
  m_vertices[15] = (left * 2) - 1;
  m_vertices[16] = -1 * (2 * top) + 1;
  m_vertices[17] = zOrder;

  return 0;
}
