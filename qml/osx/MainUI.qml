import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtQuick.Layouts 1.1
import QtQuick.Dialogs 1.2

import common 1.0
import com.bscmdesign.qmlcomponents 1.0

DragAndDropArea { id: root
    property alias backgroundColor: blur.backgroundColor

    anchors.fill: parent


    Component.onCompleted: {
        main.width = 820;
        main.height = 415;
        main.title = "Quiver";
        main.color = "transparent";
    }
    Action { id: addProjectAction
        onTriggered: fileDialog.open()
        shortcut: StandardKey.Open
    }
    FileDialog {
        id: fileDialog
        nameFilters: [ "pro files (*.pro)" ]
        onAccepted: instance.addProjectWithUrl(fileUrl)
    }


    Rectangle{ id: toolBar
        width: parent.width
        height: 44
        color: "#111"
        Button {
            Layout.fillWidth: true
            anchors.verticalCenter: parent.verticalCenter
            text: "Add New Qt Project"
//            onClicked: instance.addProjectWithUrl(fileUrl)
            action: addProjectAction
            height: parent.height
            style: ButtonStyle {
                background: Rectangle {
                    width: 480
                }

            }
        }

    }

    MacWindowBlur {
        id: blur
        window: main
        radius: 10
        backgroundColor: "#a0f7fafc"
    }

//    Rectangle { id: tools
//        width: parent.width
//        height: 24
//        color: "red"
//    }

    ListView { id: projectList
        model: instance.projects
        Connections {
            target: instance
            onProjectsChanged: console.log("Projects list changed!")

            //GREG: change this to use whatever console QML item you devise
            onProcessOutput: console.log(text)
        }
        delegate: projectListItemDelegate
        anchors.top: toolBar.bottom
        //anchors.bottom: parent.bottom
        width: parent.width
        height: parent.height
        clip: true
        // FIXME: Insert animation declarations here
        // add: Transition { }
        // removeDisplaced: Transition { }
        // remove: Transition { }
        highlight:
            Rectangle {
            color: "red"
        }
        highlightFollowsCurrentItem: true
        focus: true

    }

    Component { id: projectListItemDelegate
        Rectangle { id: projectItem
            width: parent.width
            height: 300

            color: "transparent"
            //                        color: ListView.isCurrentItem ? "black" : "red"

            MouseArea { id: projectMouse
                anchors.fill: parent
                hoverEnabled: true
                //                                onClicked: {
                //                                        parent.color = "#116CD6"
                //                                } // FIXME: Implement behavior and connect data for selected Quiver projects
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
                text: "Deploy"
                onClicked: instance.deploy(modelData.id)
            }
            Button {
                x: 450
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
