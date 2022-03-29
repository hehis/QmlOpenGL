#include "SceneRender.h"

#include <QObject>


SceneRender::SceneRender():
    m_vertexBuffer(QOpenGLBuffer::VertexBuffer),
    m_indexBuffer(QOpenGLBuffer::IndexBuffer),
    m_colorBuffer(QOpenGLBuffer::VertexBuffer),
    m_normalBuffer(QOpenGLBuffer::VertexBuffer),
    m_texCoordBuffer(QOpenGLBuffer::VertexBuffer),
    m_rotateAngle(0.0),
    m_axis(0.0f, 1.0f, 0.0f)
{
//    m_timer.setInterval(24);
//    QObject::connect(&m_timer, &QTimer::timeout, [&]{
//        m_rotateAngle += 3.0f;
//    });
//    m_timer.start();
}

void SceneRender::init()
{
    initializeOpenGLFunctions();

    m_program.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/model.vsh");
    m_program.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/model.fsh");

    m_program.link();

    createGeometry();
    setupLightingAndMatrices();


    glEnable(GL_DEPTH_TEST);
    glClearColor(1.0f, 1.0f, 1.0f ,1.0f);
    return;
}

void SceneRender::paint()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    m_program.bind();

    QMatrix4x4 model;
    model.setToIdentity();
    //model.translate(-0.2f, 0.0f, .5f);
    model.rotate(m_rotateAngle, 0.0f, 1.0f, 0.0f);

    // Set shader uniforms for light information
    m_program.setUniformValue("lightPosition", m_lightInfo.position );
    m_program.setUniformValue("lightIntensity", m_lightInfo.intensity );

    drawNode(model, m_meshLoader.getNodeData().data(), QMatrix4x4());
    processKeyEvent();

    return;
}

void SceneRender::resize(const int width, const int height)
{
    glViewport(0, 0, width, height);
    float aspectRatio  = float( width ) / float(height);
    m_projection.setToIdentity();
    m_projection.perspective(60.0f, aspectRatio, .3f, 1000);
}

void SceneRender::processMouseEvent()
{
//    for(auto& event:mouseEvents)
//    {

//    }
}

void SceneRender::processKeyEvent()
{

    for(auto& event : m_keyEventQueue)
    {
        qDebug() << "processKeyEvent" << (event.key() == Qt::Key_Left) << event.key();
        switch(event.key())
        {
        case Qt::Key_Left:
            m_rotateAngle -= 0.1f;
            break;
        case Qt::Key_Right:
            m_rotateAngle += 0.1f;
            break;
        case Qt::Key_Up:
            break;
        case Qt::Key_Down:
            break;
        default:
            break;
        }
    }
    m_keyEventQueue.clear();
}

void SceneRender::mouseEventQueue(QQueue<QMouseEvent *> mouseEvents)
{
    m_mouseEventQueue.append(mouseEvents);
}

void SceneRender::keyEventQueue(QQueue<QKeyEvent> keyEvents)
{
    //qDebug() << "keyEventQueue" << keyEvents.size();
    m_keyEventQueue.append(keyEvents);
}

void SceneRender::createGeometry()
{
    //if(!m_meshLoader.load("velociraptor_mesh_materials.dae")) {//loading dae model
    if(!m_meshLoader.load("house.glb")) {
        qDebug() << "ModelLoader failed to load model";
        return;
        //exit(1);
    }

    // Get the loaded model data from the model-loader: (v)ertices, (n)ormals, and (i)ndices
    QVector<float> *vertex, *normals; QVector<unsigned int> *index;
    m_meshLoader.getBufferData(&vertex, &normals, &index);

    // Put all the vertex data in a FBO
    m_vertexBuffer.create();
    m_vertexBuffer.setUsagePattern(QOpenGLBuffer::StaticDraw );
    m_vertexBuffer.bind();
    m_vertexBuffer.allocate(&(*vertex)[0], vertex->size() * sizeof((*vertex)[0]));

    // Put all the normal data in a FBO
    m_normalBuffer.create();
    m_normalBuffer.setUsagePattern( QOpenGLBuffer::StaticDraw );
    m_normalBuffer.bind();
    m_normalBuffer.allocate(&(*normals)[0], normals->size() * sizeof((*normals)[0]));


    // Put all the index data in a IBO
    m_indexBuffer.create();
    m_indexBuffer.setUsagePattern( QOpenGLBuffer::StaticDraw );
    m_indexBuffer.bind();
    m_indexBuffer.allocate(&(*index)[0], index->size() * sizeof((*index)[0]));

}

void SceneRender::drawNode(const QMatrix4x4 &model, const Node *node, QMatrix4x4 parent)
{
    // Prepare matrices
    QMatrix4x4 local = parent * node->transformation;
    QMatrix4x4 mv = m_view * model * local;

    m_program.bind();
    m_vertexBuffer.bind();
    int posLoc = m_program.attributeLocation("vertexPosition" );
    m_program.enableAttributeArray(posLoc);
    m_program.setAttributeBuffer(posLoc, GL_FLOAT, 0, 3, 0);

    m_normalBuffer.bind();
    int normalLoc = m_program.attributeLocation("vertexNormal" );
    m_program.enableAttributeArray(normalLoc );
    m_program.setAttributeBuffer(normalLoc, GL_FLOAT, 0, 3, 0);

    //qDebug() << "texLoc" << posLoc << normalLoc;

    m_indexBuffer.bind();

    m_program.setUniformValue("MV",  mv);
    m_program.setUniformValue("N",   mv.normalMatrix());
    m_program.setUniformValue("MVP", m_projection * mv);

    MaterialInfo materialInfo;
    materialInfo.ambient = QVector3D( 0.1f, 0.05f, 0.0f );
    materialInfo.diffuse = QVector3D( .9f, .6f, .2f );
    materialInfo.specular = QVector3D( .2f, .2f, .2f );
    materialInfo.shiness = 50.0f;
    // Draw each mesh in this node
    for(int i = 0; i<node->meshes.size(); ++i)
    {
        const Mesh& m = *node->meshes[i];

        if (m.material->name == QString("DefaultMaterial")) {
            m_program.setUniformValue("Ka",        materialInfo.ambient);
            m_program.setUniformValue("Kd",        materialInfo.diffuse);
            m_program.setUniformValue("Ks",        materialInfo.specular);
            m_program.setUniformValue("shininess", materialInfo.shiness);
        } else {
            m_program.setUniformValue("Ka",        m.material->ambient);
            m_program.setUniformValue("Kd",        m.material->diffuse);
            m_program.setUniformValue("Ks",        m.material->specular);
            m_program.setUniformValue("shininess", m.material->shiness);
        }

        glDrawElements(GL_TRIANGLES, m.indexCount, GL_UNSIGNED_INT, (const GLvoid*)(m.indexOffset * sizeof(GLuint)));
    }

    // Recursively draw this nodes children nodes
    for(int i = 0; i < node->nodes.size(); ++i)
        drawNode(model, &node->nodes[i], local);
}

void SceneRender::setupLightingAndMatrices()
{
    m_view.setToIdentity();
    m_view.lookAt(
                QVector3D(0.0f, 0.0f, 1.2f),    // Camera Position
                QVector3D(0.0f, 0.0f, 0.0f),    // Point camera looks towards
                QVector3D(0.0f, 1.0f, 0.0f));   // Up vector
//    m_view.translate(QVector3D(0.0f, 0.0f, -2.2f));

    m_lightInfo.position = QVector4D( -1.0f, 1.0f, 1.0f, 1.0f );
    m_lightInfo.intensity = QVector3D( 1.0f, 1.0f, 1.0f);

}
