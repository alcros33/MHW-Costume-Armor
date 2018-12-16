// #include "qtexample.h"
// #include <QApplication>
#include <iostream>
#include <filesystem>
#include "Config.h"

#include "Process.hpp"
#include "PlayerData.hpp"

namespace fs = std::filesystem;

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

    Process MHW("MonsterHunterWorld.exe");

    if (!MHW.isOpen())
    {
        std::cout << "MonsterHunterWorld Process Not Found" << std::endl;
        return -1;
    }

    std::cout << "MonsterHunterWorld Process Found!!" << std::endl;


    std::cout << "Searching for Steam Stuff..." << std::endl;

    auto Mod = MHW.getModuleByName("steam_api64.dll");
    int SteamID;
    fs::path SteamGamePath;
    if (!Mod.isEmpty())
    {
        SteamID = MHW.ReadMemoryInt(Mod.getBaseAddress() + 237592);
        if (!SteamID)
            std::cout<< "\tFailed to retrieve steam data."<<std::endl;
        else
        {
            std::cout << "\tSteam UserData ID: " << SteamID <<std::endl;
            SteamGamePath = GetRegKeyValue(HKEY_CURRENT_USER,"Software\\Valve\\Steam", "SteamPath");
            if (!SteamGamePath.empty())
            {
                SteamGamePath.make_preferred();
                SteamGamePath /= "userdata";
                SteamGamePath /= std::to_string(SteamID);
                SteamGamePath /= "582010";
                std::cout << "\tSteam Game Directory: " << SteamGamePath << std::endl;
            }
        }
    }

    // for (auto &p : fs::directory_iterator(SteamGamePath))
    //     std::cout << p.path() << std::endl;

    std::cout << "Please wait while we retrieve your data..." << std::endl;
    DWORD ptr = FindDataAddress(MHW);
    if (ptr == 0)
    {
        std::cout << "Failed to Find Data Adress" << std::endl;
        return -1;
    }
    std::cout << "Data Adress Found Sucessful!!" << std::endl;
    ptr -= 29;

    auto Data = GetCharData(MHW,ptr,0);

    if (Data.isEmpty())
    {
        std::cout << "Error Opening Game Data" << std::endl;
        return -1;
    }

    std::cout << Data << std::endl;

    return 0;
}