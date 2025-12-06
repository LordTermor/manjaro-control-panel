import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15

import org.kde.kirigami 2.19 as Kirigami

Kirigami.PromptDialog {

    id: confirmationDialog

    property bool uninstallation: false
    property string text: qsTr("Would you like to continue installation?")

    title: !uninstallation ? qsTr("Installation") : qsTr("Uninstallation")
    standardButtons: Kirigami.Dialog.Yes | Kirigami.Dialog.Cancel
    
    preferredWidth: Kirigami.Units.gridUnit * 25

    Label {
        textFormat: Text.MarkdownText
        text: confirmationDialog.text
    }
}
