import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import org.kde.kirigami as Kirigami
import org.manjaro.mcp.components as Components
import org.kde.kcmutils as KCMUtils

KCMUtils.SimpleKCM {

    id: root

    property var vm: kcm.vm

    actions: [
        Kirigami.Action {
            text: qsTr("About Manjaro Control Panel")
            icon.name: "manjaro"
            onTriggered: kcm.push("About.qml")
        }
    ]

    View {
        anchors.fill: parent
    }
}
