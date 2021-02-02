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

#include <gp_Circ.hxx>
#include <gp_Elips.hxx>
#include <gp_Pln.hxx>

#include <TopoDS.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>

#include <BRepLib.hxx>

#include <BRepAlgoAPI_Cut.hxx>

#include <BRepBuilderAPI.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_MakePolygon.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepBuilderAPI_Transform.hxx>

#include <BRepFilletAPI_MakeChamfer.hxx>
#include <BRepFilletAPI_MakeFillet.hxx>

#include <BRepOffsetAPI_ThruSections.hxx>

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
    this->setMinimumSize(QSize(800,600));

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
    _toolBar->setIconSize(QSize(iconSize.width()*0.75, iconSize.height()*0.75));
    _toolBar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

    const int iconHeight = _toolBar->iconSize().height();
    const QString iconColor {"#778ca3"}; //blue grey

    // mouse actions
    auto mouseGroup = new QActionGroup(this);

    auto orbitAction = new QAction("Orbit", this);
    orbitAction->setCheckable(true);
    orbitAction->setIcon(hiresPixmap(":/icons/orbit.svg", iconColor, iconHeight));
    connect(orbitAction, &QAction::triggered, _occView, &occView::orbit);
    mouseGroup->addAction(orbitAction);

    auto selectAction = new QAction("Select", this);
    selectAction->setCheckable(true);
    selectAction->setIcon(hiresPixmap(":/icons/lucide/mouse-pointer.svg", iconColor, iconHeight));
    connect(selectAction, &QAction::triggered, _occView, &occView::select);
    mouseGroup->addAction(selectAction);

    auto zoomAction = new QAction("Zoom", this);
    zoomAction->setCheckable(true);
    zoomAction->setIcon(hiresPixmap(":/icons/lucide/zoom-in.svg", iconColor, iconHeight));
    connect(zoomAction, &QAction::triggered, _occView, &occView::zoom);
    mouseGroup->addAction(zoomAction);

    auto panAction = new QAction("Pan", this);
    panAction->setCheckable(true);
    panAction->setIcon(hiresPixmap(":/icons/lucide/move.svg", iconColor, iconHeight));
    connect(panAction, &QAction::triggered, _occView, &occView::pan);
    mouseGroup->addAction(panAction);

    auto rotate= new QAction("Rotate", this);
    rotate->setCheckable(true);
    rotate->setIcon(hiresPixmap(":/icons/lucide/rotate-ccw.svg", iconColor, iconHeight));
    connect(rotate, &QAction::triggered, _occView, &occView::rotation);
    mouseGroup->addAction(rotate);

    // select first action
    orbitAction->setChecked(true);

    _toolBar->addActions(mouseGroup->actions());

    // add separator
    _toolBar->addSeparator();

    // add reset action
    auto reset = new QAction("Reset View", this);
    reset->setIcon(hiresPixmap(":/icons/lucide/reset.svg", iconColor, iconHeight));
    connect(reset, &QAction::triggered, _occView, &occView::reset);
    _toolBar->addAction(reset);

    // add separator
    _toolBar->addSeparator();

    // add primitives
    auto boxAction = new QAction("Box", this);
    boxAction->setIcon(hiresPixmap(":/icons/lucide/box.svg", iconColor, iconHeight));
    connect(boxAction, &QAction::triggered, this, &occWidget::addBox);
    _toolBar->addAction(boxAction);

    auto coneAction = new QAction("Cone", this);
    coneAction->setIcon(hiresPixmap(":/icons/lucide/triangle.svg", iconColor, iconHeight));
    connect(coneAction, &QAction::triggered, this, &occWidget::addCone);
    _toolBar->addAction(coneAction);

    auto sphereAction = new QAction("Sphere", this);
    sphereAction->setIcon(hiresPixmap(":/icons/lucide/globe.svg", iconColor, iconHeight));
    connect(sphereAction, &QAction::triggered, this, &occWidget::addSphere);
    _toolBar->addAction(sphereAction);

    auto cylinderAction = new QAction("Cylinder", this);
    cylinderAction->setIcon(hiresPixmap(":/icons/lucide/database.svg", iconColor, iconHeight));
    connect(cylinderAction, &QAction::triggered, this, &occWidget::addCylinder);
    _toolBar->addAction(cylinderAction);

    auto torusAction = new QAction("Torus", this);
    torusAction->setIcon(hiresPixmap(":/icons/lucide/disc.svg", iconColor, iconHeight));
    connect(torusAction, &QAction::triggered, this, &occWidget::addTorus);
    _toolBar->addAction(torusAction);

    auto textAction = new QAction("Text", this);
    textAction->setIcon(hiresPixmap(":/icons/lucide/type.svg", iconColor, iconHeight));
    connect(textAction, &QAction::triggered, this, &occWidget::addText);
    _toolBar->addAction(textAction);

    // add separator
    _toolBar->addSeparator();

    // add fillet,chamfer, etc
    auto filletAction = new QAction("Fillet", this);
    filletAction->setIcon(hiresPixmap(":/icons/fillet.svg", iconColor, iconHeight));
    connect(filletAction, &QAction::triggered, this, &occWidget::makeFillet);
    _toolBar->addAction(filletAction);

    auto chamferAction = new QAction("Chamfer", this);
    chamferAction->setIcon(hiresPixmap(":/icons/chamfer.svg", iconColor, iconHeight));
    connect(chamferAction, &QAction::triggered, this, &occWidget::makeChamfer);
    _toolBar->addAction(chamferAction);

    auto extrudeAction = new QAction("Extrude", this);
    extrudeAction->setIcon(hiresPixmap(":/icons/extrude.svg", iconColor, iconHeight));
    connect(extrudeAction, &QAction::triggered, this, &occWidget::makeExtrude);
    _toolBar->addAction(extrudeAction);

    auto revolAction = new QAction("Revolve", this);
    revolAction->setIcon(hiresPixmap(":/icons/revol.svg", iconColor, iconHeight));
    connect(revolAction, &QAction::triggered, this, &occWidget::makeRevol);
    _toolBar->addAction(revolAction);

    auto loftAction = new QAction("Loft", this);
    loftAction->setIcon(hiresPixmap(":/icons/loft.svg", iconColor, iconHeight));
    connect(loftAction, &QAction::triggered, this, &occWidget::makeLoft);
    _toolBar->addAction(loftAction);

    _toolBar->addSeparator();

    auto boolCutAction = new QAction("Bool Cut", this);
    boolCutAction->setIcon(hiresPixmap(":/icons/boolCut.svg", iconColor, iconHeight));
    connect(boolCutAction, &QAction::triggered, this, &occWidget::boolCut);
    _toolBar->addAction(boolCutAction);

    // add about action
    auto about = new QAction("About", this);
    about->setIcon(hiresPixmap(":/icons/lucide/info.svg", iconColor, iconHeight));
    connect(about, &QAction::triggered, this, &occWidget::about);
    _toolBar->addAction(about);


    // add spacer widget to toolBar
    auto empty = new emptySpacerWidget(this);
    _toolBar->insertWidget(about, empty);


}


