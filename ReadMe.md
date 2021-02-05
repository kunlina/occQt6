# Overview
* forked from [eryar/occQt](https://github.com/eryar/occQt), combined with [Felipeasg/basicOccWidget](https://github.com/Felipeasg/basicOccWidget) and updated to work with Qt6. 
* updated mouse navigation using `AIS_ViewController.hxx`, which didn't exist when `occQt` was written
* Uses more modern icons in svg file format from [Lucide](https://github.com/lucide-icons/lucide) plus a few custom ones


# Features
* Demonstrate operations for the OpenCASCADE viewer, such as select, pan, zoom and rotate
* Manipulate the viewer without needing a middle mouse button to be touchpad and MacOS mouse friendly
* Demonstrate primitive shape construction algorithms usage, such as box, cone, sphere, cylinder and torus creation
* Demonstrate revol, loft and fillet, chamfer operations
* Demonstrate boolean operations, such as cut, fuse and common
* Demonstrate creating helical objects such as springs

# Todo
* [wip] add heads-up view widget (3d view selection, raytracing on/off, shading, background, etc.)
* add clipping planes
* add image export
* stl/step import/export
* demonstrate more modeling algorithms, such as prism


# Build instructions
occQt6 requires:
* Qt6, which can be obtained from [https://www.qt.io](https://www.qt.io/download-qt-installer).
* OpenCASCADE, which can be obtained from [Open-Cascade-SAS/OCCT](https://github.com/Open-Cascade-SAS/OCCT) or the official download center at [https://old.opencascade.com](https://old.opencascade.com/content/latest-release)

Once both libs are installed, update the `CAS_INC_DIR` and `CAS_LIB_DIR` variables in `occQt6.pro` and run the following:
```
qmake
(n)make
```
This creates an executable in the `binaries/<operating system>/release` folder. 

Before starting the executable you need to set your `PATH` or `LD_LIBRARY_PATH` to point to both Qt's and OpenCASCADE's dynamic libraries, as well as their dependencies (e.g., freetype or freeimage for OpenCASCADE). 
