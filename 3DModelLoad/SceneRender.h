#ifndef SCENERENDER_H
#define SCENERENDER_H

#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QOpenGLBuffer>
#include <QQueue>

#include <QTimer>
#include <QMouseEvent>
#include <QKeyEvent>

#include "ModelLoader.h"

class SceneRender : public QOpenGLFunctions
{
public:
    SceneRender();

    void init();

    void paint();

    void resize(const int width, const int height);

    void processMouseEvent();

    void processKeyEvent();

    void mouseEventQueue(QQueue<QMouseEvent*> mouseEvents);

    void keyEventQueue(QQueue<QKeyEvent> keyEvents);
private:
    void createGeometry();
    void drawNode(const QMatrix4x4& model, const Node *node, QMatrix4x4 parent);
    void setupLightingAndMatrices();
private:
    QOpenGLShaderProgram    m_program;
    QOpenGLTexture          *m_tex = nullptr;
    QMatrix4x4              m_modelViewMatrix;
    QMatrix4x4              m_projectionMatrix;
    QOpenGLBuffer           m_vertexBuffer, m_indexBuffer, m_colorBuffer, m_normalBuffer;
    QOpenGLBuffer           m_texCoordBuffer;
    qreal                   m_rotateAngle;
    QVector3D               m_axis;

    QVector<QVector2D>      m_vertices;
    QVector<QVector2D>      m_textures;

    ModelLoader             m_meshLoader;
    QMatrix4x4              m_projection, m_view;
    LightInfo               m_lightInfo;

    QTimer                  m_timer;
private:
    QQueue<QMouseEvent*>    m_mouseEventQueue;
    QQueue<QKeyEvent>       m_keyEventQueue;
};

#endif // SCENERENDER_H