void occWidget::addBox()
{
    auto topoBox = BRepPrimAPI_MakeBox(3.0, 4.0, 5.0).Shape();
    auto aisBox = new AIS_Shape(topoBox);
    setShapeAttributes(aisBox, Quantity_NOC_AZURE);

    _occView->getContext()->Display(aisBox, Standard_True);
    _occView->fitAll();
}


void occWidget::addCone()
{
    gp_Ax2 axis;
    axis.SetLocation(gp_Pnt(0.0, 10.0, 0.0));

    auto topoReducer = BRepPrimAPI_MakeCone(axis, 3.0, 1.5, 5.0).Shape();
    auto aisReducer = new AIS_Shape(topoReducer);
    setShapeAttributes(aisReducer, Quantity_NOC_BISQUE);

    axis.SetLocation(gp_Pnt(8.0, 10.0, 0.0));

    auto topoCone = BRepPrimAPI_MakeCone(axis, 3.0, 0.0, 5.0).Shape();
    auto aisCone = new AIS_Shape(topoCone);
    setShapeAttributes(aisCone, Quantity_NOC_CHOCOLATE);

    _occView->getContext()->Display(aisReducer, Standard_True);
    _occView->getContext()->Display(aisCone, Standard_True);
    _occView->fitAll();
}


void occWidget::addSphere()
{
    gp_Ax2 axis;
    axis.SetLocation(gp_Pnt(0.0, 20.0, 3.0));

    auto topoSphere = BRepPrimAPI_MakeSphere(axis, 3.0).Shape();
    auto aisSphere = new AIS_Shape(topoSphere);
    setShapeAttributes(aisSphere, Quantity_NOC_BLUE1);

    _occView->getContext()->Display(aisSphere, Standard_True);
    _occView->fitAll();
}


