#include "occviewcontextmenu.h"

#include <QAction>
#include <QActionGroup>
#include <QProxyStyle>

#include "hirespixmap.h"

occViewContextMenu::occViewContextMenu(QWidget *parent) : QMenu(parent)
{
    _iconHeight = QProxyStyle().pixelMetric(QStyle::PM_SmallIconSize, 0, 0);

    auto a = new QAction("Fit all", this);
    a->setToolTip("Fit view to all");
    a->setIcon(hiresPixmap(":/icons/lucide/reset.svg", _iconHeight));
    connect(a, &QAction::triggered, this, &occViewContextMenu::fitAll);
    this->addAction(a);

    a = new QAction("Fit selection", this );
    a->setToolTip(tr("Fit view to current selection"));
    a->setIcon(hiresPixmap(":/icons/lucide/box-select.svg", _iconHeight));
    connect(a, &QAction::triggered, this, &occViewContextMenu::fitArea);
    this->addAction(a);


    this->addSeparator();
    auto viewMenu = this->addMenu("Views");
    viewMenu->setIcon(hiresPixmap(":/icons/lucide/eye.svg", _iconHeight));
    addViewActions(*viewMenu);
}


void occViewContextMenu::addViewActions(QMenu &viewMenu)
{

//    auto viewActions = QList<QAction*>();

    //        a = new QAction(QObject::tr("MNU_ZOOM"), this );
    //        a->setToolTip( QObject::tr("TBR_ZOOM") );
    //        a->setStatusTip( QObject::tr("TBR_ZOOM") );
    ////        connect(a, &QAction::triggered, this, &occView::zoom);

    //        a->setCheckable(true);
    ////        connect(a, &QAction::toggled, this, &occView::updateToggled);
    //        _viewActions->insert(viewZoom, a );

    //        a = new QAction(QObject::tr("MNU_PAN"), this );
    //        a->setToolTip( QObject::tr("TBR_PAN") );
    //        a->setStatusTip( QObject::tr("TBR_PAN") );
    ////        connect(a, &QAction::triggered, this, &occView::pan);

    //        a->setCheckable(true);
    ////        connect(a, &QAction::toggled, this, &occView::updateToggled);
    //        _viewActions->insert(viewPan, a );

    //        a = new QAction(QObject::tr("MNU_GLOBALPAN"), this );
    //        a->setToolTip( QObject::tr("TBR_GLOBALPAN") );
    //        a->setStatusTip( QObject::tr("TBR_GLOBALPAN") );
    ////        connect(a, &QAction::triggered, this, &occView::globalPan);

    //        a->setCheckable(true);
    ////        connect(a, &QAction::toggled, this, &occView::updateToggled);
    //        _viewActions->insert(viewGlobalPan, a );

    auto a = new QAction("Isometric", this );
    a->setToolTip("View isometric");
    a->setIcon(hiresPixmap(":/icons/lucide/box.svg", _iconHeight));
    connect(a, &QAction::triggered, this, &occViewContextMenu::axo);
    viewMenu.addAction(a);

    a = new QAction("Front", this );
    a->setToolTip("View from front");
    a->setIcon(hiresPixmap(":/icons/boxFront.svg", _iconHeight));
    connect(a, &QAction::triggered, this, &occViewContextMenu::front);
    viewMenu.addAction(a);

    a = new QAction("Back", this );
    a->setToolTip("View from back");
    a->setIcon(hiresPixmap(":/icons/boxBack.svg", _iconHeight));
    connect(a, &QAction::triggered, this, &occViewContextMenu::back);
    viewMenu.addAction(a);

    a = new QAction("Left", this );
    a->setToolTip("View from left");
    a->setIcon(hiresPixmap(":/icons/boxLeft.svg", _iconHeight));
    connect(a, &QAction::triggered, this, &occViewContextMenu::left);
    viewMenu.addAction(a);

    a = new QAction("Right", this );
    a->setToolTip("View from right");
    a->setIcon(hiresPixmap(":/icons/boxRight.svg", _iconHeight));
    connect(a, &QAction::triggered, this, &occViewContextMenu::right);
    viewMenu.addAction(a);

    a = new QAction("Top", this );
    a->setToolTip("View from top");
    a->setIcon(hiresPixmap(":/icons/boxTop.svg", _iconHeight));
    connect(a, &QAction::triggered, this, &occViewContextMenu::top);
    viewMenu.addAction(a);

    a = new QAction("Bottom", this );
    a->setToolTip("View from bottom");
    a->setIcon(hiresPixmap(":/icons/boxBottom.svg", _iconHeight));
    connect(a, &QAction::triggered, this, &occViewContextMenu::bottom);
    viewMenu.addAction(a);

    //        a = new QAction(QObject::tr("MNU_BACK"), this );
    //        a->setToolTip( QObject::tr("TBR_BACK") );
    //        a->setStatusTip( QObject::tr("TBR_BACK") );
    ////        connect(a, &QAction::triggered, this, &occView::back);
    //        _viewActions->insert(viewBack, a);

    //        a = new QAction(QObject::tr("MNU_TOP"), this );
    //        a->setToolTip( QObject::tr("TBR_TOP") );
    //        a->setStatusTip( QObject::tr("TBR_TOP") );
    ////        connect(a, &QAction::triggered, this, &occView::top);
    //        _viewActions->insert(viewTop, a );

    //        a = new QAction(QObject::tr("MNU_BOTTOM"), this );
    //        a->setToolTip( QObject::tr("TBR_BOTTOM") );
    //        a->setStatusTip( QObject::tr("TBR_BOTTOM") );
    ////        connect(a, &QAction::triggered, this, &occView::bottom);
    //        _viewActions->insert(viewBottom, a );

    //        a = new QAction(QObject::tr("MNU_LEFT"), this );
    //        a->setToolTip( QObject::tr("TBR_LEFT") );
    //        a->setStatusTip( QObject::tr("TBR_LEFT") );
    ////        connect(a, &QAction::triggered, this, &occView::left);
    //        _viewActions->insert(viewLeft, a );

    //        a = new QAction(QObject::tr("MNU_RIGHT"), this );
    //        a->setToolTip( QObject::tr("TBR_RIGHT") );
    //        a->setStatusTip( QObject::tr("TBR_RIGHT") );
    ////        connect(a, &QAction::triggered, this, &occView::right);
    //        _viewActions->insert(viewRight, a );



    //        a = new QAction(QObject::tr("MNU_ROTATION"), this );
    //        a->setToolTip( QObject::tr("TBR_ROTATION") );
    //        a->setStatusTip( QObject::tr("TBR_ROTATION") );
    ////        connect(a, &QAction::triggered, this, &occView::rotation);
    //        a->setCheckable( true );
    ////        connect(a, &QAction::toggled, this, &occView::updateToggled);
    //        _viewActions->insert(viewRotation, a );

    //        a = new QAction(QObject::tr("MNU_RESET"), this );
    //        a->setToolTip( QObject::tr("TBR_RESET") );
    //        a->setStatusTip( QObject::tr("TBR_RESET") );
    ////        connect(a, &QAction::triggered, this, &occView::reset);
    //        _viewActions->insert(viewReset, a );

    //        QActionGroup* ag = new QActionGroup(this);

    //        a = new QAction(QObject::tr("MNU_HLROFF"), this );
    //        a->setToolTip( QObject::tr("TBR_HLROFF") );
    //        a->setStatusTip( QObject::tr("TBR_HLROFF") );
    ////        connect(a, &QAction::triggered, this, &occView::hlrOff);
    //        a->setCheckable( true );
    //        ag->addAction(a);
    //        _viewActions->insert(viewHlrOff, a);

    //        a = new QAction(QObject::tr("MNU_HLRON"), this );
    //        a->setToolTip( QObject::tr("TBR_HLRON") );
    //        a->setStatusTip( QObject::tr("TBR_HLRON") );
    ////        connect(a, &QAction::triggered,this, &occView::hlrOn);

    //        a->setCheckable( true );
    //        ag->addAction(a);
    //        _viewActions->insert(viewHlrOn, a );


}
