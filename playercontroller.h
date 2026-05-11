#ifndef PLAYERCONTROLLER_H
#define PLAYERCONTROLLER_H

#include <QObject>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QSqlDatabase>
#include <QList>
#include <QTranslator>
#include <QGuiApplication>
#include "Track.h"

class PlayerController : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString currentTrackTitle READ currentTrackTitle NOTIFY currentTrackChanged)
    Q_PROPERTY(bool isPlaying READ isPlaying NOTIFY playStateChanged)
    Q_PROPERTY(qint64 position READ position WRITE setPosition NOTIFY positionChanged)
    Q_PROPERTY(qint64 duration READ duration NOTIFY durationChanged)
    Q_PROPERTY(QStringList playlistModel READ playlistModel NOTIFY playlistChanged)
    Q_PROPERTY(QString currentCover READ currentCover NOTIFY currentTrackChanged)

public:
    explicit PlayerController(QObject *parent = nullptr);
    ~PlayerController();

    Q_INVOKABLE void play();
    Q_INVOKABLE void pause();
    Q_INVOKABLE void stop();
    Q_INVOKABLE void next();
    Q_INVOKABLE void previous();
    Q_INVOKABLE void openFolder(const QUrl &folderUrl);
    Q_INVOKABLE void playTrack(int index);
    Q_INVOKABLE void changeLanguage(const QString &lang);

    QString currentTrackTitle() const;
    bool isPlaying() const;
    qint64 position() const;
    void setPosition(qint64 position);
    qint64 duration() const;
    QStringList playlistModel() const;
    QString currentCover() const;

signals:
    void currentTrackChanged();
    void playStateChanged();
    void errorOccurred(const QString &message);
    void positionChanged();
    void durationChanged();
    void playlistChanged();
    void languageChanged();

private:
    QMediaPlayer *m_mediaPlayer;
    QAudioOutput *m_audioOutput;
    QSqlDatabase m_db;
    QList<Track> m_playlist;
    int m_currentIndex;
    QTranslator *m_translator;

    void initDatabase();
    void saveTrackToDb(const Track &track);
    void loadTracksFromDb();
    void scanDirectory(const QString &path);
};

#endif