#ifndef CUSTOMTOOLBUTTON_H
#define CUSTOMTOOLBUTTON_H

#include <QObject>
#include <QToolButton>

class customToolButton : public QToolButton
{
    Q_OBJECT
public:
    explicit customToolButton(QWidget *parent = nullptr);
};

#endif // CUSTOMTOOLBUTTON_H
