pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls as QQC2
import QtQuick.Layouts

import org.kde.kirigami as Kirigami
import org.manjaro.mcp.mhwd as MhwdModule

/*
 * Hardware configuration page for the unified shell.
 * Wraps the MHWD module's View component in a Kirigami.Page with bus-type filtering.
 */
Kirigami.Page {
    id: root

    title: qsTr("Hardware Configuration")
    padding: 0

    // State
    property string selectedDevice: ""
    property string busTypeFilter: "all"
    property var installingDrivers: ({})

    function setDriverInstalling(driverId: string, installing: bool) {
        let updated = root.installingDrivers;
        updated[driverId] = installing;
        root.installingDrivers = updated;
    }

    MhwdModule.MhwdViewModel {
        id: viewModel

        onErrorOccurred: (title, message) => {
            errorDialog.title = title;
            errorDialog.message = message;
            errorDialog.open();
        }
    }

    Kirigami.PromptDialog {
        id: errorDialog

        property string message: ""

        title: qsTr("Error")
        subtitle: errorDialog.message
        standardButtons: Kirigami.Dialog.Ok
    }

    actions: [
        Kirigami.Action {
            text: qsTr("All Devices")
            icon.name: "computer"
            checkable: true
            checked: root.busTypeFilter === "all"
            onTriggered: root.busTypeFilter = "all"
        },
        Kirigami.Action {
            text: qsTr("PCI Devices")
            icon.name: "show-gpu-effects-symbolic"
            checkable: true
            checked: root.busTypeFilter === "pci"
            onTriggered: root.busTypeFilter = "pci"
        },
        Kirigami.Action {
            text: qsTr("USB Devices")
            icon.name: "drive-removable-media-usb"
            checkable: true
            checked: root.busTypeFilter === "usb"
            onTriggered: root.busTypeFilter = "usb"
        }
    ]

    MhwdModule.View {
        anchors.fill: parent
        viewModel: viewModel
        selectedDevice: root.selectedDevice
        showAllDevices: false
        busTypeFilter: root.busTypeFilter
        installingDrivers: root.installingDrivers

        onInstallDriver: (deviceId, driverId) => {
            root.setDriverInstalling(driverId, true);
            viewModel.installDriver(driverId).then((success) => {
                root.setDriverInstalling(driverId, false);
                if (!success) {
                    errorDialog.title = qsTr("Installation Failed");
                    errorDialog.message = qsTr("Failed to install driver: %1").arg(driverId);
                    errorDialog.open();
                }
            });
        }
        onRemoveDriver: (deviceId, driverId) => {
            root.setDriverInstalling(driverId, true);
            viewModel.removeDriver(driverId).then((success) => {
                root.setDriverInstalling(driverId, false);
                if (!success) {
                    errorDialog.title = qsTr("Removal Failed");
                    errorDialog.message = qsTr("Failed to remove driver: %1").arg(driverId);
                    errorDialog.open();
                }
            });
        }
    }
}
