#ifndef OCCTWIDGET_H
#define OCCTWIDGET_H

#include <QToolBar>
#include <QWidget>

#include "occview.h"

class occtWidget : public QWidget
{
    Q_OBJECT

public:
    occtWidget(QWidget *parent = nullptr);
    ~occtWidget();

private:
    occView* _occViewer;

    QToolBar* _toolBar;

    void about();
    void populateToolBar();

};
#endif // OCCTWIDGET_H
