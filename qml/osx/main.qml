import QtQuick 2.2
import QtQuick.Controls 1.1

ApplicationWindow { id: main
	Item { id: env
property var type: "mobile"
 property var os: "ios"
 property var name: "iPhone 6 Plus"
 property var mode: "dedicated"
 property var widthPoints: 414
 property var heightPoints: 736
 property var widthPixels: 1241
 property var heightPixels: 2208
 property var widthDevicePixels: 1080
 property var heightDevicePixels: 1920
 property var pixelDensity: 401

	}
        property string sourceFileName: "MainUI.qml"
        property string menuFileName: "MainMenu.qml"

        visible: true
        
        menuBar: mainMenuLoader.item

        width: 1024
        height: 768

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
}
