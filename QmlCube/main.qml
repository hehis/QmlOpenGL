import QtQuick 2.15
import QtQuick.Window 2.15

import VoiceAi.Controls 1.0

Window {
    id: root
    width: 640
    height: 480
    visible: true
    title: qsTr("Hello World")

    Cube
    {
        id: cube
        anchors.fill: parent
    }

    Rectangle
    {
        anchors.centerIn: parent
        width: textField.width * 1.2
        height: textField.height * 1.5
        radius: textField.height / 3
        color: "lightsteelblue"
        border.color: "white"
        border.width: 2
        Text
        {
            id: textField
            anchors.centerIn: parent
            text: "您好世界！"
            font.pixelSize: root.width / 20
        }
    }

}
