/*****************************************************************************
**                                                                          **
** This file is part of occQt6, a simple OpenCASCADE Qt demo, updated       **
** for Qt6 and OpenCASCADE 7.5.0                                            **
**                                                                          **
**  occview.h is copied almost 1:1 from                                     **
**  OpenCASCADE/samples/qt/Common/src/View.h                                **
*****************************************************************************/

#if !defined _WIN32
#define QT_CLEAN_NAMESPACE         /* avoid definition of INT32 and INT8 */
#endif

#include "occview.h"

// Qt headers
#include <Standard_WarningsDisable.hxx>
#include <QActionGroup>
#include <QApplication>
#include <QColorDialog>
#include <QCursor>
#include <QDebug>
#include <QFileInfo>
#include <QFileDialog>
#include <QDebug>
#include <QMouseEvent>
#include <QOperatingSystemVersion>
#include <QPalette>
#include <QStyleFactory>
#include <QSurfaceFormat>
#include <QWheelEvent>
#include <Standard_WarningsRestore.hxx>

// occ headers
#include <Aspect_DisplayConnection.hxx>
#include <Graphic3d_GraphicDriver.hxx>
#include <Graphic3d_TextureEnv.hxx>
#include <OpenGl_GraphicDriver.hxx>

// private headers
#include "occwindow.h"
#include "occviewcontextmenu.h"

namespace
{
//! Map Qt buttons bitmask to virtual keys.
Aspect_VKeyMouse qtMouseButtons2VKeys (Qt::MouseButtons buttons)
{
    Aspect_VKeyMouse button = Aspect_VKeyMouse_NONE;
    if ((buttons & Qt::LeftButton) != 0)
    {
        button |= Aspect_VKeyMouse_LeftButton;
    }
    if ((buttons & Qt::MiddleButton) != 0)
    {
        button |= Aspect_VKeyMouse_MiddleButton;
    }
    if ((buttons & Qt::RightButton) != 0)
    {
        button |= Aspect_VKeyMouse_RightButton;
    }
    return button;
}

//! Map Qt mouse modifiers bitmask to virtual keys.
Aspect_VKeyFlags qtMouseModifiers2VKeys (Qt::KeyboardModifiers modifiers)
{
    Aspect_VKeyFlags flags = Aspect_VKeyFlags_NONE;
    if ((modifiers & Qt::ShiftModifier) != 0)
    {
        flags |= Aspect_VKeyFlags_SHIFT;
    }
    if ((modifiers & Qt::ControlModifier) != 0)
    {
        flags |= Aspect_VKeyFlags_CTRL;
    }
    if ((modifiers & Qt::AltModifier) != 0)
    {
        flags |= Aspect_VKeyFlags_ALT;
    }
    return flags;
}
}

static QCursor* defCursor     = nullptr;
static QCursor* handCursor    = nullptr;
static QCursor* panCursor     = nullptr;
static QCursor* globPanCursor = nullptr;
static QCursor* zoomCursor    = nullptr;
static QCursor* rotCursor     = nullptr;


occView::occView(QWidget *parent) : QWidget(parent), _devPx(devicePixelRatio())
{
    init();

    _mouseDefaultGestures = myMouseGestureMap;
    _curMode = occViewEnums::curAction3d::Nothing;

    initCursors();

    // set focus policy to threat QContextMenuEvent from keyboard
    setFocusPolicy(Qt::StrongFocus);
    setAttribute(Qt::WA_PaintOnScreen);

    // Enable mouse tracking, by default mouse tracking is disabled.
    setMouseTracking(true);

    this->update();
}

occView::~occView()
{
    delete _backMenu;
}


