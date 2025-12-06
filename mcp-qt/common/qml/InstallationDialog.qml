import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import org.kde.kirigami 2.19 as Kirigami

Kirigami.Dialog {
    id: installDialog

    property alias details: detailsLabel.text
    property int progress: -1

    title: qsTr("Transaction")

    closePolicy: Popup.NoAutoClose
    showCloseButton: false
    standardButtons: Kirigami.Dialog.NoButton

    padding: 10

    ColumnLayout {
        ProgressBar {
            id: installationProgressBar
            from: 0
            to: 100
            value: installDialog.progress
            indeterminate: installDialog.progress < 0
            Layout.fillWidth: true
        }
        Label {
            text: qsTr("Processing...")
            id: detailsLabel
        }
    }
}
