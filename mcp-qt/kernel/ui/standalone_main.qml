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
}
