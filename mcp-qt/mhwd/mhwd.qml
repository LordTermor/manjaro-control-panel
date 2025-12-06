pragma ComponentBehavior: Bound

import QtQuick 2.15
import QtQuick.Controls 2.15 as QQC2
import QtQuick.Layouts 1.15
import org.kde.kirigami 2.20 as Kirigami

Kirigami.ApplicationWindow {
    id: root

    property string selectedDevice: "nvidia-gpu"
    property bool showAllDevices: false
    property var installingDrivers: ({})
    
    // Data models
    property var devicesWithDrivers: ({
        "ethernet-controller": {
            "name": "RTL8111/8168/8411 PCI Express Gigabit Ethernet Controller",
            "vendor": "Realtek",
            "category": "Network controller",
            "icon": "network-wired",
            "drivers": [{
                "id": "r8168",
                "name": "network-r8168",
                "openSource": true,
                "installed": false,
                "recommended": true
            }]
        },
        "nvidia-gpu": {
            "name": "TU117M [GeForce GTX 1650 Mobile / Max-Q]",
            "vendor": "NVIDIA Corporation",
            "category": "Display controller",
            "icon": "video-display",
            "drivers": [{
                "id": "455xx",
                "name": "video-hybrid-intel-nvidia-455xx-prime",
                "openSource": false,
                "installed": true,
                "recommended": true
            }, {
                "id": "450xx",
                "name": "video-hybrid-intel-nvidia-450xx-prime",
                "openSource": false,
                "installed": false,
                "recommended": false
            }, {
                "id": "440xx",
                "name": "video-hybrid-intel-nvidia-440xx-prime",
                "openSource": false,
                "installed": false,
                "recommended": false
            }, {
                "id": "435xx",
                "name": "video-hybrid-intel-nvidia-435xx-prime",
                "openSource": false,
                "installed": false,
                "recommended": false
            }, {
                "id": "430xx",
                "name": "video-hybrid-intel-nvidia-430xx-bumblebee",
                "openSource": false,
                "installed": false,
                "recommended": false
            }]
        }
    })
    
    property var devicesWithoutDrivers: [{
        "id": "wireless-ac",
        "name": "Wireless-AC 9560 [Jefferson Peak]",
        "vendor": "Intel Corporation",
        "category": "Network controller",
        "icon": "network-wireless"
    }, {
        "id": "audio-device",
        "name": "HD Audio Controller",
        "vendor": "Intel Corporation",
        "category": "Audio device",
        "icon": "audio-card"
    }]

    function installDriver(deviceId, driverId) {
        var key = deviceId + "-" + driverId;
        var newInstallingDrivers = root.installingDrivers;
        newInstallingDrivers[key] = true;
        root.installingDrivers = newInstallingDrivers;
        // Simulate installation
        installTimer.deviceKey = key;
        installTimer.start();
    }

    width: 1000
    height: 700
    title: "Hardware Configuration"

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

    pageStack.initialPage: Kirigami.ScrollablePage {
        id: mainPage
        title: ""

        ColumnLayout {
            id: mainLayout
            width: parent.width
            spacing: Kirigami.Units.largeSpacing * 2

            // Devices with Available Drivers Section
            ColumnLayout {
                id: devicesWithDriversSection
                Layout.fillWidth: true
                spacing: Kirigami.Units.largeSpacing

                Kirigami.Heading {
                    id: devicesWithDriversTitle
                    level: 3
                    text: "Devices with Available Drivers"
                }

                // Horizontal Device Cards
                Item {
                    id: devicesScrollContainer
                    Layout.fillWidth: true
                    height: 120

                    QQC2.ScrollView {
                        id: devicesScrollView
                        anchors.fill: parent
                        QQC2.ScrollBar.vertical.policy: QQC2.ScrollBar.AlwaysOff

                        ListView {
                            id: devicesList
                            orientation: ListView.Horizontal
                            spacing: Kirigami.Units.largeSpacing
                            model: Object.keys(root.devicesWithDrivers)

                            delegate: Item {
                                id: deviceCardContainer
                                
                                required property string modelData
                                property var device: root.devicesWithDrivers[deviceCardContainer.modelData] || {}
                                property bool isSelected: root.selectedDevice === deviceCardContainer.modelData

                                width: 320
                                height: 100

                                Kirigami.AbstractCard {
                                    id: deviceCard
                                    anchors.fill: parent
                                    
                                    onClicked: root.selectedDevice = deviceCardContainer.modelData

                                    background: Rectangle {
                                        id: deviceCardBackground
                                        color: deviceCardContainer.isSelected ? Kirigami.Theme.activeBackgroundColor : Kirigami.Theme.backgroundColor
                                        border.color: deviceCardContainer.isSelected ? Kirigami.Theme.focusColor : Kirigami.Theme.alternateBackgroundColor
                                        border.width: deviceCardContainer.isSelected ? 2 : 1
                                        radius: 4

                                        Behavior on color { ColorAnimation { duration: 200 } }
                                        Behavior on border.color { ColorAnimation { duration: 200 } }
                                    }

                                    contentItem: RowLayout {
                                        id: deviceCardContent
                                        anchors.margins: Kirigami.Units.largeSpacing
                                        spacing: Kirigami.Units.largeSpacing

                                        Rectangle {
                                            id: deviceIconBackground
                                            Layout.preferredWidth: Kirigami.Units.iconSizes.medium
                                            Layout.preferredHeight: Kirigami.Units.iconSizes.medium
                                            color: deviceCardContainer.isSelected ? Kirigami.Theme.focusColor : Kirigami.Theme.alternateBackgroundColor
                                            radius: 4

                                            Kirigami.Icon {
                                                id: deviceIcon
                                                anchors.centerIn: parent
                                                source: deviceCardContainer.device.icon || ""
                                                width: Kirigami.Units.iconSizes.smallMedium
                                                height: Kirigami.Units.iconSizes.smallMedium
                                            }
                                        }

                                        ColumnLayout {
                                            id: deviceInfoLayout
                                            Layout.fillWidth: true
                                            Layout.fillHeight: true
                                            spacing: Kirigami.Units.smallSpacing

                                            QQC2.Label {
                                                id: deviceCategory
                                                text: deviceCardContainer.device.category || ""
                                                font.pointSize: Kirigami.Theme.smallFont.pointSize
                                                color: Kirigami.Theme.disabledTextColor
                                                elide: Text.ElideRight
                                                Layout.fillWidth: true
                                            }

                                            QQC2.Label {
                                                id: deviceName
                                                text: deviceCardContainer.device.name || ""
                                                font.weight: Font.Medium
                                                wrapMode: Text.WordWrap
                                                elide: Text.ElideRight
                                                Layout.fillWidth: true
                                                Layout.fillHeight: true
                                            }

                                            QQC2.Label {
                                                id: deviceVendor
                                                text: deviceCardContainer.device.vendor || ""
                                                font.pointSize: Kirigami.Theme.smallFont.pointSize
                                                color: Kirigami.Theme.disabledTextColor
                                                elide: Text.ElideRight
                                                Layout.fillWidth: true
                                            }

                                            RowLayout {
                                                id: deviceAvailableLayout
                                                property int driverCount: deviceCardContainer.device.drivers ? deviceCardContainer.device.drivers.length : 0

                                                QQC2.Label {
                                                    id: deviceAvailableLabel
                                                    text: deviceAvailableLayout.driverCount + " driver" + (deviceAvailableLayout.driverCount !== 1 ? "s" : "") + " available"
                                                    font.pointSize: Kirigami.Theme.smallFont.pointSize
                                                    color: Kirigami.Theme.focusColor
                                                }

                                                Kirigami.Icon {
                                                    id: deviceAvailableIcon
                                                    source: "arrow-right"
                                                    width: Kirigami.Units.iconSizes.small
                                                    height: Kirigami.Units.iconSizes.small
                                                    color: Kirigami.Theme.focusColor
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }

                // Driver Options for Selected Device
                Kirigami.Card {
                    id: driverOptionsCard
                    property var selectedDeviceData: root.devicesWithDrivers[root.selectedDevice] || {}
                    Layout.fillWidth: true
                    visible: root.selectedDevice && root.devicesWithDrivers[root.selectedDevice]

                    header: RowLayout {
                        id: driverOptionsHeader
                        anchors.left: parent.left
                        anchors.right: parent.right
                        anchors.margins: Kirigami.Units.largeSpacing
                        spacing: Kirigami.Units.largeSpacing

                        Rectangle {
                            id: selectedDeviceIconBackground
                            Layout.preferredWidth: Kirigami.Units.iconSizes.medium
                            Layout.preferredHeight: Kirigami.Units.iconSizes.medium
                            color: Kirigami.Theme.focusColor
                            radius: 4

                            Kirigami.Icon {
                                id: selectedDeviceIcon
                                anchors.centerIn: parent
                                source: driverOptionsCard.selectedDeviceData.icon || ""
                                width: Kirigami.Units.iconSizes.smallMedium
                                height: Kirigami.Units.iconSizes.smallMedium
                                color: "white"
                            }
                        }

                        ColumnLayout {
                            id: selectedDeviceInfoLayout
                            Layout.fillWidth: true
                            spacing: 0

                            Kirigami.Heading {
                                id: selectedDeviceNameLabel
                                level: 4
                                text: driverOptionsCard.selectedDeviceData.name || ""
                            }

                            QQC2.Label {
                                id: selectedDeviceVendorLabel
                                text: driverOptionsCard.selectedDeviceData.vendor || ""
                                color: Kirigami.Theme.disabledTextColor
                            }
                        }

                        // Auto Install Buttons
                        RowLayout {
                            id: autoInstallLayout
                            property var drivers: driverOptionsCard.selectedDeviceData.drivers || []
                            property bool hasProprietaryDrivers: {
                                for (var i = 0; i < autoInstallLayout.drivers.length; i++) {
                                    if (!autoInstallLayout.drivers[i].openSource)
                                        return true;
                                }
                                return false;
                            }
                            property bool hasOpenSourceDrivers: {
                                for (var i = 0; i < autoInstallLayout.drivers.length; i++) {
                                    if (autoInstallLayout.drivers[i].openSource)
                                        return true;
                                }
                                return false;
                            }
                            spacing: Kirigami.Units.smallSpacing

                            QQC2.Label {
                                id: autoInstallLabel
                                text: "Auto install:"
                                color: Kirigami.Theme.disabledTextColor
                                visible: (autoInstallLayout.hasProprietaryDrivers && autoInstallLayout.hasOpenSourceDrivers) || root.selectedDevice === "nvidia-gpu"
                            }

                            QQC2.Button {
                                id: proprietaryButton
                                text: "Proprietary"
                                visible: (autoInstallLayout.hasProprietaryDrivers && autoInstallLayout.hasOpenSourceDrivers) || root.selectedDevice === "nvidia-gpu"
                                enabled: autoInstallLayout.hasProprietaryDrivers

                                background: Rectangle {
                                    color: proprietaryButton.enabled ? "#e97517" : Kirigami.Theme.disabledTextColor
                                    radius: 4
                                }

                                contentItem: QQC2.Label {
                                    text: proprietaryButton.text
                                    color: "white"
                                    font.weight: Font.Medium
                                    horizontalAlignment: Text.AlignHCenter
                                    verticalAlignment: Text.AlignVCenter
                                }
                            }

                            QQC2.Label {
                                id: separatorLabel
                                text: "|"
                                color: Kirigami.Theme.disabledTextColor
                                visible: (autoInstallLayout.hasProprietaryDrivers && autoInstallLayout.hasOpenSourceDrivers) || root.selectedDevice === "nvidia-gpu"
                            }

                            QQC2.Button {
                                id: openSourceButton
                                text: "Open Source"
                                visible: (autoInstallLayout.hasProprietaryDrivers && autoInstallLayout.hasOpenSourceDrivers) || root.selectedDevice === "nvidia-gpu"
                                enabled: autoInstallLayout.hasOpenSourceDrivers

                                background: Rectangle {
                                    color: openSourceButton.enabled ? Kirigami.Theme.focusColor : Kirigami.Theme.disabledTextColor
                                    radius: 4
                                }

                                contentItem: QQC2.Label {
                                    text: openSourceButton.text
                                    color: "white"
                                    font.weight: Font.Medium
                                    horizontalAlignment: Text.AlignHCenter
                                    verticalAlignment: Text.AlignVCenter
                                }
                            }

                            QQC2.Button {
                                id: singleAutoInstallButton
                                text: "Auto install"
                                visible: !(autoInstallLayout.hasProprietaryDrivers && autoInstallLayout.hasOpenSourceDrivers) && root.selectedDevice !== "nvidia-gpu"

                                background: Rectangle {
                                    color: Kirigami.Theme.positiveTextColor
                                    radius: 4
                                }

                                contentItem: QQC2.Label {
                                    text: singleAutoInstallButton.text
                                    color: "white"
                                    font.weight: Font.Medium
                                    horizontalAlignment: Text.AlignHCenter
                                    verticalAlignment: Text.AlignVCenter
                                }
                            }
                        }
                    }

                    contentItem: ColumnLayout {
                        id: driverOptionsContent
                        anchors.margins: Kirigami.Units.largeSpacing
                        spacing: Kirigami.Units.largeSpacing

                        Repeater {
                            id: driversRepeater
                            model: driverOptionsCard.selectedDeviceData.drivers || []

                            delegate: Rectangle {
                                id: driverItem
                                
                                required property var modelData
                                
                                Layout.fillWidth: true
                                height: driverContent.implicitHeight + Kirigami.Units.largeSpacing * 2
                                color: driverItem.modelData.recommended ? Kirigami.Theme.positiveBackgroundColor : Kirigami.Theme.backgroundColor
                                border.color: driverItem.modelData.recommended ? Kirigami.Theme.positiveTextColor : Kirigami.Theme.alternateBackgroundColor
                                border.width: 1
                                radius: 4

                                RowLayout {
                                    id: driverContent
                                    anchors.fill: parent
                                    anchors.margins: Kirigami.Units.largeSpacing
                                    spacing: Kirigami.Units.largeSpacing

                                    ColumnLayout {
                                        id: driverInfoLayout
                                        Layout.fillWidth: true
                                        spacing: Kirigami.Units.smallSpacing

                                        RowLayout {
                                            id: driverNameLayout
                                            spacing: Kirigami.Units.largeSpacing

                                            QQC2.Label {
                                                id: driverNameLabel
                                                text: driverItem.modelData.name || ""
                                                font.weight: Font.Medium
                                            }

                                            QQC2.Label {
                                                id: recommendedLabel
                                                text: "Recommended"
                                                visible: driverItem.modelData.recommended || false
                                                color: Kirigami.Theme.positiveTextColor
                                                font.pointSize: Kirigami.Theme.smallFont.pointSize
                                                font.weight: Font.Medium
                                                leftPadding: Kirigami.Units.smallSpacing
                                                rightPadding: Kirigami.Units.smallSpacing
                                                topPadding: Kirigami.Units.smallSpacing / 2
                                                bottomPadding: Kirigami.Units.smallSpacing / 2

                                                background: Rectangle {
                                                    color: Kirigami.Theme.positiveBackgroundColor
                                                    radius: 4
                                                }
                                            }
                                        }

                                        RowLayout {
                                            id: driverPropertiesLayout
                                            spacing: Kirigami.Units.largeSpacing * 2

                                            QQC2.Label {
                                                id: driverTypeLabel
                                                text: (driverItem.modelData.openSource || false) ? "Open Source" : "Proprietary"
                                                font.pointSize: Kirigami.Theme.smallFont.pointSize
                                                font.weight: Font.Medium
                                                color: (driverItem.modelData.openSource || false) ? Kirigami.Theme.focusColor : "#e97517"
                                                leftPadding: Kirigami.Units.smallSpacing
                                                rightPadding: Kirigami.Units.smallSpacing
                                                topPadding: Kirigami.Units.smallSpacing / 2
                                                bottomPadding: Kirigami.Units.smallSpacing / 2

                                                background: Rectangle {
                                                    color: (driverItem.modelData.openSource || false) ? Qt.rgba(Kirigami.Theme.focusColor.r, Kirigami.Theme.focusColor.g, Kirigami.Theme.focusColor.b, 0.2) : "#e9751720"
                                                    radius: 4
                                                }
                                            }

                                            RowLayout {
                                                id: driverStatusLayout
                                                spacing: Kirigami.Units.smallSpacing

                                                Kirigami.Icon {
                                                    id: driverStatusIcon
                                                    source: (driverItem.modelData.installed || false) ? "checkmark" : "radio"
                                                    width: Kirigami.Units.iconSizes.small
                                                    height: Kirigami.Units.iconSizes.small
                                                    color: (driverItem.modelData.installed || false) ? Kirigami.Theme.positiveTextColor : Kirigami.Theme.disabledTextColor
                                                }

                                                QQC2.Label {
                                                    id: driverStatusLabel
                                                    text: (driverItem.modelData.installed || false) ? "Installed" : "Not Installed"
                                                    font.pointSize: Kirigami.Theme.smallFont.pointSize
                                                    font.weight: Font.Medium
                                                    color: (driverItem.modelData.installed || false) ? Kirigami.Theme.positiveTextColor : Kirigami.Theme.disabledTextColor
                                                }
                                            }
                                        }
                                    }

                                    QQC2.Button {
                                        id: installButton
                                        property string driverKey: root.selectedDevice + "-" + (driverItem.modelData.id || "")
                                        property bool isInstalling: root.installingDrivers[installButton.driverKey] || false

                                        visible: !(driverItem.modelData.installed || false)
                                        text: installButton.isInstalling ? "Installing..." : "Install"
                                        enabled: !installButton.isInstalling
                                        icon.name: "download"
                                        onClicked: root.installDriver(root.selectedDevice, driverItem.modelData.id || "")

                                        background: Rectangle {
                                            color: installButton.enabled ? Kirigami.Theme.positiveTextColor : Kirigami.Theme.disabledTextColor
                                            radius: 4
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }

            // Other Devices Section
            ColumnLayout {
                id: otherDevicesSection
                Layout.fillWidth: true
                spacing: Kirigami.Units.largeSpacing

                Kirigami.Heading {
                    id: otherDevicesTitle
                    level: 3
                    text: "Other Devices"
                }

                ColumnLayout {
                    id: otherDevicesLayout
                    spacing: Kirigami.Units.smallSpacing

                    Repeater {
                        id: otherDevicesRepeater
                        model: root.devicesWithoutDrivers

                        delegate: Rectangle {
                            id: otherDeviceItem
                            
                            required property var modelData
                            
                            Layout.fillWidth: true
                            height: otherDeviceContent.implicitHeight + Kirigami.Units.largeSpacing * 2
                            color: Kirigami.Theme.backgroundColor
                            border.color: Kirigami.Theme.backgroundColor
                            border.width: 1
                            radius: 4

                            RowLayout {
                                id: otherDeviceContent
                                anchors.fill: parent
                                anchors.margins: Kirigami.Units.largeSpacing
                                spacing: Kirigami.Units.largeSpacing

                                Rectangle {
                                    id: otherDeviceIconBackground
                                    Layout.preferredWidth: Kirigami.Units.iconSizes.small + Kirigami.Units.largeSpacing
                                    Layout.preferredHeight: Kirigami.Units.iconSizes.small + Kirigami.Units.largeSpacing
                                    color: Kirigami.Theme.alternateBackgroundColor
                                    radius: 4

                                    Kirigami.Icon {
                                        id: otherDeviceIcon
                                        anchors.centerIn: parent
                                        source: otherDeviceItem.modelData.icon || ""
                                        width: Kirigami.Units.iconSizes.small
                                        height: Kirigami.Units.iconSizes.small
                                    }
                                }

                                ColumnLayout {
                                    id: otherDeviceInfoLayout
                                    Layout.fillWidth: true
                                    spacing: 0

                                    QQC2.Label {
                                        id: otherDeviceCategory
                                        text: otherDeviceItem.modelData.category || ""
                                        font.pointSize: Kirigami.Theme.smallFont.pointSize
                                        color: Kirigami.Theme.disabledTextColor
                                    }

                                    QQC2.Label {
                                        id: otherDeviceName
                                        text: otherDeviceItem.modelData.name || ""
                                        font.weight: Font.Medium
                                    }

                                    QQC2.Label {
                                        id: otherDeviceVendor
                                        text: otherDeviceItem.modelData.vendor || ""
                                        font.pointSize: Kirigami.Theme.smallFont.pointSize
                                        color: Kirigami.Theme.disabledTextColor
                                    }
                                }

                                QQC2.Label {
                                    id: noDriversLabel
                                    text: "No drivers available"
                                    font.pointSize: Kirigami.Theme.smallFont.pointSize
                                    color: Kirigami.Theme.disabledTextColor
                                }
                            }
                        }
                    }
                }
            }

            // Bottom Controls
            RowLayout {
                id: bottomControlsLayout
                Layout.fillWidth: true
                Layout.topMargin: Kirigami.Units.largeSpacing

                QQC2.CheckBox {
                    id: showAllDevicesCheckbox
                    text: "Show all devices"
                    checked: root.showAllDevices
                    onCheckedChanged: root.showAllDevices = showAllDevicesCheckbox.checked
                }

                Item {
                    id: bottomSpacer
                    Layout.fillWidth: true
                }

                QQC2.Button {
                    id: allSettingsButton
                    text: "All Settings"
                    flat: true
                }

                QQC2.Button {
                    id: quitButton
                    text: "Quit"
                    highlighted: true
                    onClicked: Qt.quit()
                }
            }
        }
    }

    // Header
    header: Item {
        id: appHeader
        height: headerLayout.implicitHeight + Kirigami.Units.largeSpacing * 2

        RowLayout {
            id: headerLayout
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            anchors.margins: Kirigami.Units.largeSpacing

            Kirigami.Icon {
                id: headerIcon
                source: "configure"
                Layout.preferredWidth: Kirigami.Units.iconSizes.small
                Layout.preferredHeight: Kirigami.Units.iconSizes.small
                color: Kirigami.Theme.textColor
            }

            ColumnLayout {
                id: headerTextLayout
                Layout.fillWidth: true
                spacing: 0

                Kirigami.Heading {
                    id: headerTitle
                    level: 2
                    text: "Hardware Configuration"
                }

                QQC2.Label {
                    id: headerSubtitle
                    text: "Manage your system drivers"
                    color: Kirigami.Theme.disabledTextColor
                    font.pointSize: Kirigami.Theme.smallFont.pointSize
                }
            }

            QQC2.ToolButton {
                id: closeButton
                icon.name: "window-close"
                onClicked: Qt.quit()
            }
        }
    }
}
