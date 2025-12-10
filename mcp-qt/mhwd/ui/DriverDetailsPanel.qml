pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls as QQC2
import QtQuick.Layouts
import org.kde.kirigami as Kirigami

Kirigami.AbstractCard {
    id: root

    // Required interface
    property var selectedDeviceData: null
    required property string deviceId
    required property var installingDrivers
    required property var viewModel
    
    readonly property var drivers: selectedDeviceData?.drivers

    signal installDriver(deviceId: string, driverId: string)
    signal removeDriver(deviceId: string, driverId: string)

    Layout.fillWidth: true
    Layout.leftMargin: Kirigami.Units.largeSpacing * 2

    header: Kirigami.Heading {
            id: headerTitle

            level: 4
            text: qsTr("Available Drivers")
        }


    contentItem: Column {
        spacing: Kirigami.Units.largeSpacing

        Repeater {
            model: root.drivers

            delegate: DriverItem {
                required property string id
                required property string name
                required property string version
                required property string info
                required property bool openSource
                required property bool installed
                required property bool recommended
                required property int priority

                width: parent.width

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
                    return !!root.installingDrivers?.[id];
                }

                onInstallClicked: (deviceId, driverId) => root.installDriver(deviceId, driverId)
                onRemoveClicked: (deviceId, driverId) => root.removeDriver(deviceId, driverId)
            }
        }
    }
}
