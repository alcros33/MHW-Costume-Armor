#include <iostream>
#include <QApplication>

#include "MainWindow.hpp"
#include "MHMemory.hpp"
#include "Config.h"

std::string ProjectName()
{
    std::string Res = PROJECT_NAME;
    Res += " (";
    Res += std::to_string(PROJECT_VERSION_MAJOR);
    Res += ".";
    Res += std::to_string(PROJECT_VERSION_MINOR);
    Res += ".";
    Res += std::to_string(PROJECT_VERSION_PATCH);
    Res += ")";
    return Res;
}


int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QCoreApplication::addLibraryPath("./plugins");

    MainWindow w;
    #ifndef NDEBUG
        std::cout << ProjectName() << std::endl;
        w.debugPrints();
    #endif

    w.show();

    return app.exec();
}