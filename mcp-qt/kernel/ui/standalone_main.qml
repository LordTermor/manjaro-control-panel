import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import org.kde.kirigami as Kirigami
import org.manjaro.mcp.components as Components

Kirigami.ApplicationWindow {
    id: root

    width: 800
    height: 600
    
    title: qsTr("MCP Kernel Manager")

    pageStack.initialPage: View {}

    footer: ToolBar {
        contentItem: RowLayout {
            Item {
                Layout.fillWidth: true
            }

            Button {
                text: qsTr("Manjaro Control Panel")
                icon.name: "help-about"
                flat: true

                onClicked: aboutWindow.show()
            }
        }
    }

    Kirigami.ApplicationWindow {
        id: aboutWindow

        width: 600
        height: 500
        title: qsTr("About MCP")

        pageStack.initialPage: About {}
    }
}
