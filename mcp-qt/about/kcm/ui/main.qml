import QtQuick
import QtQuick.Controls as QQC2
import QtQuick.Layouts

import org.kde.kirigami as Kirigami
import org.manjaro.mcp.components as Components

/**
 * About page for Manjaro Control Panel - Kernel Module
 * Displays information about MCP, authors, and libraries
 */
Kirigami.ScrollablePage {
    id: aboutPage
    
    title: qsTr("About MCP")
    
    ColumnLayout {
        anchors.fill: parent
        spacing: 0
        
        Item {
            Layout.fillHeight: true
            Layout.minimumHeight: Kirigami.Units.largeSpacing
        }
        
        // Content section - centered
        ColumnLayout {
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignHCenter
            Layout.maximumWidth: Kirigami.Units.gridUnit * 40
            spacing: Kirigami.Units.largeSpacing * 2
            Layout.margins: Kirigami.Units.largeSpacing * 2
            
            // Title and slogan
            ColumnLayout {
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignHCenter
                spacing: Kirigami.Units.smallSpacing
                
                RowLayout {
                    Layout.alignment: Qt.AlignHCenter
                    spacing: Kirigami.Units.largeSpacing
                    
                    Image {
                        Layout.preferredWidth: Kirigami.Units.iconSizes.huge
                        Layout.preferredHeight: Kirigami.Units.iconSizes.huge
                        source: "qrc:/assets/icon.svg"
                        sourceSize.width: Kirigami.Units.iconSizes.huge
                        sourceSize.height: Kirigami.Units.iconSizes.huge
                    }
                    
                    Kirigami.Heading {
                        level: 1
                        text: "Manjaro Control Panel"
                    }
                }
                
                Kirigami.Heading {
                    level: 2
                    Layout.alignment: Qt.AlignHCenter
                    text: qsTr("\"You're in control\"")
                    opacity: 0.7
                }
                
                QQC2.Label {
                    Layout.alignment: Qt.AlignHCenter
                    text: qsTr("Version %1").arg(Components.VersionInfo.mcpVersion)
                    font.pointSize: Kirigami.Theme.smallFont.pointSize
                    color: Kirigami.Theme.disabledTextColor
                }
                
                QQC2.Label {
                    Layout.fillWidth: true
                    Layout.alignment: Qt.AlignHCenter
                    horizontalAlignment: Text.AlignHCenter
                    textFormat: QQC2.Label.MarkdownText
                    wrapMode: QQC2.Label.WrapAtWordBoundaryOrAnywhere
                    text: qsTr("A control center for Manjaro Linux maintenance")
                }
            }
            
            Kirigami.Separator {
                Layout.fillWidth: true
            }
            
            // Authors and Libraries section
            RowLayout {
                Layout.fillWidth: true
                spacing: Kirigami.Units.gridUnit * 3
                Layout.alignment: Qt.AlignHCenter
                
                // Authors column
                ColumnLayout {
                    Layout.alignment: Qt.AlignTop
                    spacing: Kirigami.Units.smallSpacing
                    
                    Kirigami.Heading {
                        level: 3
                        Layout.alignment: Qt.AlignHCenter
                        text: qsTr("Authors")
                    }
                    
                    GridLayout {
                        Layout.alignment: Qt.AlignHCenter
                        columns: 2
                        columnSpacing: Kirigami.Units.largeSpacing
                        rowSpacing: Kirigami.Units.smallSpacing
                        
                        QQC2.Label {
                            font.bold: true
                            text: "Artem Grinev"
                        }
                        QQC2.Label {
                            text: qsTr("developer")
                        }
                        
                        QQC2.Label {
                            font.bold: true
                            text: "Bogdan Covaciu"
                        }
                        QQC2.Label {
                            text: qsTr("icon, design ideas")
                        }
                        
                        QQC2.Label {
                            font.bold: true
                            text: "Mark Wagie"
                        }
                        QQC2.Label {
                            text: qsTr("quality assurance")
                        }
                    }
                }
                
                // Libraries column
                ColumnLayout {
                    Layout.alignment: Qt.AlignTop
                    spacing: Kirigami.Units.smallSpacing
                    
                    Kirigami.Heading {
                        level: 3
                        Layout.alignment: Qt.AlignHCenter
                        text: qsTr("Libraries")
                    }
                    
                    GridLayout {
                        Layout.alignment: Qt.AlignHCenter
                        columns: 2
                        columnSpacing: Kirigami.Units.largeSpacing
                        rowSpacing: Kirigami.Units.smallSpacing
                        
                        QQC2.Label {
                            font.bold: true
                            text: "Qt"
                        }
                        QQC2.Label {
                            text: Components.VersionInfo.qtVersion
                        }
                        
                        QQC2.Label {
                            font.bold: true
                            text: "KDE Frameworks"
                        }
                        QQC2.Label {
                            text: Components.VersionInfo.kf6Version
                        }
                        
                        QQC2.Label {
                            font.bold: true
                            text: "Kirigami"
                        }
                        QQC2.Label {
                            text: Components.VersionInfo.kirigamiVersion
                        }
                        
                        QQC2.Label {
                            font.bold: true
                            text: "libpamac"
                        }
                        QQC2.Label {
                            text: Components.VersionInfo.libpamacVersion
                        }
                    }
                }
            }
            
            Kirigami.Separator {
                Layout.fillWidth: true
            }
            
            // Links section
            RowLayout {
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignHCenter
                spacing: Kirigami.Units.largeSpacing
                
                QQC2.Button {
                    icon.name: "git"
                    text: qsTr("Contribute")
                    onClicked: Qt.openUrlExternally("https://gitlab.com/applications/mcp")
                }
                
                QQC2.Button {
                    icon.name: "license"
                    text: "GNU GPLv3"
                    onClicked: Qt.openUrlExternally("https://www.gnu.org/licenses/gpl-3.0.en.html")
                }
            }
            
            // Kernelnewbies section
            ColumnLayout {
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignHCenter
                spacing: Kirigami.Units.smallSpacing
                
                Kirigami.Heading {
                    level: 3
                    Layout.alignment: Qt.AlignHCenter
                    text: qsTr("Thanks")
                }
                
                RowLayout {
                    Layout.alignment: Qt.AlignHCenter
                    spacing: Kirigami.Units.smallSpacing
                    
                    QQC2.Label {
                        id: kernelnewbiesLabel
                        
                        text: qsTr("kernelnewbies.org")
                        font.pointSize: Kirigami.Theme.defaultFont.pointSize
                        color: Kirigami.Theme.linkColor
                        
                        MouseArea {
                            anchors.fill: parent
                            
                            cursorShape: Qt.PointingHandCursor
                            hoverEnabled: true
                            
                            onClicked: Qt.openUrlExternally("https://kernelnewbies.org")
                            onEntered: kernelnewbiesLabel.font.underline = true
                            onExited: kernelnewbiesLabel.font.underline = false
                        }
                    }
                    
                    Kirigami.Icon {
                        source: "internet-services"
                        implicitWidth: Kirigami.Units.iconSizes.small
                        implicitHeight: Kirigami.Units.iconSizes.small
                        color: Kirigami.Theme.linkColor
                    }
                    
                    QQC2.Label {
                        text: qsTr("‒ for changelogs")
                        color: Kirigami.Theme.disabledTextColor
                    }
                }
            }
            
            Item {
                Layout.preferredHeight: Kirigami.Units.largeSpacing
            }
            
            // Mascot section
            Item {
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignHCenter
                Layout.preferredHeight: Kirigami.Units.gridUnit * 12
                
                Image {
                    id: mascot
                    anchors.centerIn: parent
                    width: Kirigami.Units.gridUnit * 10
                    height: width
                    source: "qrc:/assets/mascot.svg"
                    sourceSize.width: width
                    fillMode: Image.PreserveAspectFit
                    
                    property int clickCount: 0
                    
                    transform: Scale {
                        id: transform
                        origin.x: mascot.width / 2
                        origin.y: mascot.height / 2
                    }
                    
                    SequentialAnimation {
                        id: squishAnimation
                        
                        NumberAnimation {
                            target: transform
                            property: "yScale"
                            to: 0.85
                            duration: 100
                            easing.type: Easing.OutQuad
                        }
                        NumberAnimation {
                            target: transform
                            property: "yScale"
                            to: 1.0
                            duration: 150
                            easing.type: Easing.OutBounce
                        }
                    }
                    
                    SequentialAnimation {
                        id: flipAnimation
                        
                        NumberAnimation {
                            target: transform
                            property: "yScale"
                            to: 0.85
                            duration: 100
                            easing.type: Easing.OutQuad
                        }
                        NumberAnimation {
                            target: transform
                            property: "yScale"
                            to: 1.0
                            duration: 150
                            easing.type: Easing.OutBounce
                        }
                        
                        NumberAnimation {
                            target: transform
                            easing.type: Easing.InOutQuad
                            property: "xScale"
                            to: -1
                            duration: 200
                        }
                        NumberAnimation {
                            easing.type: Easing.InOutQuad
                            target: transform
                            property: "xScale"
                            to: 1
                            duration: 300
                        }
                        NumberAnimation {
                            target: transform
                            easing.type: Easing.InOutQuad
                            property: "xScale"
                            to: -1
                            duration: 400
                        }
                        NumberAnimation {
                            easing.type: Easing.InOutQuad
                            target: transform
                            property: "xScale"
                            to: 1
                            duration: 500
                        }
                        NumberAnimation {
                            target: transform
                            easing.type: Easing.InOutQuad
                            property: "xScale"
                            to: 0.8
                            duration: 150
                        }
                        
                        PauseAnimation {
                            duration: 100
                        }

                        NumberAnimation {
                            easing.type: Easing.InOutQuad
                            target: transform
                            property: "xScale"
                            to: 1
                            duration: 200
                        }
                        
                        onFinished: mascot.clickCount = 0
                    }
                    
                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            mascot.clickCount++
                            
                            if (mascot.clickCount >= 5) {
                                flipAnimation.start()
                            } else {
                                squishAnimation.start()
                            }
                        }
                    }
                }
            }
            
            Item {
                Layout.preferredHeight: Kirigami.Units.largeSpacing
            }
        }
        
        Item {
            Layout.fillHeight: true
        }
    }
}
