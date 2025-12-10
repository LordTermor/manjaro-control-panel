pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls as QQC2
import QtQuick.Layouts
import org.kde.kirigami as Kirigami
import org.manjaro.mcp.components

Kirigami.AbstractCard {
    id: root

    // Required interface
    required property var driverData
    required property string deviceId
    required property bool isInstalling

    signal installClicked(string deviceId, string driverId)
    signal removeClicked(string deviceId, string driverId)

    

    contentItem: RowLayout {
        id: driverContent

       
        QQC2.Label {
            text: root.driverData?.name ?? ""
            font.weight: Font.Medium
        }

        Badge {
            visible: !!root.driverData?.recommended
            
            text: qsTr("Recommended")
            badgeColor: Kirigami.Theme.positiveTextColor
        }

       
        Item {
            Layout.fillWidth: true

            
        }

         Badge {
            text: !!root.driverData?.openSource ? qsTr("Open Source") : qsTr("Proprietary")
            badgeColor: !!root.driverData?.openSource ? "#3498db" : "#e97517"
        }
         Kirigami.Icon {
            source: "qrc:/mhwd/assets/nvidia-svgrepo-com.svg"
            color: Kirigami.Theme.textColor
            isMask: true
            visible: root.driverData?.name.includes("nvidia")

            Layout.preferredWidth: 32
            Layout.preferredHeight: 32
        }

        Kirigami.Icon {
            source: "qrc:/mhwd/assets/amd-svgrepo-com.svg"
            color: Kirigami.Theme.textColor
            isMask: true
            visible: root.driverData?.name.includes("amd")

            Layout.preferredWidth: 32
            Layout.preferredHeight: 32
        }

        Kirigami.Icon {
            source: "qrc:/mhwd/assets/intel-svgrepo-com.svg"
            color: Kirigami.Theme.textColor
            isMask: true
            visible: root.driverData?.name.includes("intel")

            Layout.preferredWidth: 32
            Layout.preferredHeight: 32
        }


        Item {
            Layout.preferredWidth: Kirigami.Units.largeSpacing

            
        }

        QQC2.Button {
            visible: !root.driverData?.installed
            enabled: !root.isInstalling
            
            icon.name: root.isInstalling ? "view-refresh" : "download"
            icon.color: Kirigami.Theme.positiveTextColor
            display: QQC2.AbstractButton.IconOnly
            
            QQC2.ToolTip.visible: hovered
            QQC2.ToolTip.text: root.isInstalling ? qsTr("Installing...") : qsTr("Install")
            
            onClicked: {
                root.installClicked(root.deviceId, root.driverData.id);
            }
        }
    }
}