// ------------------------------------------------------------------------------------------------
// protected functions
// ------------------------------------------------------------------------------------------------
void occView::init()
{
    Handle(Aspect_DisplayConnection) aDisplayConnection;
    Handle_OpenGl_GraphicDriver aGraphicDriver;

    // Create a 3D viewer.
    aDisplayConnection = new Aspect_DisplayConnection();
    aGraphicDriver = new OpenGl_GraphicDriver(aDisplayConnection);

    // fix OpenGL issues on MacOS: https://old.opencascade.com/content/ray-tracing-requires-opengl-31-and-higher
#if __APPLE__
    aGraphicDriver->ChangeOptions().contextCompatible = false;
    // below command is not needed, kept here in case it is required in the future
    // aGraphicDriver->ChangeOptions().ffpEnable=false;
#endif

    _viewer = new V3d_Viewer(aGraphicDriver);

    // Create a graphic structure in this Viewer
    _struct = new Graphic3d_Structure (_viewer->StructureManager());

    // Create an interactive context.
    _context = new AIS_InteractiveContext(_viewer);
    _context->SetDisplayMode(AIS_Shaded, Standard_True);

    _curDrawStyle = occViewEnums::drawStyle::shadedWithEdges;

    if ( _view.IsNull() )
        _view = _context->CurrentViewer()->CreateView();

    Handle(occWindow) hWnd = new occWindow(this);

    _view->SetWindow (hWnd);
    if ( !hWnd->IsMapped() )
        hWnd->Map();

    _view->MustBeResized();

    // Set up lights etc
    _viewer->SetDefaultLights();
    _viewer->SetLightOn();

    // set up selection style
    auto style = _context->SelectionStyle();
    style->SetColor(Quantity_NOC_GREEN);
    style->SetDisplayMode(AIS_Shaded);
    _context->SetSelectionStyle(style);

    auto bgcolor = Quantity_Color(.13, .13, .13, Quantity_TOC_sRGB);
    _view->SetBackgroundColor(bgcolor);
    _view->MustBeResized();

    // Initialize position, color and length of Trihedron axes. The scale is in percent of the window width.
    auto trihedronScale = this->devicePixelRatio() * 0.1;
    _view->TriedronDisplay(Aspect_TOTP_LEFT_LOWER, Quantity_NOC_GOLD, trihedronScale, V3d_ZBUFFER);

    if (_isRaytracing)
        _view->ChangeRenderingParams().Method = Graphic3d_RM_RAYTRACING;
}




//Get paint engine for the OpenGL viewer.
QPaintEngine* occView::paintEngine() const
{
    return nullptr;
}

void occView::paintEvent( QPaintEvent* /*event*/ )
{
    _view->InvalidateImmediate();
    FlushViewEvents(_context, _view, true);
    //    _view->Redraw();
}

void occView::resizeEvent( QResizeEvent* /*event*/ )
{
    if(!_view.IsNull())
        _view->MustBeResized();
}

void occView::OnSelectionChanged(const Handle(AIS_InteractiveContext)&,
                                 const Handle(V3d_View)&)
{
    emit selectionChanged();
}

void occView::wireframe()
{
    QApplication::setOverrideCursor(Qt::WaitCursor);
    _view->SetComputedMode(false);
    _context->SetDisplayMode(AIS_Shaded, Standard_False);
    _context->SetDisplayMode(AIS_WireFrame, Standard_True);
    _curDrawStyle = occViewEnums::drawStyle::wireframe;
    _view->Redraw();
    QApplication::restoreOverrideCursor();
}

void occView::hlrOn()
{
    QApplication::setOverrideCursor(Qt::WaitCursor);
    _view->SetComputedMode(true);
    _view->Redraw();
    _curDrawStyle = occViewEnums::drawStyle::hlrOn;
    QApplication::restoreOverrideCursor();
}

void occView::hlrOff()
{
    QApplication::setOverrideCursor(Qt::WaitCursor);
    _view->SetComputedMode(false);
    _view->Redraw();
    QApplication::restoreOverrideCursor();
}

void occView::shaded()
{
    QApplication::setOverrideCursor(Qt::WaitCursor);
    _context->SetDisplayMode(AIS_Shaded, Standard_True);
    _view->SetComputedMode(false);
    _view->Redraw();
    _curDrawStyle = occViewEnums::drawStyle::shaded;
    QApplication::restoreOverrideCursor();

}

