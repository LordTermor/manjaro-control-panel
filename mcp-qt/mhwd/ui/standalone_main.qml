pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls as QQC2
import QtQuick.Layouts
import org.kde.kirigami as Kirigami

Kirigami.ApplicationWindow {
    id: root

    property string selectedDevice: ""
    property bool showAllDevices: false
    property var installingDrivers: ({})
    property var expandedCategories: ({
        "Display": true,
        "Network": true,
        "Audio": true,
        "Storage": true,
        "Input": true,
        "Misc": false
    })
    
    // Organized by category - populated from lspci and lsusb
    property var deviceCategories: ({
        "Display": {
            "icon": "video-display",
            "devices": [
                {
                    "id": "nvidia-gtx-1660ti",
                    "name": "GeForce GTX 1660 Ti",
                    "vendor": "NVIDIA Corporation",
                    "icon": "video-display",
                    "hasDrivers": true,
                    "drivers": [{
                        "id": "550xx",
                        "name": "video-nvidia-550xx",
                        "openSource": false,
                        "installed": true,
                        "recommended": true
                    }, {
                        "id": "535xx",
                        "name": "video-nvidia-535xx",
                        "openSource": false,
                        "installed": false,
                        "recommended": false
                    }, {
                        "id": "470xx",
                        "name": "video-nvidia-470xx",
                        "openSource": false,
                        "installed": false,
                        "recommended": false
                    }, {
                        "id": "nouveau",
                        "name": "video-nouveau",
                        "openSource": true,
                        "installed": false,
                        "recommended": false
                    }]
                },
                {
                    "id": "amd-granite-ridge",
                    "name": "Granite Ridge [Radeon Graphics]",
                    "vendor": "AMD/ATI",
                    "icon": "video-display",
                    "hasDrivers": false
                }
            ]
        },
        "Network": {
            "icon": "network-wired",
            "devices": [
                {
                    "id": "realtek-rtl8125",
                    "name": "RTL8125 2.5GbE Controller",
                    "vendor": "Realtek Semiconductor",
                    "icon": "network-wired",
                    "hasDrivers": true,
                    "drivers": [{
                        "id": "r8125",
                        "name": "network-r8125-dkms",
                        "openSource": true,
                        "installed": false,
                        "recommended": true
                    }]
                },
                {
                    "id": "intel-ax210",
                    "name": "Wi-Fi 6E AX210/AX1675",
                    "vendor": "Intel Corporation",
                    "icon": "network-wireless",
                    "hasDrivers": false
                }
            ]
        },
        "Audio": {
            "icon": "audio-card",
            "devices": [
                {
                    "id": "nvidia-audio",
                    "name": "TU116 High Definition Audio Controller",
                    "vendor": "NVIDIA Corporation",
                    "icon": "audio-card",
                    "hasDrivers": false
                },
                {
                    "id": "amd-rembrandt-audio",
                    "name": "Radeon High Definition Audio [Rembrandt/Strix]",
                    "vendor": "AMD/ATI",
                    "icon": "audio-card",
                    "hasDrivers": false
                },
                {
                    "id": "amd-hd-audio",
                    "name": "Family 17h/19h/1ah HD Audio Controller",
                    "vendor": "AMD",
                    "icon": "audio-card",
                    "hasDrivers": false
                },
                {
                    "id": "logitech-z407",
                    "name": "Logi Z407",
                    "vendor": "Logitech, Inc.",
                    "icon": "audio-speakers",
                    "hasDrivers": false
                }
            ]
        },
        "Storage": {
            "icon": "drive-harddisk",
            "devices": [
                {
                    "id": "samsung-nvme-1",
                    "name": "NVMe SSD Controller SM981/PM981/PM983 (Slot 1)",
                    "vendor": "Samsung Electronics",
                    "icon": "drive-harddisk-solidstate",
                    "hasDrivers": false
                },
                {
                    "id": "samsung-nvme-2",
                    "name": "NVMe SSD Controller SM981/PM981/PM983 (Slot 2)",
                    "vendor": "Samsung Electronics",
                    "icon": "drive-harddisk-solidstate",
                    "hasDrivers": false
                },
                {
                    "id": "amd-sata",
                    "name": "600 Series Chipset SATA Controller",
                    "vendor": "AMD",
                    "icon": "drive-harddisk",
                    "hasDrivers": false
                }
            ]
        },
        "Input": {
            "icon": "input-mouse",
            "devices": [
                {
                    "id": "razer-viper-v2",
                    "name": "Razer Viper V2 Pro",
                    "vendor": "Razer USA, Ltd",
                    "icon": "input-mouse",
                    "hasDrivers": true,
                    "drivers": [{
                        "id": "openrazer",
                        "name": "openrazer-meta",
                        "openSource": true,
                        "installed": false,
                        "recommended": true
                    }]
                },
                {
                    "id": "razer-blackwidow",
                    "name": "Razer BlackWidow V3",
                    "vendor": "Razer USA, Ltd",
                    "icon": "input-keyboard",
                    "hasDrivers": true,
                    "drivers": [{
                        "id": "openrazer",
                        "name": "openrazer-meta",
                        "openSource": true,
                        "installed": false,
                        "recommended": true
                    }]
                }
            ]
        },
        "Misc": {
            "icon": "computer",
            "devices": [
                {
                    "id": "nvidia-usbc-ucsi",
                    "name": "TU116 USB Type-C UCSI Controller",
                    "vendor": "NVIDIA Corporation",
                    "icon": "usb",
                    "hasDrivers": false
                },
                {
                    "id": "nvidia-usb3",
                    "name": "TU116 USB 3.1 Host Controller",
                    "vendor": "NVIDIA Corporation",
                    "icon": "usb",
                    "hasDrivers": false
                },
                {
                    "id": "amd-usb3-1",
                    "name": "Raphael/Granite Ridge USB 3.1 xHCI (Port 1)",
                    "vendor": "AMD",
                    "icon": "usb",
                    "hasDrivers": false
                },
                {
                    "id": "amd-usb3-2",
                    "name": "Raphael/Granite Ridge USB 3.1 xHCI (Port 2)",
                    "vendor": "AMD",
                    "icon": "usb",
                    "hasDrivers": false
                },
                {
                    "id": "amd-usb2",
                    "name": "Raphael/Granite Ridge USB 2.0 xHCI",
                    "vendor": "AMD",
                    "icon": "usb",
                    "hasDrivers": false
                },
                {
                    "id": "amd-600-usb3",
                    "name": "600 Series Chipset USB 3.2 Controller",
                    "vendor": "AMD",
                    "icon": "usb",
                    "hasDrivers": false
                },
                {
                    "id": "amd-psp",
                    "name": "Family 19h PSP/CCP",
                    "vendor": "AMD",
                    "icon": "security-high",
                    "hasDrivers": false
                },
                {
                    "id": "intel-bt-ax210",
                    "name": "AX210 Bluetooth",
                    "vendor": "Intel Corp.",
                    "icon": "network-bluetooth",
                    "hasDrivers": false
                },
                {
                    "id": "csr-bt",
                    "name": "Bluetooth Dongle (HCI mode)",
                    "vendor": "Cambridge Silicon Radio",
                    "icon": "network-bluetooth",
                    "hasDrivers": false
                },
                {
                    "id": "ite-rgb",
                    "name": "RGB LED Controller",
                    "vendor": "Integrated Technology Express",
                    "icon": "led",
                    "hasDrivers": false
                },
                {
                    "id": "amd-smbus",
                    "name": "FCH SMBus Controller",
                    "vendor": "AMD",
                    "icon": "computer",
                    "hasDrivers": false
                },
                {
                    "id": "amd-lpc",
                    "name": "FCH LPC Bridge",
                    "vendor": "AMD",
                    "icon": "computer",
                    "hasDrivers": false
                }
            ]
        }
    })

    function installDriver(deviceId, driverId) {
        var key = deviceId + "-" + driverId;
        var newInstallingDrivers = root.installingDrivers;
        newInstallingDrivers[key] = true;
        root.installingDrivers = newInstallingDrivers;
        installTimer.deviceKey = key;
        installTimer.start();
    }

    width: 1000
    height: 700
    title: qsTr("Hardware Configuration")

    Timer {
        id: installTimer
        property string deviceKey: ""
        interval: 2000
        onTriggered: {
            var newInstallingDrivers = root.installingDrivers;
            newInstallingDrivers[installTimer.deviceKey] = false;
            root.installingDrivers = newInstallingDrivers;
        }
    }

    pageStack.initialPage: View {
        selectedDevice: root.selectedDevice
        showAllDevices: root.showAllDevices
        installingDrivers: root.installingDrivers
        expandedCategories: root.expandedCategories
        deviceCategories: root.deviceCategories

        onInstallDriver: (deviceId, driverId) => root.installDriver(deviceId, driverId)
    }

    header: Item {
        height: headerLayout.implicitHeight + Kirigami.Units.largeSpacing * 2

        RowLayout {
            id: headerLayout
            anchors.fill: parent
            anchors.margins: Kirigami.Units.largeSpacing

            Kirigami.Icon {
                source: "configure"
                Layout.preferredWidth: Kirigami.Units.iconSizes.small
                Layout.preferredHeight: Kirigami.Units.iconSizes.small
                color: Kirigami.Theme.textColor
            }

            ColumnLayout {
                Layout.fillWidth: true
                spacing: 0

                Kirigami.Heading {
                    level: 2
                    text: qsTr("Hardware Configuration")
                }

                QQC2.Label {
                    text: qsTr("Manage your system drivers")
                    color: Kirigami.Theme.disabledTextColor
                    font.pointSize: Kirigami.Theme.smallFont.pointSize
                }
            }

            QQC2.ToolButton {
                icon.name: "window-close"
                onClicked: Qt.quit()
            }
        }
    }
}
