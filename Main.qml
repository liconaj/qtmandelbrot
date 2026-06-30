import QtQuick
import QtQuick.Controls.Basic

ApplicationWindow {
    id: window
    width: minimumWidth
    height: minimumHeight
    minimumWidth: 640
    minimumHeight: 450
    visible: true
    title: qsTr("Mandelbrot Explorer")

    Image {
        id: mandelbrot
        anchors.centerIn: parent
        anchors.fill: parent
        asynchronous: true
        source: `image://renderer/mandelbrot?w=${mandelbrot.sourceSize.width}&h=${mandelbrot.sourceSize.height}`
        fillMode: Image.PreserveAspectFit

        onWidthChanged: reloadMandelbrot()
        onHeightChanged: reloadMandelbrot()

        function reloadMandelbrot() {
            if (throttlingTimer.running) {
                throttlingTimer.restart();
            } else {
                throttlingTimer.start();
            }
        }

        Timer {
            id: throttlingTimer
            interval: 100
            onTriggered: {
                mandelbrot.sourceSize.width = mandelbrot.width
                mandelbrot.sourceSize.height = mandelbrot.height
            }       
        }
    }
}
