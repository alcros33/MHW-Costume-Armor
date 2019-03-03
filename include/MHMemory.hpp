#pragma once
#include <filesystem>

#include "PlayerData.hpp"

namespace fs = std::filesystem;

fs::path CurrentExecutableDir();

class MH_Memory
{
public:
    MH_Memory(const std::string &ProcName = "MonsterHunterWorld.exe", const std::string &SteamDLL = "steam_api64.dll");

    bool ProcessOpen() const { return _MHProcess.isOpen(); }
    bool SteamFound() const { return _SteamFound; }
    bool DataAddressFound() const { return _DataPtr != 0; }

    int getSteamID() const { return _SteamID;}
    fs::path getSteamPath() const { return _SteamPath; }
    PlayerData getPlayerData() const { return _Data; }
    PlayerData& getPlayerData() { return _Data; }
    Process getProcess() const { return _MHProcess; }

    void setSteamDirectory(const fs::path &Path);

    bool FetchPlayerData(int slot);
    void FindAddress(std::string Ver="Latest");
    bool BackupSaveData() const ;
    bool WriteArmor(int CharSlot, bool isSafe = true);

    fs::path ExeFilePath = CurrentExecutableDir();
    fs::path BACKUP_DIR = CurrentExecutableDir().append("Backups");
    fs::path LogPath = CurrentExecutableDir().append("CostumeArmor.log");

    static std::map<std::string,int> Versions;

  private:
    Process _MHProcess;
    int _SteamID = 0;
    fs::path _SteamPath;
    PlayerData _Data;
    DWORD64 _DataPtr = 0;
    bool _SteamFound = false;
};
