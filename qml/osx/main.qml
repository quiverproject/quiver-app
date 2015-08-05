import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.4

import BSComponents 1.0

ApplicationWindow { id: main
    color: "transparent"

        Item { id: env

        }
        property string sourceFileName: "MainUI.qml"
        property string menuFileName: "MainMenu.qml"
        property string toolBarFileName: "MainToolBar.qml"

        property alias backgroundColor: blur.backgroundColor

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

        Loader { id: toolBarLoader
                source: toolBarFileName
        }

        MacWindowBlur {
            id: blur
            window: root
            radius: 25
            backgroundColor: "#a0f7fafc"
        }
}
