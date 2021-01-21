/***** MIT *******************************************************************
**                                                                          **
** This file is part of occQt6, a simple OpenCASCADE Qt demo, updated       **
** for Qt6 and OpenCASCADE 7.5.0                                            **
**                                                                          **
** Copyright (c) 2020 Marius Schollmeier (mschollmeier01@gmail.com)         **
**                                                                          **
** Permission is hereby granted, free of charge, to any person              **
** obtaining a copy of this software and associated documentation           **
** files (the "Software"), to deal in the Software without restriction,     **
** including without limitation the rights to use, copy, modify, merge,     **
** publish, distribute, sublicense, and/or sell copies of the Software,     **
** and to permit persons to whom the Software is furnished to do so,        **
** subject to the following conditions:                                     **
**                                                                          **
** The above copyright notice and this permission notice shall be included  **
** in all copies or substantial portions of the Software.                   **
**                                                                          **
** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,          **
** EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES          **
** OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND                 **
** NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT              **
** HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,             **
** WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,       **
** OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER            **
** DEALINGS IN THE SOFTWARE.                                                **
*****************************************************************************/

#include "occwidget.h"

#include <QAction>
#include <QActionGroup>
#include <QMessageBox>
#include <QToolBar>
#include <QVBoxLayout>

// occ headers
#include <Standard_Version.hxx>

#include <TopoDS.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>

#include <BRepLib.hxx>

#include <BRepBuilderAPI.hxx>
#include <BRepFilletAPI_MakeChamfer.hxx>
#include <BRepFilletAPI_MakeFillet.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepPrimAPI_MakeCone.hxx>
#include <BRepPrimAPI_MakeSphere.hxx>
#include <BRepPrimAPI_MakeCylinder.hxx>
#include <BRepPrimAPI_MakeTorus.hxx>
#include <BRepPrimAPI_MakePrism.hxx>
#include <BRepPrimAPI_MakeRevol.hxx>

#include <Graphic3d_Group.hxx>
#include <Graphic3d_Text.hxx>


// private headers
#include "emptyspacerwidget.h"
#include "hirespixmap.h"

occWidget::occWidget(QWidget *parent)
    : QWidget(parent)
{

    // add toolBar
    auto layout = new QVBoxLayout(this);
    layout->setSpacing(0);
    layout->setContentsMargins(3,3,3,3);

    _toolBar = new QToolBar;
    layout->addWidget(_toolBar);

    _occView = new occView(this);
    layout->addWidget(_occView);
    this->setLayout(layout);
    this->populateToolBar();

    // show and force size update to redraw occt window
    this->show();
    this->setMinimumSize(QSize(640,480));

    this->setWindowTitle("Qt6 with OpenCASCADE demo - occQt6");

}


// ------------------------------------------------------------------------------------------------
// private functions
// ------------------------------------------------------------------------------------------------
void occWidget::about()
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
                        "Lucide icons are licensed under the "
                        "<a href=\"https://github.com/lucide-icons/lucide/blob/master/LICENSE\">ISC License</a><br><br>"
                        "The program is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE "
                        "WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE."
                       ).arg(QT_VERSION_STR, OCC_VERSION_COMPLETE, datetime));
    about.setStandardButtons(QMessageBox::Close);
    about.setWindowTitle("About occtQt6");
    //about.setIcon(QMessageBox::Information);
    about.exec();
}


