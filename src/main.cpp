#include "occtwidget.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    occtWidget w;
    w.show();
    return a.exec();
}
