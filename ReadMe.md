# Overview
* forked from [eryar/occQt](https://github.com/eryar/occQt), combined with [Felipeasg/basicOccWidget](https://github.com/Felipeasg/basicOccWidget) and updated to work with Qt6. 
* Also uses more modern icons in svg file format from [Lucide](https://github.com/lucide-icons/lucide)


# ToDo
* [wip] Demonstrate the operations for the OpenCASCADE viewer, such as pan, zoom and rotate
* [wip] Manipulate the view without needing a middle mouse button to be touchpad and MacOS mouse friendly
* Demonstrate the primitive shape construction algorithms usage, such as box, cone, sphere, cylinder and torus creation;
* Demonstrate some modeling algorithms, such as prism, revol, loft and fillet, chamfer;
* Demonstrate boolean operations, such as cut, fuse and common;
* Demonstrate the helix algorithms;

# Build instructions
occQt6 requires:
* Qt6, which can be obtained from [https://www.qt.io](https://www.qt.io/download-qt-installer).
* OpenCASCADE, which can be obtained from [Open-Cascade-SAS/OCCT](https://github.com/Open-Cascade-SAS/OCCT) or the official download center at [https://old.opencascade.com](https://old.opencascade.com/content/latest-release)

Once both libs are installed, update the `CAS_INC_DIR` and `CAS_LIB_DIR` variables in `occQt6.pro` and run the following:
```
qmake
(n)make
```
This creates an executable in the `binaries/<operating system>/release` folder. You need to set your `PATH` variables to point to both Qt's and OpenCASCADE's dynamic libraries, as well as their dependencies (e.g., freetype or freeimage for OpenCASCADE). 