void occWidget::populateToolBar()
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
    selectAction->setIcon(hiresPixmap(":/lucideicons/mouse-pointer.svg", iconColor, iconHeight));
    connect(selectAction, &QAction::triggered, _occView, &occView::select);
    mouseGroup->addAction(selectAction);

    auto zoomAction = new QAction("Zoom", this);
    zoomAction->setCheckable(true);
    zoomAction->setIcon(hiresPixmap(":/lucideicons/zoom-in.svg", iconColor, iconHeight));
    connect(zoomAction, &QAction::triggered, _occView, &occView::zoom);
    mouseGroup->addAction(zoomAction);

    auto panAction = new QAction("Pan", this);
    panAction->setCheckable(true);
    panAction->setIcon(hiresPixmap(":/lucideicons/move.svg", iconColor, iconHeight));
    connect(panAction, &QAction::triggered, _occView, &occView::pan);
    mouseGroup->addAction(panAction);

    auto rotate= new QAction("Rotate", this);
    rotate->setCheckable(true);
    rotate->setIcon(hiresPixmap(":/lucideicons/rotate-ccw.svg", iconColor, iconHeight));
    connect(rotate, &QAction::triggered, _occView, &occView::rotate);
    mouseGroup->addAction(rotate);

    // select first action
    selectAction->setChecked(true);

    _toolBar->addActions(mouseGroup->actions());

    // add separator
    _toolBar->addSeparator();

    // add reset action
    auto reset = new QAction("Reset", this);
    reset->setIcon(hiresPixmap(":/lucideicons/reset.svg", iconColor, iconHeight));
    connect(reset, &QAction::triggered, _occView, &occView::reset);
    _toolBar->addAction(reset);

    // add separator
    _toolBar->addSeparator();

    // add primitives
    auto boxAction = new QAction("Box", this);
    boxAction->setIcon(hiresPixmap(":/lucideicons/box.svg", iconColor, iconHeight));
    connect(boxAction, &QAction::triggered, this, &occWidget::addBox);
    _toolBar->addAction(boxAction);

    auto coneAction = new QAction("Cone", this);
    coneAction->setIcon(hiresPixmap(":/lucideicons/triangle.svg", iconColor, iconHeight));
    connect(coneAction, &QAction::triggered, this, &occWidget::addCone);
    _toolBar->addAction(coneAction);

    auto sphereAction = new QAction("Sphere", this);
    sphereAction->setIcon(hiresPixmap(":/lucideicons/globe.svg", iconColor, iconHeight));
    connect(sphereAction, &QAction::triggered, this, &occWidget::addSphere);
    _toolBar->addAction(sphereAction);

    auto cylinderAction = new QAction("Cylinder", this);
    cylinderAction->setIcon(hiresPixmap(":/lucideicons/database.svg", iconColor, iconHeight));
    connect(cylinderAction, &QAction::triggered, this, &occWidget::addCylinder);
    _toolBar->addAction(cylinderAction);

    auto torusAction = new QAction("Torus", this);
    torusAction->setIcon(hiresPixmap(":/lucideicons/disc.svg", iconColor, iconHeight));
    connect(torusAction, &QAction::triggered, this, &occWidget::addTorus);
    _toolBar->addAction(torusAction);

    auto textAction = new QAction("Text", this);
    textAction->setIcon(hiresPixmap(":/lucideicons/type.svg", iconColor, iconHeight));
    connect(textAction, &QAction::triggered, this, &occWidget::addText);
    _toolBar->addAction(textAction);

    // add separator
    _toolBar->addSeparator();

    // add fillet,chamfer, etc
    auto filletAction = new QAction("Fillet", this);
    filletAction->setIcon(hiresPixmap(":/lucideicons/fillet.svg", iconColor, iconHeight));
    connect(filletAction, &QAction::triggered, this, &occWidget::makeFillet);
    _toolBar->addAction(filletAction);

    auto chamferAction = new QAction("Chamfer", this);
    chamferAction->setIcon(hiresPixmap(":/lucideicons/chamfer.svg", iconColor, iconHeight));
    connect(chamferAction, &QAction::triggered, this, &occWidget::makeChamfer);
    _toolBar->addAction(chamferAction);


    // add about action
    auto about = new QAction("About", this);
    about->setIcon(hiresPixmap(":/lucideicons/info.svg", iconColor, iconHeight));
    connect(about, &QAction::triggered, this, &occWidget::about);
    _toolBar->addAction(about);


    // add spacer widget to toolBar
    auto empty = new emptySpacerWidget(this);
    _toolBar->insertWidget(about, empty);


}


