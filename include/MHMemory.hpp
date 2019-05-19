#pragma once
#include "filesystem.hpp"

#include "PlayerData.hpp"

namespace fs = filesystem;

fs::Path CurrentExecutableDir();

class MH_Memory
{
public:
    MH_Memory(const std::string &ProcName = "MonsterHunterWorld.exe", const std::string &SteamDLL = "steam_api64.dll");

    bool processOpen() const { return _MHProcess.isOpen(); }
    bool steamFound() const { return _steamFound; }
    bool dataAddressFound() const { return _dataPtr != 0; }

    int getSteamID() const { return _steamID;}
    fs::Path getSteamPath() const { return _steamPath; }
    PlayerData getPlayerData() const { return _Data; }
    PlayerData& getPlayerData() { return _Data; }
    Process getProcess() const { return _MHProcess; }

    void setSteamDirectory(const fs::Path &Path);

    bool fetchPlayerData(int slot);
    void findAddress(std::string Ver="Latest");
    bool backupSaveData() const ;
    bool writeArmor(int CharSlot, bool isSafe = true);

    fs::Path exeFilePath = CurrentExecutableDir();
    fs::Path backupDir = CurrentExecutableDir().append("Backups");
    fs::Path logPath = CurrentExecutableDir().append("CostumeArmor.log");

    static std::map<std::string, SearchPattern> Versions;
    static std::map<std::string, int> CharSlotDist;

  private:
    Process _MHProcess;
    int _steamID = 0;
    fs::Path _steamPath;
    PlayerData _Data;
    DWORD64 _dataPtr = 0;
    bool _steamFound = false;
    int _slotDist;
};
