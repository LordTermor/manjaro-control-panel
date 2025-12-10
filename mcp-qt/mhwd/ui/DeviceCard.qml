pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls as QQC2
import QtQuick.Layouts
import QtQuick.Effects
import org.kde.kirigami as Kirigami
import org.manjaro.mcp.components

Kirigami.AbstractCard {
    id: root

    // Required interface
    required property var deviceData
    required property bool isSelected
    required property string selectedDevice
    property int driverCount: 0

    padding: 0

    signal deviceClicked()

    // Internal
    property bool isHovered: false
    
    readonly property bool hasInstalledDriver: {
        const drivers = root.deviceData?.drivers ?? [];
        for (let i = 0; i < drivers.length; i++) {
            if (!!drivers[i].installed) return true;
        }
        return false;
    }


    background: Rectangle {
        radius: 4

        Behavior on color {
            ColorAnimation {
                duration: 200
                
            }
        }
        
        Behavior on border.color {
            ColorAnimation {
                duration: 200
                
            }
        }
        
        Behavior on border.width {
            NumberAnimation {
                duration: 200
                easing.type: Easing.InOutQuad
            }
        }

        states: [
            State {
                name: "selected"
                when: root.isSelected
                PropertyChanges {
                    target: root.background
                    color: Kirigami.Theme.activeBackgroundColor
                    border.color: Kirigami.Theme.focusColor
                    border.width: 2
                }
            },
            State {
                name: "hoveredWithDrivers"
                when: root.isHovered && !!root.deviceData?.hasDrivers
                PropertyChanges {
                    target: root.background
                    color: Kirigami.Theme.focusColor
                    border.color: Kirigami.Theme.focusColor
                    border.width: 1
                }
            },
            State {
                name: "hoveredNoDrivers"
                when: root.isHovered && !root.deviceData?.hasDrivers
                PropertyChanges {
                    target: root.background
                    color: Kirigami.Theme.alternateBackgroundColor
                    border.color: Kirigami.Theme.textColor
                    border.width: 1
                }
            },
            State {
                name: "default"
                when: !root.isSelected && !root.isHovered
                PropertyChanges {
                    target: root.background
                    color: Kirigami.Theme.backgroundColor
                    border.color: Kirigami.Theme.alternateBackgroundColor
                    border.width: 1
                }
            }
        ]
    }
    /*  */
    MouseArea {
        anchors.fill: parent
        
        cursorShape: !!root.deviceData?.hasDrivers ? Qt.PointingHandCursor : Qt.ArrowCursor
        hoverEnabled: true
        
        onClicked: {
            if (!!root.deviceData?.hasDrivers) {
                root.deviceClicked();
            }
        }
        onEntered: root.isHovered = true
        onExited: root.isHovered = false
    }

    contentItem: ColumnLayout {
        spacing: 0
        anchors.margins: 0
        RowLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.margins: Kirigami.Units.largeSpacing
            spacing: Kirigami.Units.largeSpacing

            Rectangle {
            Layout.preferredWidth: Kirigami.Units.iconSizes.large
            Layout.preferredHeight: Kirigami.Units.iconSizes.large
            Layout.alignment: Qt.AlignTop
            
            color: root.isSelected
                ? Kirigami.Theme.focusColor
                : (root.isHovered && !!root.deviceData?.hasDrivers)
                    ? Kirigami.Theme.focusColor
                    : Kirigami.Theme.alternateBackgroundColor
            radius: 4

            Behavior on color {
                ColorAnimation {
                    duration: 200
                    easing.type: Easing.InOutQuad
                }
            }

            Kirigami.Icon {
                anchors.centerIn: parent
                source: root.deviceData?.icon ?? ""
                width: Kirigami.Units.iconSizes.medium
                height: Kirigami.Units.iconSizes.medium
            }
        }

        ColumnLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: Kirigami.Units.smallSpacing

            RowLayout {
                Layout.fillWidth: true
                spacing: Kirigami.Units.smallSpacing

                QQC2.Label {
                    text: root.deviceData?.name ?? ""
                    font.weight: Font.Medium
                    wrapMode: Text.WordWrap
                    elide: Text.ElideRight
                    maximumLineCount: 2
                    Layout.fillWidth: true
                }
            }

            QQC2.Label {
                text: root.deviceData?.vendor ?? ""
                font.pointSize: Kirigami.Theme.smallFont.pointSize
                color: Kirigami.Theme.disabledTextColor
                elide: Text.ElideRight
                Layout.fillWidth: true
            }

            Flow {
                Layout.fillWidth: true
                visible: !!root.deviceData?.driver
                spacing: Kirigami.Units.smallSpacing

                QQC2.Label {
                    text: qsTr("Current driver:")
                    font.pointSize: Kirigami.Theme.smallFont.pointSize
                    color: Kirigami.Theme.disabledTextColor
                }

                Badge {
                    text: root.deviceData?.driver ?? ""
                    badgeColor: Kirigami.Theme.disabledTextColor
                }
            }

          

            QQC2.Label {
                visible: !root.deviceData?.hasDrivers && !root.deviceData?.driver
                text: qsTr("No drivers available")
                font.pointSize: Kirigami.Theme.smallFont.pointSize
                color: Kirigami.Theme.disabledTextColor
            }
        }
        }
        Item {
                Layout.fillHeight: true
        }
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: Kirigami.Units.iconSizes.medium
            
            
            visible: !!root.deviceData?.hasDrivers && root.driverCount > 0
            color: Qt.rgba(0.2, 0.7, 0.4, 0.08)
            
            RowLayout {
                anchors.fill: parent
                anchors.leftMargin: Kirigami.Units.largeSpacing
                anchors.rightMargin: Kirigami.Units.largeSpacing
                spacing: Kirigami.Units.smallSpacing

               

                QQC2.Label {
                    text: root.driverCount + (root.driverCount === 1 ? " driver available" : " drivers available")
                    font.pointSize: Kirigami.Theme.smallFont.pointSize
                    font.weight: Font.Medium
                    color: "#27ae60"
                }

                Item {
                    Layout.fillWidth: true
                }

                Kirigami.Icon {
                    source: "download"
                    Layout.preferredWidth: Kirigami.Units.iconSizes.small
                    Layout.preferredHeight: Kirigami.Units.iconSizes.small
                    color: "#27ae60"
                }

            }
        }
    }

    layer.enabled: root.deviceData.hasDrivers
    layer.effect: MultiEffect {
        shadowEnabled: true
        shadowColor: root.hasInstalledDriver ? Kirigami.Theme.positiveTextColor : Kirigami.Theme.focusColor
        shadowBlur: 0.4
        shadowOpacity: root.isSelected ? 0.6 : 0.3
    }
}
