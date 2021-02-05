#include "hudwidget.h"

#include <QDebug>
#include <QHBoxLayout>
#include <QMenu>
#include <QToolBar>
#include <QToolButton>

#include "hirespixmap.h"

hudWidget::hudWidget(QWidget *parent) : QWidget(parent)
{
    setAttribute(Qt::WA_TranslucentBackground);
    auto layout = new QHBoxLayout(this);
    layout->setSpacing(0);
    layout->setContentsMargins(0,0,0,0);

    auto toolBar = new QToolBar;
    auto iconSize = toolBar->iconSize();
    toolBar->setIconSize(iconSize*0.65);
    layout->addWidget(toolBar);

    this->setLayout(layout);

    const int iconHeight {toolBar->iconSize().height()};
//    const int iconWidth {toolBar->iconSize().width()};

/*    auto menu = new QMenu;
    menu->setWindowFlags(Qt::FramelessWindowHint);
    menu->setContentsMargins(0,0,0,0);
//menu->setStyleSheet("background:rgba(0,0,0,50%);");
//    menu->setFixedWidth(iconWidth);
//    menu->setWindowOpacity(0.0);
    menu->setAttribute(Qt::WA_TranslucentBackground)*/;

    auto axoAction = new QAction;
    axoAction->setIcon(hiresPixmap(":/icons/lucide/box.svg", iconHeight));
    toolBar->addAction(axoAction);

    auto leftAction = new QAction;
    leftAction->setIcon(hiresPixmap(":/icons/boxLeft.svg", iconHeight));
    toolBar->addAction(leftAction);

    auto rightAction = new QAction;
    rightAction->setIcon(hiresPixmap(":/icons/boxRight.svg", iconHeight));
    toolBar->addAction(rightAction);

//    auto toolButton = new QToolButton;
//    toolButton->setIcon(hiresPixmap(":/icons/lucide/box.svg", iconColor, iconHeight));
//    toolButton->setMenu(menu);
//    toolButton->setPopupMode(QToolButton::InstantPopup);
//    toolButton->setWindowFlags(toolButton->windowFlags() | Qt::FramelessWindowHint);
//    toolButton->setAttribute(Qt::WA_TranslucentBackground);

//    toolBar->addWidget(toolButton);

    this->setContentsMargins(0,0,0,0);
}