void occView::setRaytracedShadows(bool state)
{
    _view->ChangeRenderingParams().IsShadowEnabled = state;
    _isShadowsEnabled = state;
    _context->UpdateCurrentViewer();
}

void occView::setRaytracedReflections(bool state)
{
    _view->ChangeRenderingParams().IsReflectionEnabled = state;
    _isReflectionsEnabled = state;
    _context->UpdateCurrentViewer();
}


void occView::onRaytraceAction()
{
    auto sentBy = qobject_cast<QAction*>(sender());

    if (sentBy == _raytraceActions->at(occViewEnums::toolRaytracingId))
    {
        bool state = _raytraceActions->at(occViewEnums::toolRaytracingId)->isChecked();
        QApplication::setOverrideCursor(Qt::WaitCursor);
        if (state)
            enableRaytracing();
        else
            disableRaytracing();
        QApplication::restoreOverrideCursor();
    }

    if (sentBy == _raytraceActions->at(occViewEnums::toolShadowsId))
        setRaytracedShadows(_raytraceActions->at(occViewEnums::toolShadowsId)->isChecked());

    if (sentBy == _raytraceActions->at(occViewEnums::toolReflectionsId))
        setRaytracedReflections(_raytraceActions->at(occViewEnums::toolReflectionsId)->isChecked());

    if (sentBy == _raytraceActions->at(occViewEnums::toolAntialiasingId))
        setRaytracedAntialiasing (_raytraceActions->at(occViewEnums::toolAntialiasingId)->isChecked());
}


void occView::setRaytracedAntialiasing(bool state)
{
    _view->ChangeRenderingParams().IsAntialiasingEnabled = state;
    _isAntialiasingEnabled = state;
    _context->UpdateCurrentViewer();
}


void occView::enableRaytracing()
{
    if (!_isRaytracing)
        _view->ChangeRenderingParams().Method = Graphic3d_RM_RAYTRACING;


    _isRaytracing = true;
    _context->UpdateCurrentViewer();
}

void occView::disableRaytracing()
{
    if (_isRaytracing)
        _view->ChangeRenderingParams().Method = Graphic3d_RM_RASTERIZATION;

    _isRaytracing = false;
    _context->UpdateCurrentViewer();
}


void occView::updateToggled(bool isOn)
{
    auto sentBy = qobject_cast<QAction*>(sender());

    if( !isOn )
        return;

    for (int i = occViewEnums::viewFitAll; i < occViewEnums::viewHlrOff; i++ )
    {
        QAction* action = _viewActions->at(i);

        if ( ( action == _viewActions->at( occViewEnums::viewFitArea ) ) ||
             ( action == _viewActions->at( occViewEnums::viewZoom ) ) ||
             ( action == _viewActions->at( occViewEnums::viewPan ) ) ||
             ( action == _viewActions->at( occViewEnums::viewGlobalPan ) ) ||
             ( action == _viewActions->at( occViewEnums::viewRotation ) ) )
        {
            if ( action && ( action != sentBy ) )
            {
                action->setCheckable(true);
                action->setChecked(false);
            }
            else
            {
                if (sentBy == _viewActions->at(occViewEnums::viewFitArea))
                    setCursor( *handCursor );
                else if ( sentBy == _viewActions->at(occViewEnums::viewZoom) )
                    setCursor( *zoomCursor );
                else if ( sentBy == _viewActions->at(occViewEnums::viewPan) )
                    setCursor( *panCursor );
                else if ( sentBy == _viewActions->at(occViewEnums::viewGlobalPan) )
                    setCursor( *globPanCursor );
                else if ( sentBy == _viewActions->at(occViewEnums::viewRotation) )
                    setCursor( *rotCursor );
                else
                    setCursor( *defCursor );

                sentBy->setCheckable( false );
            }
        }
    }
}

