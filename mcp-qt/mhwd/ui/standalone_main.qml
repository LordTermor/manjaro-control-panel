pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls as QQC2
import QtQuick.Layouts
import org.kde.kirigami as Kirigami
import org.manjaro.mcp.mhwd

Kirigami.ApplicationWindow {
    id: root

    // State
    property string selectedDevice: ""
    property bool showAllDevices: false
    property string busTypeFilter: "all"
    
    // Data
    property var installingDrivers: ({})
    
    function setDriverInstalling(driverId: string, installing: bool) {
        let updated = root.installingDrivers;
        updated[driverId] = installing;
        root.installingDrivers = updated;
    }
    
    MhwdViewModel {
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

    width: 1000
    height: 700
    title: qsTr("Hardware Configuration")

    header: QQC2.ToolBar {
        RowLayout {
            anchors.fill: parent
            anchors.margins: Kirigami.Units.smallSpacing

            QQC2.Label {
                text: qsTr("Bus Type:")
                Layout.leftMargin: Kirigami.Units.largeSpacing
            }

            QQC2.ComboBox {
                id: busTypeCombo

                Layout.preferredWidth: 150

                model: [
                    {text: qsTr("All Devices"), value: "all"},
                    {text: qsTr("PCI Devices"), value: "pci"},
                    {text: qsTr("USB Devices"), value: "usb"}
                ]
                textRole: "text"
                valueRole: "value"

                onActivated: (index) => {
                    root.busTypeFilter = model[index].value;
                }
            }

            Item {
                Layout.fillWidth: true
            }
        }
    }

    pageStack.initialPage: View {
        viewModel: viewModel
        selectedDevice: root.selectedDevice
        showAllDevices: root.showAllDevices
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
