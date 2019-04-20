[![license](https://img.shields.io/badge/license-MIT-blue.svg)](LICENCE)

# MHW Costume Armor

MHW Costume Armor is a Monster Hunter World MOD which includes graphic user interface to customize the layered armor equipped.

It is a C++ implementaion of the original [MHW Transmog](https://www.nexusmods.com/monsterhunterworld/mods/43) MOD based on a decompilation of it.

Available also at [Nexus Mods](https://www.nexusmods.com/monsterhunterworld/mods/672)

## Some Benchmarks
I tested out against original Transmog on My computer (i7 6700, GTX 970M, 8GB RAM) on what I considered "Normal" workload.
AKA: MHW open, Firefox playing a youtube video and Discord Running in the back.

I measured the time of the "data retreival" part and memory usage.
* Original Transmog -> 23 seconds (Average) and 250MB RAM (Peak)
* Costume Armor -> 4 seconds (Average) and 60MB RAM (Peak)

## Release!
Checkout the compiled binaries on the latest [Release](https://github.com/alcros33/MHW-Costume-Armor/releases) !

## State of Development
* Memory reading (working properly).
* Memory writing (working properly).
* GUI Basic Functions (working properly).
* GUI Design and vanity (A E S T H E T I C).
* GUI Additional features :
    * Select ComboBox Instead of ID Input (working properly).
    * Save and Load ArmorSets by Name (working beta).
    * "Unsafe Mode" (working beta).
    * Delete and Rename Saved Sets(pending...)

### GUI Preview
![](Preview/preview1.png)
![](Preview/preview2.png)

# Dependencies To Build
* CMake # [Download Link!](https://cmake.org/download/)
* QT5 (Select MinGW 7.30)# [Download Link!](https://www.qt.io/download)
* MinGW 7.30 # When installing QT5 pick the actual MinGW compiler from the "tools" section
* Python 3 and `openpyxl` (`pip install openpyxl`)

Add the following folders to Path `C:\Qt\Tools\mingw730_64\bin` and `C:\Qt\5.12.0\mingw73_64\bin` (Guide [Info](https://www.computerhope.com/issues/ch000549.htm)).

## Extra Libraries that I include
Logging powered by EasyLogging++ [Available here](https://github.com/zuhd-org/easyloggingpp)

Json powered by nholmannJson [Available her](https://github.com/nlohmann/json)

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

