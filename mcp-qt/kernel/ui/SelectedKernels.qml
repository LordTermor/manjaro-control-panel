import QtQuick
import QtQuick.Controls as QQC2

import org.kde.kirigami as Kirigami

Item {
    id: root

    signal showChangelog(string changelogUrl)
    signal install(string name, var extraModules)
    signal remove(string name, var extraModules)

    required property var inUseKernel
    required property var recommendedKernel
    property bool actionsEnabled: true

    Row {
        anchors.fill: parent
        
        spacing: Kirigami.Units.largeSpacing

        // In-use kernel
        Item {
            width: (parent.width - Kirigami.Units.largeSpacing) / 2
            height: parent.height
            visible: root.inUseKernel.valid

            KernelDelegate {
                id: inUseCard
                
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.top: parent.top
                anchors.bottom: inUseLabel.visible ? inUseLabel.top : parent.bottom
                anchors.bottomMargin: inUseLabel.visible ? Kirigami.Units.smallSpacing : 0
                
                name: root.inUseKernel.name
                version: root.inUseKernel.version
                majorVersion: root.inUseKernel.majorVersion
                minorVersion: root.inUseKernel.minorVersion
                isInstalled: root.inUseKernel.isInstalled
                isInUse: root.inUseKernel.isInUse
                isRecommended: root.inUseKernel.isRecommended
                isLTS: root.inUseKernel.isLTS
                extraModules: root.inUseKernel.extraModules
                changelogUrl: root.inUseKernel.changelogUrl
                actionsEnabled: root.actionsEnabled

                onShowChangelog: {
                    root.showChangelog(root.inUseKernel.changelogUrl)
                }

                onInstall: {
                    root.install(root.inUseKernel.name, root.inUseKernel.extraModules)
                }

                onRemove: {
                    root.remove(root.inUseKernel.name, root.inUseKernel.extraModules)
                }
            }

            QQC2.Label {
                id: inUseLabel
                
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.bottom: parent.bottom
                
                visible: text !== ""
                text: qsTr("Current")
                font.pointSize: Kirigami.Theme.smallFont.pointSize
                font.weight: Font.Medium
                color: Kirigami.Theme.disabledTextColor
            }
        }

        // Recommended kernel
        Item {
            width: (parent.width - Kirigami.Units.largeSpacing) / 2
            height: parent.height
            visible: root.recommendedKernel.valid

            KernelDelegate {
                id: recommendedCard
                
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.top: parent.top
                anchors.bottom: recommendedLabel.visible ? recommendedLabel.top : parent.bottom
                anchors.bottomMargin: recommendedLabel.visible ? Kirigami.Units.smallSpacing : 0
                
                name: root.recommendedKernel.name
                version: root.recommendedKernel.version
                majorVersion: root.recommendedKernel.majorVersion
                minorVersion: root.recommendedKernel.minorVersion
                isInstalled: root.recommendedKernel.isInstalled
                isInUse: root.recommendedKernel.isInUse
                isRecommended: root.recommendedKernel.isRecommended
                isLTS: root.recommendedKernel.isLTS
                extraModules: root.recommendedKernel.extraModules
                changelogUrl: root.recommendedKernel.changelogUrl
                actionsEnabled: root.actionsEnabled

                onShowChangelog: {
                    root.showChangelog(root.recommendedKernel.changelogUrl)
                }

                onInstall: {
                    root.install(root.recommendedKernel.name, root.recommendedKernel.extraModules)
                }

                onRemove: {
                    root.remove(root.recommendedKernel.name, root.recommendedKernel.extraModules)
                }
            }

            QQC2.Label {
                id: recommendedLabel
                
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.bottom: parent.bottom
                
                visible: text !== ""
                text: {
                    if (root.recommendedKernel.isInstalled) return qsTr("Recommended (choose in boot menu)")
                    return qsTr("Recommended")
                }
                font.pointSize: Kirigami.Theme.smallFont.pointSize
                font.weight: Font.Medium
                color: Kirigami.Theme.disabledTextColor
            }
        }
    }
}