void occWidget::addCylinder()
{
    gp_Ax2 axis;
    axis.SetLocation(gp_Pnt(0.0, 30.0, 0.0));

    auto topoCylinder = BRepPrimAPI_MakeCylinder(axis, 3.0, 5.0).Shape();
    auto aisCylinder = new AIS_Shape(topoCylinder);
    setShapeAttributes(aisCylinder, Quantity_NOC_RED);

    axis.SetLocation(gp_Pnt(8.0, 30.0, 0.0));

    auto topoPie = BRepPrimAPI_MakeCylinder(axis, 3.0, 5.0, M_PI_2 * 3.0).Shape();
    auto aisPie = new AIS_Shape(topoPie);
    setShapeAttributes(aisPie, Quantity_NOC_TAN);

    _occView->getContext()->Display(aisCylinder, Standard_True);
    _occView->getContext()->Display(aisPie, Standard_True);
    _occView->fitAll();
}


void occWidget::addTorus()
{
    gp_Ax2 axis;
    axis.SetLocation(gp_Pnt(0.0, 40.0, 0.0));

    auto topoTorus = BRepPrimAPI_MakeTorus(axis, 3.0, 1.0).Shape();
    auto aisTorus = new AIS_Shape(topoTorus);
    setShapeAttributes(aisTorus, Quantity_NOC_YELLOW);

    axis.SetLocation(gp_Pnt(8.0, 40.0, 0.0));

    auto topoElbow = BRepPrimAPI_MakeTorus(axis, 3.0, 1.0, M_PI_2).Shape();
    auto aisElbow = new AIS_Shape(topoElbow);
    setShapeAttributes(aisElbow, Quantity_NOC_THISTLE);

    _occView->getContext()->Display(aisTorus, Standard_True);
    _occView->getContext()->Display(aisElbow, Standard_True);

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

    auto aisShape = new AIS_Shape(MF.Shape());
    this->setShapeAttributes(aisShape, Quantity_NOC_VIOLET);

    _occView->getContext()->Display(aisShape, Standard_True);
    _occView->fitAll();
}


void occWidget::makeChamfer()
{
    gp_Ax2 axis;
    axis.SetLocation(gp_Pnt(8.0, 50.0, 0.0));

    auto topoBox = BRepPrimAPI_MakeBox(axis, 3.0, 4.0, 5.0).Shape();
    BRepFilletAPI_MakeChamfer MC(topoBox);
    TopTools_IndexedDataMapOfShapeListOfShape aEdgeFaceMap;

    TopExp::MapShapesAndAncestors(topoBox, TopAbs_EDGE, TopAbs_FACE, aEdgeFaceMap);

    for (Standard_Integer i = 1; i <= aEdgeFaceMap.Extent(); ++i)
    {
        TopoDS_Edge anEdge = TopoDS::Edge(aEdgeFaceMap.FindKey(i));
        TopoDS_Face aFace = TopoDS::Face(aEdgeFaceMap.FindFromIndex(i).First());
        MC.Add(0.6, 0.6, anEdge, aFace);
    }

    auto aisShape = new AIS_Shape(MC.Shape());
    setShapeAttributes(aisShape, Quantity_NOC_TOMATO);

    _occView->getContext()->Display(aisShape, Standard_True);
    _occView->fitAll();
}


