#include <QtTest>
#include <QSignalSpy>
#include <QSqlDatabase>
#include <QSqlQuery>
#include "playercontroller.h"

class PlayerTest : public QObject {
    Q_OBJECT
private slots:
    void initTestCase() {
        if (QSqlDatabase::contains("qt_sql_default_connection")) {
            QSqlDatabase::removeDatabase("qt_sql_default_connection");
        }
    }
    void test_unit_initialState() {
        PlayerController player;
        QCOMPARE(player.isPlaying(), false);
    }
    void test_unit_trackTitlePlaceholder() {
        PlayerController player;
        QVERIFY(!player.currentTrackTitle().isEmpty());
    }
    void test_integration_databaseOpen() {
        PlayerController player;
        QSqlDatabase db = QSqlDatabase::database();
        QVERIFY(db.isOpen());
    }
    void test_integration_dbWritable() {
        PlayerController player;
        QSqlQuery query;
        QVERIFY(query.exec("INSERT INTO tracks (title, url) VALUES ('t', 'u')"));
        QVERIFY(query.exec("DELETE FROM tracks WHERE title='t'"));
    }
    void test_ui_languageSignal() {
        PlayerController player;
        QSignalSpy spy(&player, SIGNAL(languageChanged()));
        player.changeLanguage("en");
        QCOMPARE(spy.count(), 1);
    }
    void test_ui_stopState() {
        PlayerController player;
        player.stop();
        QCOMPARE(player.isPlaying(), false);
    }
    void test_ui_playlistEmpty() {
        PlayerController player;
        QCOMPARE(player.playlistModel().size(), 0);
    }
};

QTEST_MAIN(PlayerTest)
#include "tst_player.moc"