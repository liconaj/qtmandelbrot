import QtQuick
import QtQuick.Layouts
import QtQuick.Controls.Fusion
import com.liconaj.qtmandelbrot

ApplicationWindow {
    id: window
    minimumWidth: 940
    minimumHeight: 480
    width: minimumWidth
    height: minimumHeight
    visible: true
    title: qsTr("Mandelbrot Set Explorer")

    QtObject {
       id: defaultParameters
       readonly property int imageWidth: 800
       readonly property int imageHeight: 600
       readonly property double centerRe: -0.5
       readonly property double centerIm: 0
       readonly property double zoom: 100
       readonly property int maxIterations: 100
    }

    RowLayout {
        anchors.fill: parent
        spacing: 0

        Pane {
            Layout.fillWidth: true
            Layout.fillHeight: true
            padding: 0
            background: Rectangle {
                color: "#222222"
            }

            Mandelbrot {
                id: mandelbrot
                visible: true
                anchors.centerIn: parent
                anchors.fill: parent
                imageWidth: defaultParameters.imageWidth
                imageHeight: defaultParameters.imageHeight
                zoom: defaultParameters.zoom
                maxIterations: defaultParameters.maxIterations
                centerRe: defaultParameters.centerRe
                centerIm: defaultParameters.centerIm

                function reset() {
                    imageWidth = defaultParameters.imageWidth
                    imageHeight = defaultParameters.imageHeight
                    zoom = defaultParameters.zoom
                    maxIterations = defaultParameters.maxIterations
                    centerRe = defaultParameters.centerRe
                    centerIm = defaultParameters.centerIm
                }
            }
        }

        Pane {
            padding: 28
            Layout.preferredWidth: 300
            Layout.fillHeight: true

            focusPolicy: Qt.ClickFocus

            ColumnLayout {
                anchors.fill: parent

                Label {
                    text: "View render size"
                }

                RowLayout {
                    spacing: 4

                    Label {
                        text: "Width: "
                    }

                    TextField {
                        Layout.fillWidth: true
                        text: mandelbrot.imageWidth
                        horizontalAlignment: Qt.AlignHCenter
                        rightPadding: trailingElement1.width + 12

                        validator: IntValidator {
                            bottom: 50
                            top: 10000
                        }

                        onEditingFinished: {
                            mandelbrot.imageWidth = parseInt(text)
                        }

                        Label {
                            id: trailingElement1
                            anchors.verticalCenter: parent.verticalCenter
                            anchors.rightMargin: 8
                            anchors.right: parent.right
                            opacity: 0.5
                            text: "px"
                        }
                    }

                    Item {
                        Layout.preferredWidth: 16
                    }

                    Label {
                        text: "Height: "
                    }

                    TextField {
                        Layout.fillWidth: true
                        text: mandelbrot.imageHeight
                        horizontalAlignment: Qt.AlignHCenter
                        rightPadding: trailingElement2.width + 12

                        validator: IntValidator {
                            bottom: 50
                            top: 10000
                        }

                        onEditingFinished: {
                            mandelbrot.imageHeight = parseInt(text)
                        }

                        Label {
                            id: trailingElement2
                            anchors.verticalCenter: parent.verticalCenter
                            anchors.rightMargin: 8
                            anchors.right: parent.right
                            opacity: 0.5
                            text: "px"
                        }
                    }
                }

                Label {
                    Layout.topMargin: 20
                    text: "Max iterations"
                }

                RowLayout {
                    Slider {
                        id: maxIterationsSlider

                        Layout.fillWidth: true

                        from: 1
                        value: mandelbrot.maxIterations
                        to: 1500
                        stepSize: 1

                        onMoved: mandelbrot.maxIterations = value
                    }

                    TextField {
                        Layout.preferredWidth: 48
                        Layout.fillWidth: false
                        text: mandelbrot.maxIterations
                        validator: IntValidator {
                            bottom: maxIterationsSlider.from
                            top: maxIterationsSlider.to
                        }
                        inputMethodHints: Qt.ImhDigitsOnly
                        onEditingFinished: {
                            if (text) {
                                mandelbrot.maxIterations = parseInt(text)
                            }
                        }
                    }
                }

                Label {
                    text: qsTr("Coordinates at center")
                    Layout.topMargin: 20
                    Layout.bottomMargin: 8
                }

                GridLayout {
                    columns: 2

                    Label {
                        Layout.column: 0
                        Layout.row: 0
                        Layout.fillWidth: true
                        text: "Re: "
                    }
                    DoubleSpinBox {
                        id: centerRealSpinBox

                        Layout.preferredWidth: 200
                        Layout.column: 1
                        Layout.row: 0

                        locale: Qt.locale("es_US")
                        from: -5.0
                        value: mandelbrot.centerRe
                        to: 5.0
                        stepSize: 10 / zoomSpinBox.value
                        editable: true
                        decimals: 9

                        onValueModified: {
                            mandelbrot.centerRe = value
                        }
                    }

                    Label {
                        Layout.fillWidth: true
                        Layout.column: 0
                        Layout.row: 1
                        text: "Im: "
                    }
                    DoubleSpinBox {
                        id: centerImagSpinBox

                        Layout.column: 1
                        Layout.row: 1
                        Layout.preferredWidth: 200

                        locale: Qt.locale("es_US")
                        from: -5.0
                        value: mandelbrot.centerIm
                        to: 5.0
                        stepSize: 10 / zoomSpinBox.value
                        editable: true
                        decimals: 9

                        onValueModified: {
                            mandelbrot.centerIm = value
                        }
                    }
                }

                GridLayout {
                    columns: 2
                    Layout.fillWidth: true
                    Layout.topMargin: 20
                    Label {
                        Layout.fillWidth: true
                        text: "Zoom: "
                    }
                    DoubleSpinBox {
                        id: zoomSpinBox

                        Layout.preferredWidth: 200

                        locale: Qt.locale("es_US")
                        from: 100
                        value: mandelbrot.zoom
                        to: 2.0e15
                        stepSize: 100
                        editable: true
                        decimals: 0

                        onValueModified: {
                            mandelbrot.zoom = value
                        }
                    }
                }

                Item {
                    Layout.fillHeight: true
                }


                Button {
                    Layout.fillWidth: true
                    text: "Reset parameters"
                    onClicked: mandelbrot.reset()
                }
            }
        }
    }
}
