import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import org.kde.kirigami as Kirigami
import org.manjaro.mcp.components as Components
import org.kde.kcmutils as KCMUtils

KCMUtils.SimpleKCM {

    id: root

    property var vm: kcm.vm
    property string busTypeFilter: "all"

    actions: [
        Kirigami.Action {
            text: qsTr("All Devices")
            icon.name: "computer"
            checkable: true
            checked: root.busTypeFilter === "all"
            onTriggered: root.busTypeFilter = "all"
        },
        Kirigami.Action {
            text: qsTr("PCI Devices")
            icon.name: "expansion-card"
            checkable: true
            checked: root.busTypeFilter === "pci"
            onTriggered: root.busTypeFilter = "pci"
        },
        Kirigami.Action {
            text: qsTr("USB Devices")
            icon.name: "drive-removable-media-usb"
            checkable: true
            checked: root.busTypeFilter === "usb"
            onTriggered: root.busTypeFilter = "usb"
        }
    ]

    View {
        anchors.fill: parent
        viewModel: root.vm
        busTypeFilter: root.busTypeFilter
    }
}
