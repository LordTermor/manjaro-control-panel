import QtQuick
import QtQuick.Controls as QQC2
import QtQuick.Layouts

import org.kde.kirigami as Kirigami
import org.manjaro.mcp.components as Components

QQC2.Page {
    id: root

    padding: 0

    Kirigami.PromptDialog {
        id: errorDialog
        
        title: qsTr("Error")
        standardButtons: Kirigami.Dialog.NoButton
    }

    Components.ConfirmationDialog {
        id: confirmationDialog

        property bool uninstallation: false
        property var kernelData: null

        onAccepted: {
            if (!confirmationDialog.uninstallation) {
                vm.installKernel(confirmationDialog.kernelData)
            } else {
                vm.removeKernel(confirmationDialog.kernelData)
            }
        }

        function open(kernelData: var, uninstallation: bool) {
            confirmationDialog.kernelData = kernelData
            confirmationDialog.uninstallation = uninstallation
            confirmationDialog.visible = true
        }

        ColumnLayout {
            spacing: Kirigami.Units.largeSpacing

            QQC2.Label {
                Layout.fillWidth: true

                textFormat: Text.MarkdownText
                wrapMode: Text.WordWrap
                text: !confirmationDialog.uninstallation 
                    ? qsTr("New kernel **%1** is ready to install.").arg(confirmationDialog.kernelData?.name ?? "")
                    : qsTr("Do you want to remove **%1**?").arg(confirmationDialog.kernelData?.name ?? "")
            }
            
            Kirigami.ShadowedRectangle {
                Layout.fillWidth: true
                Layout.preferredHeight: extraPackagesColumn.implicitHeight + Kirigami.Units.largeSpacing * 2

                visible: (extraPackagesList.count > 0) && !confirmationDialog.uninstallation

                color: Kirigami.Theme.backgroundColor
                radius: Kirigami.Units.mediumSpacing
                
                shadow.size: Kirigami.Units.smallSpacing
                shadow.color: Qt.rgba(0, 0, 0, 0.1)

                ColumnLayout {
                    id: extraPackagesColumn

                    anchors.fill: parent
                    anchors.margins: Kirigami.Units.largeSpacing

                    spacing: Kirigami.Units.mediumSpacing

                    QQC2.Label {
                        Layout.fillWidth: true

                        font.weight: Font.DemiBold
                        text: qsTr("Following extra packages will be installed:")
                    }

                    ListView {
                        id: extraPackagesList

                        Layout.fillWidth: true
                        Layout.preferredHeight: contentHeight

                        model: confirmationDialog.kernelData?.extraModules ?? []
                        interactive: false
                        spacing: Kirigami.Units.smallSpacing

                        delegate: RowLayout {
                            width: ListView.view.width
                            spacing: Kirigami.Units.mediumSpacing

                            Kirigami.Icon {
                                Layout.preferredWidth: Kirigami.Units.iconSizes.small
                                Layout.preferredHeight: Kirigami.Units.iconSizes.small

                                source: "package-installed-updated"
                                color: Kirigami.Theme.textColor
                            }

                            QQC2.Label {
                                Layout.fillWidth: true

                                text: modelData
                                elide: Text.ElideRight
                            }
                        }
                    }
                }
            }
        }
    }

    ColumnLayout {
        anchors.fill: parent
        
        spacing: Kirigami.Units.smallSpacing

        // Loading overlay
        Rectangle {
            id: loadingOverlay
            
            Layout.fillWidth: true
            Layout.fillHeight: true
            
            visible: loadingProgress.visible
            color: Kirigami.Theme.backgroundColor
            z: 999
            
            ColumnLayout {
                anchors.centerIn: parent
                spacing: Kirigami.Units.largeSpacing
                width: parent.width * 0.6
                
                QQC2.BusyIndicator {
                    Layout.alignment: Qt.AlignHCenter
                    running: loadingOverlay.visible
                }
                
                QQC2.Label {
                    id: loadingProgress
                    
                    Layout.fillWidth: true
                    Layout.alignment: Qt.AlignHCenter
                    
                    visible: false
                    horizontalAlignment: Text.AlignHCenter
                    wrapMode: Text.WordWrap
                    
                    Connections {
                        target: vm
                        
                        function onFetchProgress(current, total, kernelName) {
                            if (total === 0) {
                                loadingProgress.visible = false
                                return
                            }
                            
                            loadingProgress.visible = true
                            
                            if (current >= total) {
                                loadingProgress.visible = false
                            } else {
                                loadingProgress.text = qsTr("Loading kernel metadata: %1 / %2\n%3").arg(current).arg(total).arg(kernelName)
                            }
                        }
                    }
                }
            }
        }

        SelectedKernels {
            Layout.fillWidth: true
            Layout.preferredHeight: 100
            Layout.margins: Kirigami.Units.largeSpacing
            
            inUseKernel: vm.inUseKernelData
            recommendedKernel: vm.recommendedKernelData
            actionsEnabled: !confirmationDialog.visible && !errorDialog.visible

            onShowChangelog: (changelogUrl) => {
                Qt.openUrlExternally(changelogUrl)
            }

            onInstall: (kernelData) => confirmationDialog.open(kernelData, false)

            onRemove: (kernelData) => confirmationDialog.open(kernelData, true)
        }

        Item {
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.margins: Kirigami.Units.smallSpacing

            KernelListView {
                id: kernelListView
                
                anchors.fill: parent
                anchors.rightMargin: kernelScrollBar.width
                
                model: vm.model
                actionsEnabled: !confirmationDialog.visible && !errorDialog.visible

                onShowChangelog: (changelogUrl) => {
                    Qt.openUrlExternally(changelogUrl)
                }

                onInstall: (kernelData) => confirmationDialog.open(kernelData, false)
                
                onRemove: (kernelData) => confirmationDialog.open(kernelData, true)
                
            }

            QQC2.ScrollBar {
                id: kernelScrollBar
                
                anchors.right: parent.right
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                
                policy: QQC2.ScrollBar.AlwaysOn
                orientation: Qt.Vertical
                size: kernelListView.height / kernelListView.contentHeight
                position: kernelListView.contentY / kernelListView.contentHeight
                active: true
                
                onPositionChanged: {
                    if (pressed) {
                        kernelListView.contentY = position * kernelListView.contentHeight
                    }
                }
            }
        }
    }
}
