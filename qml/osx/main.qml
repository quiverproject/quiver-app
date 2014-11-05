import QtQuick 2.2
import QtQuick.Controls 1.1

ApplicationWindow { id: main
	Item { id: env
property var type: "desktop"
 property var os: "osx"
 property var displayName: '27" Cinema Display'
 property var screenMode: "windowable" // or "dedicated" i.e. phone etc.
 property var widthReal: 650  // mm  -- FIX: these measurements are device size not display size.
 property var heightReal: 491 // mm
 property var widthTruePixels: 2560  // May be higher regular pixels for retina
 property var heightTruePixels: 1440
 property var widthPixels: 2560
 property var heightPixels: 1440
 property var scale: 1.0

	}
        property string sourceFileName: "MainUI.qml"
        property string menuFileName: "MainMenu.qml"

        visible: true
        
        menuBar: mainMenuLoader.item

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
