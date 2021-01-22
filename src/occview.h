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

#ifndef OCCVIEW_H
#define OCCVIEW_H

// Qt headers
#include <QRubberBand>
#include <QWidget>

// occt headers
#include <AIS_InteractiveContext.hxx>
#include <V3d_View.hxx>

class occView : public QWidget
{
    Q_OBJECT
public:
    // constructor
    explicit occView(QWidget *parent = nullptr);

    const Handle(AIS_InteractiveContext)& getContext() const { return _context;}
    const Handle(Graphic3d_Structure)& getStruct() const {return _struct;}

signals:
    void selectionChanged();

public slots:
    // operations for the view.
    void fitAll() { _view->FitAll(); _view->ZFitAll(); _view->Redraw();}
    void pan()    { _mouseMode = mouseActionMode::DynamicPanning;}
    void select() { _mouseMode = mouseActionMode::Selecting;}
    void reset()  { _view->FitAll(); _view->ZFitAll(); _view->Redraw();}
    void rotate() { _mouseMode = mouseActionMode::DynamicRotation;}
    void zoom()   { _mouseMode = mouseActionMode::DynamicZooming;}


protected:
    virtual QPaintEngine* paintEngine() const;

    // Paint events.
    virtual void paintEvent(QPaintEvent* event);
    virtual void resizeEvent(QResizeEvent* event);

    // Mouse events.
    virtual void mousePressEvent(QMouseEvent* event);
    virtual void mouseReleaseEvent(QMouseEvent* event);
    virtual void mouseMoveEvent(QMouseEvent* event);
    virtual void wheelEvent(QWheelEvent* event);

private:
    void init();
    void dragEvent(int x, int y);
    void drawRubberBand(int minX, int minY, int maxX, int maxY);

    // Occ viewer
    Handle(V3d_Viewer) _viewer;
    Handle(V3d_View) _view;

    Handle(Graphic3d_Structure) _struct;

    // Occ context
    Handle(AIS_InteractiveContext) _context;

    //! save the degenerate mode state.
    //Standard_Boolean _degenerateModeIsOn;

    //! rubber rectangle for the mouse selection.
    QRubberBand* _rectBand = nullptr;

    // mouse action modes
    enum mouseActionMode
    {
        Selecting,
        DynamicZooming,
    // UNUSED   WindowZooming,
        DynamicPanning,
    // UNUSED   GlobalPanning,
        DynamicRotation
    };

    //mouse current action mode.
    mouseActionMode _mouseMode {mouseActionMode::Selecting};

    // mouse position.
    Standard_Integer _mouseXmin;
    Standard_Integer _mouseYmin;
    Standard_Integer _mouseXmax;
    Standard_Integer _mouseYmax;

    // Button events.
    void onLButtonDown(int flags, QPoint point);
    void onMButtonDown(int flags, QPoint point);
    void onRButtonDown(int flags, QPoint point);

    void onLButtonUp(int flags, QPoint point);
    void onMButtonUp(int flags, QPoint point);
    void onRButtonUp(int flags, QPoint point);

    void onMouseMove(QMouseEvent* event, QPoint point);
    void onMouseWheel(int flags, QPointF point, QPoint delta);
};

#endif // OCCVIEW_H
