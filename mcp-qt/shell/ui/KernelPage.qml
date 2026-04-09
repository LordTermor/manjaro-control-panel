import QtQuick
import QtQuick.Controls as QQC2
import QtQuick.Layouts

import org.kde.kirigami as Kirigami
import org.manjaro.mcp.components as Components
import org.manjaro.mcp.kernel as KernelModule

/*
 * Kernel management page for the unified shell.
 * Wraps the kernel module's View component in a Kirigami.Page.
 */
Kirigami.Page {
    id: root

    title: qsTr("Kernel Manager")
    padding: 0

    KernelModule.View {
        anchors.fill: parent
    }
}
