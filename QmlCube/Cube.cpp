#include "Cube.h"
#include <QDebug>
#include <QThread>
#include <QImage>

Cube::Cube():
    m_vertexBuffer(QOpenGLBuffer::VertexBuffer),
    m_indexBuffer(QOpenGLBuffer::IndexBuffer),
    m_colorBuffer(QOpenGLBuffer::VertexBuffer),
    m_texCoordBuffer(QOpenGLBuffer::VertexBuffer),
    m_rotateAngle(0.0),
    m_axis(0.0f, 1.0f, 0.0f)
{
    connect(this, &Cube::windowChanged, this, &Cube::onWindowChanged);
    qDebug() << "current thread id: " << QThread::currentThreadId();
    startTimer(10);
}

void Cube::onWindowChanged(QQuickWindow *window)
{
    qDebug() << "onWindowChanged" << (window == nullptr);
    if(window == nullptr)
        return;
    connect(window, &QQuickWindow::beforeRendering, this, &Cube::render, Qt::DirectConnection);
    window->setClearBeforeRendering( false );
}

void Cube::render()
{
    //qDebug() << "render()" << QThread::currentThreadId();
    static bool ret = init();
    Q_UNUSED(ret)

    // 运动
    m_modelViewMatrix.rotate(m_rotateAngle, m_axis.x( ),
                              m_axis.y( ), m_axis.z( ) );

    // 渲染
    glViewport( 0, 0, window( )->width( ), window( )->height( ) );
    glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    //glEnable(GL_CULL_FACE);
    glFrontFace(GL_CW);

    //int attr = -1;
    m_shaderProgram->bind( );
    m_vertexBuffer.bind( );
    int posLoc = m_shaderProgram->attributeLocation("aPos" );
    m_shaderProgram->enableAttributeArray( posLoc );
    m_shaderProgram->setAttributeBuffer(posLoc, GL_FLOAT, 0, 3, 0);
    //m_shaderProgram->setAttributeBuffer(posLoc, GL_FLOAT, 0, 3, sizeof(GLfloat) * 8);

    m_colorBuffer.bind( );
    int colorLoc = m_shaderProgram->attributeLocation("aColor" );
    m_shaderProgram->enableAttributeArray(colorLoc );
    m_shaderProgram->setAttributeBuffer(colorLoc, GL_FLOAT, 0, 3, 0);
    //m_shaderProgram->setAttributeBuffer(colorLoc, GL_FLOAT, sizeof(GLfloat) * 3, 3, sizeof(GLfloat) * 8);

    m_texCoordBuffer.bind();
    int texLoc = m_shaderProgram->attributeLocation("aTexCoord");//texLoc等attribute只有在fragment shader有进行使用才生效。
    m_shaderProgram->enableAttributeArray(texLoc);
    m_shaderProgram->setAttributeBuffer(texLoc, GL_FLOAT, 0, 2, 0);
    //m_shaderProgram->setAttributeBuffer(texLoc, GL_FLOAT, sizeof(GLfloat) * 6, 2, sizeof(GLfloat) * 8);
    qDebug() << "texLoc" << posLoc << colorLoc << texLoc;

    m_indexBuffer.bind();
    m_shaderProgram->setUniformValue("modelViewMatrix", m_modelViewMatrix );
    m_shaderProgram->setUniformValue("projectionMatrix", m_projectionMatrix );
    //m_shaderProgram->setUniformValue("ourTexture", 0);
    m_texture->bind(0);//绑定纹理需要在glDrawElements之前
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, nullptr);

    //glActiveTexture(GL_TEXTURE0);
//    QOpenGLFunctions *glFuncs = QOpenGLContext::currentContext()->functions();
//    glFuncs->glActiveTexture(GL_TEXTURE0);
//    glBindTexture(GL_TEXTURE_2D, m_texture->textureId());




    //m_shaderProgram->disableAttributeArray( posLoc );
    //m_shaderProgram->disableAttributeArray( colorLoc );
    m_indexBuffer.release();
    m_vertexBuffer.release();
    //m_vao.release();
    //m_colorBuffer.release();
    //m_texCoordBuffer.release();
    m_shaderProgram->release();
}

