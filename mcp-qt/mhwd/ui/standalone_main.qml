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
    
    // Data
    property var installingDrivers: ({})
    
    MhwdViewModel {
        id: viewModel
        
        onDriverOperationStarted: (deviceId, driverId) => {
            const key = deviceId + "-" + driverId;
            let newInstallingDrivers = root.installingDrivers;
            newInstallingDrivers[key] = true;
            root.installingDrivers = newInstallingDrivers;
        }
        
        onDriverOperationCompleted: (deviceId, driverId, success) => {
            const key = deviceId + "-" + driverId;
            let newInstallingDrivers = root.installingDrivers;
            newInstallingDrivers[key] = false;
            root.installingDrivers = newInstallingDrivers;
            
            if (!success) {
                errorDialog.title = qsTr("Driver Operation Failed");
                errorDialog.message = qsTr("Failed to install/remove driver");
                errorDialog.open();
            }
        }
        
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

    pageStack.initialPage: View {
        viewModel: viewModel
        selectedDevice: root.selectedDevice
        showAllDevices: root.showAllDevices
        installingDrivers: root.installingDrivers

        onInstallDriver: (deviceId, driverId) => viewModel.installDriver(deviceId, driverId)
        onRemoveDriver: (deviceId, driverId) => viewModel.removeDriver(deviceId, driverId)
    }
}
