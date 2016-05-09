import QtQuick 2.5
import QtQuick.Window 2.0
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.4

ApplicationWindow { id: main

    color: "transparent"
        Item { id: env
//            color: "transparent"

        }
        property string sourceFileName: "MainUI.qml"
        property string menuFileName: "MainMenu.qml"
        property string toolBarFileName: "MainToolBar.qml"

        visible: true

        menuBar: mainMenuLoader.item
        toolBar: toolBarLoader.item

        Connections {
                target: quiver
                onPendingConnectionRequestChanged: {
                        mainLoader.source = ""
                        mainLoader.source = sourceFileName
                }
        }

        Loader { id: mainLoader
                source: sourceFileName
                anchors.fill: parent
                focus: true
        }

        Loader { id: mainMenuLoader
                source: menuFileName
        }

//        Loader { id: toolBarLoader
//                source: toolBarFileName
//        }

        Shortcut {
            sequence: StandardKey.Close
            context: Qt.ApplicationShortcut
//            onActivated: main.visible = !main.visible
            onActivated: main.hide()
        }
}
