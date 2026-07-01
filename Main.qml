import QtQuick
import QtQuick.Layouts
import QtQuick.Controls.Fusion
import com.liconaj.qtmandelbrot

ApplicationWindow {
    id: window
    minimumWidth: 1100
    minimumHeight: 600
    width: minimumWidth
    height: minimumHeight
    visible: true
    title: qsTr("Mandelbrot Explorer")

    Backend {
        id: backend
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

            Image {
                anchors.centerIn: parent
                anchors.fill: autoViewportSize.checked ? parent : null
                id: viewport
                width: backend.viewportWidth
                height: backend.viewportHeight
                asynchronous: true
                source: backend.source
                fillMode: Image.PreserveAspectFit

                onWidthChanged: {
                    if (autoViewportSize.checked) {
                        backend.viewportWidth = width
                    }
                }

                onHeightChanged: {
                    if (autoViewportSize.checked) {
                        backend.viewportHeight = height
                    }
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

                CheckBox {
                    id: autoViewportSize
                    text: "Auto viewport size"
                }

                RowLayout {
                    spacing: 4

                    Label {
                        text: "W: "
                    }

                    TextField {
                        enabled: !autoViewportSize.checked
                        Layout.fillWidth: true
                        text: backend.viewportWidth

                        validator: IntValidator {
                            bottom: 50
                            top: 10000
                        }

                        onEditingFinished: {
                            backend.viewportWidth = parseInt(text)
                        }
                    }

                    Item {
                        Layout.preferredWidth: 16
                    }

                    Label {
                        text: "H: "
                    }

                    TextField {
                        enabled: !autoViewportSize.checked
                        Layout.fillWidth: true
                        text: backend.viewportHeight

                        validator: IntValidator {
                            bottom: 50
                            top: 10000
                        }

                        onEditingFinished: {
                            backend.viewportHeight = parseInt(text)
                        }
                    }
                }

                Rectangle {
                    color: "#555555"
                    Layout.topMargin: 32
                    Layout.bottomMargin: 32
                    Layout.fillWidth: true
                    Layout.preferredHeight: 1
                }


                Label {
                    text: "Max iterations"
                }

                RowLayout {
                    Slider {
                        id: maxIterationsSlider

                        Layout.fillWidth: true

                        from: 1
                        value: backend.maxIterations
                        to: 1500
                        stepSize: 1

                        onValueChanged: backend.maxIterations = value
                    }

                    TextField {
                        Layout.preferredWidth: 48
                        Layout.fillWidth: false
                        text: backend.maxIterations
                        validator: IntValidator {
                            bottom: maxIterationsSlider.from
                            top: maxIterationsSlider.to
                        }
                        inputMethodHints: Qt.ImhDigitsOnly
                        onEditingFinished: {
                            if (text) {
                                backend.maxIterations = parseInt(text)
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
                        value: backend.centerReal
                        to: 5.0
                        stepSize: 10 / zoomSpinBox.value
                        editable: true
                        decimals: 9

                        onValueChanged: backend.centerReal = value
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
                        value: backend.centerImag
                        to: 5.0
                        stepSize: 10 / zoomSpinBox.value
                        editable: true
                        decimals: 9

                        onValueChanged: backend.centerImag = value
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
                        value: backend.zoom
                        to: 2.0e15
                        stepSize: value
                        editable: true
                        decimals: 0

                        onValueChanged: {
                            backend.zoom = value
                        }
                    }
                }

                Item {
                    Layout.fillHeight: true
                }


                Button {
                    Layout.fillWidth: true
                    text: "Reset parameters"
                    onClicked: {
                        autoViewportSize.checked = false
                        backend.reset()
                    }
                }

            }
        }
    }
}
