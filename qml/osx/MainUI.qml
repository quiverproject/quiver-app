import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.1
import QtQuick.Layouts 1.1

import common 1.0

DragAndDropArea { id: root
        anchors.fill: parent

        Component.onCompleted: {
                main.width = 1024;
                main.height = 768;
                main.title = "Quiver";
        }

//        Rectangle{ id: toolBar
//                width: parent.width
//                height: 44
//                color: "#DDD"
//                Button {
//                        Layout.fillWidth: true
//                        anchors.verticalCenter: parent.verticalCenter
//                        text: "Add New Qt Project"
//                }
//        }

        ListView { id: projectList
                model: instance.projects
                Connections {
                        target: instance
                        onProjectsChanged: console.log("Projects list changed!")
                }
                delegate: projectListItemDelegate
                //anchors.top: toolBar.bottom
                //anchors.bottom: parent.bottom
                width: parent.width
                height: parent.height
                clip: true
                // FIXME: Insert animation declarations here
                // add: Transition { }
                // removeDisplaced: Transition { }
                // remove: Transition { }
        }

        Component { id: projectListItemDelegate
                Rectangle { id: projectItem
                        width: parent.width
                        height: 300
                        MouseArea { id: projectMouse
                                anchors.fill: parent
                                hoverEnabled: true
                                onClicked: {
                                        parent.color = "#116CD6"
                                } // FIXME: Implement behavior and connect data for selected Quiver projects
                        }
                        Text { id: projectName
                                anchors.top: parent.top
                                anchors.leftMargin: 22
                                anchors.rightMargin: 22
                                text: modelData.name
                                font.pixelSize: 44
                                color: "#222"
                        }
                        Button {
                                x: 250
                                text: "Launch"
                                onClicked: instance.launch(modelData.id)
                        }
                        Button {
                                x: 350
                                text: "Delete"
                                onClicked: instance.remove(modelData.id)
                        }

                        RowLayout {
                                anchors.top: projectName.bottom
                                Repeater {
                                        model: modelData.platforms
                                        ColumnLayout {
                                                CheckBox {
                                                        id: osCheckBox
                                                        checked: modelData.enabled
                                                        onClicked: modelData.setEnabled(checked)
                                                        style: CheckBoxStyle {
                                                                label: Text {
                                                                        text: modelData.name
                                                                        font.bold: true
                                                                }
                                                        }
                                                }
                                                ColumnLayout {
                                                        anchors.top: osCheckBox.bottom
                                                        Repeater {
                                                                model: modelData.configs
                                                                CheckBox {
                                                                        text: modelData.name
                                                                        checked: modelData.enabled
                                                                        onClicked: modelData.setEnabled(checked)
                                                                        enabled: osCheckBox.checked
                                                                }
                                                        }
                                                }
                                        }
                                }
                        }
                }
        }
}