bool Cube::init()
{
    //初始化着色器
    //qDebug() << "runOnce thread id " << QThread::currentThreadId();
    m_shaderProgram = new QOpenGLShaderProgram;
    m_shaderProgram->addShaderFromSourceFile( QOpenGLShader::Vertex,
                                             ":/cube.vsh" );
    m_shaderProgram->addShaderFromSourceFile( QOpenGLShader::Fragment,
                                             ":/cube.fsh" );
    m_shaderProgram->link( );

    // 初始化顶点缓存
    const GLfloat length = 0.5f;
//    const GLfloat vertices[] =
//    {
//        -length, -0, -length,
//        length, -0, -length,
//        0, 0, length
//    };

    const GLfloat vertices[] =
    {
        -length, -length, 0,
        length, -length, 0,
        length, length, 0,
        -length, length, 0
    };
//    const GLfloat vertices[] = {
//        //位置                 //颜色              //纹理位置
//         0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // top right
//         0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // bottom right
//        -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // bottom left
//        -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f  // top left
//    };

    //m_vao.create();
    //m_vao.bind();
    m_vertexBuffer.setUsagePattern(QOpenGLBuffer::StaticDraw);
    m_vertexBuffer.create();
    m_vertexBuffer.bind();
    m_vertexBuffer.allocate(vertices, 4 * 8 * sizeof(GLfloat));

    // 初始化颜色的缓存
    const GLfloat colors[] =
    {
        1.0f, 0.0f, 1.0f, 1.0f,
        1.0f, 0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f, 1.0f,
    };
    m_colorBuffer.setUsagePattern( QOpenGLBuffer::StaticDraw );
    m_colorBuffer.create();
    m_colorBuffer.bind();
    m_colorBuffer.allocate(colors, sizeof(colors));

    const GLfloat texCoords[] =
    {
        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 1.0f
    };
    m_texCoordBuffer.setUsagePattern(QOpenGLBuffer::StaticDraw);
    m_texCoordBuffer.create();
    m_texCoordBuffer.bind();
    m_texCoordBuffer.allocate(texCoords, sizeof(texCoords));

    // 初始化索引缓存
    GLubyte indices[] =
    {
        0, 1, 2,
        0, 2, 3
    };

//    GLubyte indices[] =
//    {
//        0, 1, 2, 0, 2, 3,// 下面
//        7, 6, 4, 6, 5, 4,// 上面
//        7, 4, 3, 4, 0, 3,// 左面
//        5, 6, 1, 6, 2, 1,// 右面
//        4, 5, 0, 5, 1, 0,// 前面
//        3, 2, 6, 3, 6, 7,// 背面
//    };

    m_indexBuffer.setUsagePattern( QOpenGLBuffer::StaticDraw );
    m_indexBuffer.create( );
    m_indexBuffer.bind( );
    m_indexBuffer.allocate(indices, sizeof(indices) );

    // 设定模型矩阵和投影矩阵
    float aspectRatio  = float( window()->width()) / float( window()->height());
    m_projectionMatrix.perspective( 45.0f,
                                    aspectRatio,
                                    0.5f,
                                    500.0f);
    //初始化位置
    m_modelViewMatrix.setToIdentity( );
    m_modelViewMatrix.translate(0.0f, 0.0f, -5.0f);

    auto img = QImage("wall.jpg").mirrored();
    qDebug() << "img load reuslt " << img.isNull();
    m_texture = new QOpenGLTexture(img);
    m_texture->setMinificationFilter(QOpenGLTexture::Nearest);
    m_texture->setMagnificationFilter(QOpenGLTexture::Linear);
    m_texture->setWrapMode(QOpenGLTexture::Repeat);
    //m_texture->bind(0);
    m_shaderProgram->setUniformValue("ourTexture", 0);//绑定纹理单元
    connect(window()->openglContext(),
             &QOpenGLContext::aboutToBeDestroyed,
             this, &Cube::release);

    return true;
}

void Cube::release()
{
    qDebug( "Vertex buffer and index buffer are to be destroyed." );
    m_vertexBuffer.destroy();
    m_indexBuffer.destroy();
    m_colorBuffer.destroy();
    m_texCoordBuffer.destroy();
}

void Cube::timerEvent(QTimerEvent *event)
{
    //qDebug() << "timerEvent" << m_rotateAngle;
    m_rotateAngle = 0.5f;
    window()->update();
    //this->update();
}
