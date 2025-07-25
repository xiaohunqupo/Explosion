pragma Singleton

import QtQuick

QtObject {
    property color bgColor: Qt.color('#212121')
    property color primaryBgColor: Qt.color('#3a3939')
    property color primaryColor: Qt.color('#e74c3c')
    property color primaryHoverColor: Qt.color('#f55b4b')
    property color primaryFocusColor: Qt.color('#c0392b')
    property color primaryDisabledColor: Qt.color('#4b4a49')
    property color secondaryColor: Qt.color('#d58845')
    property color secondaryHoverColor: Qt.color('#ec9d58')
    property color secondaryFocusColor: Qt.color('#9b6a40')
    property color secondaryBgColor: Qt.color('#8e8e8e')
    property color disabledColor: Qt.color('#676563')
    property color fontColor: Qt.color('#ecf0f1')
    property color placeHolderFontColor: Qt.color('#c0c3c4')
    property color linkFontColor: Qt.color('#91b9c4')

    property FontLoader normalFont: FontLoader { source: 'file:%1/Font/MiSans-Medium.ttf'.arg(appResDir) }
    property FontLoader boldFont: FontLoader { source: 'file:%1/Font/MiSans-Semibold.ttf'.arg(appResDir) }
    property int tiele1FontSize: 20
    property int title2FontSize: 18
    property int title3FontSize: 16
    property int contentFontSize: 14
    property int iconFontSize: 18
    property string fontFamily: 'MiSans'
}
