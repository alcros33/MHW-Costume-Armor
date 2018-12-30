#include <iostream>
#include <QApplication>

#include "MainWindow.hpp"
#include "MHMemory.hpp"
#include "Config.h"

std::string ProjectName()
{
    std::stringstream Res;
    Res << PROJECT_NAME;
    Res << " (" << std::to_string(PROJECT_VERSION_MAJOR);
    Res << "." << std::to_string(PROJECT_VERSION_MINOR);
    Res << "." << std::to_string(PROJECT_VERSION_PATCH)  << ")";
    return Res.str();
}


int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QCoreApplication::addLibraryPath("./plugins");

    MainWindow w;

    DEBUG_LOG(INFO, ProjectName() );
    w.debugPrints();

    w.show();

    return app.exec();
}