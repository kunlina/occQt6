#include "occviewcontextmenu.h"

#include <QAction>
#include <QActionGroup>
#include <QProxyStyle>

#include "hirespixmap.h"

occViewContextMenu::occViewContextMenu(QWidget *parent,
                                       occViewEnums::drawStyle drawStyle)
    : QMenu(parent),
      _curDrawStyle(drawStyle)
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

    auto drawMenu = this->addMenu("Draw style");
    addDrawStyles(*drawMenu);
}


void occViewContextMenu::addViewActions(QMenu &viewMenu)
{
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
}


void occViewContextMenu::addDrawStyles(QMenu &drawMenu)
{
    auto points = new QAction("Points", this );
    points->setToolTip("Show objects as points");
    connect(points, &QAction::triggered, this, &occViewContextMenu::points);
    points->setCheckable( true );

    auto wireframe = new QAction("Wireframe", this );
    wireframe->setToolTip("Show wireframe objects");
    connect(wireframe, &QAction::triggered, this, &occViewContextMenu::wireframe);
    wireframe->setCheckable( true );

    auto hlr = new QAction("Hidden line removal", this );
    hlr->setToolTip("Remove hidden lines");
    connect(hlr, &QAction::triggered, this, &occViewContextMenu::hlrOn);
    hlr->setCheckable( true );

    auto shaded = new QAction("Shaded", this );
    shaded->setToolTip("Show shaded objects");
    connect(shaded, &QAction::triggered, this, &occViewContextMenu::shaded);
    shaded->setCheckable( true );

    auto shadedWEdges = new QAction("Shaded with edges", this );
    shadedWEdges->setToolTip("Show shaded objects with edges");
    connect(shadedWEdges, &QAction::triggered, this, &occViewContextMenu::shadedWithEdges);
    shadedWEdges->setCheckable( true );

    switch (_curDrawStyle) {
    case occViewEnums::drawStyle::points:
        points->setChecked(true);
        break;
    case occViewEnums::drawStyle::wireframe:
        wireframe->setChecked(true);
        break;
    case occViewEnums::drawStyle::hlrOn:
        hlr->setChecked(true);
        break;
    case occViewEnums::drawStyle::shaded:
        shaded->setChecked(true);
        break;
    case occViewEnums::drawStyle::shadedWithEdges:
        shadedWEdges->setChecked(true);
        break;
    }

    drawMenu.addAction(points);
    drawMenu.addAction(wireframe);
    drawMenu.addAction(hlr);
    drawMenu.addAction(shaded);
    drawMenu.addAction(shadedWEdges);
}
