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

#include <Standard_WarningsDisable.hxx>
#include <QAction>
#include <QActionGroup>
#include <QCloseEvent>
#include <QEvent>
#include <QMessageBox>
#include <QToolBar>
#include <QVBoxLayout>
#include <Standard_WarningsRestore.hxx>

// occ headers
#include <Standard_Version.hxx>

#include <gp_Circ.hxx>
#include <gp_Elips.hxx>
#include <gp_Lin2d.hxx>
#include <gp_Pln.hxx>

#include <BRepLib.hxx>

#include <BRepAlgoAPI_Common.hxx>
#include <BRepAlgoAPI_Cut.hxx>
#include <BRepAlgoAPI_Fuse.hxx>

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

#include <BRepOffsetAPI_MakePipe.hxx>

#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepPrimAPI_MakeCone.hxx>
#include <BRepPrimAPI_MakeSphere.hxx>
#include <BRepPrimAPI_MakeCylinder.hxx>
#include <BRepPrimAPI_MakeTorus.hxx>
#include <BRepPrimAPI_MakePrism.hxx>
#include <BRepPrimAPI_MakeRevol.hxx>

#include <GCE2d_MakeSegment.hxx>

#include <Geom_ConicalSurface.hxx>
#include <Geom_ToroidalSurface.hxx>
#include <Geom_CylindricalSurface.hxx>

#include <Graphic3d_Group.hxx>
#include <Graphic3d_Text.hxx>

#include <TopoDS.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>

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

    _hudWidget = new hudWidget();
    _hudWidget->setWindowFlags(Qt::FramelessWindowHint);// | Qt::WindowStaysOnTopHint);

    _hudWidget->show();
    // center, top
    //    auto pX =  0.5*(this->width() - orW->width());
    //    auto pY = _toolBar->pos().y() + _toolBar->height() + this->contentsMargins().top() + 1;
    //    orW->move(mapToGlobal(QPoint(pX,pY)));

    // bottom left
    //    auto pX =  this->contentsMargins().left();
    //    auto pY = this->height() - this->contentsMargins().bottom() - _hudWidget->height();
    //    _hudWidget->move(mapToGlobal(QPoint(pX,pY)));

    this->raise();
    _hudWidget->raise();

    // https://stackoverflow.com/questions/25466030/make-qwidget-transparent
}

// ------------------------------------------------------------------------------------------------
// protected functions
// ------------------------------------------------------------------------------------------------
void occWidget::closeEvent(QCloseEvent* event)
{
    _hudWidget->close();
    delete _hudWidget;
    event->accept();
}


