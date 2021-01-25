/*****************************************************************************
**                                                                          **
** This file is part of occQt6, a simple OpenCASCADE Qt demo, updated       **
** for Qt6 and OpenCASCADE 7.5.0                                            **
**                                                                          **
**  occview.h is copied almost 1:1 from                                     **
**  OpenCASCADE/samples/qt/Common/src/View.h                                **
*****************************************************************************/

#ifndef OCCVIEW_H
#define OCCVIEW_H

// Qt headers
#include <Standard_WarningsDisable.hxx>
#include <QMenu>
#include <QRubberBand>
#include <QWidget>
#include <Standard_WarningsRestore.hxx>

// occt headers
#include <AIS_InteractiveContext.hxx>
#include <AIS_ViewController.hxx>
#include <V3d_View.hxx>

class TopoDS_Shape;
class QRubberBand;

class occView : public QWidget, protected AIS_ViewController
{
    Q_OBJECT
    enum curAction3d { curAction3d_Nothing,
                       curAction3d_DynamicZooming,
                       curAction3d_WindowZooming,
                       curAction3d_DynamicPanning,
                       curAction3d_GlobalPanning,
                       curAction3d_DynamicRotation
                     };

public:

    enum viewAction { viewFitAllId,
                      viewFitAreaId,
                      viewZoomId,
                      viewPanId,
                      viewGlobalPanId,
                      viewFrontId,
                      viewBackId,
                      viewTopId,
                      viewBottomId,
                      viewLeftId,
                      viewRightId,
                      viewAxoId,
                      viewRotationId,
                      viewResetId,
                      viewHlrOffId,
                      viewHlrOnId
                    };

    enum raytraceAction { toolRaytracingId,
                          toolShadowsId,
                          toolReflectionsId,
                          toolAntialiasingId
                        };

    // constructor
    occView(QWidget* parent = nullptr );

    // destructor
    ~occView();

    //explicit occView(QWidget *parent = nullptr);
    //const Handle(AIS_InteractiveContext)& getContext() const { return _context;}
    const Handle(Graphic3d_Structure)& getStruct() const {return _struct;}

    virtual void init();
    bool dump (Standard_CString file);
    QList<QAction*>* getViewActions();
    QList<QAction*>* getRaytraceActions();
    void noActiveActions();
    void isShadingMode();

    void enableRaytracing();
    void disableRaytracing();

    void setRaytracedShadows(bool state);
    void setRaytracedReflections(bool state);
    void setRaytracedAntialiasing(bool state);

    bool isRaytracingMode() const {return _isRaytracing;}
    bool isShadowsEnabled() const {return _isShadowsEnabled;}
    bool isReflectionsEnabled() const {return _isReflectionsEnabled;}
    bool isAntialiasingEnabled() const {return _isAntialiasingEnabled;}

    static QString getMessages(int type,
                               TopAbs_ShapeEnum subShapeType,
                               TopAbs_ShapeEnum shapeType);
    static QString getShapeType(TopAbs_ShapeEnum shapeType);

    Standard_EXPORT static void onButtonUserAction(int ExerciseSTEP,
                                                   Handle(AIS_InteractiveContext)& );
    Standard_EXPORT static void doSelection(int Id,
                                            Handle(AIS_InteractiveContext)& );
    Standard_EXPORT static void onSetSelectionMode(Handle(AIS_InteractiveContext)&,
                                                   Standard_Integer&,
                                                   TopAbs_ShapeEnum& SelectionMode,
                                                   Standard_Boolean& );

    //getters
    Handle(V3d_View)& getView() {return _view;}
    Handle(AIS_InteractiveContext)& getContext() {return _context;};

signals:
    void selectionChanged();

public slots:
    // operations for the view.
    //    void fitAll() { _view->FitAll(); _view->ZFitAll(); _view->Redraw();}
    //    void pan()    { _mouseMode = mouseActionMode::DynamicPanning;}
    //    void select() { _mouseMode = mouseActionMode::Selecting;}
    //    void reset()  { _view->FitAll(); _view->ZFitAll(); _view->Redraw();}
    //    void rotate() { _mouseMode = mouseActionMode::DynamicRotation;}
    //    void zoom()   { _mouseMode = mouseActionMode::DynamicZooming;}
    void fitAll() { _view->FitAll(); _view->ZFitAll(); _view->Redraw();}
    void fitArea() {setCurAction(curAction3d_WindowZooming);};
    void zoom() {setCurAction(curAction3d_DynamicZooming);};
    void pan() {setCurAction(curAction3d_DynamicPanning);};
    void globalPan() { _curZoom = _view->Scale(); _view->FitAll(); setCurAction(curAction3d_GlobalPanning);}
    void front() {_view->SetProj(V3d_Yneg);};
    void back() {_view->SetProj(V3d_Ypos);};
    void top() {_view->SetProj(V3d_Zpos);};
    void bottom() {_view->SetProj(V3d_Zneg);};
    void left() {_view->SetProj(V3d_Xneg);};
    void right() {_view->SetProj(V3d_Xpos);};
    void axo() {_view->SetProj(V3d_XposYnegZpos);};
    void rotation() {setCurAction(curAction3d_DynamicRotation);};
    void reset() {fitAll();};

