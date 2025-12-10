pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls as QQC2
import QtQuick.Layouts
import org.kde.kirigami as Kirigami

Kirigami.ScrollablePage {
    id: root

    property string selectedDevice: ""
    property bool showAllDevices: false
    property var installingDrivers: ({})
    property var expandedCategories: ({})
    property var deviceCategories: ({})

    signal installDriver(string deviceId, string driverId)

    title: ""

    function toggleCategory(categoryName) {
        var newExpanded = root.expandedCategories;
        newExpanded[categoryName] = !newExpanded[categoryName];
        root.expandedCategories = newExpanded;
    }

    function selectDevice(deviceId) {
        root.selectedDevice = root.selectedDevice === deviceId ? "" : deviceId;
    }

    ColumnLayout {
        width: parent.width
        spacing: Kirigami.Units.largeSpacing

        // Device Categories
        Repeater {
            model: Object.keys(root.deviceCategories)

            delegate: CategorySection {
                required property string modelData

                categoryName: modelData
                categoryData: root.deviceCategories[modelData] || {}
                isExpanded: root.expandedCategories[modelData] || false
                selectedDevice: root.selectedDevice
                installingDrivers: root.installingDrivers

                onToggleExpanded: root.toggleCategory(modelData)
                onDeviceSelected: (deviceId) => root.selectDevice(deviceId)
                onInstallDriver: (deviceId, driverId) => root.installDriver(deviceId, driverId)
            }
        }

        // Bottom Controls
        RowLayout {
            Layout.fillWidth: true
            Layout.topMargin: Kirigami.Units.largeSpacing

            QQC2.CheckBox {
                text: qsTr("Show all devices")
                checked: root.showAllDevices
                onCheckedChanged: root.showAllDevices = checked
            }

            Item {
                Layout.fillWidth: true
            }

            QQC2.Button {
                text: qsTr("All Settings")
                flat: true
            }

            QQC2.Button {
                text: qsTr("Quit")
                highlighted: true
                onClicked: Qt.quit()
            }
        }
    }
}