void occWidget::makeExtrude()
{
    // prism a vertex result is an edge.
    auto vertex = BRepBuilderAPI_MakeVertex(gp_Pnt(0.0, 60.0, 0.0));
    auto prismVertex = BRepPrimAPI_MakePrism(vertex, gp_Vec(0.0, 0.0, 5.0));
    auto aisPrismVertex = new AIS_Shape(prismVertex);

    // prism an edge result is a face.
    auto edge = BRepBuilderAPI_MakeEdge(gp_Pnt(5.0, 60.0, 0.0), gp_Pnt(10.0, 60.0, 0.0));
    auto prismEdge = BRepPrimAPI_MakePrism(edge, gp_Vec(0.0, 0.0, 5.0));
    auto aisPrismEdge = new AIS_Shape(prismEdge);

    // prism a wire result is a shell.
    gp_Ax2 axis;
    axis.SetLocation(gp_Pnt(16.0, 60.0, 0.0));

    auto circleEdge = BRepBuilderAPI_MakeEdge(gp_Circ(axis, 3.0));
    auto circleWire = BRepBuilderAPI_MakeWire(circleEdge);
    auto prismCircle = BRepPrimAPI_MakePrism(circleWire, gp_Vec(0.0, 0.0, 5.0));
    auto aisPrismCircle = new AIS_Shape(prismCircle);

    // prism a face or a shell result is a solid.
    axis.SetLocation(gp_Pnt(24.0, 60.0, 0.0));
    auto ellipseEdge = BRepBuilderAPI_MakeEdge(gp_Elips(axis, 3.0, 2.0));
    auto ellipseWire = BRepBuilderAPI_MakeWire(ellipseEdge);
    auto ellipseFace = BRepBuilderAPI_MakeFace(gp_Pln(gp::XOY()), ellipseWire);
    auto prismEllipse = BRepPrimAPI_MakePrism(ellipseFace, gp_Vec(0.0, 0.0, 5.0));
    auto aisPrismEllipse = new AIS_Shape(prismEllipse);

    setShapeAttributes(aisPrismVertex, Quantity_NOC_PAPAYAWHIP);
    setShapeAttributes(aisPrismEdge, Quantity_NOC_PEACHPUFF);
    setShapeAttributes(aisPrismCircle, Quantity_NOC_PERU);
    setShapeAttributes(aisPrismEllipse, Quantity_NOC_PINK);

    _occView->getContext()->Display(aisPrismVertex, Standard_True);
    _occView->getContext()->Display(aisPrismEdge, Standard_True);
    _occView->getContext()->Display(aisPrismCircle, Standard_True);
    _occView->getContext()->Display(aisPrismEllipse, Standard_True);
    _occView->fitAll();
}


void occWidget::makeRevol()
{
    gp_Ax1 axis;

    // revol a vertex, result is an edge.
    axis.SetLocation(gp_Pnt(0.0, 70.0, 0.0));
    auto vertex = BRepBuilderAPI_MakeVertex(gp_Pnt(2.0, 70.0, 0.0));
    auto revolVertex = BRepPrimAPI_MakeRevol(vertex, axis);
    auto aisRevolVertex = new AIS_Shape(revolVertex);

    // revol an edge, result is a face.
    axis.SetLocation(gp_Pnt(8.0, 70.0, 0.0));
    auto edge = BRepBuilderAPI_MakeEdge(gp_Pnt(6.0, 70.0, 0.0), gp_Pnt(6.0, 70.0, 5.0));
    auto revolEdge = BRepPrimAPI_MakeRevol(edge, axis);
    auto aisRevolEdge = new AIS_Shape(revolEdge);

    // revol a wire, result is a shell.
    axis.SetLocation(gp_Pnt(20.0, 70.0, 0.0));
    axis.SetDirection(gp::DY());

    auto circleEdge = BRepBuilderAPI_MakeEdge(gp_Circ(gp_Ax2(gp_Pnt(15.0, 70.0, 0.0), gp::DZ()), 1.5));
    auto circleWire = BRepBuilderAPI_MakeWire(circleEdge);
    auto revolCircle = BRepPrimAPI_MakeRevol(circleWire, axis, M_PI_2);
    auto aisRevolCircle = new AIS_Shape(revolCircle);

    // revol a face, result is a solid.
    axis.SetLocation(gp_Pnt(30.0, 70.0, 0.0));
    axis.SetDirection(gp::DY());

    auto ellipseEdge = BRepBuilderAPI_MakeEdge(gp_Elips(gp_Ax2(gp_Pnt(25.0, 70.0, 0.0), gp::DZ()), 3.0, 2.0));
    auto ellipseWire = BRepBuilderAPI_MakeWire(ellipseEdge);
    auto ellipseFace = BRepBuilderAPI_MakeFace(gp_Pln(gp::XOY()), ellipseWire);
    auto revolEllipse = BRepPrimAPI_MakeRevol(ellipseFace, axis, M_PI_4);
    auto aisRevolEllipse = new AIS_Shape(revolEllipse);

    setShapeAttributes(aisRevolVertex, Quantity_NOC_LIMEGREEN);
    setShapeAttributes(aisRevolEdge, Quantity_NOC_LINEN);
    setShapeAttributes(aisRevolCircle, Quantity_NOC_MAGENTA1);
    setShapeAttributes(aisRevolEllipse, Quantity_NOC_MAROON);

    _occView->getContext()->Display(aisRevolVertex, Standard_True);
    _occView->getContext()->Display(aisRevolEdge, Standard_True);
    _occView->getContext()->Display(aisRevolCircle, Standard_True);
    _occView->getContext()->Display(aisRevolEllipse, Standard_True);
    _occView->fitAll();
}


