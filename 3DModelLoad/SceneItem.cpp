#include "SceneItem.h"
#include <QQuickWindow>
#include <QOpenGLFramebufferObjectFormat>
#include <QThread>

class SceneFboRender : public QQuickFramebufferObject::Renderer
{
public:
    SceneFboRender()
    {
        m_render.init();
    }
    void render() override{
        m_render.paint();
        //qDebug() << "render"  << QThread::currentThreadId();
        //m_window->resetOpenGLState();

    }
    QOpenGLFramebufferObject *createFramebufferObject(const QSize &size) override
    {
        QOpenGLFramebufferObjectFormat format;
        format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
        format.setSamples(4);
        m_render.resize(size.width(), size.height());
        return new QOpenGLFramebufferObject(size, format);
    }

    void synchronize(QQuickFramebufferObject *item) override{
        SceneItem *pItem = qobject_cast<SceneItem*>(item);
        //auto mouseEvent = pItem->mouseEventQueue();
        auto keyEvent = pItem->keyEventQueue();
        //qDebug() << "synchronize"  << keyEvent.size();
        m_render.keyEventQueue(keyEvent);
        //m_render.
        //qDebug() << "synchronize" << QThread::currentThreadId();

    }
private:
    //QQuickWindow        *m_window;
    //SceneItem           *m_item;
    SceneRender         m_render;
};

SceneItem::SceneItem(QQuickFramebufferObject *parent):QQuickFramebufferObject(parent)
{
    setMirrorVertically(true);//反转页面
    startTimer(24);//使用timer刷新频率
    setFlag(ItemAcceptsInputMethod, true);
    setFocus(true);//相当于在qml使用focus:true,必须加上这个才能使keyPressEvent生效
    setAcceptedMouseButtons(Qt::AllButtons);
    setAcceptHoverEvents(true);


}

QQueue<QMouseEvent *> SceneItem::mouseEventQueue()
{
    return m_mouseEventQueue;
}

QQueue<QKeyEvent> SceneItem::keyEventQueue()
{
    auto events = m_keyEventQueue;
    //qDebug() << "keyEventQueue00"  << events.size();
    m_keyEventQueue.clear();
    //qDebug() << "keyEventQueue"  << events.size();
    return events;
}

void SceneItem::timerEvent(QTimerEvent *event)
{
    update();
}

QQuickFramebufferObject::Renderer *SceneItem::createRenderer() const
{
    return new SceneFboRender;
}

void SceneItem::mousePressEvent(QMouseEvent *event)
{
    //qDebug() << "mousePressEvent"  << QThread::currentThreadId();
    m_mouseEventQueue.push_back(event);
}

void SceneItem::mouseMoveEvent(QMouseEvent *event)
{
    m_mouseEventQueue.push_back(event);
}

void SceneItem::mouseReleaseEvent(QMouseEvent *event)
{
    //qDebug() << "mouseReleaseEvent----";
    m_mouseEventQueue.push_back(event);
}

void SceneItem::keyPressEvent(QKeyEvent *event)
{
    qDebug() << "keyPressEvent----" << event->key();
    m_keyEventQueue.push_back(*event);
}

void SceneItem::keyReleaseEvent(QKeyEvent *event)
{
    //qDebug() << "keyReleaseEvent----";
    m_keyEventQueue.push_back(*event);
}
