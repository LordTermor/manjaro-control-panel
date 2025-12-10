import QtQuick
import QtQuick.Controls as QQC2
import org.kde.kirigami as Kirigami

Rectangle {
    id: root

    // Configuration
    property string text: ""
    property color badgeColor: "#3498db"
    property real borderAlpha: 0.3
    property real backgroundAlpha: 0.15

    width: label.width + 16
    height: label.height + 4

    color: "transparent"
    border.color: Qt.rgba(root.badgeColor.r, root.badgeColor.g, root.badgeColor.b, root.borderAlpha)
    border.width: 1
    radius: 4

    Rectangle {
        anchors.fill: parent

        color: Qt.rgba(root.badgeColor.r, root.badgeColor.g, root.badgeColor.b, root.backgroundAlpha)
        radius: root.radius
    }

    QQC2.Label {
        id: label

        anchors.centerIn: parent

        text: root.text
        font.pointSize: 8
        font.weight: Font.Bold
        font.capitalization: Font.AllUppercase
        color: root.badgeColor
    }
}
