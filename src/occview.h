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

    const Handle(AIS_InteractiveContext)& getContext() const;

signals:
    void selectionChanged();

public slots:
    // operations for the view.
    void fitAll() { _myView->FitAll(); _myView->ZFitAll(); _myView->Redraw();}
    void pan()    { _mouseMode = mouseActionMode::DynamicPanning;}
    void reset()  { _myView->Reset();}
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

    //! the occ viewer.
    Handle(V3d_Viewer) _myViewer;

    //! the occ view.
    Handle(V3d_View) _myView;

    //! the occ context.
    Handle(AIS_InteractiveContext) _myContext;

    //! save the degenerate mode state.
    Standard_Boolean _degenerateModeIsOn;

    //! rubber rectangle for the mouse selection.
    QRubberBand* _rectBand = nullptr;

    // mouse action modes
    enum mouseActionMode
    {
        Nothing,
        DynamicZooming,
        WindowZooming,
        DynamicPanning,
        GlobalPanning,
        DynamicRotation
    };

    //mouse current mode.
    mouseActionMode _mouseMode;

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



signals:

};

#endif // OCCVIEW_H
