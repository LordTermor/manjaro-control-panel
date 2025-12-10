pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls as QQC2
import QtQuick.Layouts
import org.kde.kirigami as Kirigami

Kirigami.Card {
    id: root

    // Required interface
    property var selectedDeviceData: null
    required property string deviceId
    required property var installingDrivers

    signal installDriver(string deviceId, string driverId)
    signal removeDriver(string deviceId, string driverId)

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
            id: driverTypeInfo
            
            readonly property var drivers: root.selectedDeviceData?.drivers ?? []
            readonly property bool hasProprietaryDrivers: {
                for (let i = 0; i < driverTypeInfo.drivers.length; i++) {
                    if (!driverTypeInfo.drivers[i].openSource)
                        return true;
                }
                return false;
            }
            readonly property bool hasOpenSourceDrivers: {
                for (let i = 0; i < driverTypeInfo.drivers.length; i++) {
                    if (!!driverTypeInfo.drivers[i].openSource)
                        return true;
                }
                return false;
            }
            
            spacing: Kirigami.Units.smallSpacing

            QQC2.Label {
                visible: driverTypeInfo.hasProprietaryDrivers && driverTypeInfo.hasOpenSourceDrivers
                text: qsTr("Auto install:")
                color: Kirigami.Theme.disabledTextColor
            }

            QQC2.Button {
                visible: driverTypeInfo.hasProprietaryDrivers && driverTypeInfo.hasOpenSourceDrivers
                enabled: driverTypeInfo.hasProprietaryDrivers
                text: qsTr("Proprietary")

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
                visible: driverTypeInfo.hasProprietaryDrivers && driverTypeInfo.hasOpenSourceDrivers
                text: "|"
                color: Kirigami.Theme.disabledTextColor
            }

            QQC2.Button {
                visible: driverTypeInfo.hasProprietaryDrivers && driverTypeInfo.hasOpenSourceDrivers
                enabled: driverTypeInfo.hasOpenSourceDrivers
                text: qsTr("Open Source")

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
            model: root.selectedDeviceData?.driversModel

            delegate: DriverItem {
                required property string id
                required property string name
                required property string version
                required property string info
                required property bool openSource
                required property bool installed
                required property bool recommended
                required property int priority

                driverData: ({
                    "id": id,
                    "name": name,
                    "version": version,
                    "info": info,
                    "openSource": openSource,
                    "installed": installed,
                    "recommended": recommended,
                    "priority": priority
                })
                deviceId: root.deviceId
                isInstalling: {
                    const key = root.deviceId + "-" + id;
                    return !!root.installingDrivers?.[key];
                }

                onInstallClicked: (deviceId, driverId) => root.installDriver(deviceId, driverId)
                onRemoveClicked: (deviceId, driverId) => root.removeDriver(deviceId, driverId)
            }
        }
    }
}
