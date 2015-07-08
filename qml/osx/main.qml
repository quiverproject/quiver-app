import QtQuick 2.3
import QtQuick.Controls 1.2

ApplicationWindow { id: main
        Item { id: env

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

        Loader { id: toolBarLoader
                source: toolBarFileName
        }
}
