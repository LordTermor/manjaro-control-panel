pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls as QQC2
import QtQuick.Layouts
import org.kde.kirigami as Kirigami
import org.manjaro.mcp.mhwd

ColumnLayout {
    id: root

    // Required interface
    required property string categoryName
    required property string categoryIcon
    required property int categoryIndex
    required property var categoryDevices // Array of device objects
    required property string selectedDevice
    required property var installingDrivers
    required property var viewModel

    property bool isExpanded: true

    signal deviceSelected(deviceId: string)
    signal installDriver(deviceId: string, driverId: string)
    signal removeDriver(deviceId: string, driverId: string)

    

    readonly property int deviceCount: categoryDevices?.length ?? 0

    readonly property int driversAvailableCount: {
        if (!categoryDevices) return 0;
        let count = 0;
        for (let i = 0; i < categoryDevices.length; i++) {
            if (categoryDevices[i].hasDrivers) {
                count++;
            }
        }
        return count;
    }

    Layout.fillWidth: true
    spacing: Kirigami.Units.smallSpacing

    // Category Header
    Kirigami.AbstractCard {
        Layout.fillWidth: true
        
        onClicked: root.isExpanded = !root.isExpanded

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
                    source: root.categoryIcon
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
                visible: root.driversAvailableCount > 0
                text: root.driversAvailableCount + " with drivers"
                font.pointSize: Kirigami.Theme.smallFont.pointSize
                color: Kirigami.Theme.focusColor
                font.weight: Font.Medium
            }
        }
    }

    // Device Grid
    Flow {
        id: deviceGrid
        Layout.fillWidth: true
        Layout.leftMargin: Kirigami.Units.largeSpacing * 2
        
        visible: root.isExpanded
        spacing: Kirigami.Units.largeSpacing

        property int calculatedHeight: 0

        Repeater {
            model: root.categoryDevices

            delegate: DeviceCard {
                required property var modelData

                width: Math.max(280, Math.min(400, (deviceGrid.width - deviceGrid.spacing * 2) / 3))
                height: deviceGrid.calculatedHeight
                
                onImplicitHeightChanged: {
                    deviceGrid.calculatedHeight = Math.max(deviceGrid.calculatedHeight, implicitHeight);
                }

                deviceData: modelData
                isSelected: root.selectedDevice === modelData.id
                selectedDevice: root.selectedDevice
                driverCount: modelData.drivers.length

                onDeviceClicked: root.deviceSelected(modelData.id)
            }
        }
    }

    // Driver Details Panel (shown when device is selected)
    DriverDetailsPanel {
        id: driverPanel
        
        selectedDeviceData: {
            if (!root.categoryDevices || !root.selectedDevice) return null;
            
            for (let i = 0; i < root.categoryDevices.length; i++) {
                if (root.categoryDevices[i].id === root.selectedDevice) {
                    return root.categoryDevices[i];
                }
            }
            return null;
        }
        
        visible: root.isExpanded && !!selectedDeviceData?.hasDrivers
        deviceId: root.selectedDevice
        installingDrivers: root.installingDrivers
        viewModel: root.viewModel
        

        onInstallDriver: (deviceId, driverId) => root.installDriver(deviceId, driverId)
        onRemoveDriver: (deviceId, driverId) => root.removeDriver(deviceId, driverId)
    }
}
