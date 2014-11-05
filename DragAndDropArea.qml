//This custom QML object will be for enabling drag and drop of directories
//into Quiver app.

//FIXME: Copied the code from DragAndDropTextItem.QML in externaldraganddrop QtQuick example
//FIXME: Customize to accept directories and eliminate unnecessary code from source example

import QtQuick 2.2

Rectangle {
    id: item
    property string display
    color: "#EEE"
    Text {
        anchors.fill: parent
        text: item.display
        wrapMode: Text.WordWrap
    }
    DropArea {
        anchors.fill: parent
        keys: ["text/plain"]
        onEntered: {
            item.color = "#FCC"
        }
        onExited: {
            item.color = "#EEE"
        }
        onDropped: {
            item.color = "#EEE"
            if (drop.hasText) {
                if (drop.proposedAction == Qt.MoveAction || drop.proposedAction == Qt.CopyAction) {
                    item.display = drop.text
                    drop.acceptProposedAction()
                }
            }
        }
    }
    MouseArea {
        id: mouseArea
        anchors.fill: parent
        drag.target: draggable
    }
    Item {
        id: draggable
        anchors.fill: parent
        Drag.active: mouseArea.drag.active
        Drag.hotSpot.x: 0
        Drag.hotSpot.y: 0
        Drag.mimeData: { "text/plain": item.display }
        Drag.dragType: Drag.Automatic
        Drag.onDragStarted: {
        }
        Drag.onDragFinished: {
            if (dropAction == Qt.MoveAction) {
                item.display = ""
            }
        }
    } // Item
}