bool occWidget::event(QEvent *event)
{
    switch (event->type())
    {
    case QEvent::Show:
        //_hudWidget->show();
        //QTimer::singleShot(50, this, SLOT(widgetSizeMove()));
        //Wait until the Main Window be shown
        break;
    case QEvent::WindowActivate:
    case QEvent::Resize:
    case QEvent::Move:
        hudWidgetMove();
        break;
    default:
        break;
    }

    return QWidget::event(event);
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


QAction* occWidget::addActionToToolBar(QString iconText,
                                       QString iconFileName,
                                       QString toolTipText,
                                       bool addToToolBar)
{

    const int iconHeight {_toolBar->iconSize().height()};
    auto action = new QAction(iconText, this);
    action->setToolTip(toolTipText);
    iconFileName.prepend(":/icons/");
    action->setIcon(hiresPixmap(iconFileName, iconHeight));
    if (addToToolBar)
        _toolBar->addAction(action);

    return action;
}


void occWidget::hudWidgetMove()
{
    if (_hudWidget)
    {
        // bottom left
        const int pX =  this->contentsMargins().left();
        const int pY = this->height() - this->contentsMargins().bottom() - _hudWidget->height();
        _hudWidget->move(mapToGlobal(QPoint(pX,pY)));
        _hudWidget->raise();
    }
}

void occWidget::populateToolBar()
{
    _toolBar->setToolButtonStyle(Qt::ToolButtonFollowStyle);

#if __APPLE__
    auto iconSize = _toolBar->iconSize();
    _toolBar->setIconSize(iconSize*0.75);
#endif

    // mouse actions
    bool addToToolbar {false};
    auto orbitAction = addActionToToolBar("Orbit", "orbit.svg", "Orbit mode", addToToolbar);
    auto selectAction = addActionToToolBar("Select", "lucide/mouse-pointer.svg", "Select items", addToToolbar);
    auto zoomAction = addActionToToolBar("Zoom", "lucide/zoom-in.svg", "Zoom mode", addToToolbar);
    auto panAction = addActionToToolBar("Pan", "lucide/move.svg", "Pan mode", addToToolbar);
    auto rotateAction = addActionToToolBar("Rotate", "lucide/rotate-ccw.svg", "Rotate mode", addToToolbar);

    orbitAction->setCheckable(true);
    selectAction->setCheckable(true);
    zoomAction->setCheckable(true);
    panAction->setCheckable(true);
    rotateAction->setCheckable(true);
    // select first action
    orbitAction->setChecked(true);
    // add to QActionGroup and then add to toolbar
    auto mouseGroup = new QActionGroup(this);
    mouseGroup->addAction(orbitAction);
    mouseGroup->addAction(selectAction);
    mouseGroup->addAction(zoomAction);
    mouseGroup->addAction(panAction);
    mouseGroup->addAction(rotateAction);
    _toolBar->addActions(mouseGroup->actions());

    // all actions below will be added to the toolbar directly
    addToToolbar = true;

    // add reset view action
    _toolBar->addSeparator();
    auto reset = addActionToToolBar("Reset View", "lucide/reset.svg", "Reset view", addToToolbar);// new QAction("Reset View", this);

    // add primitives
    _toolBar->addSeparator();
    auto boxAction = addActionToToolBar("Box", "lucide/box.svg", "Add box", addToToolbar);// new QAction("Box", this);
    auto coneAction = addActionToToolBar("Cone", "lucide/triangle.svg", "Add triangles", addToToolbar); // new QAction("Cone", this);
    auto sphereAction = addActionToToolBar("Sphere", "lucide/globe.svg", "Add sphere", addToToolbar);
    auto cylinderAction = addActionToToolBar("Cylinder", "lucide/database.svg", "Add cylinders", addToToolbar);
    auto torusAction = addActionToToolBar("Torus", "lucide/disc.svg", "Add torus", addToToolbar);
    auto textAction = addActionToToolBar("Text", "lucide/type.svg", "Add text", addToToolbar);

    // add fillet,chamfer, etc
    _toolBar->addSeparator();
    auto filletAction = addActionToToolBar("Fillet", "fillet.svg", "Add fillet example", addToToolbar);
    auto chamferAction = addActionToToolBar("Chamfer", "chamfer.svg", "Add chamfer example", addToToolbar);
    auto extrudeAction = addActionToToolBar("Extrude", "extrude.svg", "Add extrusion example", addToToolbar);
    auto revolAction = addActionToToolBar("Revolve", "revol.svg", "Add revolution example", addToToolbar);
    auto loftAction = addActionToToolBar("Loft", "loft.svg", "Add loft example", addToToolbar);

    // boolean operations
    _toolBar->addSeparator();
    auto boolCutAction = addActionToToolBar("Boolean Cut", "boolCut.svg", "Add Boolean cut operation", addToToolbar);
    auto boolFuseAction = addActionToToolBar("Boolean Fuse", "boolFuse.svg", "Add Boolean fuse operation", addToToolbar);
    auto boolCommonAction = addActionToToolBar("Boolean Common", "boolCommon.svg", "Add Boolean common operation", addToToolbar);

    // draw helices
    _toolBar->addSeparator();
    auto helixAction = addActionToToolBar("Helices", "helix.svg", "add helices example", addToToolbar);

    // add about action
    auto about = addActionToToolBar("About", "lucide/info.svg", "About occQt6", addToToolbar);

    // add spacer widget to toolBar
    auto empty = new emptySpacerWidget(this);
    _toolBar->insertWidget(about, empty);

    // connections
    connect(orbitAction, &QAction::triggered, _occView, &occView::orbit);
    connect(selectAction, &QAction::triggered, _occView, &occView::select);
    connect(zoomAction, &QAction::triggered, _occView, &occView::zoom);
    connect(panAction, &QAction::triggered, _occView, &occView::pan);
    connect(rotateAction, &QAction::triggered, _occView, &occView::rotation);

    connect(reset, &QAction::triggered, _occView, &occView::reset);

    connect(boxAction, &QAction::triggered, this, &occWidget::addBox);
    connect(coneAction, &QAction::triggered, this, &occWidget::addCone);
    connect(sphereAction, &QAction::triggered, this, &occWidget::addSphere);
    connect(cylinderAction, &QAction::triggered, this, &occWidget::addCylinder);
    connect(torusAction, &QAction::triggered, this, &occWidget::addTorus);
    connect(textAction, &QAction::triggered, this, &occWidget::addText);

    connect(filletAction, &QAction::triggered, this, &occWidget::makeFillet);
    connect(chamferAction, &QAction::triggered, this, &occWidget::makeChamfer);
    connect(extrudeAction, &QAction::triggered, this, &occWidget::makeExtrude);
    connect(revolAction, &QAction::triggered, this, &occWidget::makeRevol);
    connect(loftAction, &QAction::triggered, this, &occWidget::makeLoft);

    connect(boolCutAction, &QAction::triggered, this, &occWidget::boolCut);
    connect(boolFuseAction, &QAction::triggered, this, &occWidget::boolFuse);
    connect(boolCommonAction, &QAction::triggered, this, &occWidget::boolCommon);

    connect(helixAction, &QAction::triggered, this, &occWidget::testHelix);

    connect(about, &QAction::triggered, this, &occWidget::about);
}


void occWidget::setShapeAttributes(Handle(AIS_Shape) shape, Quantity_Color color)
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


void occWidget::addBox()
{
    TopoDS_Shape topoBox = BRepPrimAPI_MakeBox(3.0, 4.0, 5.0).Shape();
    Handle(AIS_Shape) aisBox = new AIS_Shape(topoBox);
    setShapeAttributes(aisBox, Quantity_NOC_AZURE);

    _occView->getContext()->Display(aisBox, Standard_True);
    _occView->fitAll();
}


void occWidget::addCone()
{
    gp_Ax2 axis;
    axis.SetLocation(gp_Pnt(0.0, 10.0, 0.0));

    TopoDS_Shape topoReducer = BRepPrimAPI_MakeCone(axis, 3.0, 1.5, 5.0).Shape();
    Handle(AIS_Shape) aisReducer = new AIS_Shape(topoReducer);
    setShapeAttributes(aisReducer, Quantity_NOC_BISQUE);

    axis.SetLocation(gp_Pnt(8.0, 10.0, 0.0));

    TopoDS_Shape topoCone = BRepPrimAPI_MakeCone(axis, 3.0, 0.0, 5.0).Shape();
    Handle(AIS_Shape) aisCone = new AIS_Shape(topoCone);
    setShapeAttributes(aisCone, Quantity_NOC_CHOCOLATE);

    _occView->getContext()->Display(aisReducer, Standard_True);
    _occView->getContext()->Display(aisCone, Standard_True);
    _occView->fitAll();
}


void occWidget::addSphere()
{
    gp_Ax2 axis;
    axis.SetLocation(gp_Pnt(0.0, 20.0, 3.0));

    TopoDS_Shape topoSphere = BRepPrimAPI_MakeSphere(axis, 3.0).Shape();
    Handle(AIS_Shape) aisSphere = new AIS_Shape(topoSphere);
    setShapeAttributes(aisSphere, Quantity_NOC_BLUE1);

    _occView->getContext()->Display(aisSphere, Standard_True);
    _occView->fitAll();
}


void occWidget::addCylinder()
{
    gp_Ax2 axis;
    axis.SetLocation(gp_Pnt(0.0, 30.0, 0.0));

    TopoDS_Shape topoCylinder = BRepPrimAPI_MakeCylinder(axis, 3.0, 5.0).Shape();
    Handle(AIS_Shape) aisCylinder = new AIS_Shape(topoCylinder);
    setShapeAttributes(aisCylinder, Quantity_NOC_RED);

    axis.SetLocation(gp_Pnt(8.0, 30.0, 0.0));

    TopoDS_Shape topoPie = BRepPrimAPI_MakeCylinder(axis, 3.0, 5.0, M_PI_2 * 3.0).Shape();
    Handle(AIS_Shape) aisPie = new AIS_Shape(topoPie);
    setShapeAttributes(aisPie, Quantity_NOC_TAN);

    _occView->getContext()->Display(aisCylinder, Standard_True);
    _occView->getContext()->Display(aisPie, Standard_True);
    _occView->fitAll();
}


void occWidget::addTorus()
{
    gp_Ax2 axis;
    axis.SetLocation(gp_Pnt(0.0, 40.0, 0.0));

    TopoDS_Shape topoTorus = BRepPrimAPI_MakeTorus(axis, 3.0, 1.0).Shape();
    Handle(AIS_Shape) aisTorus = new AIS_Shape(topoTorus);
    setShapeAttributes(aisTorus, Quantity_NOC_YELLOW);

    axis.SetLocation(gp_Pnt(8.0, 40.0, 0.0));

    TopoDS_Shape topoElbow = BRepPrimAPI_MakeTorus(axis, 3.0, 1.0, M_PI_2).Shape();
    Handle(AIS_Shape) aisElbow = new AIS_Shape(topoElbow);
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
    Handle(Graphic3d_AspectText3d) textAspect = new Graphic3d_AspectText3d();
    textAspect->SetTextZoomable(false);
    textAspect->SetTextAngle(0.0);
    group->SetPrimitivesAspect(textAspect);

    // add a text primitive to the structure
    float fontHeight = 16.0f * devicePixelRatioF();
    Handle(Graphic3d_Text) text = new Graphic3d_Text(fontHeight);
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

    TopoDS_Shape topoBox = BRepPrimAPI_MakeBox(axis, 3.0, 4.0, 5.0).Shape();
    BRepFilletAPI_MakeFillet MF(topoBox);

    // Add all the edges to fillet.
    for (TopExp_Explorer ex(topoBox, TopAbs_EDGE); ex.More(); ex.Next())
        MF.Add(1.0, TopoDS::Edge(ex.Current()));

    Handle(AIS_Shape) aisShape = new AIS_Shape(MF.Shape());
    this->setShapeAttributes(aisShape, Quantity_NOC_VIOLET);

    _occView->getContext()->Display(aisShape, Standard_True);
    _occView->fitAll();
}


void occWidget::makeChamfer()
{
    gp_Ax2 axis;
    axis.SetLocation(gp_Pnt(8.0, 50.0, 0.0));

    TopoDS_Shape topoBox = BRepPrimAPI_MakeBox(axis, 3.0, 4.0, 5.0).Shape();
    BRepFilletAPI_MakeChamfer MC(topoBox);
    TopTools_IndexedDataMapOfShapeListOfShape aEdgeFaceMap;

    TopExp::MapShapesAndAncestors(topoBox, TopAbs_EDGE, TopAbs_FACE, aEdgeFaceMap);

    for (Standard_Integer i = 1; i <= aEdgeFaceMap.Extent(); ++i)
    {
        TopoDS_Edge anEdge = TopoDS::Edge(aEdgeFaceMap.FindKey(i));
        TopoDS_Face aFace = TopoDS::Face(aEdgeFaceMap.FindFromIndex(i).First());
        MC.Add(0.6, 0.6, anEdge, aFace);
    }

    Handle(AIS_Shape) aisShape = new AIS_Shape(MC.Shape());
    setShapeAttributes(aisShape, Quantity_NOC_TOMATO);

    _occView->getContext()->Display(aisShape, Standard_True);
    _occView->fitAll();
}


void occWidget::makeExtrude()
{
    // prism a vertex result is an edge.
    TopoDS_Vertex vertex = BRepBuilderAPI_MakeVertex(gp_Pnt(0.0, 60.0, 0.0));
    TopoDS_Shape prismVertex = BRepPrimAPI_MakePrism(vertex, gp_Vec(0.0, 0.0, 5.0));
    Handle(AIS_Shape) aisPrismVertex = new AIS_Shape(prismVertex);

    // prism an edge result is a face.
    TopoDS_Edge edge = BRepBuilderAPI_MakeEdge(gp_Pnt(5.0, 60.0, 0.0), gp_Pnt(10.0, 60.0, 0.0));
    TopoDS_Shape prismEdge = BRepPrimAPI_MakePrism(edge, gp_Vec(0.0, 0.0, 5.0));
    Handle(AIS_Shape) aisPrismEdge = new AIS_Shape(prismEdge);

    // prism a wire result is a shell.
    gp_Ax2 axis;
    axis.SetLocation(gp_Pnt(16.0, 60.0, 0.0));

    TopoDS_Edge circleEdge = BRepBuilderAPI_MakeEdge(gp_Circ(axis, 3.0));
    TopoDS_Wire circleWire = BRepBuilderAPI_MakeWire(circleEdge);
    TopoDS_Shape prismCircle = BRepPrimAPI_MakePrism(circleWire, gp_Vec(0.0, 0.0, 5.0));
    Handle(AIS_Shape) aisPrismCircle = new AIS_Shape(prismCircle);

    // prism a face or a shell result is a solid.
    axis.SetLocation(gp_Pnt(24.0, 60.0, 0.0));
    TopoDS_Edge ellipseEdge = BRepBuilderAPI_MakeEdge(gp_Elips(axis, 3.0, 2.0));
    TopoDS_Wire ellipseWire = BRepBuilderAPI_MakeWire(ellipseEdge);
    TopoDS_Face ellipseFace = BRepBuilderAPI_MakeFace(gp_Pln(gp::XOY()), ellipseWire);
    TopoDS_Shape prismEllipse = BRepPrimAPI_MakePrism(ellipseFace, gp_Vec(0.0, 0.0, 5.0));
    Handle(AIS_Shape) aisPrismEllipse = new AIS_Shape(prismEllipse);

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
    TopoDS_Vertex vertex = BRepBuilderAPI_MakeVertex(gp_Pnt(2.0, 70.0, 0.0));
    TopoDS_Shape revolVertex = BRepPrimAPI_MakeRevol(vertex, axis);
    Handle(AIS_Shape) aisRevolVertex = new AIS_Shape(revolVertex);

    // revol an edge, result is a face.
    axis.SetLocation(gp_Pnt(8.0, 70.0, 0.0));
    TopoDS_Edge edge = BRepBuilderAPI_MakeEdge(gp_Pnt(6.0, 70.0, 0.0), gp_Pnt(6.0, 70.0, 5.0));
    TopoDS_Shape revolEdge = BRepPrimAPI_MakeRevol(edge, axis);
    Handle(AIS_Shape) aisRevolEdge = new AIS_Shape(revolEdge);

    // revol a wire, result is a shell.
    axis.SetLocation(gp_Pnt(20.0, 70.0, 0.0));
    axis.SetDirection(gp::DY());

    TopoDS_Edge circleEdge = BRepBuilderAPI_MakeEdge(gp_Circ(gp_Ax2(gp_Pnt(15.0, 70.0, 0.0), gp::DZ()), 1.5));
    TopoDS_Wire circleWire = BRepBuilderAPI_MakeWire(circleEdge);
    TopoDS_Shape revolCircle = BRepPrimAPI_MakeRevol(circleWire, axis, M_PI_2);
    Handle(AIS_Shape) aisRevolCircle = new AIS_Shape(revolCircle);

    // revol a face, result is a solid.
    axis.SetLocation(gp_Pnt(30.0, 70.0, 0.0));
    axis.SetDirection(gp::DY());

    TopoDS_Edge ellipseEdge = BRepBuilderAPI_MakeEdge(gp_Elips(gp_Ax2(gp_Pnt(25.0, 70.0, 0.0), gp::DZ()), 3.0, 2.0));
    TopoDS_Wire ellipseWire = BRepBuilderAPI_MakeWire(ellipseEdge);
    TopoDS_Face ellipseFace = BRepBuilderAPI_MakeFace(gp_Pln(gp::XOY()), ellipseWire);
    TopoDS_Shape revolEllipse = BRepPrimAPI_MakeRevol(ellipseFace, axis, M_PI_4);
    Handle(AIS_Shape) aisRevolEllipse = new AIS_Shape(revolEllipse);

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
    TopoDS_Edge circleEdge = BRepBuilderAPI_MakeEdge(
                gp_Circ(
                    gp_Ax2(gp_Pnt(0.0, 80.0, 0.0), gp::DZ()),
                    1.5)
                );
    TopoDS_Wire circleWire = BRepBuilderAPI_MakeWire(circleEdge);

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

    Handle(AIS_Shape) aisShell = new AIS_Shape(shellGenerator.Shape());
    Handle(AIS_Shape) aisSolid = new AIS_Shape(transform.Shape());

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

    TopoDS_Shape topoBox = BRepPrimAPI_MakeBox(axis, 3.0, 4.0, 5.0).Shape();
    TopoDS_Shape topoSphere = BRepPrimAPI_MakeSphere(axis, 2.5).Shape();
    TopoDS_Shape cutShape1 = BRepAlgoAPI_Cut(topoBox, topoSphere);
    TopoDS_Shape cutShape2 = BRepAlgoAPI_Cut(topoSphere, topoBox);

    gp_Trsf aTrsf;
    aTrsf.SetTranslation(gp_Vec(8.0, 0.0, 0.0));
    BRepBuilderAPI_Transform transform1(cutShape1, aTrsf);

    aTrsf.SetTranslation(gp_Vec(16.0, 0.0, 0.0));
    BRepBuilderAPI_Transform transform2(cutShape2, aTrsf);

    Handle(AIS_Shape) aisBox = new AIS_Shape(topoBox);
    Handle(AIS_Shape) aisSphere = new AIS_Shape(topoSphere);
    Handle(AIS_Shape) aisCutShape1 = new AIS_Shape(transform1.Shape());
    Handle(AIS_Shape) aisCutShape2 = new AIS_Shape(transform2.Shape());

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


void occWidget::boolFuse()
{
    gp_Ax2 axis;
    axis.SetLocation(gp_Pnt(0.0, 100.0, 0.0));

    TopoDS_Shape topoBox = BRepPrimAPI_MakeBox(axis, 3.0, 4.0, 5.0).Shape();
    TopoDS_Shape topoSphere = BRepPrimAPI_MakeSphere(axis, 2.5).Shape();
    TopoDS_Shape fusedShape = BRepAlgoAPI_Fuse(topoBox, topoSphere);

    gp_Trsf trsf;
    trsf.SetTranslation(gp_Vec(8.0, 0.0, 0.0));
    BRepBuilderAPI_Transform transform(fusedShape, trsf);

    Handle(AIS_Shape) aisBox = new AIS_Shape(topoBox);
    Handle(AIS_Shape) aisSphere = new AIS_Shape(topoSphere);
    Handle(AIS_Shape) aisFusedShape = new AIS_Shape(transform.Shape());

    setShapeAttributes(aisBox, Quantity_NOC_SPRINGGREEN);
    setShapeAttributes(aisSphere, Quantity_NOC_STEELBLUE);
    setShapeAttributes(aisFusedShape, Quantity_NOC_ROSYBROWN);

    _occView->getContext()->Display(aisBox, Standard_True);
    _occView->getContext()->Display(aisSphere, Standard_True);
    _occView->getContext()->Display(aisFusedShape, Standard_True);
    _occView->fitAll();
}


void occWidget::boolCommon()
{
    gp_Ax2 axis;
    axis.SetLocation(gp_Pnt(0.0, 110.0, 0.0));

    TopoDS_Shape topoBox = BRepPrimAPI_MakeBox(axis, 3.0, 4.0, 5.0).Shape();
    TopoDS_Shape topoSphere = BRepPrimAPI_MakeSphere(axis, 2.5).Shape();
    TopoDS_Shape commonShape = BRepAlgoAPI_Common(topoBox, topoSphere);

    gp_Trsf trsf;
    trsf.SetTranslation(gp_Vec(8.0, 0.0, 0.0));
    BRepBuilderAPI_Transform transform(commonShape, trsf);

    Handle(AIS_Shape) aisBox = new AIS_Shape(topoBox);
    Handle(AIS_Shape) aisSphere = new AIS_Shape(topoSphere);
    Handle(AIS_Shape) aisCommonShape = new AIS_Shape(transform.Shape());

    setShapeAttributes(aisBox, Quantity_NOC_SPRINGGREEN);
    setShapeAttributes(aisSphere, Quantity_NOC_STEELBLUE);
    setShapeAttributes(aisCommonShape, Quantity_NOC_ROYALBLUE);

    _occView->getContext()->Display(aisBox, Standard_True);
    _occView->getContext()->Display(aisSphere, Standard_True);
    _occView->getContext()->Display(aisCommonShape, Standard_True);
    _occView->fitAll();
}

void occWidget::testHelix()
{
    makeCylindricalHelix();
    makeConicalHelix();
    makeToroidalHelix();
}

void occWidget::makeCylindricalHelix()
{
    Standard_Real radius = 3.0;
    Standard_Real pitch = 1.0;

    // the pcurve is a 2d line in the parametric space.
    gp_Lin2d line2d(gp_Pnt2d(0.0, 0.0), gp_Dir2d(radius, pitch));

    Handle(Geom2d_TrimmedCurve) segment = GCE2d_MakeSegment(line2d, 0.0, M_PI * 2.0).Value();

    Handle(Geom_CylindricalSurface) cylinder = new Geom_CylindricalSurface(gp::XOY(), radius);

    TopoDS_Edge helixEdge = BRepBuilderAPI_MakeEdge(segment, cylinder, 0.0, 6.0 * M_PI).Edge();

    gp_Trsf trsf;
    trsf.SetTranslation(gp_Vec(0.0, 120.0, 0.0));
    BRepBuilderAPI_Transform transform(helixEdge, trsf);

    Handle(AIS_Shape) aisHelixCurve = new AIS_Shape(transform.Shape());

    _occView->getContext()->Display(aisHelixCurve, Standard_True);

    // sweep a circle profile along the helix curve.
    // there is no curve3d in the pcurve edge, so approx one.
    BRepLib::BuildCurve3d(helixEdge);

    gp_Ax2 axis;
    axis.SetDirection(gp_Dir(0.0, 4.0, 1.0));
    axis.SetLocation(gp_Pnt(radius, 0.0, 0.0));

    gp_Circ profileCircle(axis, 0.3);

    TopoDS_Edge profileEdge = BRepBuilderAPI_MakeEdge(profileCircle).Edge();
    TopoDS_Wire profileWire = BRepBuilderAPI_MakeWire(profileEdge).Wire();
    TopoDS_Face profileFace = BRepBuilderAPI_MakeFace(profileWire).Face();

    TopoDS_Wire helixWire = BRepBuilderAPI_MakeWire(helixEdge).Wire();

    BRepOffsetAPI_MakePipe pipeMaker(helixWire, profileFace);

    if (pipeMaker.IsDone())
    {
        trsf.SetTranslation(gp_Vec(8.0, 120.0, 0.0));
        BRepBuilderAPI_Transform pipeTransform(pipeMaker.Shape(), trsf);

        Handle(AIS_Shape) aisPipe = new AIS_Shape(pipeTransform.Shape());
        setShapeAttributes(aisPipe, Quantity_NOC_CORAL);
        _occView->getContext()->Display(aisPipe, Standard_True);
        _occView->fitAll();
    }
}


void occWidget::makeConicalHelix()
{
    Standard_Real radius = 3.0;
    Standard_Real pitch = 1.0;

    // the pcurve is a 2d line in the parametric space.
    gp_Lin2d line2d(gp_Pnt2d(0.0, 0.0), gp_Dir2d(radius, pitch));
    Handle(Geom2d_TrimmedCurve) segment = GCE2d_MakeSegment(line2d, 0.0, M_PI * 2.0).Value();
    Handle(Geom_ConicalSurface) cylinder = new Geom_ConicalSurface(gp::XOY(), M_PI / 6.0, radius);

    TopoDS_Edge helixEdge = BRepBuilderAPI_MakeEdge(segment, cylinder, 0.0, 6.0 * M_PI).Edge();

    gp_Trsf trsf;
    trsf.SetTranslation(gp_Vec(18.0, 120.0, 0.0));
    BRepBuilderAPI_Transform transform(helixEdge, trsf);

    Handle(AIS_Shape) aisHelixCurve = new AIS_Shape(transform.Shape());

    _occView->getContext()->Display(aisHelixCurve, Standard_True);

    // sweep a circle profile along the helix curve.
    // there is no curve3d in the pcurve edge, so approx one.
    BRepLib::BuildCurve3d(helixEdge);

    gp_Ax2 axis;
    axis.SetDirection(gp_Dir(0.0, 4.0, 1.0));
    axis.SetLocation(gp_Pnt(radius, 0.0, 0.0));

    gp_Circ profileCircle(axis, 0.3);

    TopoDS_Edge profileEdge = BRepBuilderAPI_MakeEdge(profileCircle).Edge();
    TopoDS_Wire profileWire = BRepBuilderAPI_MakeWire(profileEdge).Wire();
    TopoDS_Face profileFace = BRepBuilderAPI_MakeFace(profileWire).Face();

    TopoDS_Wire helixWire = BRepBuilderAPI_MakeWire(helixEdge).Wire();

    BRepOffsetAPI_MakePipe pipeMaker(helixWire, profileFace);

    if (pipeMaker.IsDone())
    {
        trsf.SetTranslation(gp_Vec(28.0, 120.0, 0.0));
        BRepBuilderAPI_Transform pipeTransform(pipeMaker.Shape(), trsf);

        Handle(AIS_Shape) aisPipe = new AIS_Shape(pipeTransform.Shape());
        setShapeAttributes(aisPipe, Quantity_NOC_DARKGOLDENROD);
        _occView->getContext()->Display(aisPipe, Standard_True);
        _occView->fitAll();
    }
}


void occWidget::makeToroidalHelix()
{
    Standard_Real radius = 1.0;
    Standard_Real slope = 0.05;

    // the pcurve is a 2d line in the parametric space.
    gp_Lin2d line2d(gp_Pnt2d(0.0, 0.0), gp_Dir2d(slope, 1.0));
    Handle(Geom2d_TrimmedCurve) segment = GCE2d_MakeSegment(line2d, 0.0, M_PI * 2.0).Value();
    Handle(Geom_ToroidalSurface) cylinder = new Geom_ToroidalSurface(gp::XOY(), radius * 5.0, radius);

    TopoDS_Edge helixEdge = BRepBuilderAPI_MakeEdge(segment, cylinder, 0.0, 2.0 * M_PI / slope).Edge();

    gp_Trsf trsf;
    trsf.SetTranslation(gp_Vec(45.0, 120.0, 0.0));
    BRepBuilderAPI_Transform transform(helixEdge, trsf);

    Handle(AIS_Shape) aisHelixCurve = new AIS_Shape(transform.Shape());

    _occView->getContext()->Display(aisHelixCurve, Standard_True);

    // sweep a circle profile along the helix curve.
    // there is no curve3d in the pcurve edge, so approx one.
    BRepLib::BuildCurve3d(helixEdge);

    gp_Ax2 axis;
    axis.SetDirection(gp_Dir(0.0, 0.0, 1.0));
    axis.SetLocation(gp_Pnt(radius * 6.0, 0.0, 0.0));

    gp_Circ profileCircle(axis, 0.3);

    TopoDS_Edge profileEdge = BRepBuilderAPI_MakeEdge(profileCircle).Edge();
    TopoDS_Wire profileWire = BRepBuilderAPI_MakeWire(profileEdge).Wire();
    TopoDS_Face profileFace = BRepBuilderAPI_MakeFace(profileWire).Face();

    TopoDS_Wire helixWire = BRepBuilderAPI_MakeWire(helixEdge).Wire();

    BRepOffsetAPI_MakePipe pipeMaker(helixWire, profileFace);

    if (pipeMaker.IsDone())
    {
        trsf.SetTranslation(gp_Vec(60.0, 120.0, 0.0));
        BRepBuilderAPI_Transform pipeTransform(pipeMaker.Shape(), trsf);

        Handle(AIS_Shape) aisPipe = new AIS_Shape(pipeTransform.Shape());
        setShapeAttributes(aisPipe, Quantity_NOC_CORNSILK1);
        _occView->getContext()->Display(aisPipe, Standard_True);
        _occView->fitAll();
    }
}
