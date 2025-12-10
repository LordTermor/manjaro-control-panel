pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls as QQC2
import QtQuick.Layouts
import org.kde.kirigami as Kirigami

Kirigami.Card {
    id: root

    required property var selectedDeviceData
    required property string deviceId
    required property var installingDrivers

    signal installDriver(string deviceId, string driverId)

    Layout.fillWidth: true
    Layout.leftMargin: Kirigami.Units.largeSpacing * 2

    header: RowLayout {
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.margins: Kirigami.Units.largeSpacing
        spacing: Kirigami.Units.largeSpacing

        Kirigami.Heading {
            level: 4
            text: qsTr("Available Drivers")
            Layout.fillWidth: true
        }

        RowLayout {
            property var drivers: root.selectedDeviceData?.drivers ?? []
            property bool hasProprietaryDrivers: {
                for (var i = 0; i < drivers.length; i++) {
                    if (!drivers[i].openSource)
                        return true;
                }
                return false;
            }
            property bool hasOpenSourceDrivers: {
                for (var i = 0; i < drivers.length; i++) {
                    if (drivers[i].openSource)
                        return true;
                }
                return false;
            }
            
            spacing: Kirigami.Units.smallSpacing

            QQC2.Label {
                text: qsTr("Auto install:")
                color: Kirigami.Theme.disabledTextColor
                visible: parent.hasProprietaryDrivers && parent.hasOpenSourceDrivers
            }

            QQC2.Button {
                text: qsTr("Proprietary")
                visible: parent.hasProprietaryDrivers && parent.hasOpenSourceDrivers
                enabled: parent.hasProprietaryDrivers

                background: Rectangle {
                    color: parent.enabled ? "#e97517" : Kirigami.Theme.disabledTextColor
                    radius: 4
                }

                contentItem: QQC2.Label {
                    text: parent.text
                    color: "white"
                    font.weight: Font.Medium
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
            }

            QQC2.Label {
                text: "|"
                color: Kirigami.Theme.disabledTextColor
                visible: parent.hasProprietaryDrivers && parent.hasOpenSourceDrivers
            }

            QQC2.Button {
                text: qsTr("Open Source")
                visible: parent.hasProprietaryDrivers && parent.hasOpenSourceDrivers
                enabled: parent.hasOpenSourceDrivers

                background: Rectangle {
                    color: parent.enabled ? Kirigami.Theme.focusColor : Kirigami.Theme.disabledTextColor
                    radius: 4
                }

                contentItem: QQC2.Label {
                    text: parent.text
                    color: "white"
                    font.weight: Font.Medium
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
            }
        }
    }

    contentItem: ColumnLayout {
        anchors.margins: Kirigami.Units.largeSpacing
        spacing: Kirigami.Units.largeSpacing

        Repeater {
            model: root.selectedDeviceData?.drivers ?? []

            delegate: DriverItem {
                required property var modelData
                required property int index

                driverData: modelData
                deviceId: root.deviceId
                isInstalling: {
                    var key = root.deviceId + "-" + (modelData.id || "");
                    return root.installingDrivers[key] || false;
                }

                onInstallClicked: (deviceId, driverId) => root.installDriver(deviceId, driverId)
            }
        }
    }
}
