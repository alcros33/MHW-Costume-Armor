// #include "qtexample.h"
// #include <QApplication>
#include <iostream>
#include "Config.h"

#include "MHMemory.hpp"

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
    // QApplication app(argc, argv);
    // MHWMkArmor w;
    // w.show();

    // return app.exec();

    std::cout << ProjectName() << std::endl;

    MH_Memory MHManager;

    if (!MHManager.ProcessOpen())
    {
        std::cout << "Couldn't Find MonsterHunterWorld Process" << std::endl;
        return -1;
    }

    std::cout << "MonsterHunterWorld Process Found!!" << std::endl;

    if (MHManager.SteamFound() )
    {
        std::cout << "\tSteam UserData ID: " << MHManager.getSteamID() << std::endl;
        std::cout << "\tSteam Game Directory: " << MHManager.getSteamPath() << std::endl;
    }

    // for (auto &p : fs::directory_iterator(SteamGamePath))
    //     std::cout << p.path() << std::endl;

    std::cout << "Please wait while we retrieve your data..." << std::endl;
    MHManager.FindAddress();
    if (!MHManager.DataAddressFound())
    {
        std::cout << "Failed to Find Data Address" << std::endl;
        return -1;
    }
    std::cout << "Data Address Found Sucessfully!!" << std::endl;

    std::cout << "Trying to read Data from Character Slot 1" << std::endl;

    if (!MHManager.FetchPlayerData(0))
    {
        std::cout << "Error Reading Game Data" << std::endl;
        return -1;
    }

    std::cout << MHManager.getPlayerData() << std::endl;

    MHManager.getPlayerData().setArmorPiece(Armor::HEAD,125); // Xenojivia, Not crashing

    if (MHManager.WriteArmor(0))
    {
        std::cout << "Armor Written Sucessfully!!" << std::endl;
        MHManager.FetchPlayerData(0);
        std::cout<< MHManager.getPlayerData() << std::endl;
    }
    std::cout << "Exiting..." << std::endl;
    return 0;
}