import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.1
import QtQuick.Dialogs 1.0

import BSComponents 1.0

BSToolBar {
        id: toolBar
        window: main
        titleHidden: true

        FileDialog {
                id: fileDialog
                nameFilters: [ "pro files (*.pro)" ]
                onAccepted: instance.addProjectWithUrl(fileUrl)
        }

        Action { id: addProjectAction
                onTriggered: fileDialog.open()
                shortcut: StandardKey.Open
        }

        RowLayout {
                BSToolButton { id: button_one
                        text: "Add New Qt Project"
                        tooltip: "Add New Qt Project"
                        iconSource: "../../../quiver-app/assets/goat-hey.png"
                        action: addProjectAction
                }

                BSToolButton { id: button_two
                        text: "Second Button"
                        tooltip: "Bleh2"
                        iconSource: "../../../quiver-app/assets/goat-hey.png"
                }
        }
}
