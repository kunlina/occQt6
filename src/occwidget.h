#ifndef OCCTWIDGET_H
#define OCCTWIDGET_H

#include <QToolBar>
#include <QWidget>

#include "occview.h"

class occWidget : public QWidget
{
    Q_OBJECT

public:
    occWidget(QWidget *parent = nullptr);
    ~occWidget() = default;

private:
    occView* _occViewer;

    QToolBar* _toolBar;

    void about();
    void populateToolBar();

};
#endif // OCCTWIDGET_H