void occView::initCursors()
{
    if ( !defCursor )
        defCursor = new QCursor( Qt::ArrowCursor );
    if ( !handCursor )
        handCursor = new QCursor( Qt::PointingHandCursor );
    if ( !panCursor )
        panCursor = new QCursor( Qt::SizeAllCursor );
    if ( !globPanCursor )
        globPanCursor = new QCursor( Qt::CrossCursor );
    if ( !zoomCursor )
        zoomCursor = new QCursor (Qt::SizeVerCursor);//new QCursor( QPixmap( ApplicationCommonWindow::getResourceDir() + QString( "/" ) + QObject::tr( "ICON_CURSOR_ZOOM" ) ) );
    if ( !rotCursor )
        rotCursor = new QCursor (Qt::SizeHorCursor); //new QCursor( QPixmap( ApplicationCommonWindow::getResourceDir() + QString( "/" ) + QObject::tr( "ICON_CURSOR_ROTATE" ) ) );
}

//QList<QAction*>* occView::getViewActions()
//{
//    initViewActions();
//    return _viewActions;
//}


QList<QAction*>* occView::getRaytraceActions()
{
    initRaytraceActions();
    return _raytraceActions;
}

//void occView::initViewActions()
//{
//    if (_viewActions)
//        return;

//    _viewActions = new QList<QAction*>();

//    QAction* a;

//    a = new QAction(QObject::tr("MNU_FITALL"), this);
//    a->setToolTip(QObject::tr("TBR_FITALL"));
//    a->setStatusTip(QObject::tr("TBR_FITALL"));
//    connect(a, &QAction::triggered, this, &occView::fitAll);
//    _viewActions->insert(viewFitAll, a);

//    a = new QAction(QObject::tr("MNU_FITAREA"), this );
//    a->setToolTip(QObject::tr("TBR_FITAREA"));
//    a->setStatusTip(QObject::tr("TBR_FITAREA"));
//    connect(a, &QAction::triggered, this, &occView::fitArea);

//    a->setCheckable(true);
//    connect(a, &QAction::toggled, this, &occView::updateToggled);
//    _viewActions->insert(viewFitArea, a);

//    a = new QAction(QObject::tr("MNU_ZOOM"), this );
//    a->setToolTip( QObject::tr("TBR_ZOOM") );
//    a->setStatusTip( QObject::tr("TBR_ZOOM") );
//    connect(a, &QAction::triggered, this, &occView::zoom);

//    a->setCheckable(true);
//    connect(a, &QAction::toggled, this, &occView::updateToggled);
//    _viewActions->insert(viewZoom, a );

//    a = new QAction(QObject::tr("MNU_PAN"), this );
//    a->setToolTip( QObject::tr("TBR_PAN") );
//    a->setStatusTip( QObject::tr("TBR_PAN") );
//    connect(a, &QAction::triggered, this, &occView::pan);

//    a->setCheckable(true);
//    connect(a, &QAction::toggled, this, &occView::updateToggled);
//    _viewActions->insert(viewPan, a );

//    a = new QAction(QObject::tr("MNU_GLOBALPAN"), this );
//    a->setToolTip( QObject::tr("TBR_GLOBALPAN") );
//    a->setStatusTip( QObject::tr("TBR_GLOBALPAN") );
//    connect(a, &QAction::triggered, this, &occView::globalPan);

//    a->setCheckable(true);
//    connect(a, &QAction::toggled, this, &occView::updateToggled);
//    _viewActions->insert(viewGlobalPan, a );

//    a = new QAction(QObject::tr("MNU_FRONT"), this );
//    a->setToolTip( QObject::tr("TBR_FRONT") );
//    a->setStatusTip( QObject::tr("TBR_FRONT") );
//    connect(a, &QAction::triggered, this, &occView::front);
//    _viewActions->insert(viewFront, a );

//    a = new QAction(QObject::tr("MNU_BACK"), this );
//    a->setToolTip( QObject::tr("TBR_BACK") );
//    a->setStatusTip( QObject::tr("TBR_BACK") );
//    connect(a, &QAction::triggered, this, &occView::back);
//    _viewActions->insert(viewBack, a);

//    a = new QAction(QObject::tr("MNU_TOP"), this );
//    a->setToolTip( QObject::tr("TBR_TOP") );
//    a->setStatusTip( QObject::tr("TBR_TOP") );
//    connect(a, &QAction::triggered, this, &occView::top);
//    _viewActions->insert(viewTop, a );

