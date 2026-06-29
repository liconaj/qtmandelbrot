import QtQuick
import QtQuick.Controls.Basic

ApplicationWindow {
    id: window
    width: 640
    height: 480
    minimumWidth: width
    minimumHeight: height
    visible: true
    title: qsTr("Mandelbrot Explorer")

    Label {
        anchors.centerIn: parent
        text: qsTr("Hello, world!")
    }
}
