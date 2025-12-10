import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import org.kde.kirigami as Kirigami
import org.manjaro.mcp.components as Components
import org.kde.kcmutils as KCMUtils

KCMUtils.SimpleKCM {

    id: root

    property var vm: kcm.vm

    View {
        anchors.fill: parent
    }
}