//    a = new QAction(QObject::tr("MNU_BOTTOM"), this );
//    a->setToolTip( QObject::tr("TBR_BOTTOM") );
//    a->setStatusTip( QObject::tr("TBR_BOTTOM") );
//    connect(a, &QAction::triggered, this, &occView::bottom);
//    _viewActions->insert(viewBottom, a );

//    a = new QAction(QObject::tr("MNU_LEFT"), this );
//    a->setToolTip( QObject::tr("TBR_LEFT") );
//    a->setStatusTip( QObject::tr("TBR_LEFT") );
//    connect(a, &QAction::triggered, this, &occView::left);
//    _viewActions->insert(viewLeft, a );

//    a = new QAction(QObject::tr("MNU_RIGHT"), this );
//    a->setToolTip( QObject::tr("TBR_RIGHT") );
//    a->setStatusTip( QObject::tr("TBR_RIGHT") );
//    connect(a, &QAction::triggered, this, &occView::right);
//    _viewActions->insert(viewRight, a );

//    a = new QAction(QObject::tr("MNU_AXO"), this );
//    a->setToolTip( QObject::tr("TBR_AXO") );
//    a->setStatusTip( QObject::tr("TBR_AXO") );
//    connect(a, &QAction::triggered, this, &occView::axo);
//    _viewActions->insert(viewAxo, a );

//    a = new QAction(QObject::tr("MNU_ROTATION"), this );
//    a->setToolTip( QObject::tr("TBR_ROTATION") );
//    a->setStatusTip( QObject::tr("TBR_ROTATION") );
//    connect(a, &QAction::triggered, this, &occView::rotation);
//    a->setCheckable( true );
//    connect(a, &QAction::toggled, this, &occView::updateToggled);
//    _viewActions->insert(viewRotation, a );

//    a = new QAction(QObject::tr("MNU_RESET"), this );
//    a->setToolTip( QObject::tr("TBR_RESET") );
//    a->setStatusTip( QObject::tr("TBR_RESET") );
//    connect(a, &QAction::triggered, this, &occView::reset);
//    _viewActions->insert(viewReset, a );

//    QActionGroup* ag = new QActionGroup(this);

//    a = new QAction(QObject::tr("MNU_HLROFF"), this );
//    a->setToolTip( QObject::tr("TBR_HLROFF") );
//    a->setStatusTip( QObject::tr("TBR_HLROFF") );
//    connect(a, &QAction::triggered, this, &occView::hlrOff);
//    a->setCheckable( true );
//    ag->addAction(a);
//    _viewActions->insert(viewHlrOff, a);

//    a = new QAction(QObject::tr("MNU_HLRON"), this );
//    a->setToolTip( QObject::tr("TBR_HLRON") );
//    a->setStatusTip( QObject::tr("TBR_HLRON") );
//    connect(a, &QAction::triggered,this, &occView::hlrOn);

//    a->setCheckable( true );
//    ag->addAction(a);
//    _viewActions->insert(viewHlrOn, a );
//}


