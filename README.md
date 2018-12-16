[![license](https://img.shields.io/badge/license-MIT-blue.svg)](LICENCE)

# MHW MK. Armor

MK. Armor is a C++ graphic implementaion of the original [MHW Transmog](https://www.nexusmods.com/monsterhunterworld/mods/43) MOD based on a decompilation of it.

## State of Development
Memory reading working properly.

## Depndencies

* CMake # [Download Link!](https://cmake.org/download/)
* QT5 (Select MinGW 7.30)# [Download Link!](https://www.qt.io/download)
* MinGW (x64, posix, seh) (MinGW is usually included with QT5) # [Download Link!](https://sourceforge.net/projects/mingw-w64/files/latest/download)

## Building

Build by making a build directory (i.e. `build/`), run `cmake` in that dir, and then use `mingw32-make` to build the desired target.

``` bash
$ mkdir build && cd build
$ cmake .. -DCMAKE_BUILD_TYPE=[Debug | Release] -G "MinGW Makefiles"
$ mingw32-make
$ mingw32-make gtest     # Makes and runs the tests.(Only Available in debug mode )
```
 




