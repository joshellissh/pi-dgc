#ifndef IMAGES_H
#define IMAGES_H

#include "dirent.h"
#include <QString>
#include <QByteArray>
#include <QDebug>
#include <map>

using namespace std;

typedef map<string, QImage> ImageMap;

inline ImageMap *loadImages(QString imageDirectory) {
    DIR *dir;
    struct dirent *ent;
    ImageMap *imageMap = new ImageMap;

    QByteArray array = imageDirectory.toLocal8Bit();
    char* buffer = array.data();

    if ((dir = opendir (buffer)) != NULL) {
        while ((ent = readdir (dir)) != NULL) {
            if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0)
                continue;

            QString imagePath = QString("%1%2").arg(imageDirectory).arg(ent->d_name);

            imageMap->insert(pair<string, QImage>(ent->d_name, QImage(imagePath)));
            qDebug() << "Loading image:" << imagePath;
        }
        closedir (dir);
    } else {
        printf("Unable to load images.");
        exit(1);
    }

    return imageMap;
}

#endif // IMAGES_H
