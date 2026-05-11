#ifndef TRACK_H
#define TRACK_H

#include <QString>
#include <QUrl>

struct Track {
    QString title;
    QString artist;
    QUrl fileUrl;
    QString coverSource;
};

#endif