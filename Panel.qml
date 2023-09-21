import QtQuick 2.0

Rectangle {
    width: 100
    height: 100
    color: "#FF6464"

    Text {
    anchors.left: parent.left
    anchors.right: parent.right
    text: "QML Window"
    fontSizeMode: Text.Fit; minimumPixelSize: 10; font.pixelSize: 72
    color: "black"
    }
}