#include "occtwidget.h"

#include <QAction>
#include <QActionGroup>
#include <QVBoxLayout>
#include <QToolBar>

occtWidget::occtWidget(QWidget *parent)
    : QWidget(parent)
{

    // add toolBar
    auto layout = new QVBoxLayout(this);
    layout->setSpacing(0);
    layout->setContentsMargins(3,3,3,3);

    _toolBar = new QToolBar;
    layout->addWidget(_toolBar);

    _occViewer = new occView(this);
    layout->addWidget(_occViewer);
    this->setLayout(layout);
    this->populateToolBar();

    // show and force size update to redraw occt window
    this->show();
    this->setMinimumSize(QSize(640,480));

}

occtWidget::~occtWidget()
{
}


// ------------------------------------------------------------------------------------------------
// private functions
// ------------------------------------------------------------------------------------------------
void occtWidget::populateToolBar()
{
    // view actions
    auto viewGroup = new QActionGroup(this);

    auto zoom = new QAction("Zoom", this);
    zoom->setCheckable(true);
    zoom->setIcon(QPixmap(":/zoom-in.svg"));
    connect(zoom, &QAction::triggered, _occViewer, &occView::zoom);
    viewGroup->addAction(zoom);

    auto pan = new QAction("Pan", this);
    pan->setCheckable(true);
    pan->setIcon(QPixmap(":/select.svg"));
    connect(pan, &QAction::triggered, _occViewer, &occView::pan);
    viewGroup->addAction(pan);

    auto rotate= new QAction("Rotate", this);
    rotate->setCheckable(true);
    rotate->setIcon(QPixmap(":/rotation.svg"));
    connect(rotate, &QAction::triggered, _occViewer, &occView::rotate);
    viewGroup->addAction(rotate);

    zoom->setChecked(true);

    _toolBar->addActions(viewGroup->actions());

    auto iconSize = _toolBar->iconSize();
    _toolBar->setIconSize(QSize(iconSize.width()*0.8, iconSize.height()*0.8));
}
