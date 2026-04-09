pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls as QQC2
import QtQuick.Layouts

import org.kde.kirigami as Kirigami
import org.manjaro.mcp.components as Components
import org.manjaro.mcp.mhwd as Mhwd

/*
 * Unified MCP shell with sidebar navigation for kernel and hardware modules.
 */
Kirigami.ApplicationWindow {
    id: root

    // Internal
    property string __activePage: "kernel"

    width: 1000
    height: 700
    title: qsTr("Manjaro Control Panel")

    globalDrawer: Kirigami.OverlayDrawer {
        id: drawer

        modal: false
        drawerOpen: true

        contentItem: ColumnLayout {
            Layout.preferredWidth: Kirigami.Units.gridUnit * 20

            QQC2.ItemDelegate {
                Layout.fillWidth: true
                text: qsTr("Kernel Manager")
                icon.name: "chip"
                highlighted: root.__activePage === "kernel"

                onClicked: {
                    root.__activePage = "kernel";
                    root.pageStack.replace(kernelPageComponent);
                }
            }
            QQC2.ItemDelegate {
                Layout.fillWidth: true
                text: qsTr("Hardware Configuration")
                icon.name: "computer"
                highlighted: root.__activePage === "hardware"

                onClicked: {
                    root.__activePage = "hardware";
                    root.pageStack.replace(hardwarePageComponent);
                }
            }
            Item {
                Layout.fillHeight: true
            }
        }
    }

    Component {
        id: kernelPageComponent

        KernelPage {}
    }

    Component {
        id: hardwarePageComponent

        HardwarePage {}
    }

    pageStack.initialPage: kernelPageComponent
}
