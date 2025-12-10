import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as QQC2
import QtQuick.Effects
import org.kde.kirigami as Kirigami
import org.manjaro.mcp.components

Kirigami.AbstractCard {
    id: root

    // Configuration
    property bool actionsEnabled: true

    // Data - Kernel properties
    required property string name
    required property string version
    required property int majorVersion
    required property int minorVersion
    required property bool isInstalled
    required property bool isInUse
    required property bool isRecommended
    required property bool isLTS
    required property var extraModules
    required property string changelogUrl
    required property var kernelData

    // Internal
    readonly property bool isRealtime: root.name.includes("-rt")

    signal showChangelog()
    signal install(kernelData: var)
    signal remove(kernelData: var)

    highlighted: root.isRecommended

    contentItem: RowLayout {
        spacing: Kirigami.Units.largeSpacing

        Item {
            Layout.preferredWidth: 60
            Layout.preferredHeight: 60
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            
            QQC2.Label {
                anchors.centerIn: parent
                
                text: root.majorVersion + "." + root.minorVersion
                font.pointSize: 14
                font.weight: Font.Bold
                color: Kirigami.Theme.textColor
            }
        }

        ColumnLayout {
            Layout.fillWidth: true
            
            spacing: 8

            Flow {
                Layout.fillWidth: true
                spacing: 8

                QQC2.Label {
                    text: "Linux " + root.version
                    font.pointSize: 13
                    font.weight: Font.DemiBold
                }

                Badge {
                    visible: root.isRealtime
                    
                    text: qsTr("Real-Time")
                    badgeColor: "#3498db"
                }

                Badge {
                    visible: root.isLTS
                    
                    text: qsTr("LTS")
                    badgeColor: "#e97517"
                }
            }

            Flow {
                Layout.fillWidth: true
                
                spacing: 4
                
                QQC2.Label {
                    text: root.name + (root.isLTS ? " • Long-term support" : (root.isInUse ? " • Current kernel" : ""))
                    font.pointSize: Kirigami.Theme.smallFont.pointSize
                    color: Kirigami.Theme.disabledTextColor
                }
                
                QQC2.Label {
                    text: "•"
                    font.pointSize: Kirigami.Theme.smallFont.pointSize
                    color: Kirigami.Theme.disabledTextColor
                }
                
                Row {
                    spacing: 4
                    
                    QQC2.Label {
                        id: changelogLink
                        
                        text: qsTr("Changelog")
                        font.pointSize: Kirigami.Theme.smallFont.pointSize
                        color: Kirigami.Theme.linkColor
                        
                        MouseArea {
                            anchors.fill: parent
                            
                            cursorShape: Qt.PointingHandCursor
                            hoverEnabled: true
                            
                            onClicked: root.showChangelog()
                            onEntered: changelogLink.font.underline = true
                            onExited: changelogLink.font.underline = false
                        }
                    }
                    
                    Kirigami.Icon {
                        source: "internet-services"
                        implicitWidth: Kirigami.Units.iconSizes.small
                        implicitHeight: Kirigami.Units.iconSizes.small
                        color: Kirigami.Theme.linkColor
                    }
                }
            }
        }

        Item {
            Layout.fillWidth: true
        }

        RowLayout {
            Layout.rightMargin: Kirigami.Units.largeSpacing
            
            spacing: Kirigami.Units.smallSpacing

            QQC2.Button {
                visible: root.isInstalled && !root.isInUse && !root.isRecommended
                enabled: root.actionsEnabled && vm.currentTransactionKernelName === ""
                
                icon.name: vm.currentTransactionKernelName === root.name ? "view-refresh" : "delete"
                icon.color: "#e74c3c"
                display: QQC2.AbstractButton.IconOnly
                
                QQC2.ToolTip.visible: hovered
                QQC2.ToolTip.text: qsTr("Remove")
                
                onClicked: root.remove(root.kernelData)
            }

            QQC2.Button {
                visible: !root.isInstalled
                enabled: root.actionsEnabled && vm.currentTransactionKernelName === ""
                
                icon.name: vm.currentTransactionKernelName === root.name ? "view-refresh" : "download"
                icon.color: Kirigami.Theme.positiveTextColor
                display: QQC2.AbstractButton.IconOnly
                
                QQC2.ToolTip.visible: hovered
                QQC2.ToolTip.text: qsTr("Install")
                
                onClicked: root.install(root.kernelData)
            }
        }
    }


    layer.enabled: root.isRecommended || root.isInUse
    layer.effect: MultiEffect {
        shadowEnabled: true
        shadowColor: root.isInUse ? "#3498db" : Kirigami.Theme.positiveTextColor
        shadowBlur: 0.4
        shadowOpacity: 0.5
    }
}
