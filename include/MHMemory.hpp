#pragma once
#include <filesystem>

#include "PlayerData.hpp"

namespace fs = std::filesystem;

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

    bool FetchPlayerData(int slot);
    void FindAddress();
    bool BackupSaveData() const ;
    bool WriteArmor(int CharSlot, bool isSafe = true);

    fs::path BACKUP_DIR = fs::current_path().append("Backups");

private:
    Process _MHProcess;
    int _SteamID = 0;
    fs::path _SteamPath;
    PlayerData _Data;
    DWORD64 _DataPtr = 0;
    bool _SteamFound = false;
};