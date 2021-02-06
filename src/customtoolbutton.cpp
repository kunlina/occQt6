#include "customtoolbutton.h"

#include <QAction>

customToolButton::customToolButton(QWidget *parent) : QToolButton(parent)
{
    setPopupMode(QToolButton::MenuButtonPopup);
    connect(this, &QToolButton::triggered, this, &QToolButton::setDefaultAction);
}
