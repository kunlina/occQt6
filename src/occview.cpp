#include "occview.h"

#include <QDebug>
#include <QMouseEvent>
#include <QPalette>
#include <QStyleFactory>
#include <QWheelEvent>

// occt headers
#include <Aspect_DisplayConnection.hxx>
#include <Aspect_Handle.hxx>
#include <OpenGl_GraphicDriver.hxx>


#ifdef WNT
#include <WNT_Window.hxx>
#elif defined(__APPLE__) && !defined(MACOSX_USE_GLX)
#include <Cocoa_Window.hxx>
#else
#undef Bool
#undef CursorShape
#undef None
#undef KeyPress
#undef KeyRelease
#undef FocusIn
#undef FocusOut
#undef FontChange
#undef Expose
#include <Xw_Window.hxx>
#endif

static Handle(Graphic3d_GraphicDriver)& GetGraphicDriver()
{
    static Handle(Graphic3d_GraphicDriver) aGraphicDriver;
    return aGraphicDriver;
}

occView::occView(QWidget *parent) : QWidget(parent)
{
    // No Background
    setBackgroundRole( QPalette::NoRole );

    // set focus policy to threat QContextMenuEvent from keyboard
    setFocusPolicy(Qt::StrongFocus);
    setAttribute(Qt::WA_PaintOnScreen);
    setAttribute(Qt::WA_NoSystemBackground);

    // Enable the mouse tracking, by default the mouse tracking is disabled.
    setMouseTracking(true);

    init();
    this->update();
}


// ------------------------------------------------------------------------------------------------
// protected functions
// ------------------------------------------------------------------------------------------------

//Get paint engine for the OpenGL viewer. [ virtual public ]
QPaintEngine* occView::paintEngine() const
{
    return nullptr;
}

void occView::paintEvent( QPaintEvent* /*event*/ )
{
    _myView->Redraw();
}

void occView::resizeEvent( QResizeEvent* /*event*/ )
{
    if(!_myView.IsNull())
        _myView->MustBeResized();
}


void occView::mousePressEvent( QMouseEvent* event )
{
    switch (event->button()) {
    case Qt::LeftButton:
        onLButtonDown((event->buttons() | event->modifiers()), event->pos());
        break;
    case Qt::MiddleButton:
        onMButtonDown((event->buttons() | event->modifiers()), event->pos());
        break;
    case Qt::RightButton:
        onRButtonDown((event->buttons() | event->modifiers()), event->pos());
        break;
    default:
        return;
    }
    event->accept();
}

void occView::mouseReleaseEvent( QMouseEvent* event )
{
    switch (event->button()) {
    case Qt::LeftButton:
        onLButtonUp(event->buttons() | event->modifiers(), event->pos());
        break;
    case Qt::MiddleButton:
        onMButtonUp(event->buttons() | event->modifiers(), event->pos());
        break;
    case Qt::RightButton:
        onRButtonUp(event->buttons() | event->modifiers(), event->pos());
        break;
    default:
        return;
    }
    event->accept();
}

void occView::mouseMoveEvent( QMouseEvent* event )
{
    onMouseMove(event, event->pos());
    event->accept();
}

void occView::wheelEvent( QWheelEvent* event )
{

    QPoint numPixels = event->pixelDelta();
    QPoint numDegrees = event->angleDelta() / 8;

    if (!numPixels.isNull())
        onMouseWheel(event->buttons(), event->position(), numPixels);
    else if (!numDegrees.isNull()) {
        QPoint numSteps = numDegrees / 15;
        onMouseWheel(event->buttons(), event->position(), numSteps);
    }
    event->accept();
}


// ------------------------------------------------------------------------------------------------
// private functions
// ------------------------------------------------------------------------------------------------
void occView::dragEvent(int x, int y)
{
    _myContext->Select(_mouseXmin, _mouseYmin, x, y, _myView, Standard_True);
    emit selectionChanged();
}


void occView::drawRubberBand(int minX, int minY, int maxX, int maxY)
{
    QRect aRect;

    // Set the rectangle correctly.
    (minX < maxX) ? (aRect.setX(minX)) : (aRect.setX(maxX));
    (minY < maxY) ? (aRect.setY(minY)) : (aRect.setY(maxY));

    aRect.setWidth(abs(maxX - minX));
    aRect.setHeight(abs(maxY - minY));

    if (!_rectBand)
    {
        _rectBand = new QRubberBand(QRubberBand::Rectangle, this);

        // setStyle is important, set to windows style will just draw
        // rectangle frame, otherwise will draw a solid rectangle.
        _rectBand->setStyle(QStyleFactory::create("windows"));
    }

    if (_rectBand) {
        _rectBand->setGeometry(aRect);
        _rectBand->show();
    }
}