void occView::initRaytraceActions()
{
    if ( _raytraceActions )
        return;

    _raytraceActions = new QList<QAction*>();
    QAction* a;

    a = new QAction(QObject::tr("MNU_TOOL_RAYTRACING"), this );
    a->setToolTip( QObject::tr("TBR_TOOL_RAYTRACING") );
    a->setStatusTip( QObject::tr("TBR_TOOL_RAYTRACING") );
    a->setCheckable( true );
    a->setChecked( false );
    connect( a, &QAction::triggered, this, &occView::onRaytraceAction);
    _raytraceActions->insert(occViewEnums::toolRaytracingId, a );

    a = new QAction(QObject::tr("MNU_TOOL_SHADOWS"), this );
    a->setToolTip( QObject::tr("TBR_TOOL_SHADOWS") );
    a->setStatusTip( QObject::tr("TBR_TOOL_SHADOWS") );
    a->setCheckable( true );
    a->setChecked( true );
    connect(a, &QAction::triggered, this, &occView::onRaytraceAction);
    _raytraceActions->insert(occViewEnums::toolShadowsId, a );

    a = new QAction(QObject::tr("MNU_TOOL_REFLECTIONS"), this );
    a->setToolTip( QObject::tr("TBR_TOOL_REFLECTIONS") );
    a->setStatusTip( QObject::tr("TBR_TOOL_REFLECTIONS") );
    a->setCheckable( true );
    a->setChecked( false );
    connect(a, &QAction::triggered, this, &occView::onRaytraceAction);
    _raytraceActions->insert(occViewEnums::toolReflectionsId, a );

    a = new QAction(QObject::tr("MNU_TOOL_ANTIALIASING"), this );
    a->setToolTip( QObject::tr("TBR_TOOL_ANTIALIASING") );
    a->setStatusTip( QObject::tr("TBR_TOOL_ANTIALIASING") );
    a->setCheckable( true );
    a->setChecked( false );
    connect(a, &QAction::triggered, this, &occView::onRaytraceAction);
    _raytraceActions->insert(occViewEnums::toolAntialiasingId, a );
}


void occView::activateCursor(occViewEnums::curAction3d mode)
{
    switch( mode )
    {
    case occViewEnums::curAction3d::DynamicPanning:
        setCursor( *panCursor );
        break;
    case occViewEnums::curAction3d::DynamicZooming:
        setCursor( *zoomCursor );
        break;
    case occViewEnums::curAction3d::DynamicRotation:
        setCursor( *rotCursor );
        break;
    case occViewEnums::curAction3d::GlobalPanning:
        setCursor( *globPanCursor );
        break;
    case occViewEnums::curAction3d::WindowZooming:
        setCursor( *handCursor );
        break;
    case occViewEnums::curAction3d::Nothing:
    default:
        setCursor( *defCursor );
        break;
    }
}


void occView::mousePressEvent(QMouseEvent* event)
{
    Graphic3d_Vec2i point;
    point.SetValues(_devPx*event->pos().x(), _devPx*event->pos().y());
    const Aspect_VKeyFlags flags = qtMouseModifiers2VKeys (event->modifiers());
    if (!_view.IsNull() && UpdateMouseButtons(point, qtMouseButtons2VKeys (event->buttons()), flags, false))
        updateView();

    _clickPos = point;
}


void occView::mouseReleaseEvent(QMouseEvent* event)
{
    Graphic3d_Vec2i point;
    point.SetValues(_devPx*event->pos().x(), _devPx*event->pos().y());
    const Aspect_VKeyFlags flags = qtMouseModifiers2VKeys(event->modifiers());
    if (!_view.IsNull() && UpdateMouseButtons(point, qtMouseButtons2VKeys(event->buttons()), flags, false))
        updateView();


    if (_curMode == occViewEnums::curAction3d::GlobalPanning)
        _view->Place(point.x(), point.y(), _curZoom);

    // required to reset mouse mode, e.g. after WindowZooming
    if (_curMode != occViewEnums::curAction3d::Nothing)
        setCurAction(occViewEnums::curAction3d::Nothing);

    if (event->button() == Qt::RightButton && (flags & Aspect_VKeyFlags_CTRL) == 0 && (_clickPos - point).cwiseAbs().maxComp() <= 4)
        popup(point.x(), point.y());
}

void occView::mouseMoveEvent(QMouseEvent* event)
{
    Graphic3d_Vec2i point;
    point.SetValues(_devPx*event->pos().x(), _devPx*event->pos().y());
    if (!_view.IsNull() && UpdateMousePosition(point, qtMouseButtons2VKeys(event->buttons()), qtMouseModifiers2VKeys(event->modifiers()), false))
        updateView();
}


void occView::wheelEvent(QWheelEvent* event)
{
    Graphic3d_Vec2i pos;
    pos.SetValues(event->position().x(), event->position().y());
    int numPixels = event->pixelDelta().y();
    int numDegrees = event->angleDelta().y() / 8;

    Standard_Real delta {0.};
    if (numPixels != 0)
        delta = numPixels;
    else if (numDegrees != 0)
        delta = numDegrees / 15;

    if (!_view.IsNull() && UpdateZoom(Aspect_ScrollDelta(pos, delta)))
        updateView();
}

