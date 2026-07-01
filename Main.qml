import QtQuick
import QtQuick.Layouts
import QtQuick.Controls.Basic
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
                anchors.fill: scaleImageOutputSize.checked ? parent : null
                id: viewport
                width: backend.viewportWidth
                height: backend.viewportHeight
                asynchronous: true
                source: backend.source
                fillMode: Image.PreserveAspectFit

                onWidthChanged: {
                    if (scaleImageOutputSize.checked) {
                        backend.viewportWidth = width
                    }
                }

                onHeightChanged: {
                    if (scaleImageOutputSize.checked) {
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
                    id: scaleImageOutputSize
                    text: "Scale image output size"
                }

                GridLayout {
                    columns: 2
                    columnSpacing: 8

                    TextField {
                        enabled: !scaleImageOutputSize.checked
                        Layout.fillWidth: true
                        Layout.column: 0
                        Layout.row: 0
                        text: backend.viewportWidth

                        validator: IntValidator {
                            bottom: 50
                            top: 10000
                        }

                        onAcceptableInputChanged: {
                            backend.viewportWidth = parseInt(text)
                        }
                    }
                    TextField {
                        enabled: !scaleImageOutputSize.checked
                        Layout.fillWidth: true
                        Layout.column: 1
                        Layout.row: 0
                        text: backend.viewportHeight

                        validator: IntValidator {
                            bottom: 50
                            top: 10000
                        }

                        onAcceptableInputChanged: {
                            backend.viewportHeight = parseInt(text)
                        }
                    }
                }

                Label {
                    text: "Max iterations"
                    Layout.topMargin: 20
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
                        text: maxIterationsSlider.value
                        validator: IntValidator {
                            bottom: maxIterationsSlider.from
                            top: maxIterationsSlider.to
                        }
                        inputMethodHints: Qt.ImhDigitsOnly
                        onTextChanged: {
                            maxIterationsSlider.value = parseInt(text)
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
                        stepSize: 0.01
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
                        stepSize: 0.01
                        editable: true
                        decimals: 9

                        onValueChanged: backend.centerImag = value
                    }
                }


                Item {
                    height: 20
                }

                GridLayout {
                    columns: 2
                    Layout.fillWidth: true
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
                        stepSize: 100
                        editable: true
                        decimals: 0

                        onValueChanged: backend.zoom = value
                    }
                }

                Item {
                    Layout.fillHeight: true
                }


                Button {
                    Layout.fillWidth: true
                    text: "Reset parameters"
                    onClicked: {
                        scaleImageOutputSize.checked = false
                        backend.reset()
                    }
                }

            }
        }
    }
}
