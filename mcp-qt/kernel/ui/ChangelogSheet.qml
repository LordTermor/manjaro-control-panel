import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import org.kde.kirigami as Kirigami

Kirigami.OverlaySheet {
    id: root

    // Data
    property var majorVersion
    property var minorVersion
    property string text: ""

    header: Kirigami.Heading {
        textFormat: Text.RichText
        text: qsTr(`${root.majorVersion}.${root.minorVersion} Changelog ― by <a href='https://kernelnewbies.org'>Linux Kernel Newbies</a>`)
        
        onLinkActivated: (link) => Qt.openUrlExternally(link)
    }

    BusyIndicator {
        id: loadingProgressBar
        
        visible: root.text.length === 0
    }

    TextArea {
        id: changelogText
        
        implicitWidth: root.width * 0.8
        rightPadding: 20
        
        background: Item {}
        readOnly: true
        
        font.pointSize: 9
        textFormat: Text.RichText
        wrapMode: Text.WrapAtWordBoundaryOrAnywhere
        text: root.text
        
        onLinkActivated: (link) => {
            if (link.toString().startsWith("#")) {
                return
            }

            Qt.openUrlExternally(link)
        }
    }
}