void occView::updateView()
{
    this->update();
}


void occView::defineMouseGestures()
{
    myMouseGestureMap.Clear();
    AIS_MouseGesture aRot = AIS_MouseGesture_RotateOrbit;
    activateCursor(_curMode);
    switch (_curMode)
    {
    case occViewEnums::curAction3d::Nothing:
        //noActiveActions();
        myMouseGestureMap = _mouseDefaultGestures;
        break;
    case occViewEnums::curAction3d::DynamicZooming:
        myMouseGestureMap.Bind (Aspect_VKeyMouse_LeftButton, AIS_MouseGesture_Zoom);
        break;
    case occViewEnums::curAction3d::GlobalPanning:
        break;
    case occViewEnums::curAction3d::WindowZooming:
        myMouseGestureMap.Bind (Aspect_VKeyMouse_LeftButton, AIS_MouseGesture_ZoomWindow);
        break;
    case occViewEnums::curAction3d::DynamicPanning:
        myMouseGestureMap.Bind (Aspect_VKeyMouse_LeftButton, AIS_MouseGesture_Pan);
        break;
    case occViewEnums::curAction3d::DynamicRotation:
        myMouseGestureMap.Bind (Aspect_VKeyMouse_LeftButton, aRot);
        break;
    case occViewEnums::curAction3d::Selecting:
        myMouseGestureMap.Bind(Aspect_VKeyMouse_LeftButton, AIS_MouseGesture_SelectRectangle);
        break;
    }
}


void occView::popup(int /*x*/, int /*y*/)
{
    //QMdiArea* ws = ApplicationCommonWindow::getWorkspace();
    //QMdiSubWindow* w = ws->activeSubWindow();

    qDebug() << "popup called";

    if (_context->NbSelected()) // if any object is selected
    {
        //QList<QAction*>* aList = stApp->getToolActions();
        QMenu* myToolMenu = new QMenu( 0 );
        //myToolMenu->addAction( aList->at( ApplicationCommonWindow::ToolWireframeId ) );
        //myToolMenu->addAction( aList->at( ApplicationCommonWindow::ToolShadingId ) );
        //myToolMenu->addAction( aList->at( ApplicationCommonWindow::ToolColorId ) );

        //        QMenu* myMaterMenu = new QMenu( myToolMenu );

        //QList<QAction*>* aMeterActions = ApplicationCommonWindow::getApplication()->getMaterialActions();

        //QString dir = ApplicationCommonWindow::getResourceDir() + QString( "/" );
        //myMaterMenu = myToolMenu->addMenu( QPixmap( dir+QObject::tr("ICON_TOOL_MATER")), QObject::tr("MNU_MATER") );
        //for ( int i = 0; i < aMeterActions->size(); i++ )
        //  myMaterMenu->addAction( aMeterActions->at( i ) );

        //myToolMenu->addAction( aList->at( ApplicationCommonWindow::ToolTransparencyId ) );
        //myToolMenu->addAction( aList->at( ApplicationCommonWindow::ToolDeleteId ) );
        addItemInPopup(myToolMenu);
        myToolMenu->exec( QCursor::pos() );
        delete myToolMenu;
    }
    else
    {
        auto contextMenu = occViewContextMenu(nullptr, _curDrawStyle);

        //connections
        connect(&contextMenu, &occViewContextMenu::fitAll, this, &occView::fitAll);
        connect(&contextMenu, &occViewContextMenu::fitArea, this, &occView::fitArea);
        connect(&contextMenu, &occViewContextMenu::axo, this, &occView::axo);
        connect(&contextMenu, &occViewContextMenu::front, this, &occView::front);
        connect(&contextMenu, &occViewContextMenu::back, this, &occView::back);
        connect(&contextMenu, &occViewContextMenu::left, this, &occView::left);
        connect(&contextMenu, &occViewContextMenu::right, this, &occView::right);
        connect(&contextMenu, &occViewContextMenu::top, this, &occView::top);
        connect(&contextMenu, &occViewContextMenu::bottom, this, &occView::bottom);
        connect(&contextMenu, &occViewContextMenu::wireframe, this, &occView::wireframe);
        connect(&contextMenu, &occViewContextMenu::hlrOn, this, &occView::hlrOn);
        connect(&contextMenu, &occViewContextMenu::shaded, this, &occView::shaded);

        // execute menu
        contextMenu.exec(QCursor::pos());
    }
//        if (!_backMenu)
//        {
//            _backMenu = new QMenu(nullptr);

//            //initViewActions()
//            auto viewMenu = _backMenu->addMenu(tr("View"));
//            for (auto action : *_viewActions)
//                viewMenu->addAction(action);

//            initRaytraceActions();
//            auto raytraceMenu = _backMenu->addMenu(tr("Raytracing"));
//            for (auto action : *_raytraceActions)
//                raytraceMenu->addAction(action);


//            QAction* a = new QAction( QObject::tr("Change Background"), this );
//            a->setToolTip( QObject::tr("TBR_CH_BACK") );
//            connect(a, &QAction::triggered, this, &occView::onBackground);
//            _backMenu->addAction(a);
//            addItemInPopup(_backMenu);

//            a = new QAction( QObject::tr("MNU_CH_ENV_MAP"), this );
//            a->setToolTip( QObject::tr("TBR_CH_ENV_MAP") );
//            connect(a, &QAction::triggered, this, &occView::onEnvironmentMap);
//            a->setCheckable(true);
//            a->setChecked(false);
//            _backMenu->addAction(a);
//            addItemInPopup(_backMenu);
//        }

//        _backMenu->exec(QCursor::pos());
//    }
    //if ( w )
    //  w->setFocus();
}

