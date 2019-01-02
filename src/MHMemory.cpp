#include <iomanip>
#include <ctime>
#include <sstream>

#include "MHMemory.hpp"
INITIALIZE_EASYLOGGINGPP

MH_Memory::MH_Memory(const std::string &ProcName, const std::string &SteamDLL) : _MHProcess(ProcName)
{
    INIT_LOGGER("CostumeArmor.log");

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
            DEBUG_LOG(ERROR,"Steam ID or Path NOT FOUND");
            DEBUG_LOG_HEX(DEBUG,"Current Steam Addr : "<< ( (long) Mod.getBaseAddress() + 237592) );
            DEBUG_LOG(DEBUG,"Current Steam ID : " << _SteamID );
            DEBUG_LOG(DEBUG,"Current Steam Path : " << _SteamPath);
        }
    }
}

void MH_Memory::FindAddress()
{
    if ( !this->ProcessOpen() )
        return;
    
    _DataPtr = FindDataAddress(_MHProcess);
    if (_DataPtr == 0)
        return;
    _DataPtr -= 29;

    DEBUG_LOG_HEX(DEBUG,"Address of the Characters Data : "<< _DataPtr);
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
    {
        delete[] CharDataBuffer;
        delete[] lpBuffer;
        return false;
    }

    _Data = PlayerData(CharDataBuffer, lpBuffer[0]);
    delete[] CharDataBuffer;
    delete[] lpBuffer ;
    return true;
}


std::string GetDateTime()
{
    std::time_t t = std::time(nullptr);

    std::stringstream Result;

    Result << std::put_time(std::localtime(&t), "%Y-%m-%d %H-%M-%S");

    return Result.str();
}

bool MH_Memory::BackupSaveData() const
{
    if (!this->SteamFound())
    {
        DEBUG_LOG(ERROR,"Steam Dir was not Found");
        return false;
    }

    if (!fs::exists(MH_Memory::BACKUP_DIR))
        if (!fs::create_directory(MH_Memory::BACKUP_DIR))
        {
            DEBUG_LOG(ERROR,"Backup Dir was not Found and couldn't created");
            return false;
        }

    auto SourcePath = this->_SteamPath;
    SourcePath /= "remote";
    SourcePath /= "SAVEDATA1000";

    if (!fs::exists(SourcePath))
    {
        DEBUG_LOG(ERROR,"Couldn't Find Save Data");
        return false;
    }

    auto DestPath = MH_Memory::BACKUP_DIR;
    DestPath /= ("Backup " + GetDateTime());
    try
    {
        fs::copy(SourcePath, DestPath);
    }
    catch (std::exception &e)
    {
        DEBUG_LOG(ERROR,"Couldn't Copy Save data. Error : " << e.what() );
        return false;
    }

    return fs::exists(DestPath);
}

bool MH_Memory::WriteArmor(int CharSlot, bool isSafe)
{
    if ( !this->ProcessOpen() || !this->DataAddressFound() )
        return false;
    if (!this->BackupSaveData())
    {
        DEBUG_LOG(ERROR,"Couldn't Backup SaveData");
        if(isSafe)
        {
            DEBUG_LOG(WARNING,"Can't write to memory without backup in safe mode.");
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
        Status &= _MHProcess.WriteMemoryUInt(_DataPtr + index * 4 + 1285888 * CharSlot, _Data.getArmorPiece(index));
    }

    return Status;
}