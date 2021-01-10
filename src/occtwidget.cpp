#include "occtwidget.h"

#include <QAction>
#include <QActionGroup>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QToolBar>

#include <Standard_Version.hxx>

#include "emptyspacerwidget.h"
#include "hirespixmap.h"

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

    this->setWindowTitle("Qt6 with OpenCASCADE demo - occQt6");

}

occtWidget::~occtWidget()
{
}


// ------------------------------------------------------------------------------------------------
// private functions
// ------------------------------------------------------------------------------------------------
void occtWidget::about()
{
    QString datetime = QStringLiteral(__DATE__) + QStringLiteral(" ") + QStringLiteral(__TIME__);

    QMessageBox about;
    about.setText(QString("occQt6 V%1").arg(VERSION_STRING));
    about.setInformativeText(
                QString("Compiled with Qt %1 and OpenCASCADE %2<br><br>"
                        "Built on %3<br><br>"
                        "occQt6 is a demo application about Qt and OpenCASCADE, "
                        "originally developed by Shing Liu.<br><br>"
                        "Copyright &copy; 2020 Marius Schollmeier<br><br>"
                        "The program is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE "
                        "WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE."
                       ).arg(QT_VERSION_STR, OCC_VERSION_COMPLETE, datetime));
    about.setStandardButtons(QMessageBox::Close);
    about.setWindowTitle("About occtQt6");
    //about.setIcon(QMessageBox::Information);
    about.exec();
}


void occtWidget::populateToolBar()
{

    auto iconSize = _toolBar->iconSize();
    _toolBar->setIconSize(QSize(iconSize.width()*0.65, iconSize.height()*0.65));
    _toolBar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

    const int iconHeight = _toolBar->iconSize().height();
    const QString iconColor {"#778ca3"}; //blue grey

    // mouse actions
    auto mouseGroup = new QActionGroup(this);

    auto selectAction = new QAction("Select", this);
    selectAction->setCheckable(true);
    selectAction->setIcon(hiresPixmap(":/icons/mouse-pointer.svg", iconColor, iconHeight));
    connect(selectAction, &QAction::triggered, _occViewer, &occView::select);
    mouseGroup->addAction(selectAction);

    auto zoomAction = new QAction("Zoom", this);
    zoomAction->setCheckable(true);
    zoomAction->setIcon(hiresPixmap(":/icons/zoom-in.svg", iconColor, iconHeight));
    connect(zoomAction, &QAction::triggered, _occViewer, &occView::zoom);
    mouseGroup->addAction(zoomAction);

    auto panAction = new QAction("Pan", this);
    panAction->setCheckable(true);
    panAction->setIcon(hiresPixmap(":/icons/move.svg", iconColor, iconHeight));
    connect(panAction, &QAction::triggered, _occViewer, &occView::pan);
    mouseGroup->addAction(panAction);

    auto rotate= new QAction("Rotate", this);
    rotate->setCheckable(true);
    rotate->setIcon(hiresPixmap(":/icons/rotate-ccw.svg", iconColor, iconHeight));
    connect(rotate, &QAction::triggered, _occViewer, &occView::rotate);
    mouseGroup->addAction(rotate);

    // select first action
    selectAction->setChecked(true);

    _toolBar->addActions(mouseGroup->actions());

    // add separator
    _toolBar->addSeparator();

    // add reset action
    auto reset = new QAction("Reset", this);
    reset->setIcon(hiresPixmap(":/icons/reset.svg", iconColor, iconHeight));
    connect(reset, &QAction::triggered, _occViewer, &occView::reset);
    _toolBar->addAction(reset);

    // add separator
    _toolBar->addSeparator();

    // add primitives
    auto cubeAction = new QAction("Cube", this);
    cubeAction->setIcon(hiresPixmap(":/icons/box.svg", iconColor, iconHeight));
    //connect(cube, &QAction::triggered, _occViewer, &occView::reset);
    _toolBar->addAction(cubeAction);

    // add about action
    auto about = new QAction("About", this);
    about->setIcon(hiresPixmap(":/icons/help-circle.svg", iconColor, iconHeight));
    connect(about, &QAction::triggered, this, &occtWidget::about);
    _toolBar->addAction(about);


    // add spacer widget to toolBar
    auto empty = new emptySpacerWidget(this);
    _toolBar->insertWidget(about, empty);


}
