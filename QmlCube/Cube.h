#ifndef CUBE_H
#define CUBE_H

#include <QObject>
#include <QQuickItem>

#include <QVector3D>
#include <QMatrix4x4>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QQuickWindow>

#define DECLARE_Q_PROPERTY(aType, aProperty) protected: \
    aType m_ ## aProperty; public: \
    aType aProperty(){return m_ ## aProperty;} \
    void set ## aProperty(aType _ ## aProperty) \
    {\
        m_ ## aProperty = _ ## aProperty; \
        if( window() != Q_NULLPTR) \
        {\
            window()->update();\
        }\
    }


class Cube : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(qreal rotateAngle READ rotateAngle
               WRITE setrotateAngle NOTIFY rotateAngleChanged)
    Q_PROPERTY(QVector3D axis READ axis WRITE setaxis NOTIFY axisChanged)
public:
    Cube();

signals:
    void rotateAngleChanged();
    void axisChanged();
protected:
    void render();
    void onWindowChanged(QQuickWindow* window);
    void release();
    void timerEvent(QTimerEvent* event) override;
protected:
    bool init();

    QMatrix4x4              m_modelViewMatrix;
    QMatrix4x4              m_projectionMatrix;
    QOpenGLBuffer           m_vertexBuffer, m_indexBuffer;
    QOpenGLBuffer           m_colorBuffer;
    QOpenGLBuffer           m_texCoordBuffer;
    QOpenGLBuffer           m_vbo;
    QOpenGLVertexArrayObject m_vao;
    QOpenGLShaderProgram*   m_shaderProgram;
    QOpenGLTexture*         m_texture;
    //QOpenGLFunctions        m_glFuntions;

    DECLARE_Q_PROPERTY(qreal, rotateAngle)
    DECLARE_Q_PROPERTY(QVector3D, axis)

};

#endif // CUBE_H
