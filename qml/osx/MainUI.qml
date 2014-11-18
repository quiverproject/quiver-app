import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.1
import QtQuick.Layouts 1.1


    Rectangle { id: root
        anchors.fill: parent
        color: "red"

        Rectangle{ id: toolBar
            width: parent.width
            height: 44
            color: "#DDD"
            Button {
                Layout.fillWidth: true
                anchors.verticalCenter: parent.verticalCenter
                text: "Add New Qt Project"
            }
        }

        // FIXME: implement a model to feed Quiver-managed projects into the ListView
        // QuiverProjectModel { id: quiverProjectModel
        // }

        ListView { id: projectList
            model: instance.projects
            //model: 20 // FIXME: Remove when model is ready
            delegate: projectListItemDelegate
            anchors.top: toolBar.bottom
            anchors.bottom: parent.bottom
            width: parent.width
            clip: true
            // FIXME: Insert animation declarations here
            // add: Transition { }
            // removeDisplaced: Transition { }
            // remove: Transition { }
        }

        Component {id: projectListItemDelegate
            Rectangle { id: projectItem
                width: parent.width
                height: 88
                MouseArea { id: projectMouse
                    anchors.fill: parent
                    hoverEnabled: true
                    onClicked: {
                        parent.color = "blue"
                    } // FIXME: Implement behavior and connect data for selected Quiver projects
                }
                Text { id: projectName
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.leftMargin: 22
                    anchors.rightMargin: 22
                    text: modelData.name
                    font.pixelSize: 44
                    color: "#222"
                }
                Repeater {
                        model: modelData.platforms
                        anchors.verticalCenter: parent.verticalCenter
                        Text {
                                text: modelData.name
                        }
                }
            }
        }

    }
