import QtQuick
import QtQuick.Controls as QQC2
import QtQuick.Layouts
import org.kde.kirigami as Kirigami

ListView {
    id: root

    // Configuration
    property bool actionsEnabled: true

    signal showChangelog(changelogUrl: string)
    signal install(kernelData: var)
    signal remove(kernelData: var)

    boundsBehavior: ListView.StopAtBounds
    spacing: Kirigami.Units.largeSpacing
    clip: true

    section.property: "category"
    section.criteria: ViewSection.FullString
    section.delegate: Kirigami.ListSectionHeader {
        width: ListView.view.width
        
        label: {
            if (section === "Installed") return qsTr("Installed")
            if (section === "LTS") return qsTr("LTS")
            return qsTr("Other")
        }
    }

    delegate: KernelDelegate {
        width: ListView.view.width

        actionsEnabled: root.actionsEnabled

        onShowChangelog: root.showChangelog(changelogUrl)
        onInstall: (kernelData) => root.install(kernelData)
        onRemove: (kernelData) => root.remove(kernelData)
    }
}
