#include <iostream>
#include <QApplication>
#include <QVersionNumber>
#include "MainWindow.hpp"
#include "MHMemory.hpp"
#include "Config.h"

QString dir_path(int argc, char *argv[])
{
    QApplication dummy(argc, argv);
    return QCoreApplication::applicationDirPath();
}

int main(int argc, char *argv[])
{
    auto dp = dir_path(argc, argv);
    QSettings settings(dp + "\\settings.ini", QSettings::IniFormat);
    QApplication app(argc, argv);
    QCoreApplication::addLibraryPath("./plugins");

    QString project = PROJECT_NAME;
    QVersionNumber version(PROJECT_VERSION_MAJOR, PROJECT_VERSION_MINOR, PROJECT_VERSION_PATCH);
    project += " " + version.toString();
    MainWindow w("https://github.com/alcros33/MHW-Costume-Armor", version.toString(), settings);

    LOG_ENTRY(INFO, project);
    
    w.show();
    
    try
    {
        return app.exec();
    }
    catch (const std::exception &ex)
    {
        LOG_ENTRY(FATAL,"Application crashed due to "<< ex.what());
        return -1;
    }
    catch (const std::string &ex)
    {
        LOG_ENTRY(FATAL,"Application crashed due to "<< ex);
        return -1;
    }
    catch (...)
    {
        LOG_ENTRY(FATAL,"Application crashed due to unknwon exception.");
        return -1;
    }
}