    void hlrOn();
    void hlrOff();
    void updateToggled( bool );
    void onBackground();
    void onEnvironmentMap();
    void onRaytraceAction();


protected:
    virtual QPaintEngine* paintEngine() const override;

    // Paint events.
    virtual void paintEvent(QPaintEvent* event) override;
    virtual void resizeEvent(QResizeEvent* event) override;

    // Mouse events.
    virtual void mousePressEvent(QMouseEvent* event) override;
    virtual void mouseReleaseEvent(QMouseEvent* event) override;
    virtual void mouseMoveEvent(QMouseEvent* event) override;
    virtual void wheelEvent(QWheelEvent* event) override;

    virtual void addItemInPopup(QMenu* );

    // getters
    curAction3d getCurrentMode() {return _curMode;}

    void activateCursor(curAction3d);
    void popup(int x, int y );
    void updateView();

    //! Setup mouse gestures.
    void defineMouseGestures();

    // Set current action.
    void setCurAction (curAction3d action) { _curMode = action;
                                             defineMouseGestures();
                                           }

    // Handle selection changed event.
    void OnSelectionChanged (const Handle(AIS_InteractiveContext)& theCtx,
                             const Handle(V3d_View)& theView) Standard_OVERRIDE;

private:

    bool _isRaytracing {false};
    bool _isShadowsEnabled {true};
    bool _isReflectionsEnabled {false};
    bool _isAntialiasingEnabled {false};

    //void dragEvent(int x, int y);
    //void drawRubberBand(int minX, int minY, int maxX, int maxY);

    // Occ viewer
    Handle(V3d_Viewer) _viewer;
    Handle(V3d_View) _view;

    // occ structure item (for 3d text display)
    Handle(Graphic3d_Structure) _struct;

    // Occ context
    Handle(AIS_InteractiveContext) _context;

    AIS_MouseGestureMap _mouseDefaultGestures;
    Graphic3d_Vec2i _clickPos;
    curAction3d _curMode;
    Standard_Real _curZoom {0};

    QList<QAction*>* _viewActions {0};
    QList<QAction*>* _raytraceActions {0};
    QMenu* _backMenu {nullptr};

    void initCursors();
    void initViewActions();
    void initRaytraceActions();

    //! save the degenerate mode state.
    //Standard_Boolean _degenerateModeIsOn;

    //! rubber rectangle for the mouse selection.
    //QRubberBand* _rectBand = nullptr;

//    // mouse action modes
//    enum mouseActionMode
//    {
//        Selecting,
//        DynamicZooming,
//        // UNUSED   WindowZooming,
//        DynamicPanning,
//        // UNUSED   GlobalPanning,
//        DynamicRotation
//    };

//    //mouse current action mode.
//    mouseActionMode _mouseMode {mouseActionMode::Selecting};

//    // mouse position.
//    Standard_Integer _mouseXmin;
//    Standard_Integer _mouseYmin;
//    Standard_Integer _mouseXmax;
//    Standard_Integer _mouseYmax;

//    // Button events.
//    void onLButtonDown(int flags, QPoint point);
//    void onMButtonDown(int flags, QPoint point);
//    void onRButtonDown(int flags, QPoint point);

//    void onLButtonUp(int flags, QPoint point);
//    void onMButtonUp(int flags, QPoint point);
//    void onRButtonUp(int flags, QPoint point);

//    void onMouseMove(QMouseEvent* event, QPoint point);
//    void onMouseWheel(int flags, QPointF point, QPoint delta);
};

#endif // OCCVIEW_H
