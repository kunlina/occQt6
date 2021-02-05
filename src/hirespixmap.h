/***** MIT *******************************************************************
**                                                                          **
** This file is part of occQt6, a simple OpenCASCADE Qt demo, updated       **
** for Qt6 and OpenCASCADE 7.5.0                                            **
**                                                                          **
** Copyright (c) 2020 Marius Schollmeier (mschollmeier01@gmail.com)         **
**                                                                          **
** Permission is hereby granted, free of charge, to any person              **
** obtaining a copy of this software and associated documentation           **
** files (the "Software"), to deal in the Software without restriction,     **
** including without limitation the rights to use, copy, modify, merge,     **
** publish, distribute, sublicense, and/or sell copies of the Software,     **
** and to permit persons to whom the Software is furnished to do so,        **
** subject to the following conditions:                                     **
**                                                                          **
** The above copyright notice and this permission notice shall be included  **
** in all copies or substantial portions of the Software.                   **
**                                                                          **
** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,          **
** EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES          **
** OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND                 **
** NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT              **
** HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,             **
** WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,       **
** OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER            **
** DEALINGS IN THE SOFTWARE.                                                **
*****************************************************************************/

#ifndef HIRESPIXMAP_H
#define HIRESPIXMAP_H

#include <QFile>
#include <QFileInfo>
#include <QPainter>
#include <QPaintDevice>
#include <QPixmap>
#include <QSvgRenderer>
#include <QTextStream>

inline QPixmap hiresPixmap(const QString &filename, int height)
{
    //load svg and re-color accordingly
    const QLatin1String iconColor {"#778ca3"}; //blue grey

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
        icon.replace("currentColor", iconColor);
        icon.replace("#000000", iconColor);
        file.close();
        QSvgRenderer svg(QByteArray(icon.toUtf8()));
        QPainter painter(&pixmap);
        svg.render(&painter);
    }
    pixmap.setDevicePixelRatio(2.0);
    pixmap = pixmap.scaledToHeight(4*height, Qt::SmoothTransformation);

    return pixmap;
}
#endif // HIRESPIXMAP_H
