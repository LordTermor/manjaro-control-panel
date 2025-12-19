import QtQuick
import QtQuick.Controls as QQC2

import org.kde.kirigami as Kirigami

Item {
    id: root

    // Configuration
    property bool actionsEnabled: true

    // Data
    required property var inUseKernel
    required property var recommendedKernel

    signal showChangelog(changelogUrl: string)
    signal install(kernelData: var)
    signal remove(kernelData: var)

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
                kernelData: root.inUseKernel
                actionsEnabled: root.actionsEnabled

                onShowChangelog: {
                    root.showChangelog(root.inUseKernel.changelogUrl)
                }

                onInstall: (kernelData) => {
                    root.install(root.inUseKernel)
                }

                onRemove: (kernelData) => {
                    root.remove(root.inUseKernel)
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
                kernelData: root.recommendedKernel
                actionsEnabled: root.actionsEnabled

                onShowChangelog: {
                    root.showChangelog(root.recommendedKernel.changelogUrl)
                }

                onInstall: (kernelData) => {
                    root.install(root.recommendedKernel)
                }

                onRemove: (kernelData) => {
                    root.remove(root.recommendedKernel)
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
