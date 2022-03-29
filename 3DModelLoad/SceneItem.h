#ifndef SCENEITEM_H
#define SCENEITEM_H

#include <QQuickFramebufferObject>
#include <QObject>
#include <QQueue>
#include "SceneRender.h"

class SceneItem : public QQuickFramebufferObject
{
    Q_OBJECT
public:
    SceneItem(QQuickFramebufferObject *parent = nullptr);
    /**
   * @brief         返回鼠标事件，在Renderer进行处理
   * @author        haisong@voiceaitech.com
   * @date          2022-03-15
   */
    QQueue<QMouseEvent*> mouseEventQueue();

    QQueue<QKeyEvent> keyEventQueue();

protected:
    void timerEvent(QTimerEvent *event) override;
    Renderer *createRenderer() const override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
private:
    QQueue<QMouseEvent*>   m_mouseEventQueue;
    QQueue<QKeyEvent>     m_keyEventQueue;

};

#endif // SCENEITEM_H
