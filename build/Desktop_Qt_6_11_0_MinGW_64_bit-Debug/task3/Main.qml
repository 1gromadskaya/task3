import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs
import App.Player 1.0

Window {
    width: 420
    height: 720
    minimumWidth: 380
    minimumHeight: 650
    visible: true
    title: qsTr("Кроссплатформенный Плеер")
    color: "#0f0f0f"

    PlayerController {
        id: player
    }

    FolderDialog {
        id: folderDialog
        title: qsTr("Выберите папку с музыкой")
        onAccepted: player.openFolder(selectedFolder)
    }

    Drawer {
        id: drawer
        width: parent.width * 0.6
        height: parent.height
        background: Rectangle { color: "#1a1a1a" }

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 20
            spacing: 20

            Label {
                text: qsTr("Настройки")
                color: "white"
                font.pixelSize: 22
                font.bold: true
            }

            Button {
                text: "English"
                Layout.fillWidth: true
                onClicked: { player.changeLanguage("en"); drawer.close() }
            }
            Button {
                text: "Русский"
                Layout.fillWidth: true
                onClicked: { player.changeLanguage("ru"); drawer.close() }
            }
            Button {
                text: "Беларуская"
                Layout.fillWidth: true
                onClicked: { player.changeLanguage("by"); drawer.close() }
            }

            Item { Layout.fillHeight: true }
        }
    }

    function formatTime(ms) {
        if (ms < 0) return "0:00"
        let totalSeconds = Math.floor(ms / 1000)
        let minutes = Math.floor(totalSeconds / 60)
        let seconds = totalSeconds % 60
        return minutes + ":" + (seconds < 10 ? "0" : "") + seconds
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 20
        spacing: 8

        Button {
            text: "☰"
            flat: true
            palette.buttonText: "white"
            onClicked: drawer.open()
        }

        Rectangle {
            Layout.alignment: Qt.AlignHCenter
            Layout.preferredWidth: parent.width * 0.7
            Layout.preferredHeight: width
            Layout.maximumWidth: 260
            Layout.maximumHeight: 260
            color: "#1a1a1a"
            radius: 20
            clip: true

            Image {
                anchors.fill: parent
                source: player.currentCover
                fillMode: Image.PreserveAspectCrop
                visible: player.currentCover !== ""
                asynchronous: true
                opacity: visible ? 1 : 0
                Behavior on opacity { NumberAnimation { duration: 500 } }
            }

            Rectangle {
                anchors.fill: parent
                color: "#252525"
                visible: player.currentCover === ""
                radius: 20
                Text {
                    anchors.centerIn: parent
                    text: "🎵"
                    font.pixelSize: 60
                    opacity: 0.3
                }
            }
        }

        Text {
            Layout.fillWidth: true
            Layout.topMargin: 5
            horizontalAlignment: Text.AlignHCenter
            text: player.currentTrackTitle
            color: "white"
            font.pixelSize: 20
            font.bold: true
            elide: Text.ElideRight
        }

        RowLayout {
            Layout.fillWidth: true
            Text {
                text: formatTime(player.position)
                color: "#888"
                font.pixelSize: 11
                Layout.preferredWidth: 35
            }
            Slider {
                id: progressSlider
                Layout.fillWidth: true
                from: 0
                to: player.duration > 0 ? player.duration : 1
                value: player.position
                onMoved: player.position = value

                background: Rectangle {
                    height: 4
                    radius: 2
                    color: "#333"
                    Rectangle {
                        width: progressSlider.visualPosition * parent.width
                        height: parent.height
                        color: "#1DB954"
                        radius: 2
                    }
                }
                handle: Rectangle {
                    x: progressSlider.leftPadding + progressSlider.visualPosition * (progressSlider.availableWidth - width)
                    y: progressSlider.topPadding + progressSlider.availableHeight / 2 - height / 2
                    implicitWidth: 10
                    implicitHeight: 10
                    radius: 5
                    color: "#fff"
                }
            }
            Text {
                text: formatTime(player.duration)
                color: "#888"
                font.pixelSize: 11
                Layout.preferredWidth: 35
                horizontalAlignment: Text.AlignRight
            }
        }

        RowLayout {
            Layout.alignment: Qt.AlignHCenter
            spacing: 35

            Button {
                implicitWidth: 54
                implicitHeight: 54
                flat: true
                contentItem: Text {
                    text: "⏮"
                    font.pixelSize: 32
                    color: "white"
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
                onClicked: player.previous()
            }

            Button {
                id: playBtn
                implicitWidth: 70
                implicitHeight: 70
                background: Rectangle {
                    radius: 35
                    color: "white"
                    scale: playBtn.pressed ? 0.9 : 1.0
                    Behavior on scale { NumberAnimation { duration: 100 } }
                }
                contentItem: Text {
                    text: player.isPlaying ? "||" : "▶"
                    font.pixelSize: 28
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    color: "black"
                    leftPadding: player.isPlaying ? 0 : 4
                }
                onClicked: player.isPlaying ? player.pause() : player.play()
            }

            Button {
                implicitWidth: 54
                implicitHeight: 54
                flat: true
                contentItem: Text {
                    text: "⏭"
                    font.pixelSize: 32
                    color: "white"
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
                onClicked: player.next()
            }
        }

        Text {
            text: qsTr("Плейлист")
            color: "#666"
            font.bold: true
            font.pixelSize: 12
            Layout.topMargin: 5
        }

        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.maximumHeight: 120
            color: "#121212"
            radius: 12
            border.color: "#222"
            border.width: 1

            ListView {
                id: playlistView
                anchors.fill: parent
                anchors.margins: 5
                model: player.playlistModel
                clip: true
                spacing: 2
                ScrollBar.vertical: ScrollBar {
                    policy: ScrollBar.AsNeeded
                    active: true
                }

                delegate: ItemDelegate {
                    width: playlistView.width
                    height: 34
                    background: Rectangle {
                        color: player.currentTrackTitle === modelData ? "#1a3a2a" : (hovered ? "#1a1a1a" : "transparent")
                        radius: 6
                    }
                    contentItem: Text {
                        text: (index + 1) + ". " + modelData
                        color: player.currentTrackTitle === modelData ? "#1DB954" : "#ccc"
                        font.pixelSize: 12
                        verticalAlignment: Text.AlignVCenter
                        elide: Text.ElideRight
                        leftPadding: 10
                    }
                    onClicked: player.playTrack(index)
                }
            }
        }

        Button {
            Layout.fillWidth: true
            Layout.preferredHeight: 45
            Layout.topMargin: 5
            Layout.bottomMargin: 5
            contentItem: RowLayout {
                spacing: 10
                Item { Layout.fillWidth: true }
                Text { text: "📂"; font.pixelSize: 16 }
                Text {
                    text: qsTr("Выбрать папку")
                    color: "white"
                    font.bold: true
                    font.pixelSize: 14
                }
                Item { Layout.fillWidth: true }
            }
            background: Rectangle {
                color: "#1DB954"
                radius: 22
                opacity: parent.hovered ? 0.8 : 1.0
            }
            onClicked: folderDialog.open()
        }
    }
}