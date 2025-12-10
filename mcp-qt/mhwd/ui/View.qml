pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls as QQC2
import QtQuick.Layouts
import QtQml.Models
import org.kde.kirigami as Kirigami
import org.manjaro.mcp.mhwd

QQC2.Page {
    id: root

    // Required interface
    required property var viewModel
    required property string busTypeFilter

    onBusTypeFilterChanged: {
        busTypeFilterModel.invalidate();
    }

    padding: 0

    // State
    property string selectedDevice: ""
    property bool showAllDevices: false
    
    // Data
    property var installingDrivers: ({})

    signal installDriver(deviceId: string, driverId: string)
    signal removeDriver(deviceId: string, driverId: string)

    function selectDevice(deviceId: string) {
        root.selectedDevice = root.selectedDevice === deviceId ? "" : deviceId;
    }

    QQC2.ScrollView {
        id: scrollView
        anchors {
            fill: parent
            rightMargin: scrollBar.width
        }

        QQC2.ScrollBar.horizontal.policy: QQC2.ScrollBar.AlwaysOff
        QQC2.ScrollBar.vertical.policy: QQC2.ScrollBar.AlwaysOff


        ColumnLayout {
            width: scrollView.availableWidth
            spacing: Kirigami.Units.largeSpacing

            Repeater {
                model: SortFilterProxyModel {
                    id: busTypeFilterModel
                    model: root.viewModel.categoryModel
                    
                    filters: [
                        FunctionFilter {
                            enabled: root.busTypeFilter !== "all"
                            
                            component RoleData: QtObject { 
                                property var devices 
                            }
                            
                            function filter(data: RoleData): bool {
                                if (!data.devices) return false;
                                
                                for (let i = 0; i < data.devices.length; i++) {
                                    const device = data.devices[i];

                                    if (device && device.busType.toLowerCase() === root.busTypeFilter.toLowerCase()) {
                                        return true;
                                    }
                                }
                                return false;
                            }
                        }
                    ]
                }

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
                    viewModel: root.viewModel

                    onDeviceSelected: (deviceId) => root.selectDevice(deviceId)
                    onInstallDriver: (deviceId, driverId) => root.installDriver(deviceId, driverId)
                    onRemoveDriver: (deviceId, driverId) => root.removeDriver(deviceId, driverId)

                    onCategoryNameChanged: {
                       if (categoryName === "Misc") {
                            isExpanded = false;
                       }
                    }
                }
            }
        }
    }

    QQC2.ScrollBar {
        id: scrollBar
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        
        policy: QQC2.ScrollBar.AlwaysOn
        orientation: Qt.Vertical

        size: scrollView.contentItem.height / scrollView.contentItem.contentHeight
        position: scrollView.contentItem.contentY / scrollView.contentItem.contentHeight

        onPositionChanged: {
            if (pressed) {
                scrollView.contentItem.contentY = position * scrollView.contentItem.contentHeight
            }
        }
        active: true
    }
}
