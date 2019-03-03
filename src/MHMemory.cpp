#include <iomanip>
#include <ctime>
#include <sstream>

#include "MHMemory.hpp"
INITIALIZE_EASYLOGGINGPP

/// Begin MH_Memory Member definitions
MH_Memory::MH_Memory(const std::string &ProcName, const std::string &SteamDLL) : _MHProcess(ProcName)
{
    INIT_LOGGER( LogPath.string() );

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
    else
    {
        DEBUG_LOG(ERROR, "Module "<<SteamDLL<<" NOT FOUND");
        std::stringstream S;
        S << "Found Modules : ";
        for(const auto &mod : _MHProcess.getModuleList() )
            S << mod.getName() <<", ";
        DEBUG_LOG(DEBUG, S.str() );
    }
}

void MH_Memory::FindAddress(std::string Ver)
{
    if ( !this->ProcessOpen() )
        return;
    
    if (MH_Memory::Versions.find(Ver) == MH_Memory::Versions.end()) // Safety Check
        Ver = "Latest";

    _DataPtr = FindDataAddress(_MHProcess, MH_Memory::Versions[Ver]);
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

void MH_Memory::setSteamDirectory(const fs::path &Path)
{
    this->_SteamPath = Path;
    _SteamPath.make_preferred();
    _SteamPath /= "userdata";
    _SteamPath /= std::to_string(_SteamID);
    _SteamPath /= "582010";
    _SteamFound = true;
    DEBUG_LOG(DEBUG, "Steam Path customly set to "<<_SteamPath);
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
    catch (fs::filesystem_error &e)
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
    
    std::stringstream S;
    for( const auto d : _Data.getData() )
        S << ( (int)d ) << ", ";
    DEBUG_LOG(DEBUG,"Writed (" << S.str()<<") to the game." );

    return Status;
}

// ByteArray to search
// Version : 163956, Byte Arr: [231,188,66,1]
// Version : 165889, Byte Arr: [174,190,66,1]
// Version : 166849, Byte Arr: [ 47,192,66,1]

// Byte pattern to search given version
std::map<std::string, int> MH_Memory::Versions{
    {"163956", BytesToInt({231, 188, 66, 1})},
    {"165889", BytesToInt({174, 190, 66, 1})},
    {"166849", BytesToInt({ 47, 192, 66, 1})},
    {"Latest", BytesToInt({ 47, 192, 66, 1})}
};

/// End MH_Memory Member definitions
fs::path CurrentExecutableDir()
{
    TCHAR Buffer[1024];
    if (GetModuleFileName(nullptr, Buffer, 1024) != 0)
    {
        fs::path Path(Buffer);
        Path.make_preferred();
        return Path.parent_path();
    }
    DEBUG_LOG(ERROR, "Couldn't retrieve the name of current EXE File.");
    return fs::current_path();
}