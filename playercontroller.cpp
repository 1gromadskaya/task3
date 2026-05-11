#include "playercontroller.h"
#include <QDir>
#include <QSqlQuery>
#include <QSqlError>
#include <QStandardPaths>
#include <QCoreApplication>
#include <QDebug>
#include <QQmlEngine>
#include <QtQml>

PlayerController::PlayerController(QObject *parent)
    : QObject(parent), m_currentIndex(-1)
{
    m_mediaPlayer = new QMediaPlayer(this);
    m_audioOutput = new QAudioOutput(this);
    m_audioOutput->setVolume(0.5);
    m_mediaPlayer->setAudioOutput(m_audioOutput);
    m_translator = new QTranslator(this);

    connect(m_mediaPlayer, &QMediaPlayer::playbackStateChanged, this, &PlayerController::playStateChanged);
    connect(m_mediaPlayer, &QMediaPlayer::positionChanged, this, &PlayerController::positionChanged);
    connect(m_mediaPlayer, &QMediaPlayer::durationChanged, this, &PlayerController::durationChanged);

    connect(m_mediaPlayer, &QMediaPlayer::mediaStatusChanged, this, [this](QMediaPlayer::MediaStatus status){
        if (status == QMediaPlayer::EndOfMedia) next();
    });

    initDatabase();
    loadTracksFromDb();
}

PlayerController::~PlayerController() {
    if (m_db.isOpen()) m_db.close();
}

void PlayerController::changeLanguage(const QString &lang) {
    if (!qApp) return;
    qApp->removeTranslator(m_translator);
    QString filename = "apptask3_" + lang;
    QString path = QCoreApplication::applicationDirPath();
    bool loaded = m_translator->load(filename, path);
    if (!loaded) {
        path = QDir::currentPath();
        loaded = m_translator->load(filename, path);
    }
    if (loaded) {
        qApp->installTranslator(m_translator);
        QQmlEngine *engine = qmlEngine(this);
        if (engine) engine->retranslate();
    }
    emit languageChanged();
}

void PlayerController::initDatabase() {
    m_db = QSqlDatabase::addDatabase("QSQLITE");
    QString path = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(path);
    m_db.setDatabaseName(path + "/playlist.db");
    if (m_db.open()) {
        QSqlQuery query;
        query.exec("CREATE TABLE IF NOT EXISTS tracks (id INTEGER PRIMARY KEY AUTOINCREMENT, title TEXT, url TEXT, cover TEXT)");
    }
}

void PlayerController::saveTrackToDb(const Track &track) {
    QSqlQuery query;
    query.prepare("INSERT INTO tracks (title, url, cover) VALUES (:title, :url, :cover)");
    query.bindValue(":title", track.title);
    query.bindValue(":url", track.fileUrl.toString());
    query.bindValue(":cover", track.coverSource);
    query.exec();
}

void PlayerController::loadTracksFromDb() {
    m_playlist.clear();
    QSqlQuery query("SELECT title, url, cover FROM tracks");
    while (query.next()) {
        Track track;
        track.title = query.value(0).toString();
        track.fileUrl = QUrl(query.value(1).toString());
        track.coverSource = query.value(2).toString();
        m_playlist.append(track);
    }
    if (!m_playlist.isEmpty()) {
        m_currentIndex = 0;
        m_mediaPlayer->setSource(m_playlist[m_currentIndex].fileUrl);
        emit playlistChanged();
    }
}

void PlayerController::scanDirectory(const QString &path) {
    QDir dir(path);
    QStringList filters = {"*.mp3", "*.wav", "*.m4a"};
    QFileInfoList files = dir.entryInfoList(filters, QDir::Files);
    QSqlQuery query;
    query.exec("DELETE FROM tracks");
    m_playlist.clear();
    for (const QFileInfo &file : files) {
        Track track;
        track.title = file.completeBaseName();
        track.fileUrl = QUrl::fromLocalFile(file.absoluteFilePath());
        QStringList imgFilters = {"cover.jpg", "album.jpg", "*.png", "*.jpg"};
        QFileInfoList covers = file.dir().entryInfoList(imgFilters, QDir::Files);
        track.coverSource = !covers.isEmpty() ? QUrl::fromLocalFile(covers[0].absoluteFilePath()).toString() : "";
        m_playlist.append(track);
        saveTrackToDb(track);
    }
    emit playlistChanged();
    if (!m_playlist.isEmpty()) playTrack(0);
}

qint64 PlayerController::position() const { return m_mediaPlayer->position(); }
void PlayerController::setPosition(qint64 pos) { m_mediaPlayer->setPosition(pos); }
qint64 PlayerController::duration() const { return m_mediaPlayer->duration(); }

QStringList PlayerController::playlistModel() const {
    QStringList list;
    for (const auto &track : m_playlist) list << track.title;
    return list;
}

QString PlayerController::currentCover() const {
    return (m_currentIndex >= 0 && m_currentIndex < m_playlist.size()) ? m_playlist[m_currentIndex].coverSource : "";
}

void PlayerController::play() { if (m_currentIndex >= 0) m_mediaPlayer->play(); }
void PlayerController::pause() { m_mediaPlayer->pause(); }
void PlayerController::stop() { m_mediaPlayer->stop(); }

void PlayerController::playTrack(int index) {
    if (index >= 0 && index < m_playlist.size()) {
        m_currentIndex = index;
        m_mediaPlayer->setSource(m_playlist[m_currentIndex].fileUrl);
        m_mediaPlayer->play();
        emit currentTrackChanged();
    }
}

void PlayerController::next() {
    if (!m_playlist.isEmpty()) playTrack((m_currentIndex + 1) % m_playlist.size());
}

void PlayerController::previous() {
    if (!m_playlist.isEmpty()) playTrack((m_currentIndex - 1 < 0) ? m_playlist.size() - 1 : m_currentIndex - 1);
}

void PlayerController::openFolder(const QUrl &folderUrl) {
    scanDirectory(folderUrl.toLocalFile());
}

QString PlayerController::currentTrackTitle() const {
    return (m_currentIndex >= 0) ? m_playlist[m_currentIndex].title : tr("Нет трека");
}

bool PlayerController::isPlaying() const {
    return m_mediaPlayer->playbackState() == QMediaPlayer::PlayingState;
}

#include "playercontroller.moc"