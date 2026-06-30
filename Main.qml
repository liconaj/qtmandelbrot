import QtQuick
import QtQuick.Controls.Basic
import com.liconaj.qtmandelbrot

ApplicationWindow {
    id: window
    width: minimumWidth
    height: minimumHeight
    minimumWidth: 640
    minimumHeight: 450
    visible: true
    title: qsTr("Mandelbrot Explorer")

    Backend {
        id: backend
        viewportWidth: viewport.width
        viewportHeight: viewport.height
        centerReal: -0.5
        centerImag: 0
        zoom: 200
        maxIterations: 500
    }

    Image {
        id: viewport
        anchors.centerIn: parent
        anchors.fill: parent
        asynchronous: true
        source: backend.source
        fillMode: Image.PreserveAspectFit
    }
}