void occView::addItemInPopup(QMenu* /* menu */)
{
    //empty
}

//void occView::noActiveActions()
//{
//    for (int i = viewFitAll; i < viewHlrOff ; i++ )
//    {
//        QAction* anAction = _viewActions->at(i);
//        if( ( anAction == _viewActions->at(viewFitArea) ) ||
//                ( anAction == _viewActions->at(viewZoom) ) ||
//                ( anAction == _viewActions->at(viewPan) ) ||
//                ( anAction == _viewActions->at(viewGlobalPan) ) ||
//                ( anAction == _viewActions->at(viewRotation) ) )
//        {
//            setCursor( *defCursor );
//            anAction->setCheckable( true );
//            anAction->setChecked( false );
//        }
//    }
//}

void occView::onBackground()
{
    QColor aColor ;
    Standard_Real R1;
    Standard_Real G1;
    Standard_Real B1;
    _view->BackgroundColor(Quantity_TOC_sRGB,R1,G1,B1);
    aColor.setRgb((Standard_Integer)(R1 * 255), (Standard_Integer)(G1 * 255), (Standard_Integer)(B1 * 255));

    QColor aRetColor = QColorDialog::getColor(aColor);

    if( aRetColor.isValid() )
    {
        R1 = aRetColor.red()/255.;
        G1 = aRetColor.green()/255.;
        B1 = aRetColor.blue()/255.;
        _view->SetBackgroundColor(Quantity_TOC_sRGB,R1,G1,B1);
    }
    _view->Redraw();
}


void occView::onEnvironmentMap()
{
    if (_backMenu->actions().at(1)->isChecked())
    {
        QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), "",
                                                        tr("All Image Files (*.bmp *.gif *.jpg *.jpeg *.png *.tga)"));

        const TCollection_AsciiString anUtf8Path (fileName.toUtf8().data());

        Handle(Graphic3d_TextureEnv) aTexture = new Graphic3d_TextureEnv( anUtf8Path );

        _view->SetTextureEnv(aTexture);
    }
    else
        _view->SetTextureEnv (Handle(Graphic3d_TextureEnv)());

    _view->Redraw();
}

bool occView::dump(Standard_CString file)
{
    return _view->Dump(file);
}
