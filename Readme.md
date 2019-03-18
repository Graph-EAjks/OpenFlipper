OpenFlipper [![pipeline status](https://www.graphics.rwth-aachen.de:9000/OpenFlipper-Free/OpenFlipper-Free/badges/master/pipeline.svg)](https://www.graphics.rwth-aachen.de:9000/OpenFlipper-Free/OpenFlipper-Free/commits/master)
=======================

The documentation is on our website:
http://www.openflipper.org

## Download Binaries

### Windows
* [VS2017 Qt-5.10.1 64-Bit](https://www.graphics.rwth-aachen.de:9000/OpenFlipper-Free/OpenFlipper-Free/-/jobs/artifacts/master/browse/rel?job=Deploy:VS2017-Qt-5.10.1)
* [VS2015 Qt-5.10.1 64-Bit](https://www.graphics.rwth-aachen.de:9000/OpenFlipper-Free/OpenFlipper-Free/-/jobs/artifacts/master/browse/rel?job=Deploy:VS2015-Qt-5.10.1-x64)


### macOS 10.13
* [MacOS-10.13](https://www.graphics.rwth-aachen.de:9000/OpenFlipper-Free/OpenFlipper-Free/-/jobs/artifacts/master/browse/build-release?job=Deploy:Mac-Cpp11)



## Getting Source Code from git
Clone the repository with submodules:   
 git clone --recursive https://www.graphics.rwth-aachen.de:9000/OpenFlipper-Free/OpenFlipper-Free.git
 
## Required libs: 
Qt ( >= 5.9 )   http://www.qtsoftware.com/downloads  
OpenGL          http://www.opengl.org  
qwt ( >=6.0 )   optional http://qwt.sourceforge.net/  

## Compiling OpenFlipper:
A detailed compilation manual is available [here](http://openflipper.org/Documentation/latest/a00099.html).

Latest NEWS
===========

- 03/05/2019: OpenFlipper 4.0 Release (Core Profile support, Integrated Python interpreter)
- 04/21/2017: Moved all toplevel plugins to separate repositories.
- 02/28/2017: Removed SimpleOpt Command Line parser and switched to Qt parser
    - Breaking Change: Commandline parameter have to start with a dash, the slash is not recognized as parameter prefix anymore.
- 12/28/2016: Moved ObjectTypes into separate Libraries
    - Plugins have to specify their used types in the cmake file (TYPES / OPT_TYPES e.g. TYPES TRIANGLEMESH ), defines (e.g. ENABLE_TRIANGLEMESH_SUPPORT) are added automatically
    - Plugins will not be compiled if an ObjectType is missing (Excluding OPT_TYPES of course)
    - In an ObjectType you have to do the following replaces
        * sed -i s/GlobalDefines.hh/ObjectTypeDLLDefines.hh/g *.hh
        * sed -i s/DLLEXPORT/OBJECTTYPEDLLEXPORT/g *.hh
    - A cmake file has to be created in the ObjectType directory containing only include(type)
- 12/12/2016: Moved MeshTools into libs_required (Will be moved to Mesh Datatype when the type linking is changed)
- 12/12/2016: Redistributables for Visual Studio removed (Now in library Management repository)
- 12/12/2016: Moved tests directory into OpenFlipper subdir
- 12/09/2016: Moved Math_Tools to OpenFlipper/libs_required/Math_Tools
- 12/09/2016: Moved OpenMesh directory into OpenFlipper subdirectory OpenFlipper/libs_required/OpenMesh
    - You might have to delete your libs_required/OpenMesh directory manually if you get cmake errors regarding duplicate targets 
- 12/09/2016: Moved ACG directory into OpenFlipper subdirectory OpenFlipper/libs_required/ACG
- 12/08/2016: Moved CI directory into OpenFlipper subdirectory to remove toplevel dir count
- 12/08/2016: Moved the cmake directory contents into OpenFlipper/cmake (First steps to reduce number of toplevel dirs and get more modular than before)
