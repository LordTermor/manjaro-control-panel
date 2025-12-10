pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls as QQC2
import QtQuick.Layouts
import QtQuick.Effects
import org.kde.kirigami as Kirigami

Kirigami.AbstractCard {
    id: root

    required property var deviceData
    required property bool isSelected
    required property string selectedDevice

    signal deviceClicked()

    property bool isHovered: false
    property bool hasInstalledDriver: {
        var drivers = root.deviceData.drivers || [];
        for (var i = 0; i < drivers.length; i++) {
            if (drivers[i].installed) return true;
        }
        return false;
    }

    width: Math.max(280, Math.min(400, (parent.width - parent.spacing * 2) / 3))
    height: 120

    background: Rectangle {
        color: {
            if (root.isSelected) {
                return Kirigami.Theme.activeBackgroundColor;
            }
            if (root.isHovered) {
                return root.deviceData.hasDrivers 
                    ? Qt.rgba(Kirigami.Theme.focusColor.r, Kirigami.Theme.focusColor.g, Kirigami.Theme.focusColor.b, 0.1)
                    : Qt.rgba(Kirigami.Theme.textColor.r, Kirigami.Theme.textColor.g, Kirigami.Theme.textColor.b, 0.05);
            }
            return Kirigami.Theme.backgroundColor;
        }
        border.color: {
            if (root.isSelected) {
                return Kirigami.Theme.focusColor;
            }
            if (root.isHovered && root.deviceData.hasDrivers) {
                return Qt.rgba(Kirigami.Theme.focusColor.r, Kirigami.Theme.focusColor.g, Kirigami.Theme.focusColor.b, 0.5);
            }
            if (root.isHovered) {
                return Qt.rgba(Kirigami.Theme.textColor.r, Kirigami.Theme.textColor.g, Kirigami.Theme.textColor.b, 0.2);
            }
            return Kirigami.Theme.alternateBackgroundColor;
        }
        border.width: root.isSelected ? 2 : 1
        radius: 4

        Behavior on color {
            ColorAnimation {
                duration: 200
                easing.type: Easing.InOutQuad
            }
        }
        
        Behavior on border.color {
            ColorAnimation {
                duration: 200
                easing.type: Easing.InOutQuad
            }
        }
        
        Behavior on border.width {
            NumberAnimation {
                duration: 200
                easing.type: Easing.InOutQuad
            }
        }
    }
    
    MouseArea {
        anchors.fill: parent
        
        cursorShape: root.deviceData.hasDrivers ? Qt.PointingHandCursor : Qt.ArrowCursor
        hoverEnabled: true
        
        onClicked: {
            if (root.deviceData.hasDrivers) {
                root.deviceClicked();
            }
        }
        onEntered: root.isHovered = true
        onExited: root.isHovered = false
    }

    contentItem: RowLayout {
        anchors.margins: Kirigami.Units.largeSpacing
        spacing: Kirigami.Units.largeSpacing

        Rectangle {
            Layout.preferredWidth: Kirigami.Units.iconSizes.large
            Layout.preferredHeight: Kirigami.Units.iconSizes.large
            Layout.alignment: Qt.AlignTop
            
            color: {
                if (root.isSelected) {
                    return Kirigami.Theme.focusColor;
                }
                if (root.isHovered && root.deviceData.hasDrivers) {
                    return Qt.rgba(Kirigami.Theme.focusColor.r, Kirigami.Theme.focusColor.g, Kirigami.Theme.focusColor.b, 0.3);
                }
                return Kirigami.Theme.alternateBackgroundColor;
            }
            radius: 4

            Behavior on color {
                ColorAnimation {
                    duration: 200
                    easing.type: Easing.InOutQuad
                }
            }

            Kirigami.Icon {
                anchors.centerIn: parent
                source: root.deviceData.icon || ""
                width: Kirigami.Units.iconSizes.medium
                height: Kirigami.Units.iconSizes.medium
            }
        }

        ColumnLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: Kirigami.Units.smallSpacing

            QQC2.Label {
                text: root.deviceData.name || ""
                font.weight: Font.Medium
                wrapMode: Text.WordWrap
                elide: Text.ElideRight
                maximumLineCount: 2
                Layout.fillWidth: true
            }

            QQC2.Label {
                text: root.deviceData.vendor || ""
                font.pointSize: Kirigami.Theme.smallFont.pointSize
                color: Kirigami.Theme.disabledTextColor
                elide: Text.ElideRight
                Layout.fillWidth: true
            }

            Item {
                Layout.fillHeight: true
            }

            RowLayout {
                spacing: Kirigami.Units.smallSpacing
                visible: root.deviceData.hasDrivers

                property int driverCount: root.deviceData.drivers ? root.deviceData.drivers.length : 0
                property bool hasInstalled: {
                    var drivers = root.deviceData.drivers || [];
                    for (var i = 0; i < drivers.length; i++) {
                        if (drivers[i].installed) return true;
                    }
                    return false;
                }

                Kirigami.Icon {
                    source: parent.hasInstalled ? "checkmark" : "download"
                    width: Kirigami.Units.iconSizes.small
                    height: Kirigami.Units.iconSizes.small
                    color: parent.hasInstalled ? Kirigami.Theme.positiveTextColor : Kirigami.Theme.focusColor
                }

                QQC2.Label {
                    text: parent.hasInstalled 
                        ? qsTr("Driver installed")
                        : (parent.driverCount + " driver" + (parent.driverCount !== 1 ? "s" : "") + " available")
                    font.pointSize: Kirigami.Theme.smallFont.pointSize
                    font.weight: Font.Medium
                    color: parent.hasInstalled ? Kirigami.Theme.positiveTextColor : Kirigami.Theme.focusColor
                }
            }

            QQC2.Label {
                text: qsTr("No drivers available")
                visible: !root.deviceData.hasDrivers
                font.pointSize: Kirigami.Theme.smallFont.pointSize
                color: Kirigami.Theme.disabledTextColor
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