void occView::init()
{
    // Create Aspect_DisplayConnection
    auto aDisplayConnection = new Aspect_DisplayConnection();

    // Get graphic driver if it exists, otherwise initialise it
    if (GetGraphicDriver().IsNull())
        GetGraphicDriver() = new OpenGl_GraphicDriver(aDisplayConnection);

    // Get window handle. This returns something suitable for all platforms.
    auto window_handle = (WId) winId();

    // Create appropriate window for platform
#ifdef WNT
    Handle(WNT_Window) wind = new WNT_Window((Aspect_Handle) window_handle);
#elif defined(__APPLE__) && !defined(MACOSX_USE_GLX)
    Handle(Cocoa_Window) wind = new Cocoa_Window((NSView *) window_handle);
#else
    Handle(Xw_Window) wind = new Xw_Window(aDisplayConnection, (Window) window_handle);
#endif

    // Create V3dViewer and V3d_View
    _myViewer = new V3d_Viewer(GetGraphicDriver());

    _myView = _myViewer->CreateView();

    _myView->SetWindow(wind);
    if (!wind->IsMapped()) wind->Map();

    // Create AISInteractiveContext
    _myContext = new AIS_InteractiveContext(_myViewer);

    // Set up lights etc
    _myViewer->SetDefaultLights();
    _myViewer->SetLightOn();

    auto bgcolor = Quantity_Color(.12, .12, .12, Quantity_TOC_sRGB);
    _myView->SetBackgroundColor(bgcolor);
    _myView->MustBeResized();

    // Initialize position, color and length of Triedron axes. The scale is a percent of the window width.
    _myView->TriedronDisplay(Aspect_TOTP_LEFT_LOWER, Quantity_NOC_GOLD, 0.15, V3d_ZBUFFER);

    _myContext->SetDisplayMode(AIS_Shaded, Standard_True);
}



void occView::onLButtonDown(int flags, QPoint point)
{
    Q_UNUSED(flags)
    // Save the current mouse coordinates
    _mouseXmin = point.x();
    _mouseYmin = point.y();
    _mouseXmax = point.x();
    _mouseYmax = point.y();
}


void occView::onMButtonDown(int flags, QPoint point)
{
    Q_UNUSED(flags)

    // Save the current mouse coordinates
    _mouseXmin = point.x();
    _mouseYmin = point.y();
    _mouseXmax = point.x();
    _mouseYmax = point.y();

    if (_mouseMode == mouseActionMode::DynamicRotation)
        _myView->StartRotation(point.x(), point.y());
}


void occView::onRButtonDown(int flags, QPoint point)
{
    // do nothing
    Q_UNUSED(flags)
    Q_UNUSED(point)
}


void occView::onLButtonUp(int flags, QPoint point)
{
    // Hide the QRubberBand
    if (_rectBand) {
        delete _rectBand;
        _rectBand = nullptr;
    }


    // Ctrl for multi selection.
    //    if (point.x() == _mouseXmin && point.y() == _mouseYmin)
    //    {
    //        if (flags & Qt::ControlModifier)
    //        {
    //            //multiInputEvent(thePoint.x(), thePoint.y());
    //        }
    //        else
    //        {
    //            //inputEvent(thePoint.x(), thePoint.y());
    //        }
    //    }

}

void occView::onMButtonUp(int flags, QPoint point)
{
    Q_UNUSED(flags)
    //    if (thePoint.x() == myXmin && thePoint.y() == myYmin)
    //    {
    //        panByMiddleButton(thePoint);
    //    }
}

void occView::onRButtonUp(int flags, QPoint point)
{
    Q_UNUSED(flags)
    //    popup(thePoint.x(), thePoint.y());
}

void occView::onMouseMove(QMouseEvent *event, QPoint point)
{
    // Draw the rubber band when left button
    if ((event->buttons() & Qt::LeftButton) && !(event->modifiers() & Qt::ShiftModifier))
    {
        drawRubberBand(_mouseXmin, _mouseYmin, point.x(), point.y());
        dragEvent(point.x(), point.y());
    }

    // Ctrl for multi selection.
    //    if (theFlags & Qt::ControlModifier)
    //    {
    //        multiMoveEvent(thePoint.x(), thePoint.y());
    //    }
    //    else
    //    {
    //        moveEvent(thePoint.x(), thePoint.y());
    //    }

    // Middle button or left button and shift
    if ((event->buttons() & Qt::MiddleButton) || ((event->buttons() & Qt::LeftButton) && (event->modifiers() & Qt::ShiftModifier)))
    {
        switch (_mouseMode)
        {
        case mouseActionMode::DynamicRotation:
            _myView->Rotation(point.x(), point.y());
            break;

        case mouseActionMode::DynamicZooming:
            _myView->Zoom(_mouseXmin, _mouseYmin, point.x(), point.y());
            break;

        case mouseActionMode::DynamicPanning:
            _myView->Pan(point.x() - _mouseXmax, _mouseYmax - point.y());
            _mouseXmax = point.x();
            _mouseYmax = point.y();
            break;

        default:
            break;
        }
    }

}


void occView::onMouseWheel(int flags, QPointF point, QPoint delta)
{
    Q_UNUSED(flags)

    Standard_Integer aFactor = 16;

    Standard_Integer aX = point.x();
    Standard_Integer aY = point.y();

    if (delta.y() > 0)
    {
        aX += aFactor;
        aY += aFactor;
    }
    else
    {
        aX -= aFactor;
        aY -= aFactor;
    }

    _myView->Zoom(point.x(), point.y(), aX, aY);
}

