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
#include <QPalette>
#include <QStyleFactory>
#include <QWheelEvent>
#include <Standard_WarningsRestore.hxx>

// occ headers
#include <Aspect_DisplayConnection.hxx>

#include <Graphic3d_GraphicDriver.hxx>
#include <Graphic3d_TextureEnv.hxx>
#include <OpenGl_Context.hxx>
#include <OpenGl_GraphicDriver.hxx>

//#include <TCollection_AsciiString.hxx>
//#include <Graphic3d_NameOfMaterial.hxx>
//#include <V3d_TypeOfSurfaceDetail.hxx>
//#include <Standard_Type.hxx>

// private headers
#include "occwindow.h"


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


occView::occView(QWidget *parent) : QWidget(parent)
{
    //_context = context;
    init();

    this->setAttribute(Qt::WA_PaintOnScreen);
    this->setAttribute(Qt::WA_NoSystemBackground);

    _mouseDefaultGestures = myMouseGestureMap;
    _curMode = curAction3d_Nothing;

    initViewActions();
    initCursors();

    // No Background
    setBackgroundRole( QPalette::NoRole);

    // set focus policy to threat QContextMenuEvent from keyboard
    setFocusPolicy(Qt::StrongFocus);
    setAttribute(Qt::WA_PaintOnScreen);
    setAttribute(Qt::WA_NoSystemBackground);

    // Enable the mouse tracking, by default the mouse tracking is disabled.
    setMouseTracking(true);

    //init();
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
    //    if ( _view.IsNull() )
    //        _view = _context->CurrentViewer()->CreateView();

    //    Handle(occWindow) hWnd = new occWindow( this );
    //    _view->SetWindow (hWnd);
    //    if ( !hWnd->IsMapped() )
    //        hWnd->Map();

    //    _view->MustBeResized();


    Handle_Aspect_DisplayConnection aDisplayConnection;
    Handle_OpenGl_GraphicDriver aGraphicDriver;

    // 1. Create a 3D viewer.
    aDisplayConnection = new Aspect_DisplayConnection();
    aGraphicDriver = new OpenGl_GraphicDriver(aDisplayConnection);

    _viewer = new V3d_Viewer(aGraphicDriver);

    // 2. Create an interactive context.
    _context = new AIS_InteractiveContext(_viewer);
    _context->SetDisplayMode(AIS_Shaded, Standard_True);

    if ( _view.IsNull() )
        _view = _context->CurrentViewer()->CreateView();

    Handle(occWindow) hWnd = new occWindow( this );

    _view->SetWindow (hWnd);
    if ( !hWnd->IsMapped() )
        hWnd->Map();

    _view->MustBeResized();

    // Create a graphic structure in this Viewer
    _struct = new Graphic3d_Structure (_viewer->StructureManager());

    // Set up lights etc
    _viewer->SetDefaultLights();
    _viewer->SetLightOn();

    // set up selection style
    auto style = _context->SelectionStyle();
    style->SetColor(Quantity_NOC_GREEN);
    style->SetDisplayMode(AIS_Shaded);
    _context->SetSelectionStyle(style);

    auto bgcolor = Quantity_Color(.12, .12, .12, Quantity_TOC_sRGB);
    _view->SetBackgroundColor(bgcolor);
    _view->MustBeResized();

    // Initialize position, color and length of Trihedron axes. The scale is in percent of the window width.
    auto trihedronScale = this->devicePixelRatioF() * 0.1;
    _view->TriedronDisplay(Aspect_TOTP_LEFT_LOWER, Quantity_NOC_GOLD, trihedronScale, V3d_ZBUFFER);

    //    //    if (aGraphicDriver->GetSharedContext()->HasRayTracing()) {
    if (_isRaytracing)
        _view->ChangeRenderingParams().Method = Graphic3d_RM_RAYTRACING;

    //    //        Graphic3d_RenderingParams& aParams = _view->ChangeRenderingParams();
    //    //        // specifies rendering mode
    //    //        aParams.Method = Graphic3d_RM_RAYTRACING;
    //    //        // maximum ray-tracing depth
    //    //        aParams.RaytracingDepth = 3;
    //    //        // enable shadows rendering
    //    //        aParams.IsShadowEnabled = true;
    //    //        // enable specular reflections
    //    //        aParams.IsReflectionEnabled = true;
    //    //        // enable adaptive anti-aliasing
    //    //        aParams.IsAntialiasingEnabled = true;
    //    //        // enable light propagation through transparent media
    //    //        aParams.IsTransparentShadowEnabled = true;
    //    //        // update the view
    //    //        _view->Update();
    //    //    }
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

void occView::hlrOn()
{
    QApplication::setOverrideCursor(Qt::WaitCursor);
    _view->SetComputedMode (true);
    _view->Redraw();
    QApplication::restoreOverrideCursor();
}

void occView::hlrOff()
{
    QApplication::setOverrideCursor(Qt::WaitCursor);
    _view->SetComputedMode (false);
    _view->Redraw();
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

    if (sentBy == _raytraceActions->at(toolRaytracingId))
    {
        bool state = _raytraceActions->at(toolRaytracingId)->isChecked();
        QApplication::setOverrideCursor(Qt::WaitCursor);
        if (state)
            enableRaytracing();
        else
            disableRaytracing();
        QApplication::restoreOverrideCursor();
    }

    if (sentBy == _raytraceActions->at(toolShadowsId))
        setRaytracedShadows(_raytraceActions->at(toolShadowsId)->isChecked());

    if (sentBy == _raytraceActions->at(toolReflectionsId))
        setRaytracedReflections(_raytraceActions->at(toolReflectionsId)->isChecked());

    if (sentBy == _raytraceActions->at(toolAntialiasingId))
        setRaytracedAntialiasing (_raytraceActions->at(toolAntialiasingId)->isChecked());
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

    for (int i = viewFitAllId; i < viewHlrOffId; i++ )
    {
        QAction* action = _viewActions->at(i);

        if ( ( action == _viewActions->at( viewFitAreaId ) ) ||
             ( action == _viewActions->at( viewZoomId ) ) ||
             ( action == _viewActions->at( viewPanId ) ) ||
             ( action == _viewActions->at( viewGlobalPanId ) ) ||
             ( action == _viewActions->at( viewRotationId ) ) )
        {
            if ( action && ( action != sentBy ) )
            {
                action->setCheckable(true);
                action->setChecked(false);
            }
            else
            {
                if (sentBy == _viewActions->at(viewFitAreaId))
                    setCursor( *handCursor );
                else if ( sentBy == _viewActions->at(viewZoomId) )
                    setCursor( *zoomCursor );
                else if ( sentBy == _viewActions->at(viewPanId) )
                    setCursor( *panCursor );
                else if ( sentBy == _viewActions->at(viewGlobalPanId) )
                    setCursor( *globPanCursor );
                else if ( sentBy == _viewActions->at(viewRotationId) )
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

QList<QAction*>* occView::getViewActions()
{
    initViewActions();
    return _viewActions;
}


QList<QAction*>* occView::getRaytraceActions()
{
    initRaytraceActions();
    return _raytraceActions;
}

void occView::initViewActions()
{
    if (_viewActions)
        return;

    _viewActions = new QList<QAction*>();

    QAction* a;

    a = new QAction(QObject::tr("MNU_FITALL"), this);
    a->setToolTip(QObject::tr("TBR_FITALL"));
    a->setStatusTip(QObject::tr("TBR_FITALL"));
    connect(a, &QAction::triggered, this, &occView::fitAll);
    _viewActions->insert(viewFitAllId, a);

    a = new QAction(QObject::tr("MNU_FITAREA"), this );
    a->setToolTip(QObject::tr("TBR_FITAREA"));
    a->setStatusTip(QObject::tr("TBR_FITAREA"));
    connect(a, &QAction::triggered, this, &occView::fitArea);

    a->setCheckable(true);
    connect(a, &QAction::toggled, this, &occView::updateToggled);
    _viewActions->insert(viewFitAreaId, a);

    a = new QAction(QObject::tr("MNU_ZOOM"), this );
    a->setToolTip( QObject::tr("TBR_ZOOM") );
    a->setStatusTip( QObject::tr("TBR_ZOOM") );
    connect(a, &QAction::triggered, this, &occView::zoom);

    a->setCheckable(true);
    connect(a, &QAction::toggled, this, &occView::updateToggled);
    _viewActions->insert(viewZoomId, a );

    a = new QAction(QObject::tr("MNU_PAN"), this );
    a->setToolTip( QObject::tr("TBR_PAN") );
    a->setStatusTip( QObject::tr("TBR_PAN") );
    connect(a, &QAction::triggered, this, &occView::pan);

    a->setCheckable(true);
    connect(a, &QAction::toggled, this, &occView::updateToggled);
    _viewActions->insert(viewPanId, a );

    a = new QAction(QObject::tr("MNU_GLOBALPAN"), this );
    a->setToolTip( QObject::tr("TBR_GLOBALPAN") );
    a->setStatusTip( QObject::tr("TBR_GLOBALPAN") );
    connect(a, &QAction::triggered, this, &occView::globalPan);

    a->setCheckable(true);
    connect(a, &QAction::toggled, this, &occView::updateToggled);
    _viewActions->insert(viewGlobalPanId, a );

    a = new QAction(QObject::tr("MNU_FRONT"), this );
    a->setToolTip( QObject::tr("TBR_FRONT") );
    a->setStatusTip( QObject::tr("TBR_FRONT") );
    connect(a, &QAction::triggered, this, &occView::front);
    _viewActions->insert(viewFrontId, a );

    a = new QAction(QObject::tr("MNU_BACK"), this );
    a->setToolTip( QObject::tr("TBR_BACK") );
    a->setStatusTip( QObject::tr("TBR_BACK") );
    connect(a, &QAction::triggered, this, &occView::back);
    _viewActions->insert(viewBackId, a);

    a = new QAction(QObject::tr("MNU_TOP"), this );
    a->setToolTip( QObject::tr("TBR_TOP") );
    a->setStatusTip( QObject::tr("TBR_TOP") );
    connect(a, &QAction::triggered, this, &occView::top);
    _viewActions->insert(viewTopId, a );

    a = new QAction(QObject::tr("MNU_BOTTOM"), this );
    a->setToolTip( QObject::tr("TBR_BOTTOM") );
    a->setStatusTip( QObject::tr("TBR_BOTTOM") );
    connect(a, &QAction::triggered, this, &occView::bottom);
    _viewActions->insert(viewBottomId, a );

    a = new QAction(QObject::tr("MNU_LEFT"), this );
    a->setToolTip( QObject::tr("TBR_LEFT") );
    a->setStatusTip( QObject::tr("TBR_LEFT") );
    connect(a, &QAction::triggered, this, &occView::left);
    _viewActions->insert(viewLeftId, a );

    a = new QAction(QObject::tr("MNU_RIGHT"), this );
    a->setToolTip( QObject::tr("TBR_RIGHT") );
    a->setStatusTip( QObject::tr("TBR_RIGHT") );
    connect(a, &QAction::triggered, this, &occView::right);
    _viewActions->insert(viewRightId, a );

    a = new QAction(QObject::tr("MNU_AXO"), this );
    a->setToolTip( QObject::tr("TBR_AXO") );
    a->setStatusTip( QObject::tr("TBR_AXO") );
    connect(a, &QAction::triggered, this, &occView::axo);
    _viewActions->insert(viewAxoId, a );

    a = new QAction(QObject::tr("MNU_ROTATION"), this );
    a->setToolTip( QObject::tr("TBR_ROTATION") );
    a->setStatusTip( QObject::tr("TBR_ROTATION") );
    connect(a, &QAction::triggered, this, &occView::rotation);
    a->setCheckable( true );
    connect(a, &QAction::toggled, this, &occView::updateToggled);
    _viewActions->insert(viewRotationId, a );

    a = new QAction(QObject::tr("MNU_RESET"), this );
    a->setToolTip( QObject::tr("TBR_RESET") );
    a->setStatusTip( QObject::tr("TBR_RESET") );
    connect(a, &QAction::triggered, this, &occView::reset);
    _viewActions->insert(viewResetId, a );

    QActionGroup* ag = new QActionGroup(this);

    a = new QAction(QObject::tr("MNU_HLROFF"), this );
    a->setToolTip( QObject::tr("TBR_HLROFF") );
    a->setStatusTip( QObject::tr("TBR_HLROFF") );
    connect(a, &QAction::triggered, this, &occView::hlrOff);
    a->setCheckable( true );
    ag->addAction(a);
    _viewActions->insert(viewHlrOffId, a);

    a = new QAction(QObject::tr("MNU_HLRON"), this );
    a->setToolTip( QObject::tr("TBR_HLRON") );
    a->setStatusTip( QObject::tr("TBR_HLRON") );
    connect(a, &QAction::triggered,this, &occView::hlrOn);

    a->setCheckable( true );
    ag->addAction(a);
    _viewActions->insert(viewHlrOnId, a );
}


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
    _raytraceActions->insert(toolRaytracingId, a );

    a = new QAction(QObject::tr("MNU_TOOL_SHADOWS"), this );
    a->setToolTip( QObject::tr("TBR_TOOL_SHADOWS") );
    a->setStatusTip( QObject::tr("TBR_TOOL_SHADOWS") );
    a->setCheckable( true );
    a->setChecked( true );
    connect(a, &QAction::triggered, this, &occView::onRaytraceAction);
    _raytraceActions->insert(toolShadowsId, a );

    a = new QAction(QObject::tr("MNU_TOOL_REFLECTIONS"), this );
    a->setToolTip( QObject::tr("TBR_TOOL_REFLECTIONS") );
    a->setStatusTip( QObject::tr("TBR_TOOL_REFLECTIONS") );
    a->setCheckable( true );
    a->setChecked( false );
    connect(a, &QAction::triggered, this, &occView::onRaytraceAction);
    _raytraceActions->insert(toolReflectionsId, a );

    a = new QAction(QObject::tr("MNU_TOOL_ANTIALIASING"), this );
    a->setToolTip( QObject::tr("TBR_TOOL_ANTIALIASING") );
    a->setStatusTip( QObject::tr("TBR_TOOL_ANTIALIASING") );
    a->setCheckable( true );
    a->setChecked( false );
    connect(a, &QAction::triggered, this, &occView::onRaytraceAction);
    _raytraceActions->insert(toolAntialiasingId, a );
}


void occView::activateCursor(const curAction3d mode)
{
    switch( mode )
    {
    case curAction3d_DynamicPanning:
        setCursor( *panCursor );
        break;
    case curAction3d_DynamicZooming:
        setCursor( *zoomCursor );
        break;
    case curAction3d_DynamicRotation:
        setCursor( *rotCursor );
        break;
    case curAction3d_GlobalPanning:
        setCursor( *globPanCursor );
        break;
    case curAction3d_WindowZooming:
        setCursor( *handCursor );
        break;
    case curAction3d_Nothing:
    default:
        setCursor( *defCursor );
        break;
    }
}


void occView::mousePressEvent(QMouseEvent* event)
{
    Graphic3d_Vec2i point {event->pos().x(), event->pos().y()};
    point *= devicePixelRatioF();
    const Aspect_VKeyFlags flags = qtMouseModifiers2VKeys (event->modifiers());
    if (!_view.IsNull() && UpdateMouseButtons(point, qtMouseButtons2VKeys (event->buttons()), flags, false))
        updateView();

    _clickPos = point;
}


void occView::mouseReleaseEvent(QMouseEvent* event)
{
    Graphic3d_Vec2i point {event->pos().x(), event->pos().y()};
    point *= devicePixelRatioF();
    const Aspect_VKeyFlags flags = qtMouseModifiers2VKeys(event->modifiers());
    if (!_view.IsNull() && UpdateMouseButtons(point, qtMouseButtons2VKeys(event->buttons()), flags, false))
        updateView();


    if (_curMode == curAction3d_GlobalPanning)
        _view->Place(point.x(), point.y(), _curZoom);

    //if (_curMode != curAction3d_Nothing)
    //    setCurAction(curAction3d_Nothing);

    if (event->button() == Qt::RightButton && (flags & Aspect_VKeyFlags_CTRL) == 0 && (_clickPos - point).cwiseAbs().maxComp() <= 4)
        popup(point.x(), point.y());
}

void occView::mouseMoveEvent(QMouseEvent* event)
{
    Graphic3d_Vec2i newPos {event->pos().x(), event->pos().y()};
    newPos *= devicePixelRatioF();
    if (!_view.IsNull() && UpdateMousePosition(newPos, qtMouseButtons2VKeys(event->buttons()), qtMouseModifiers2VKeys(event->modifiers()), false))
        updateView();
}


void occView::wheelEvent(QWheelEvent* event)
{
    const Graphic3d_Vec2i pos {static_cast<int>(event->position().x()), static_cast<int>(event->position().y())};
    int numPixels = event->pixelDelta().y();
    int numDegrees = event->angleDelta().y() / 8;

    double delta {0.};
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
    case curAction3d_Nothing:
        noActiveActions();
        myMouseGestureMap = _mouseDefaultGestures;
        break;
    case curAction3d_DynamicZooming:
        myMouseGestureMap.Bind (Aspect_VKeyMouse_LeftButton, AIS_MouseGesture_Zoom);
        break;
    case curAction3d_GlobalPanning:
        break;
    case curAction3d_WindowZooming:
        myMouseGestureMap.Bind (Aspect_VKeyMouse_LeftButton, AIS_MouseGesture_ZoomWindow);
        break;
    case curAction3d_DynamicPanning:
        myMouseGestureMap.Bind (Aspect_VKeyMouse_LeftButton, AIS_MouseGesture_Pan);
        break;
    case curAction3d_DynamicRotation:
        myMouseGestureMap.Bind (Aspect_VKeyMouse_LeftButton, aRot);
        break;
    case curAction3d_Selecting:
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
        if (!_backMenu)
        {
            _backMenu = new QMenu(nullptr);

            QAction* a = new QAction( QObject::tr("Change Background..."), this );
            a->setToolTip( QObject::tr("TBR_CH_BACK") );
            connect(a, &QAction::triggered, this, &occView::onBackground);
            _backMenu->addAction(a);
            addItemInPopup(_backMenu);

            a = new QAction( QObject::tr("MNU_CH_ENV_MAP"), this );
            a->setToolTip( QObject::tr("TBR_CH_ENV_MAP") );
            connect(a, &QAction::triggered, this, &occView::onEnvironmentMap);
            a->setCheckable(true);
            a->setChecked(false);
            _backMenu->addAction(a);
            addItemInPopup(_backMenu);
        }

        _backMenu->exec(QCursor::pos());
    }
    //if ( w )
    //  w->setFocus();
}

void occView::addItemInPopup(QMenu* /* menu */)
{
    //empty
}

void occView::noActiveActions()
{
    for (int i = viewFitAllId; i < viewHlrOffId ; i++ )
    {
        QAction* anAction = _viewActions->at(i);
        if( ( anAction == _viewActions->at(viewFitAreaId) ) ||
                ( anAction == _viewActions->at(viewZoomId) ) ||
                ( anAction == _viewActions->at(viewPanId) ) ||
                ( anAction == _viewActions->at(viewGlobalPanId) ) ||
                ( anAction == _viewActions->at(viewRotationId) ) )
        {
            setCursor( *defCursor );
            anAction->setCheckable( true );
            anAction->setChecked( false );
        }
    }
}

void occView::onBackground()
{
    QColor aColor ;
    Standard_Real R1;
    Standard_Real G1;
    Standard_Real B1;
    _view->BackgroundColor(Quantity_TOC_RGB,R1,G1,B1);
    aColor.setRgb((Standard_Integer)(R1 * 255), (Standard_Integer)(G1 * 255), (Standard_Integer)(B1 * 255));

    QColor aRetColor = QColorDialog::getColor(aColor);

    if( aRetColor.isValid() )
    {
        R1 = aRetColor.red()/255.;
        G1 = aRetColor.green()/255.;
        B1 = aRetColor.blue()/255.;
        _view->SetBackgroundColor(Quantity_TOC_RGB,R1,G1,B1);
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



//void occView::mousePressEvent( QMouseEvent* event )
//{
//    switch (event->button()) {
//    case Qt::LeftButton:
//        onLButtonDown((event->buttons() | event->modifiers()), event->pos());
//        break;
//    case Qt::MiddleButton:
//        onMButtonDown((event->buttons() | event->modifiers()), event->pos());
//        break;
//    case Qt::RightButton:
//        onRButtonDown((event->buttons() | event->modifiers()), event->pos());
//        break;
//    default:
//        return;
//    }
//    event->accept();
//}

//void occView::mouseReleaseEvent( QMouseEvent* event )
//{
//    switch (event->button()) {
//    case Qt::LeftButton:
//        onLButtonUp(event->buttons() | event->modifiers(), event->pos());
//        break;
//    case Qt::MiddleButton:
//        onMButtonUp(event->buttons() | event->modifiers(), event->pos());
//        break;
//    case Qt::RightButton:
//        onRButtonUp(event->buttons() | event->modifiers(), event->pos());
//        break;
//    default:
//        return;
//    }
//    event->accept();
//}

//void occView::mouseMoveEvent( QMouseEvent* event )
//{
//    onMouseMove(event, event->pos());
//    event->accept();
//}

//void occView::wheelEvent( QWheelEvent* event )
//{

//    QPoint numPixels = event->pixelDelta();
//    QPoint numDegrees = event->angleDelta() / 8;

//    if (!numPixels.isNull())
//        onMouseWheel(event->buttons(), event->position(), numPixels);
//    else if (!numDegrees.isNull()) {
//        QPoint numSteps = numDegrees / 15;
//        onMouseWheel(event->buttons(), event->position(), numSteps);
//    }
//    event->accept();
//}


//// ------------------------------------------------------------------------------------------------
//// private functions
//// ------------------------------------------------------------------------------------------------
//void occView::dragEvent(int x, int y)
//{
//    // correct coordinates for devicePixelRatio
//    auto devPx = this->devicePixelRatioF();
//    const Standard_Integer xMin = devPx * _mouseXmin;
//    const Standard_Integer yMin = devPx * _mouseYmin;
//    x *= devPx;
//    y *= devPx;

//    _context->Select(xMin, yMin, x, y, _view, Standard_True);
//    emit selectionChanged();
//}


//void occView::drawRubberBand(int minX, int minY, int maxX, int maxY)
//{
//    QRect aRect;

//    // Set the rectangle correctly.
//    (minX < maxX) ? (aRect.setX(minX)) : (aRect.setX(maxX));
//    (minY < maxY) ? (aRect.setY(minY)) : (aRect.setY(maxY));

//    aRect.setWidth(abs(maxX - minX));
//    aRect.setHeight(abs(maxY - minY));

//    if (!_rectBand)
//    {
//        _rectBand = new QRubberBand(QRubberBand::Rectangle, this);

//        // setStyle is important, set to windows style will just draw
//        // rectangle frame, otherwise will draw a solid rectangle.
//        _rectBand->setStyle(QStyleFactory::create("windows"));
//    }

//    if (_rectBand) {
//        _rectBand->setGeometry(aRect);
//        _rectBand->show();
//    }
//}


//void occView::init()
//{

//    Handle_Aspect_DisplayConnection aDisplayConnection;
//    Handle_OpenGl_GraphicDriver aGraphicDriver;

//    // 1. Create a 3D viewer.
//    aDisplayConnection = new Aspect_DisplayConnection();
//    aGraphicDriver = new OpenGl_GraphicDriver(aDisplayConnection);

//    _viewer = new V3d_Viewer(aGraphicDriver);

//    // 2. Create an interactive context.
//    _context = new AIS_InteractiveContext(_viewer);
//    _context->SetDisplayMode(AIS_Shaded, Standard_True);

//    if ( _view.IsNull() )
//        _view = _context->CurrentViewer()->CreateView();

//    Handle(occWindow) hWnd = new occWindow( this );

//    _view->SetWindow (hWnd);
//    if ( !hWnd->IsMapped() )
//        hWnd->Map();

//    _view->MustBeResized();

//    // Set up lights etc
//    _viewer->SetDefaultLights();
//    _viewer->SetLightOn();

//    // set up selection style
//    auto style = _context->SelectionStyle();
//    style->SetColor(Quantity_NOC_GREEN);
//    style->SetDisplayMode(AIS_Shaded);
//    _context->SetSelectionStyle(style);

//    auto bgcolor = Quantity_Color(.12, .12, .12, Quantity_TOC_sRGB);
//    _view->SetBackgroundColor(bgcolor);
//    _view->MustBeResized();

//    // Initialize position, color and length of Triedron axes. The scale is in percent of the window width.
//    auto trihedronScale = this->devicePixelRatioF() * 0.1;
//    _view->TriedronDisplay(Aspect_TOTP_LEFT_LOWER, Quantity_NOC_GOLD, trihedronScale, V3d_ZBUFFER);

//    //    if (aGraphicDriver->GetSharedContext()->HasRayTracing()) {

//    //        Graphic3d_RenderingParams& aParams = _view->ChangeRenderingParams();
//    //        // specifies rendering mode
//    //        aParams.Method = Graphic3d_RM_RAYTRACING;
//    //        // maximum ray-tracing depth
//    //        aParams.RaytracingDepth = 3;
//    //        // enable shadows rendering
//    //        aParams.IsShadowEnabled = true;
//    //        // enable specular reflections
//    //        aParams.IsReflectionEnabled = true;
//    //        // enable adaptive anti-aliasing
//    //        aParams.IsAntialiasingEnabled = true;
//    //        // enable light propagation through transparent media
//    //        aParams.IsTransparentShadowEnabled = true;
//    //        // update the view
//    //        _view->Update();
//    //    }

//    // Create a structure in this Viewer
//    _struct = new Graphic3d_Structure (_viewer->StructureManager());

//}



//void occView::onLButtonDown(int flags, QPoint point)
//{
//    Q_UNUSED(flags)
//    // Save the current mouse coordinates
//    _mouseXmin = point.x();
//    _mouseYmin = point.y();
//    _mouseXmax = point.x();
//    _mouseYmax = point.y();
//}


//void occView::onMButtonDown(int flags, QPoint point)
//{
//    Q_UNUSED(flags)

//    // Save the current mouse coordinates
//    _mouseXmin = point.x();
//    _mouseYmin = point.y();
//    _mouseXmax = point.x();
//    _mouseYmax = point.y();

//    if (_mouseMode == mouseActionMode::DynamicRotation)
//        _view->StartRotation(point.x(), point.y());
//}


//void occView::onRButtonDown(int flags, QPoint point)
//{
//    // do nothing
//    Q_UNUSED(flags)
//    Q_UNUSED(point)
//}


//void occView::onLButtonUp(int flags, QPoint point)
//{
//    // Hide the QRubberBand
//    if (_rectBand) {
//        delete _rectBand;
//        _rectBand = nullptr;
//    }

//    if (_mouseMode == mouseActionMode::Selecting)
//        dragEvent(point.x(), point.y());

//    // Ctrl for multi selection.
//    //    if (point.x() == _mouseXmin && point.y() == _mouseYmin)
//    //    {
//    //        if (flags & Qt::ControlModifier)
//    //        {
//    //            //multiInputEvent(thePoint.x(), thePoint.y());
//    //        }
//    //        else
//    //        {
//    //            //inputEvent(thePoint.x(), thePoint.y());
//    //        }
//    //    }

//}

//void occView::onMButtonUp(int flags, QPoint point)
//{
//    Q_UNUSED(flags)
//    //    if (thePoint.x() == myXmin && thePoint.y() == myYmin)
//    //    {
//    //        panByMiddleButton(thePoint);
//    //    }
//}

//void occView::onRButtonUp(int flags, QPoint point)
//{
//    Q_UNUSED(flags)
//    //    popup(thePoint.x(), thePoint.y());
//}

//void occView::onMouseMove(QMouseEvent *event, QPoint point)
//{
//    // Draw the rubber band when left button
//    //    if ((event->buttons() & Qt::LeftButton) && !(event->modifiers() & Qt::ShiftModifier))
//    //    {
//    //        drawRubberBand(_mouseXmin, _mouseYmin, point.x(), point.y());
//    //        dragEvent(point.x(), point.y());
//    //    }

//    // Ctrl for multi selection.
//    //    if (theFlags & Qt::ControlModifier)
//    //    {
//    //        multiMoveEvent(thePoint.x(), thePoint.y());
//    //    }
//    //    else
//    //    {
//    //        moveEvent(thePoint.x(), thePoint.y());
//    //    }

//    // Left button
//    if ((event->buttons() & Qt::LeftButton))
//    {
//        switch (_mouseMode)
//        {
//        case mouseActionMode::DynamicRotation:
//            _view->Rotation(point.x(), point.y());
//            break;

//        case mouseActionMode::DynamicZooming:
//            _view->Zoom(_mouseXmin, _mouseYmin, point.x(), point.y());
//            break;

//        case mouseActionMode::DynamicPanning:
//            _view->Pan(point.x() - _mouseXmax, _mouseYmax - point.y());
//            _mouseXmax = point.x();
//            _mouseYmax = point.y();
//            break;

//        case mouseActionMode::Selecting:
//            drawRubberBand(_mouseXmin, _mouseYmin, point.x(), point.y());
//            break;

//        default:
//            break;
//        }
//    }

//}


//void occView::onMouseWheel(int flags, QPointF point, QPoint delta)
//{
//    Q_UNUSED(flags)

//    Standard_Integer aFactor = 16;

//    Standard_Integer aX = point.x();
//    Standard_Integer aY = point.y();

//    if (delta.y() > 0)
//    {
//        aX += aFactor;
//        aY += aFactor;
//    }
//    else
//    {
//        aX -= aFactor;
//        aY -= aFactor;
//    }

//    _view->Zoom(point.x(), point.y(), aX, aY);
//}

