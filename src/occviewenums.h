#ifndef OCCVIEWENUMS_H
#define OCCVIEWENUMS_H

namespace occViewEnums {

enum curAction3d {
    Nothing,
    DynamicZooming,
    WindowZooming,
    DynamicPanning,
    GlobalPanning,
    DynamicRotation,
    Selecting
};

enum viewAction {
    viewFitAll,
    viewFitArea,
    viewZoom,
    viewPan,
    viewGlobalPan,
    viewFront,
    viewBack,
    viewTop,
    viewBottom,
    viewLeft,
    viewRight,
    viewAxo,
    viewRotation,
    viewReset,
    viewHlrOff,
    viewHlrOn
};


enum raytraceAction { toolRaytracingId,
                      toolShadowsId,
                      toolReflectionsId,
                      toolAntialiasingId
                    };

enum drawStyle {
    points,
    wireframe,
    hlrOn,
    shaded,
    shadedWithEdges
};

}


#endif // OCCVIEWENUMS_H
