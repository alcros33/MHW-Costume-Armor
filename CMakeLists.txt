cmake_minimum_required(VERSION 3.12)

project(MHW_Costume_Armor)
set(MAIN_APP_NAME "MHW Costume Armor")
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
set(PROJECT_VERSION_MAJOR 0)
set(PROJECT_VERSION_MINOR 9)
set(PROJECT_VERSION_PATCH 3)

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
if (NOT MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wpedantic -Wextra -Wno-sign-compare -Wno-unused-parameter")  
    # Build-type specific flags. Change as needed.
    SET(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")
    SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wl,-subsystem,windows")
    SET(CMAKE_CXX_FLAGS_DEBUG "-g -O0")
endif()


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
file(GLOB SOURCES "src/*.cpp*")
file(GLOB APPFILES "app/*.cpp")

## EasyLogging stuff
add_definitions(-DELPP_NO_DEFAULT_LOG_FILE)

## Windows API Stuff
find_library (PSAPI Psapi)

SET(WINDOWS_ICON_RESOURCE "${PROJECT_SOURCE_DIR}/resources/app_icon.rc")

# QT5 Resources
qt5_add_resources(QT5_RESOURCES "${PROJECT_SOURCE_DIR}/resources/Icons.qrc")

# --------------------------------------------------------------------------------
#                            Build! (Change as needed)
# --------------------------------------------------------------------------------

# Compile all sources into a library. Called engine here (change if you wish).

add_library(${MAIN_LIBRARY_NAME} SHARED ${SOURCES} )
if (MSVC)
    target_link_libraries(
        ${MAIN_LIBRARY_NAME} PUBLIC 
        PSAPI
    )
    target_compile_options(
        ${MAIN_LIBRARY_NAME} PUBLIC 
        "/std:c++17" 
        "/permissive-"
    )
else()
    target_link_libraries(
        ${MAIN_LIBRARY_NAME} PUBLIC 
        PSAPI 
        stdc++fs
    )
endif()
# Add an executable for the file main.cpp, here called main.x.
# If you add more executables, copy these three lines accordingly.

add_executable(${PROJECT_NAME} ${APPFILES} ${QT5_RESOURCES} ${WINDOWS_ICON_RESOURCE})
add_dependencies(${PROJECT_NAME} ${MAIN_LIBRARY_NAME})     
target_link_libraries(${PROJECT_NAME} ${MAIN_LIBRARY_NAME} Qt5::Widgets)

set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME ${MAIN_APP_NAME})

# Script to generate ArmorData.json config file
set(SCRIPTS_DIR "${PROJECT_SOURCE_DIR}/scripts")
add_custom_command(
    TARGET ${PROJECT_NAME}
    POST_BUILD
    COMMAND python "${SCRIPTS_DIR}/GenerateJson.py" --source "${SCRIPTS_DIR}/MHW Layered Armor.xlsx" --dest "${CMAKE_BINARY_DIR}/ArmorData.json" )

# Deploy QT on Windows
if(WIN32 AND CMAKE_BUILD_TYPE STREQUAL "Release")
    set(EXPORT_LIB_DIR "$<TARGET_FILE_DIR:${PROJECT_NAME}>/plugins")
    add_custom_command(
        TARGET ${PROJECT_NAME}
        POST_BUILD
        COMMAND windeployqt --release $<TARGET_FILE:${PROJECT_NAME}> --no-translations --no-system-d3d-compiler --plugindir ${EXPORT_LIB_DIR})
endif()

if (CMAKE_BUILD_TYPE STREQUAL "Release")
    configure_file (
    "${PROJECT_SOURCE_DIR}/CHANGELOG.md"
    "${CMAKE_BINARY_DIR}/CHANGELOG.txt")
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
