cmake_minimum_required(VERSION 3.13)

project(MHW_Costume_Armor)
set(MAIN_EXE_NAME "MHW Costume Armor")
enable_language(C CXX)

# --------------------------------------------------------------------------------
#                          Cmake Basic Config
# --------------------------------------------------------------------------------

# Set version number (change as needed).
# These definitions are available by including "Config.h" in the source. 
set(PROJECT_VERSION_MAJOR 0)
set(PROJECT_VERSION_MINOR 16)
set(PROJECT_VERSION_PATCH 2)
# No in-source builds
if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
    message(FATAL_ERROR "In-source builds not allowed. Please make a new directory (called a build directory) and run CMake from there.")
endif()
# Dfault build type
if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release")
endif()

# --------------------------------------------------------------------------------
#                          Include packages, libraries, etc.
# --------------------------------------------------------------------------------
# Find the QtWidgets library
# You may need to add Qt Instalation path to CMAKE_PREFIX_PATH
# It probably is C:\\Qt\\{VERSION}\\{COMPILER}\\lib\\cmake")
find_package(Qt5 COMPONENTS Widgets Network REQUIRED)
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)
# Create code from a list of Qt designer ui files.
set(CMAKE_AUTOUIC ON)
# Create code from a qt resources
SET(CMAKE_AUTORCC ON)
# Windows API Stuff
find_library (PSAPI Psapi)
# OpenSSL Dlls
# Set the OPENSSL_ROOT_DIR in your envrionment
file(GLOB OPENSSL_DLLS "$ENV{OPENSSL_ROOT_DIR}/bin/*.dll")

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
#                          Compile flags
# --------------------------------------------------------------------------------
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
add_definitions(-DELPP_NO_DEFAULT_LOG_FILE)
if (NOT MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wpedantic -Wextra -Wno-sign-compare -Wno-unused-parameter")
    # Build-type specific flags
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O2 -DNDEBUG -Wl,-subsystem,windows")
    set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")
    # Static libs
    set(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
    set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++ -static")
endif()
message(STATUS "Building with the following extra flags: ${CMAKE_CXX_FLAGS}")

# --------------------------------------------------------------------------------
#                         Locate files (no change needed).
# --------------------------------------------------------------------------------

# We make sure that CMake sees all the files.
include_directories(
    ${PROJECT_SOURCE_DIR}/src
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/include/ui
    ${PROJECT_SOURCE_DIR}/include/utils)
# Make variables referring to all the sources and test files.
file(GLOB SOURCES "src/*.cpp*" "src/ui/*.cpp*" "${PROJECT_SOURCE_DIR}/include/ui/*.hpp*")
# Resources
SET(WINDOWS_ICON_RESOURCE "${PROJECT_SOURCE_DIR}/resources/app_icon.rc")
qt5_add_resources(QT5_RESOURCES "${PROJECT_SOURCE_DIR}/resources/Icons.qrc")

# --------------------------------------------------------------------------------
#                            Build!
# --------------------------------------------------------------------------------
# Main EXE File
add_executable(${PROJECT_NAME} ${SOURCES} ${QT5_RESOURCES} ${WINDOWS_ICON_RESOURCE})

# MSVC Stuff
if (MSVC)
    target_compile_options(
        ${MAIN_LIBRARY_NAME} PUBLIC 
        "/std:c++17" 
        "/permissive-"
    )
    target_compile_definitions(
        ${MAIN_LIBRARY_NAME} PUBLIC
        NOMINMAX=1
    )
endif()
    
target_link_libraries(${PROJECT_NAME} PUBLIC PSAPI Qt5::Widgets Qt5::Network)
set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME ${MAIN_EXE_NAME})

# --------------------------------------------------------------------------------
#                               Post-Build
# --------------------------------------------------------------------------------

#Deploy QT on Windows
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    set(EXPORT_LIB_DIR "$<TARGET_FILE_DIR:${PROJECT_NAME}>/plugins")
    add_custom_command(
        TARGET ${PROJECT_NAME}
        POST_BUILD
        COMMAND windeployqt --release $<TARGET_FILE:${PROJECT_NAME}> --no-translations --plugindir ${EXPORT_LIB_DIR} --no-opengl-sw --no-system-d3d-compiler)
endif()

if (CMAKE_BUILD_TYPE STREQUAL "Release")
    configure_file("${PROJECT_SOURCE_DIR}/CHANGELOG.md" "${CMAKE_BINARY_DIR}/CHANGELOG.txt" COPYONLY)
    file(COPY ${OPENSSL_DLLS} DESTINATION ${CMAKE_BINARY_DIR})
endif()

# --------------------------------------------------------------------------------
#                            Misc
# --------------------------------------------------------------------------------
# Have CMake generate the config header with definitions.
configure_file (
  "${PROJECT_SOURCE_DIR}/include/Config.h.in"
  "${PROJECT_BINARY_DIR}/Config.h")
# add the binary tree to the search path for include Config.h
include_directories("${PROJECT_BINARY_DIR}")
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)