void occWidget::addBox()
{
    auto topoBox = BRepPrimAPI_MakeBox(3.0, 4.0, 5.0).Shape();

    Handle(AIS_ColoredShape) aisBoxShaded = new AIS_ColoredShape(topoBox);
    aisBoxShaded->SetColor(Quantity_NOC_AZURE);

    auto attrib = aisBoxShaded->Attributes();
    attrib->SetFaceBoundaryDraw(Standard_True);
    auto line = attrib->FaceBoundaryAspect();
    line->SetColor(Quantity_NOC_BLACK);
    line->SetWidth(2.0);
    attrib->SetFaceBoundaryAspect(line);
    aisBoxShaded->SetAttributes(attrib);

    _occView->getContext()->Display(aisBoxShaded, AIS_Shaded, 0, Standard_True);

//    Handle(AIS_ColoredShape) aisBoxWireframe = new AIS_ColoredShape(topoBox);
//    aisBoxWireframe->SetColor(Quantity_NOC_BLACK);
//    _occView->getContext()->Display(aisBoxWireframe, AIS_WireFrame, -1, Standard_True); // -1 -> cannot be selected

    _occView->fitAll();
}


void occWidget::addCone()
{
    gp_Ax2 axis;
    axis.SetLocation(gp_Pnt(0.0, 10.0, 0.0));

    auto topoReducer = BRepPrimAPI_MakeCone(axis, 3.0, 1.5, 5.0).Shape();

    Handle(AIS_Shape) aisReducerShaded = new AIS_Shape(topoReducer);
    aisReducerShaded->SetColor(Quantity_NOC_BISQUE);
    _occView->getContext()->Display(aisReducerShaded, AIS_Shaded, 0, Standard_True);

    Handle(AIS_Shape) aisReducerWireframe = new AIS_Shape(topoReducer);
    aisReducerWireframe->SetColor(Quantity_NOC_BLACK);
    _occView->getContext()->Display(aisReducerWireframe, AIS_WireFrame, -1, Standard_True);


    axis.SetLocation(gp_Pnt(8.0, 10.0, 0.0));

    auto topoCone = BRepPrimAPI_MakeCone(axis, 3.0, 0.0, 5.0).Shape();

    Handle(AIS_Shape) aisConeShaded = new AIS_Shape(topoCone);
    aisConeShaded->SetColor(Quantity_NOC_CHOCOLATE);
    _occView->getContext()->Display(aisConeShaded, AIS_Shaded, 0, Standard_True);

    Handle(AIS_Shape) aisConeWireframe = new AIS_Shape(topoCone);
    aisConeWireframe->SetColor(Quantity_NOC_BLACK);
    _occView->getContext()->Display(aisConeWireframe, AIS_WireFrame, -1, Standard_True);

    _occView->fitAll();
}


void occWidget::addSphere()
{
    gp_Ax2 axis;
    axis.SetLocation(gp_Pnt(0.0, 20.0, 3.0));

    auto topoSphere = BRepPrimAPI_MakeSphere(axis, 3.0).Shape();

    Handle(AIS_Shape) aisSphereShaded = new AIS_Shape(topoSphere);
    aisSphereShaded->SetColor(Quantity_NOC_BLUE1);
    _occView->getContext()->Display(aisSphereShaded, AIS_Shaded, 0, Standard_True);

    Handle(AIS_Shape) aisSphereWireframe = new AIS_Shape(topoSphere);
    aisSphereWireframe->SetColor(Quantity_NOC_BLACK);
    _occView->getContext()->Display(aisSphereWireframe, AIS_WireFrame, -1, Standard_True);

    _occView->fitAll();
}


void occWidget::addCylinder()
{
    gp_Ax2 axis;
    axis.SetLocation(gp_Pnt(0.0, 30.0, 0.0));

    auto topoCylinder = BRepPrimAPI_MakeCylinder(axis, 3.0, 5.0).Shape();

    Handle(AIS_Shape) aisCylinderShaded = new AIS_Shape(topoCylinder);
    aisCylinderShaded->SetColor(Quantity_NOC_RED);
    _occView->getContext()->Display(aisCylinderShaded, AIS_Shaded, 0, Standard_True);

    Handle(AIS_Shape) aisCylinderWireframe = new AIS_Shape(topoCylinder);
    aisCylinderWireframe->SetColor(Quantity_NOC_BLACK);
    _occView->getContext()->Display(aisCylinderWireframe, AIS_WireFrame, -1, Standard_True);

    axis.SetLocation(gp_Pnt(8.0, 30.0, 0.0));
    auto topoPie = BRepPrimAPI_MakeCylinder(axis, 3.0, 5.0, M_PI_2 * 3.0).Shape();

    Handle(AIS_Shape) aisPieShaded = new AIS_Shape(topoPie);
    aisPieShaded->SetColor(Quantity_NOC_TAN);
    _occView->getContext()->Display(aisPieShaded, AIS_Shaded, 0, Standard_True);

    Handle(AIS_Shape) aisPieWireframe = new AIS_Shape(topoPie);
    aisPieWireframe->SetColor(Quantity_NOC_BLACK);
    _occView->getContext()->Display(aisPieWireframe, AIS_WireFrame, -1, Standard_True);

    _occView->fitAll();
}


