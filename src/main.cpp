#include "occwidget.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    occWidget w;
    w.show();
    return a.exec();
}
