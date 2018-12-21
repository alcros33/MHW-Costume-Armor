cmake_minimum_required(VERSION 3.12)

project(MHW_Mk_Armor)
set(MAIN_APP_NAME "MHWMkArmor")
set(MAIN_LIBRARY_NAME "MemoryHandler")

enable_language(C CXX)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

# --------------------------------------------------------------------------------
#                          Cmake Basic Config
# --------------------------------------------------------------------------------

# Set version number (change as needed). These definitions are available
# by including "exampleConfig.h" in the source. 
# See exampleConfig.h.in for some more details.
set(PROJECT_VERSION_MAJOR 1)
set(PROJECT_VERSION_MINOR 0)
set(PROJECT_VERSION_PATCH 0)

#Include stuff. No change needed.
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")
include(ConfigSafeGuards)
include(Colors)

# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)


# --------------------------------------------------------------------------------
#                          Include packages, libraries, etc.
# --------------------------------------------------------------------------------

# Find the QtWidgets library
find_package(Qt5Widgets CONFIG REQUIRED)
find_package(Qt5Core CONFIG REQUIRED)
# Create code from a list of Qt designer ui files.
set(CMAKE_AUTOUIC ON) # use this if you have CMake 3.x instead of the following

# --------------------------------------------------------------------------------
#                          Set binary output dir
# --------------------------------------------------------------------------------
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    SET(CMAKE_BINARY_DIR "${CMAKE_SOURCE_DIR}/bin")
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
endif()

# --------------------------------------------------------------------------------
#                          Compile flags (change as needed).
# --------------------------------------------------------------------------------
# Set the C++ standard you wish to use (will apply to all files).
# If you do not use any features that limits the standard required,
# you could omit this line.

set(CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)


# Things to always include as flags. Change as needed.
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wpedantic -Wextra -Wno-sign-compare -Wno-unused-parameter")  
# Build-type specific flags. Change as needed.
SET(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")
if(MINGW)
    SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wl,-subsystem,windows")
endif()

SET(CMAKE_CXX_FLAGS_DEBUG "-g -O0")


message(STATUS "Building with the following extra flags: ${CMAKE_CXX_FLAGS}")
# --------------------------------------------------------------------------------
#                         Locate files (no change needed).
# --------------------------------------------------------------------------------

# We make sure that CMake sees all the files.
include_directories(
    ${PROJECT_SOURCE_DIR}/src
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/include/utils)

# Make variables referring to all the sources and test files.
file(GLOB SOURCES "src/*.cpp")
file(GLOB APPFILES "app/*.cpp")

##
find_library (PSAPI Psapi)

if(MINGW)
    SET(WINDOWS_ICON_RESOURCE "${PROJECT_SOURCE_DIR}/resources/app_icon.rc")
endif()

# Resources Example
qt5_add_resources(QT5_RESOURCES "${PROJECT_SOURCE_DIR}/resources/Icons.qrc")

# --------------------------------------------------------------------------------
#                            Build! (Change as needed)
# --------------------------------------------------------------------------------

# Compile all sources into a library. Called engine here (change if you wish).

add_library(${MAIN_LIBRARY_NAME} SHARED ${SOURCES} )
target_link_libraries(${MAIN_LIBRARY_NAME} PSAPI stdc++fs Qt5::Core)

# Add an executable for the file main.cpp, here called main.x.
# If you add more executables, copy these three lines accordingly.

add_executable(${MAIN_APP_NAME} ${APPFILES} ${QT5_RESOURCES} ${WINDOWS_ICON_RESOURCE})
add_dependencies(${MAIN_APP_NAME} ${MAIN_LIBRARY_NAME})     
target_link_libraries(${MAIN_APP_NAME} stdc++fs ${MAIN_LIBRARY_NAME} Qt5::Widgets)

# Deploy QT on Windows
if(MINGW AND CMAKE_BUILD_TYPE STREQUAL "Release")
    set(EXPORT_LIB_DIR "$<TARGET_FILE_DIR:${MAIN_APP_NAME}>/plugins")
    add_custom_command(
        TARGET ${MAIN_APP_NAME}
        POST_BUILD
        COMMAND windeployqt --release $<TARGET_FILE:${MAIN_APP_NAME}> --no-translations --no-system-d3d-compiler --plugindir ${EXPORT_LIB_DIR})
endif()

# --------------------------------------------------------------------------------
#                            Misc (no change needed).
# --------------------------------------------------------------------------------
# Have CMake parse the config file, generating the config header, with
# correct definitions. Here only used to make version number available to
# the source code. Include "exampleConfig.h" (no .in suffix) in the source.
configure_file (
  "${PROJECT_SOURCE_DIR}/include/Config.h.in"
  "${PROJECT_BINARY_DIR}/Config.h")
# add the binary tree to the search path for include files
# so that we will find exampleConfig.h
include_directories("${PROJECT_BINARY_DIR}")

# Ask CMake to output a compile_commands.json file for use with things like Vim YCM.
set( CMAKE_EXPORT_COMPILE_COMMANDS 1 )
