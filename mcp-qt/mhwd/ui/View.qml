pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls as QQC2
import QtQuick.Layouts
import org.kde.kirigami as Kirigami
import org.manjaro.mcp.mhwd

Kirigami.ScrollablePage {
    id: root

    // Required interface
    required property var viewModel

    // State
    property string selectedDevice: ""
    property bool showAllDevices: false
    
    // Data
    property var installingDrivers: ({})

    signal installDriver(string deviceId, string driverId)
    signal removeDriver(string deviceId, string driverId)

    function selectDevice(deviceId: string) {
        root.selectedDevice = root.selectedDevice === deviceId ? "" : deviceId;
    }

    ColumnLayout {
        width: parent.width
        spacing: Kirigami.Units.largeSpacing

        // Device Categories
        Repeater {
            model: root.viewModel.categoryModel

            delegate: CategorySection {
                required property string name
                required property string icon
                required property int deviceCount
                required property int driversAvailableCount
                required property var devices
                required property int index

                Layout.fillWidth: true

                categoryName: name
                categoryIcon: icon
                categoryIndex: index
                categoryDevices: devices
                selectedDevice: root.selectedDevice
                installingDrivers: root.installingDrivers

                onDeviceSelected: (deviceId) => root.selectDevice(deviceId)
                onInstallDriver: (deviceId, driverId) => root.installDriver(deviceId, driverId)
                onRemoveDriver: (deviceId, driverId) => root.removeDriver(deviceId, driverId)
            }
        }
    }
}