void occWidget::makeLoft()
{
    // bottom wire.
    auto circleEdge = BRepBuilderAPI_MakeEdge(
                gp_Circ(
                    gp_Ax2(gp_Pnt(0.0, 80.0, 0.0), gp::DZ()),
                    1.5)
                );
    auto circleWire = BRepBuilderAPI_MakeWire(circleEdge);

    // top wire.
    BRepBuilderAPI_MakePolygon polygon;
    polygon.Add(gp_Pnt(-3.0, 77.0, 6.0));
    polygon.Add(gp_Pnt(3.0, 77.0, 6.0));
    polygon.Add(gp_Pnt(3.0, 83.0, 6.0));
    polygon.Add(gp_Pnt(-3.0, 83.0, 6.0));
    polygon.Close();

    BRepOffsetAPI_ThruSections shellGenerator;
    BRepOffsetAPI_ThruSections solidGenerator(true);

    shellGenerator.AddWire(circleWire);
    shellGenerator.AddWire(polygon.Wire());

    solidGenerator.AddWire(circleWire);
    solidGenerator.AddWire(polygon.Wire());

    // translate the solid.
    gp_Trsf aTrsf;
    aTrsf.SetTranslation(gp_Vec(18.0, 0.0, 0.0));
    BRepBuilderAPI_Transform transform(solidGenerator.Shape(), aTrsf);

    auto aisShell = new AIS_Shape(shellGenerator.Shape());
    auto aisSolid = new AIS_Shape(transform.Shape());

    setShapeAttributes(aisShell, Quantity_NOC_OLIVEDRAB);
    setShapeAttributes(aisSolid, Quantity_NOC_PEACHPUFF);

    _occView->getContext()->Display(aisShell, Standard_True);
    _occView->getContext()->Display(aisSolid, Standard_True);
    _occView->fitAll();
}


void occWidget::boolCut()
{
    gp_Ax2 axis;
    axis.SetLocation(gp_Pnt(0.0, 90.0, 0.0));

    auto topoBox = BRepPrimAPI_MakeBox(axis, 3.0, 4.0, 5.0).Shape();
    auto topoSphere = BRepPrimAPI_MakeSphere(axis, 2.5).Shape();
    auto cutShape1 = BRepAlgoAPI_Cut(topoBox, topoSphere);
    auto cutShape2 = BRepAlgoAPI_Cut(topoSphere, topoBox);

    gp_Trsf aTrsf;
    aTrsf.SetTranslation(gp_Vec(8.0, 0.0, 0.0));
    BRepBuilderAPI_Transform transform1(cutShape1, aTrsf);

    aTrsf.SetTranslation(gp_Vec(16.0, 0.0, 0.0));
    BRepBuilderAPI_Transform transform2(cutShape2, aTrsf);

    auto aisBox = new AIS_Shape(topoBox);
    auto aisSphere = new AIS_Shape(topoSphere);
    auto aisCutShape1 = new AIS_Shape(transform1.Shape());
    auto aisCutShape2 = new AIS_Shape(transform2.Shape());

    setShapeAttributes(aisBox, Quantity_NOC_SPRINGGREEN);
    setShapeAttributes(aisSphere, Quantity_NOC_STEELBLUE);
    setShapeAttributes(aisCutShape1, Quantity_NOC_TAN);
    setShapeAttributes(aisCutShape2, Quantity_NOC_SALMON);

    _occView->getContext()->Display(aisBox, Standard_True);
    _occView->getContext()->Display(aisSphere, Standard_True);
    _occView->getContext()->Display(aisCutShape1, Standard_True);
    _occView->getContext()->Display(aisCutShape2, Standard_True);
    _occView->fitAll();
}


void occWidget::setShapeAttributes(AIS_Shape *shape, const Quantity_Color color)
{
    shape->SetColor(color);
    auto attrib = shape->Attributes();
    auto line = attrib->FaceBoundaryAspect();
    line->SetColor(Quantity_NOC_BLACK);
    line->SetWidth(2.0);
    attrib->SetFaceBoundaryDraw(Standard_True);
    attrib->SetFaceBoundaryAspect(line);
    shape->SetAttributes(attrib);
}

