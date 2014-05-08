import QtQuick 2.0
import Sailfish.Silica 1.0

Page {
    id: page
    SilicaListView {
        anchors.fill: parent
        header: PageHeader {
            title: "Page"
        }
        model: ListModel {
            ListElement {
                text: "First entry"
            }
            ListElement {
                text: "Second entry"
            }
        }
        delegate: BackgroundItem {
            Label {
                anchors.centerIn: parent
                text: model.text
            }
        }

        PullDownMenu {
            MenuItem {
                text: "Patched text !"
            }
        }
    }
}
