pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls as QQC2
import QtQuick.Layouts
import org.kde.kirigami as Kirigami

ColumnLayout {
    id: root

    required property string categoryName
    required property var categoryData
    required property bool isExpanded
    required property string selectedDevice
    required property var installingDrivers

    signal toggleExpanded()
    signal deviceSelected(string deviceId)
    signal installDriver(string deviceId, string driverId)

    property int deviceCount: root.categoryData.devices ? root.categoryData.devices.length : 0
    property int driversAvailableCount: {
        var count = 0;
        var devices = root.categoryData.devices || [];
        for (var i = 0; i < devices.length; i++) {
            if (devices[i].hasDrivers) count++;
        }
        return count;
    }

    Layout.fillWidth: true
    spacing: Kirigami.Units.smallSpacing

    // Category Header
    Kirigami.AbstractCard {
        Layout.fillWidth: true
        
        onClicked: root.toggleExpanded()

        background: Rectangle {
            color: Kirigami.Theme.alternateBackgroundColor
            radius: 4

            Behavior on color {
                ColorAnimation {
                    duration: 200
                    easing.type: Easing.InOutQuad
                }
            }
        }

        contentItem: RowLayout {
            anchors.margins: Kirigami.Units.largeSpacing
            spacing: Kirigami.Units.largeSpacing

            Kirigami.Icon {
                source: root.isExpanded ? "arrow-down" : "arrow-right"
                Layout.preferredWidth: Kirigami.Units.iconSizes.small
                Layout.preferredHeight: Kirigami.Units.iconSizes.small

                Behavior on rotation {
                    NumberAnimation {
                        duration: 200
                        easing.type: Easing.InOutQuad
                    }
                }
            }

            Rectangle {
                Layout.preferredWidth: Kirigami.Units.iconSizes.medium
                Layout.preferredHeight: Kirigami.Units.iconSizes.medium
                color: Kirigami.Theme.backgroundColor
                radius: 4

                Kirigami.Icon {
                    anchors.centerIn: parent
                    source: root.categoryData.icon || ""
                    width: Kirigami.Units.iconSizes.smallMedium
                    height: Kirigami.Units.iconSizes.smallMedium
                }
            }

            Kirigami.Heading {
                level: 3
                text: root.categoryName
                Layout.fillWidth: true
            }

            QQC2.Label {
                text: root.deviceCount + " device" + (root.deviceCount !== 1 ? "s" : "")
                font.pointSize: Kirigami.Theme.smallFont.pointSize
                color: Kirigami.Theme.disabledTextColor
            }

            QQC2.Label {
                text: root.driversAvailableCount > 0 ? (root.driversAvailableCount + " with drivers") : ""
                visible: root.driversAvailableCount > 0
                font.pointSize: Kirigami.Theme.smallFont.pointSize
                color: Kirigami.Theme.focusColor
                font.weight: Font.Medium
            }
        }
    }

    // Device Grid
    Flow {
        Layout.fillWidth: true
        Layout.leftMargin: Kirigami.Units.largeSpacing * 2
        
        visible: root.isExpanded
        spacing: Kirigami.Units.largeSpacing

        Repeater {
            model: root.categoryData.devices || []

            delegate: DeviceCard {
                required property var modelData

                deviceData: modelData
                isSelected: root.selectedDevice === modelData.id
                selectedDevice: root.selectedDevice

                onDeviceClicked: root.deviceSelected(modelData.id)
            }
        }
    }

    // Driver Details Panel (shown when device is selected)
    DriverDetailsPanel {
        id: driverPanel
        
        property var computedDeviceData: {
            var devices = root.categoryData.devices || [];
            for (var i = 0; i < devices.length; i++) {
                if (devices[i].id === root.selectedDevice) {
                    return devices[i];
                }
            }
            return null;
        }
        
        visible: root.isExpanded && !!computedDeviceData && computedDeviceData.hasDrivers

        selectedDeviceData: computedDeviceData || {}
        deviceId: root.selectedDevice
        installingDrivers: root.installingDrivers

        onInstallDriver: (deviceId, driverId) => root.installDriver(deviceId, driverId)
    }
}
