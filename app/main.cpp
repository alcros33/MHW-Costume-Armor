#include <iostream>
#include <QApplication>

#include "MainWindow.hpp"
#include "MHMemory.hpp"
#include "Config.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QCoreApplication::addLibraryPath("./plugins");

    MainWindow w;
    std::string Project = QString("%1 (%2.%3.%4)").arg(PROJECT_NAME).arg(PROJECT_VERSION_MAJOR).arg(PROJECT_VERSION_MINOR).arg(PROJECT_VERSION_PATCH).toStdString();

    DEBUG_LOG(INFO, Project);
    w.debugPrints();

    #ifndef NDEBUG
        if (argc > 1)
        {
            if (std::string(argv[1]) == "--test-gui")
                w._show_test_gui();
            else
                w.show();
        }
        else
            w.show();
    #else
        w.show();
    #endif
    
    try
    {
        return app.exec();
    }
    catch (const std::exception &ex)
    {
        DEBUG_LOG(ERROR,"Application crashed due to "<< ex.what());
        return -1;
    }
    catch (const std::string &ex)
    {
        DEBUG_LOG(ERROR,"Application crashed due to "<< ex);
        return -1;
    }
    catch (...)
    {
        DEBUG_LOG(ERROR,"Application crashed due to unknwon exception.");
        return -1;
    }
}