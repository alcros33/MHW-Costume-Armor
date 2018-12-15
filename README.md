[![license](https://img.shields.io/badge/license-MIT-blue.svg)](https://github.com/bsamseth/cpp-project/blob/master/LICENCE)

# MHW MK. Armor

MK. Armor is a C++ implementaion MOD Program for Monster Hunter World which allows you to select which layered skin your character should display.

It is based on a decompilation of MHWTransmog MOD

## Depndencies

### Windows

* CMake # [Download Link!](https://cmake.org/download/)
* QT5 (Select MinGW 7.30)# [Download Link!](https://www.qt.io/download)
* MinGW (x64, posix, seh) (MinGW is usually included with QT5) # [Download Link!](https://sourceforge.net/projects/mingw-w64/files/latest/download)

## Building

Build by making a build directory (i.e. `build/`), run `cmake` in that dir, and then use `mingw32-make` to build the desired target.

### Windows

``` bash
$ mkdir build && cd build
$ cmake .. -DCMAKE_BUILD_TYPE=[Debug | Release] -G "MinGW Makefiles"
$ mingw32-make
$ mingw32-make gtest     # Makes and runs the tests.(Only Available in debug mode )
```
 