void occWidget::addTorus()
{
    gp_Ax2 axis;
    axis.SetLocation(gp_Pnt(0.0, 40.0, 0.0));

    auto topoTorus = BRepPrimAPI_MakeTorus(axis, 3.0, 1.0).Shape();

    Handle(AIS_Shape) aisTorusShaded = new AIS_Shape(topoTorus);
    aisTorusShaded->SetColor(Quantity_NOC_YELLOW);
    _occView->getContext()->Display(aisTorusShaded, AIS_Shaded, 0, Standard_True);

    Handle(AIS_Shape) aisTorusWireframe = new AIS_Shape(topoTorus);
    aisTorusWireframe->SetColor(Quantity_NOC_BLACK);
    _occView->getContext()->Display(aisTorusWireframe, AIS_WireFrame, 0, Standard_True);

    axis.SetLocation(gp_Pnt(8.0, 40.0, 0.0));
    auto topoElbow = BRepPrimAPI_MakeTorus(axis, 3.0, 1.0, M_PI_2).Shape();

    Handle(AIS_Shape) anAisElbowShaded = new AIS_Shape(topoElbow);
    anAisElbowShaded->SetColor(Quantity_NOC_THISTLE);
    _occView->getContext()->Display(anAisElbowShaded, AIS_Shaded, 0, Standard_True);

    Handle(AIS_Shape) anAisElbowWireframe = new AIS_Shape(topoElbow);
    anAisElbowWireframe->SetColor(Quantity_NOC_BLACK);
    _occView->getContext()->Display(anAisElbowWireframe, AIS_WireFrame, 0, Standard_True);

    _occView->fitAll();
}


void occWidget::addText()
{
    // Create a structure in this Viewer
    auto aStruct = _occView->getStruct();

    // Create a group of primitives in this structure
    auto group = aStruct->NewGroup();

    // change the text aspect
    auto textAspect = new Graphic3d_AspectText3d();
    textAspect->SetTextZoomable(false);
    textAspect->SetTextAngle(0.0);
    group->SetPrimitivesAspect(textAspect);

    // add a text primitive to the structure
    float fontHeight = 16.0f * devicePixelRatioF();
    auto text = new Graphic3d_Text(fontHeight);
    text->SetText("occQt6");
    text->SetPosition (gp_Pnt (-10, 10, 5));
    group->AddText(text);

    aStruct->Display();
    _occView->fitAll();
}


void occWidget::makeFillet()
{
    gp_Ax2 axis;
    axis.SetLocation(gp_Pnt(0.0, 50.0, 0.0));

    auto topoBox = BRepPrimAPI_MakeBox(axis, 3.0, 4.0, 5.0).Shape();
    BRepFilletAPI_MakeFillet MF(topoBox);

    // Add all the edges to fillet.
    for (TopExp_Explorer ex(topoBox, TopAbs_EDGE); ex.More(); ex.Next())
        MF.Add(1.0, TopoDS::Edge(ex.Current()));

    auto aisShapeShaded = new AIS_Shape(MF.Shape());
    this->setShapeAttributes(aisShapeShaded, Quantity_NOC_VIOLET);

    _occView->getContext()->Display(aisShapeShaded, AIS_Shaded, 0, Standard_True);

 _occView->fitAll();
}


void occWidget::makeChamfer()
{

}


void occWidget::setShapeAttributes(AIS_Shape *shape, const Quantity_Color color)
{
    shape->SetColor(color);

    auto attrib = shape->Attributes();
    attrib->SetFaceBoundaryDraw(Standard_True);

    auto line = attrib->FaceBoundaryAspect();
    line->SetColor(Quantity_NOC_BLACK);
    line->SetWidth(devicePixelRatio());

    attrib->SetFaceBoundaryAspect(line);

    shape->SetAttributes(attrib);
}
