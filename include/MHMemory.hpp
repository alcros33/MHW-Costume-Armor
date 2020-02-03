#pragma once
#include <array>
#include <QCoreApplication>
#include "Process.hpp"
#include "file_helpers.hpp"

namespace Armor
{
    const int HEAD = 0;
    const int BODY = 1;
    const int ARMS = 2;
    const int WAIST = 3;
    const int LEGS = 4;
    const std::array<std::string, 5> NAMES{"Head", "Body", "Arms", "Waist", "Legs"};

    const u_int NOTHING = 512;
} // namespace Armor

struct SearchPattern
{
    u_int uintPattern;
    u_int lastBits;
};

using PlayerData = std::array<u_int, 5>;

class MH_Memory
{
public:
    MH_Memory();

    bool processIsOpen() const { return _MHProcess.isOpen(); }
    bool steamFound() const { return _steamFound; }
    bool dataAddressFound() const { return _dataPtr != 0; }
    
    bool validateProcess();

    PlayerData getPlayerData() const { return _data; }
    PlayerData& getPlayerData() { return _data; }
    Process getProcess() const { return _MHProcess; }
    int getSteamID() const { return _steamID;}
    QDir getSteamPath() const { return _steamPath; }

    void setSteamDirectory(const QDir &p);
    void unSetSteamDirectory();

    bool openProcess();
    void findSteamPath();
    void findDataAddress(QString selected_version="Latest");
    bool backupSaveData() const;

    bool readArmor(int slot);
    bool writeArmor(int char_slot, bool no_backup_ok = false);

    const QDir exeDir = QCoreApplication::applicationDirPath();
    const QDir backupDir = QCoreApplication::applicationDirPath() + "\\Backups";
    const std::string procName = "MonsterHunterWorld.exe";
    const std::string steamDLL = "steam_api64.dll";

    static std::map<QString, SearchPattern> versions;
    static std::map<QString, int> charSlotDist;

  private:
    Process _MHProcess;
    int _steamID = 0;
    QDir _steamPath;
    bool _steamFound = false;
    DWORD64 _dataPtr = 0;
    int _slotDist;
    PlayerData _data{Armor::NOTHING};
};

DWORD64 find_data_address(Process &Proc, SearchPattern Pa);