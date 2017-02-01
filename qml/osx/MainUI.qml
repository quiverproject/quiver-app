import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtQuick.Layouts 1.1
import QtQuick.Dialogs 1.2
import QtQuick.Window 2.1

import common 1.0
import com.bscmdesign.qmlcomponents 1.0

DragAndDropArea { id: root
//    property alias backgroundColor: blur.backgroundColor

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
    FileDialog { id: fileDialog
        nameFilters: [ "pro files (*.pro)" ]
        onAccepted: instance.addProjectWithUrl(fileUrl)
    }
    MacWindowBlur { id: blur
        window: main
        radius: 10
        backgroundColor: "#a0f7fa"
    }

//    MainToolBar{}

        Rectangle { id: toolBar
        width: parent.width
        height: 44

        anchors.top: parent.top
        //        anchors.topMargin: 1

        color: "#2b2b2b"

        CustomBorder {
            commonBorder: false
            lBorderwidth: 0
            rBorderwidth: 0
            tBorderwidth: 1
            bBorderwidth: 1
            borderColor: "#9D9E9B"
        }

        Row {
            Layout.fillWidth: true
            Layout.fillHeight: true
            height: parent.height
            spacing: 1
            Button {
                text: "Add New Qt Project"
                //            onClicked: instance.addProjectWithUrl(fileUrl)
                action: addProjectAction
                height: parent.height
                style: ButtonStyle {
                    background: Rectangle {
                        //                        width: 480
                    }
                }
            }

            Button { id: showButton
                text: terminalWindow.visible ? "Hide Terminal" : "Show Terminal"
                height: parent.height
                style: ButtonStyle {
                    background: Rectangle {
                        //                        width: 480
                    }
                }
                onClicked: terminalWindow.visible = !terminalWindow.visible
            }

            Button { id: testFlags
                text: "toggle flags"
                height: parent.height
                style: ButtonStyle {
                    background: Rectangle {
                        //                        width: 480
                    }
                }
                onClicked: main.flags ^= Qt.FramelessWindowHint|Qt.NoDropShadowWindowHint
            }
        }

    }


    ListView { id: projectList
        width: parent.width
        height: parent.height


        anchors.top: toolBar.bottom
        anchors.topMargin: 1
//        currentIndex:

        model: instance.projects
        delegate: projectListItemDelegate

        Connections {
            target: instance
            onProjectsChanged: console.log("Projects list changed!")

            onProcessOutput: {
                    console.log(text);
                    text = text.replace(/^(.*?) ([^\s]+) \(([^)]*)\)/, '<font color="green">$1</font> <font color="cyan">$2</font> <font color="red">($3)</font>'); //project name, platform, and device profile in parens
                    text = text.replace(/\[(.+? \d{4})\]/, '<font color="#fff">[$1]</font>'); //timestamp in square brackets
                    text = text.replace(/\b(file:\/\/(\/[^:]+:\d+:\d+)):/, '<a href="$2"><font color="#558EFC">$1</font></a>:'); //make file:/// links active to open in qt creator
                    quiverTerminal.append(text);
            }
        }



        // FIXME: Insert animation declarations here
        //        add: Transition {
        //            from: "fromState"
        //            to: "toState"

        //        }
        //        addDisplaced: Transition {
        //            from: "fromState"
        //            to: "toState"

        //        }
        //        removeDisplaced: Transition {
        //            from: "fromState"
        //            to: "toState"

        //        }
        //        remove: Transition {
        //            from: "fromState"
        //            to: "toState"

        //        }

        highlight:
            Rectangle {
            color: "#F5F5F5"
            //            gradient: Gradient {
            //                    GradientStop { position: 0.0; color: "#126fdc" }
            //                    GradientStop { position: 1.0; color: "#3b91fe" }
            //                }
        }
        highlightFollowsCurrentItem: false
//        highlightMoveDuration: 1000

        focus: true
        clip: true

        spacing: 1




        Item {
            Component { id: projectListItemDelegate
                Rectangle { id: projectItem
                    width: parent.width
                    height: 480

                    //                    color: "white"
                    color: ListView.isCurrentItem ? "#0C71FE" : "#252525"

                    CustomBorder {
                        commonBorder: false
                        lBorderwidth: 0
                        rBorderwidth: 0
                        tBorderwidth: 0
                        bBorderwidth: 1
                        borderColor: "#9D9E9B"
                    }

                    MouseArea { id: projectMouse
                        anchors.fill: parent
                        hoverEnabled: true
                        //                                onClicked: {
                        //                                        parent.color = "#116CD6"
                        //                                } // FIXME: Implement behavior and connect data for selected Quiver projects
                        onClicked: {
                            projectList.forceActiveFocus()
                            projectList.currentIndex = index
                            console.log("a list item has been clicked")

                        }
                    }

                    ColumnLayout { id: projectColumn
                        anchors.top: parent.top
                        anchors.topMargin: 12
                        anchors.left: parent.left
                        anchors.leftMargin: 14

                        RowLayout {
                            Layout.fillWidth: true

                            ColumnLayout {
                                Layout.fillHeight: true
                                Text { id: projectName
                                    //                                anchors.top: parent.top
                                    //                                anchors.leftMargin: 22
                                    //                                anchors.rightMargin: 22
                                    text: modelData.name
                                    font {
                                        family: "SF UI Display"
                                        styleName: "Regular"
                                        pixelSize: 44
                                    }
                                    color: "white"
                                }
                                Text { id: projectPath
                                    text: "/Users/username/project/QuiverProject/"
                                    color: "#AAA"
                                    font {
                                        family: "SF UI Display"
                                        styleName: "Medium"
                                        pixelSize: 12
                                    }
                                }
                            }

                            Button {
                                text: "Launch"
                                onClicked: instance.launch(modelData.id)
                                style: ButtonStyle {
                                    background: Rectangle {
                                        width: 75
                                        height: 12
                                        color: control.pressed ? "#616161" : "black"
                                        radius: 4
                                    }
                                    label: Text {
                                        color: "white"
                                        text: "Launch"
                                    }
                                }
                            }

                        }

                        RowLayout {
                            Layout.fillWidth: true

                            Button {
                                text: "Deploy"
                                onClicked: instance.deploy(modelData.id)
                            }
                            Button {
                                text: "Delete"
                                onClicked: instance.remove(modelData.id)
                            }
                        }
                    }

                    RowLayout {
                        anchors.top: projectColumn.bottom
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
                                            color: "white"
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




    }




    property var terminalWindow: Window {
        width: 650
        height: 437
        //        color: "#215400"
        title: "Quiver Terminal"
        flags: Qt.Window | Qt.WindowFullscreenButtonHint



//        Flickable {
////            width: parent.width
////            height: parent.height
//            contentWidth: contentItem.childrenRect.width
//            contentHeight: contentItem.childrenRect.height
//            anchors.fill: parent



//        FocusScope { id: rootScope // from http://stackoverflow.com/questions/26170972/textedit-line-background-color (GG 20161216)
//            width: parent.width
//            height: parent.height

        Rectangle {
            width: parent.width
            height: parent.height
//            anchors.fill: parent

            color: "#282C34"

            Rectangle {
                height: quiverTerminal.cursorRectangle.height
                width: parent.width
                visible: quiverTerminal.focus
                y: quiverTerminal.cursorRectangle.y
                color: "#2C323C"
            }

            TextArea { id: quiverTerminal
                width: parent.width
                height: parent.height

                anchors.fill: parent
//                anchors.margins: -4


                focus: true

                onLinkActivated: instance.openInQtCreator(link)

//                text: "Quiver Output Output Output Output Output Output Output Output Output Output Output Output Output Output Output \n Quiver Output \n Quiver Output \n Quiver Output \n Quiver Output \n Quiver Output \n Quiver Output \n Quiver Output \n Quiver Output \n Quiver Output \n Quiver Output \n Quiver Output \n Quiver Output \n Quiver Output \n Quiver Output \n Quiver Output \n Quiver Output \n Quiver Output \n Quiver Output \n Quiver Output \n Quiver Output \n Quiver Output \n Quiver Output \n Quiver Output \n Quiver Output \n Quiver Output \n "

                //
                // FIXME: Use regex to highlight all terminal text
                //
                //    /(\[(.*?)\])/g    works to match every instance of date/time stamp in square brackets
                //
                //    /\(([^\)]+)\)/g    works to match every instance of device profile in parens
                //
                //

                readOnly: true
                selectByMouse: true
//                persistentSelection: true

                font {
                    family: "Menlo"
                    styleName: "Regular"
                    pixelSize: 18
                }
//                color: "#DE6D77"
//                selectionColor: "#3E4450"
//                selectedTextColor: color
//                backgroundVisible: false
                //                textColor: "white"
                textFormat: TextEdit.RichText
                textMargin: 0
                wrapMode: TextEdit.Wrap
                frameVisible: false
                style: TextAreaStyle {
                    renderType: Text.NativeRendering
                    textColor: "#DE6D77"
                    selectionColor: "#3E4450"
                    selectedTextColor: textColor
                    backgroundColor: "#2B2B2B"
                    padding {
                        top: 0
                        right: 0
                        bottom: 0
                        left: 0
                    }
                    // ScrollViewStyle properties
                    transientScrollBars: true
                    scrollBarBackground: Rectangle { id: scrollBackground
                        implicitWidth: 11
//                        anchors.top:parent.top
                        //                                implicitHeight: 11
                        color: styleData.hovered ? Qt.rgba(255,255,255,0.25) : "transparent"
                    }
                    handle: Rectangle {
                        implicitWidth: 9
                        implicitHeight: 10
                        anchors.fill: parent
                        anchors.topMargin: 2
                        anchors.leftMargin: 2
                        anchors.bottomMargin: 4
                        anchors.horizontalCenter: scrollBackground.horizontalCenter
                        color: "white"
                        opacity: styleData.hovered ? 0.5 : 0.25
                        radius: 4
                    }
                    //                    frame: Rectangle { color: "transparent"}
                    corner: Rectangle {}
                    decrementControl: Item {}
                    incrementControl: Item {}
                    frame: Rectangle {
                        implicitWidth: 10
                        border.width: 0
                        border.color: "pink"
                    }

                }
            }


//            MouseArea {
//                anchors.fill: parent
//                //                onClicked: testWindow.color = "#e0c31e"
//            }
            //            Button {
            //                anchors.right: parent.right
            //                anchors.top: parent.top
            //                anchors.margins: defaultSpacing
            //                text: testWindow.visibility === Window.FullScreen ? "exit fullscreen" : "go fullscreen"
            //                width: 150
            //                onClicked: {
            //                    if (testWindow.visibility === Window.FullScreen)
            //                        testWindow.visibility = Window.AutomaticVisibility
            //                    else
            //                        testWindow.visibility = Window.FullScreen
            //                }
            //            }
            //            Button {
            //                anchors.left: parent.left
            //                anchors.top: parent.top
            //                anchors.margins: defaultSpacing
            //                text: "X"
            //                width: 30
            //                onClicked: testWindow.visible = false
            //            }



//            ColumnLayout {
//                anchors.fill: parent
//                anchors.margins: 9
////                RowLayout {
////                    Layout.fillWidth: true
////                    TextField {
////                        id: input
////                        Layout.fillWidth: true
////                        focus: true
////                        onAccepted: {
////                            // call our evaluation function on root
////                            root.jsCall(input.text)
////                        }
////                    }
////                    //                      Button {
////                    //                          text: qsTr("Send")
////                    //                          onClicked: {
////                    //                              // call our evaluation function on root
////                    //                              root.jsCall(input.text)
////                    //                          }
////                    //                      }
////                }
//                Item {
//                    Layout.fillWidth: true
//                    Layout.fillHeight: true
//                    Rectangle {
//                        anchors.fill: parent
//                        color: '#333'
//                        border.color: Qt.darker(color)
//                        opacity: 0.2
//                        radius: 2
//                    }

//                    ScrollView {
//                        id: scrollView
//                        anchors.fill: parent
//                        anchors.margins: 9
//                        ListView {
//                            id: resultView
//                            model: ListModel {
//                                id: outputModel
//                            }
//                            delegate: ColumnLayout {
//                                width: ListView.view.width
//                                Label {
//                                    Layout.fillWidth: true
//                                    color: 'green'
//                                    text: "> " + model.expression
//                                }
//                                Label {
//                                    Layout.fillWidth: true
//                                    color: 'blue'
//                                    text: "" + model.result
//                                }
//                                Rectangle {
//                                    height: 1
//                                    Layout.fillWidth: true
//                                    color: '#333'
//                                    opacity: 0.2
//                                }
//                            }
//                        }
//                    }
//                }
//            }


        }
//    }
//    }

    }

}
