pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls as QQC2
import QtQuick.Layouts
import org.kde.kirigami as Kirigami

Rectangle {
    id: root

    // Required interface
    required property var driverData
    required property string deviceId
    required property bool isInstalling

    signal installClicked(string deviceId, string driverId)
    signal removeClicked(string deviceId, string driverId)

    Layout.fillWidth: true
    height: driverContent.implicitHeight + Kirigami.Units.largeSpacing * 2

    color: !!root.driverData?.recommended ? Kirigami.Theme.positiveBackgroundColor : Kirigami.Theme.backgroundColor
    border.color: !!root.driverData?.recommended ? Kirigami.Theme.positiveTextColor : Kirigami.Theme.alternateBackgroundColor
    border.width: 1
    radius: 4

    RowLayout {
        id: driverContent
        anchors.fill: parent
        anchors.margins: Kirigami.Units.largeSpacing
        spacing: Kirigami.Units.largeSpacing

        ColumnLayout {
            Layout.fillWidth: true
            spacing: Kirigami.Units.smallSpacing

            RowLayout {
                spacing: Kirigami.Units.largeSpacing

                QQC2.Label {
                    text: root.driverData?.name ?? ""
                    font.weight: Font.Medium
                }

                QQC2.Label {
                    visible: !!root.driverData?.recommended
                    text: qsTr("Recommended")
                    
                    color: Kirigami.Theme.positiveTextColor
                    font.pointSize: Kirigami.Theme.smallFont.pointSize
                    font.weight: Font.Medium
                    leftPadding: Kirigami.Units.smallSpacing
                    rightPadding: Kirigami.Units.smallSpacing
                    topPadding: Kirigami.Units.smallSpacing / 2
                    bottomPadding: Kirigami.Units.smallSpacing / 2

                    background: Rectangle {
                        color: Kirigami.Theme.positiveBackgroundColor
                        radius: 4
                    }
                }
            }

            RowLayout {
                spacing: Kirigami.Units.largeSpacing * 2

                QQC2.Label {
                    text: !!root.driverData?.openSource ? qsTr("Open Source") : qsTr("Proprietary")
                    
                    color: !!root.driverData?.openSource ? Kirigami.Theme.focusColor : "#e97517"
                    font.pointSize: Kirigami.Theme.smallFont.pointSize
                    font.weight: Font.Medium
                    leftPadding: Kirigami.Units.smallSpacing
                    rightPadding: Kirigami.Units.smallSpacing
                    topPadding: Kirigami.Units.smallSpacing / 2
                    bottomPadding: Kirigami.Units.smallSpacing / 2

                    background: Rectangle {
                        color: !!root.driverData?.openSource ? Qt.rgba(Kirigami.Theme.focusColor.r, Kirigami.Theme.focusColor.g, Kirigami.Theme.focusColor.b, 0.2) : "#e9751720"
                        radius: 4
                    }
                }

                RowLayout {
                    spacing: Kirigami.Units.smallSpacing

                    Kirigami.Icon {
                        source: !!root.driverData?.installed ? "checkmark" : "radio"
                        width: Kirigami.Units.iconSizes.small
                        height: Kirigami.Units.iconSizes.small
                        color: !!root.driverData?.installed ? Kirigami.Theme.positiveTextColor : Kirigami.Theme.disabledTextColor
                    }

                    QQC2.Label {
                        text: !!root.driverData?.installed ? qsTr("Installed") : qsTr("Not Installed")
                        
                        color: !!root.driverData?.installed ? Kirigami.Theme.positiveTextColor : Kirigami.Theme.disabledTextColor
                        font.pointSize: Kirigami.Theme.smallFont.pointSize
                        font.weight: Font.Medium
                    }
                }
            }
        }

        QQC2.Button {
            visible: !root.driverData?.installed
            enabled: !root.isInstalling
            text: root.isInstalling ? qsTr("Installing...") : qsTr("Install")
            icon.name: "download"
            
            onClicked: root.installClicked(root.deviceId, root.driverData?.id ?? "")

            background: Rectangle {
                color: parent.enabled ? Kirigami.Theme.positiveTextColor : Kirigami.Theme.disabledTextColor
                radius: 4
            }
        }

        QQC2.Button {
            visible: !!root.driverData?.installed
            enabled: !root.isInstalling
            text: root.isInstalling ? qsTr("Removing...") : qsTr("Remove")
            icon.name: "delete"
            
            onClicked: root.removeClicked(root.deviceId, root.driverData?.id ?? "")

            background: Rectangle {
                color: parent.enabled ? Kirigami.Theme.negativeTextColor : Kirigami.Theme.disabledTextColor
                radius: 4
            }
        }
    }
}
