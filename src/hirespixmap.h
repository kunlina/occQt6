
#ifndef HIRESPIXMAP_H
#define HIRESPIXMAP_H

#include <QFile>
#include <QFileInfo>
#include <QPainter>
#include <QPixmap>
#include <QSvgRenderer>
#include <QTextStream>

inline QPixmap hiresPixmap(const QString &filename, const QString &colorString, int height)
{
    //load svg and re-color accordingly
    const int pixmapWidth = 144;
    const int pixmapHeight = 144;
    QPixmap pixmap(pixmapWidth, pixmapHeight);
    pixmap.fill(Qt::transparent);

    if (QFileInfo::exists(filename) && QFileInfo(filename).isFile())
    {
        QFile file(filename);
        file.open(QIODevice::ReadOnly);
        QTextStream in(&file);
        QString icon = in.readAll();
        icon.replace("currentColor", colorString);
        icon.replace("#000000", colorString);
        file.close();
        QSvgRenderer svg(QByteArray(icon.toUtf8()));
        QPainter painter(&pixmap);
        svg.render(&painter);
    }

    pixmap.setDevicePixelRatio(2.0);
    pixmap = pixmap.scaledToHeight(2*height, Qt::SmoothTransformation);

    return pixmap;
}



#endif // HIRESPIXMAP_H
