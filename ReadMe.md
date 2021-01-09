# Overview
* forked from [eryar/occQt](https://github.com/eryar/occQt), but updated to work with OpenCASCADE 7.5.0 and Qt6. 
* Also uses more modern icons in svg file format
* Demonstrate the operations for the OpenCASCADE viewer, such as pan, zoom and rotate; the view manipulate mainly by the middle button of the mouse, and left button is used to pick and select;

# ToDo
* Demonstrate the primitive shape construction algorithms usage, such as box, cone, sphere, cylinder and torus creation;
* Demonstrate some modeling algorithms, such as prism, revol, loft and fillet, chamfer;
* Demonstrate boolean operations, such as cut, fuse and common;
* Demonstrate the helix algorithms;

# Build instructions
occQt6 requires:
* Qt6, which can be obtained from [https://www.qt.io](https://www.qt.io/download-qt-installer).
* OpenCASCADE, which can be obtained from [Open-Cascade-SAS/OCCT](https://github.com/Open-Cascade-SAS/OCCT)

Once both are installed, update the `CASROOT` variable in `occQt6.pro` and run the following:
```
qmake
(n)make
```
This creates an executable in the `binaries/<operating system>/release` folder.

