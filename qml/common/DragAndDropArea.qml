import QtQuick 2.2

Rectangle {
        property color defaultColor: "#EEE"
        property color highlightColor: "#FCC"
        color: defaultColor
        DropArea {
                anchors.fill: parent
                keys: ["text/uri-list"]
                onEntered: {
                        item.color = highlightColor
                }
                onExited: {
                        item.color = defaultColor
                }
                onDropped: {
                        item.color = defaultColor
                        if (drop.hasUrls) {
                                if (drop.proposedAction == Qt.MoveAction || drop.proposedAction == Qt.CopyAction) {
                                        drop.acceptProposedAction()
                                        instance.fileDropped(drop.urls)
                                }
                        }
                }
        }
}
