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

#include "occview.h"

#include <QDebug>
#include <QMouseEvent>
#include <QPalette>
#include <QStyleFactory>
#include <QWheelEvent>

// occ headers
#include <Aspect_DisplayConnection.hxx>

#include <Graphic3d_GraphicDriver.hxx>
#include <OpenGl_GraphicDriver.hxx>

#include <TCollection_AsciiString.hxx>
#include <Graphic3d_NameOfMaterial.hxx>
//#include <V3d_TypeOfSurfaceDetail.hxx>
#include <Standard_Type.hxx>

// private headers
#include "occwindow.h"

//#ifdef WNT
//#include <WNT_Window.hxx>
//#elif defined(__APPLE__) && !defined(MACOSX_USE_GLX)
//#include <Cocoa_Window.hxx>
//#else
//#undef Bool
//#undef CursorShape
//#undef None
//#undef KeyPress
//#undef KeyRelease
//#undef FocusIn
//#undef FocusOut
//#undef FontChange
//#undef Expose
//#include <Xw_Window.hxx>
//#endif

//static Handle(Graphic3d_GraphicDriver)& GetGraphicDriver()
//{
//    static Handle(Graphic3d_GraphicDriver) aGraphicDriver;
//    return aGraphicDriver;
//}

occView::occView(QWidget *parent) : QWidget(parent)
{
    // No Background
    setBackgroundRole( QPalette::NoRole);

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
    _view->Redraw();
}

void occView::resizeEvent( QResizeEvent* /*event*/ )
{
    if(!_view.IsNull())
        _view->MustBeResized();
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
    _context->Select(_mouseXmin, _mouseYmin, x, y, _view, Standard_True);
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

    // Set up lights etc
    _viewer->SetDefaultLights();
    _viewer->SetLightOn();

    auto bgcolor = Quantity_Color(.12, .12, .12, Quantity_TOC_sRGB);
    _view->SetBackgroundColor(bgcolor);
    _view->MustBeResized();

    // Initialize position, color and length of Triedron axes. The scale is a percent of the window width.
    _view->TriedronDisplay(Aspect_TOTP_LEFT_LOWER, Quantity_NOC_GOLD, 0.15, V3d_ZBUFFER);

#if 0
    Handle(OpenGl_GraphicDriver) aGraphicDriver;

    if(aGraphicDriver.IsNull())
    {
        Handle(Aspect_DisplayConnection) aDisplayConnection;
        aGraphicDriver = new OpenGl_GraphicDriver(aDisplayConnection);
    }

    TCollection_ExtendedString a3DName ("Visu3D");

    mViewer = new V3d_Viewer (aGraphicDriver,
                              a3DName.ToExtString(),
                              "",
                              1000.0,
                              V3d_XposYnegZpos,
                              Quantity_NOC_GRAY30,
                              V3d_ZBUFFER,
                              V3d_GOURAUD,
                              V3d_WAIT,
                              Standard_True,
                              Standard_True,
                              V3d_TEX_NONE);

    mViewer->SetDefaultLights();
    mViewer->SetLightOn();


    mContext = new AIS_InteractiveContext(mViewer);

    if ( mView.IsNull() )
        mView = mContext->CurrentViewer()->CreateView();

    Handle(OcctWindow) hWnd = new OcctWindow( this );
    mView->SetWindow (hWnd);
    if ( !hWnd->IsMapped() )
    {
        hWnd->Map();
    }

    mView->SetBackgroundColor(Quantity_NOC_BLACK);
    mView->MustBeResized();
#endif
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
        _view->StartRotation(point.x(), point.y());
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
            _view->Rotation(point.x(), point.y());
            break;

        case mouseActionMode::DynamicZooming:
            _view->Zoom(_mouseXmin, _mouseYmin, point.x(), point.y());
            break;

        case mouseActionMode::DynamicPanning:
            _view->Pan(point.x() - _mouseXmax, _mouseYmax - point.y());
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

    _view->Zoom(point.x(), point.y(), aX, aY);
}

