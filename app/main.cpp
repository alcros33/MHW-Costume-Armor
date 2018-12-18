#include <QApplication>
#include <iostream>

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


    MainWindow w;
    #ifndef NDEBUG
        std::cout << ProjectName() << std::endl;
        w.debugPrints();
    #endif

    w.show();

    return app.exec();

    // std::cout << MHManager.getPlayerData() << std::endl;

    // MHManager.getPlayerData().setArmorPiece(Armor::HEAD,125); // Xenojivia, Not crashing

    // if (MHManager.WriteArmor(0))
    // {
    //     std::cout << "Armor Written Sucessfully!!" << std::endl;
    //     MHManager.FetchPlayerData(0);
    //     std::cout<< MHManager.getPlayerData() << std::endl;
    // }

    // return 0;
}