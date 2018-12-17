#include "MHMemory.hpp"

MH_Memory::MH_Memory(const std::string &ProcName, const std::string &SteamDLL) : _MHProcess(ProcName)
{
    if (!_MHProcess.isOpen())
        return;

    auto Mod = _MHProcess.getModuleByName(SteamDLL);
    if (!Mod.isEmpty())
    {
        _SteamID = _MHProcess.ReadMemoryInt(Mod.getBaseAddress() + 237592);
        _SteamPath = GetRegKeyValue(HKEY_CURRENT_USER, "Software\\Valve\\Steam", "SteamPath");
        if (_SteamID != 0 && !_SteamPath.empty() )
        {
            _SteamPath.make_preferred();
            _SteamPath /= "userdata";
            _SteamPath /= std::to_string(_SteamID);
            _SteamPath /= "582010";
            _SteamFound = true;
        }
        else
        {
            #ifndef NDEBUG
                std::cout<< "Steam ID or Path NOT FOUND"<<std::endl;
                std::cout<< "Current Steam Addr : ";
                PrintHexLine(   (long) Mod.getBaseAddress() + 237592);
                std::cout<< "Current Steam ID : " << _SteamID << std::endl;
                std::cout << "Current Steam Path : " << _SteamPath << std::endl;
            #endif
        }
    }
}

bool MH_Memory::FetchPlayerData(int slot)
{
    if ( !this->ProcessOpen() || !this->DataAddressFound() )
        return false;

    if (slot < 0)
        slot = 0;
    if (slot > 2)
        slot = 2;
    
    byte *CharDataBuffer = _MHProcess.ReadMemory(_DataPtr + 1285888 * slot, 28);
    byte *lpBuffer = _MHProcess.ReadMemory(_DataPtr + 1285888 * slot - 394460, 1);

    if (!CharDataBuffer || !lpBuffer)
        return false;

    _Data = PlayerData(CharDataBuffer, lpBuffer[0]);
    return true;
}

void MH_Memory::FindAddress()
{
    _DataPtr = FindDataAddress(_MHProcess);
    if (_DataPtr == 0)
        return;
    _DataPtr -= 29;
    
    #ifndef NDEBUG
        std::cout<< "Address of the Characters Data : ";
        PrintHexLine(_DataPtr);
    #endif
}

const fs::path MH_Memory::BACKUP_DIR = fs::current_path().append("Backups");

bool MH_Memory::BackupSaveData() const
{
    if (!this->SteamFound())
    {
        #ifndef NDEBUG
            std::cout<< "Steam Dir was not Found" <<std::endl;
        #endif
        return false;
    }
    if (!fs::exists(MH_Memory::BACKUP_DIR))
        if (!fs::create_directory(MH_Memory::BACKUP_DIR))
        {
            #ifndef NDEBUG
                std::cout<< "Backup Dir was not Found and couldn't created" << std::endl;
            #endif
            return false;
        }

    auto SourcePath = this->_SteamPath;
    SourcePath /= "remote";
    SourcePath /= "SAVEDATA1000";

    if (!fs::exists(SourcePath))
    {
        #ifndef NDEBUG
            std::cout << "Couldn't Find Save Data" << std::endl;
        #endif
        return false;
    }

    auto DestPath = MH_Memory::BACKUP_DIR;
    DestPath /= "BackUp - 0"; // TODO Replace 0 with datetime

    if (fs::exists(DestPath))
        return false;
    
    fs::copy(SourcePath, DestPath);

    return fs::exists(DestPath);
}

bool MH_Memory::WriteArmor(int CharSlot, bool isSafe)
{
    if ( !this->ProcessOpen() || !this->DataAddressFound() )
        return false;
    if (!this->BackupSaveData())
    {
        std::cout << "Couldn't Backup SaveData" << std::endl;
        if(isSafe)
        {
            std::cout << "Can't write without backup in safe mode." << std::endl;
            return false;
        }
    }

    if (CharSlot < 0)
        CharSlot = 0;
    if (CharSlot > 2)
        CharSlot = 2;

    bool Status = true;

    for (int index = 0; index < 5; ++index)
    {
        Status &= _MHProcess.WriteMemoryInt(_DataPtr + index * 4 + 1285888 * CharSlot, _Data.getArmorPiece(index));
    }

    return Status;
}