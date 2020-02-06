[![license](https://img.shields.io/badge/license-MIT-blue.svg)](LICENCE)

# MHW Costume Armor

MHW Costume Armor is a Monster Hunter World MOD which includes graphic user interface to customize the layered armor equipped.

It is a C++ implementaion of the original [MHW Transmog](https://www.nexusmods.com/monsterhunterworld/mods/43).

Available also at [Nexus Mods](https://www.nexusmods.com/monsterhunterworld/mods/672)

## Release!
Checkout the compiled binaries on the latest [Release](https://github.com/alcros33/MHW-Costume-Armor/releases) !

### GUI Preview
![](Preview/preview1.png)
![](Preview/preview2.png)

# Dependencies To Build
* CMake # [Download Link!](https://cmake.org/download/)
* QT5 (Select MinGW 7.30)# [Download Link!](https://www.qt.io/download)
* MinGW 7.30 # When installing QT5 pick the actual MinGW compiler from the "tools" section
* Python 3 and `openpyxl` (`pip install openpyxl`)

Add the following folders to Path `C:\Qt\Tools\mingw730_64\bin` and `C:\Qt\{VERSION}\mingw73_64\bin` (Guide [Info](https://www.computerhope.com/issues/ch000549.htm)).

## Extra Libraries that I include
Logging powered by EasyLogging++ [Available here](https://github.com/amrayn/easyloggingpp)
Sorry I'm to lazy to do the git submodule sutff.

## Building Using MinGW

``` bash
$ mkdir build
$ cd build
$ cmake .. -DCMAKE_BUILD_TYPE=[Debug | Release] -G "MinGW Makefiles"
$ mingw32-make
```

## Building Using Visual Studio
* Visual Studio 2017
* Have MSVC x64 in your Qt install

Build either with CMake directly, or use File > Open > CMake in Visual 
Studio, then select Debug or Release, and press build.

### Directly with CMake
```cmake
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=[Debug | Release] -G "Visual Studio 15 2017 Win64"
cmake --build . --config [Debug | Release]
```

