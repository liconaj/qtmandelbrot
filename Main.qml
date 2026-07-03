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
    title: qsTr("Mandelbrot Explorer")

    QtObject {
       id: parameters
       property int renderWidth
       property int renderHeight
       property double zoom
       property double centerRe
       property double centerIm
       property int maxIterations

       Component.onCompleted: reset()

       function reset() {
           zoom = 100;
           renderWidth = 640;
           renderHeight = 450;
           centerRe = -0.5;
           centerIm = 0;
           maxIterations = 100;
       }
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

            MandelbrotRenderer {
                id: renderer
                visible: true
                anchors.centerIn: parent
                anchors.fill: parent
                renderWidth: parameters.renderWidth
                renderHeight: parameters.renderHeight
                zoom: parameters.zoom
                maxIterations: parameters.maxIterations
                centerRe: parameters.centerRe
                centerIm: parameters.centerIm
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
                        text: parameters.renderWidth
                        horizontalAlignment: Qt.AlignHCenter
                        rightPadding: trailingElement1.width + 12

                        validator: IntValidator {
                            bottom: 50
                            top: 10000
                        }

                        onEditingFinished: {
                            parameters.renderWidth = parseInt(text)
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
                        text: parameters.renderHeight
                        horizontalAlignment: Qt.AlignHCenter
                        rightPadding: trailingElement2.width + 12

                        validator: IntValidator {
                            bottom: 50
                            top: 10000
                        }

                        onEditingFinished: {
                            parameters.renderHeight = parseInt(text)
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
                        value: parameters.maxIterations
                        to: 1500
                        stepSize: 1

                        onMoved: parameters.maxIterations = value
                    }

                    TextField {
                        Layout.preferredWidth: 48
                        Layout.fillWidth: false
                        text: parameters.maxIterations
                        validator: IntValidator {
                            bottom: maxIterationsSlider.from
                            top: maxIterationsSlider.to
                        }
                        inputMethodHints: Qt.ImhDigitsOnly
                        onEditingFinished: {
                            if (text) {
                                parameters.maxIterations = parseInt(text)
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
                        value: parameters.centerRe
                        to: 5.0
                        stepSize: 10 / zoomSpinBox.value
                        editable: true
                        decimals: 9

                        onValueModified: {
                            parameters.centerRe = value
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
                        value: parameters.centerIm
                        to: 5.0
                        stepSize: 10 / zoomSpinBox.value
                        editable: true
                        decimals: 9

                        onValueModified: {
                            parameters.centerIm = value
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
                        value: parameters.zoom
                        to: 2.0e15
                        stepSize: 100
                        editable: true
                        decimals: 0

                        onValueModified: {
                            parameters.zoom = value
                        }
                    }
                }

                Item {
                    Layout.fillHeight: true
                }


                Button {
                    Layout.fillWidth: true
                    text: "Reset parameters"
                    onClicked: parameters.reset()
                }
            }
        }
    }
}
