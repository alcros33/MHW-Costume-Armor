#include <iomanip>
#include <ctime>
#include <sstream>

#include "MHMemory.hpp"
INITIALIZE_EASYLOGGINGPP

/// Begin MH_Memory Member definitions
MH_Memory::MH_Memory(const std::string &ProcName, const std::string &SteamDLL) : _MHProcess(ProcName)
{
    INIT_LOGGER( logPath.str() );

    if (!_MHProcess.isOpen())
        return;

    this->_slotDist = MH_Memory::CharSlotDist["Latest"];
    auto Mod = _MHProcess.getModuleByName(SteamDLL);
    if (!Mod.isEmpty())
    {
        _steamID = _MHProcess.readMemoryInt(Mod.getBaseAddress() + 237592);
        _steamPath = GetRegKeyValue(HKEY_CURRENT_USER, "Software\\Valve\\Steam", "SteamPath");
        if (_steamID != 0 && !_steamPath.empty() )
        {
            _steamPath /= "userdata";
            _steamPath /= std::to_string(_steamID);
            _steamPath /= "582010";
            _steamFound = true;
        }
        else
        {
            DEBUG_LOG(ERROR,"Steam ID or Path NOT FOUND");
            DEBUG_LOG_HEX(DEBUG,"Current Steam Addr : "<< ( (long) Mod.getBaseAddress() + 237592) );
            DEBUG_LOG(DEBUG,"Current Steam ID : " << _steamID );
            DEBUG_LOG(DEBUG,"Current Steam Path : " << _steamPath);
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

void MH_Memory::findAddress(std::string Ver)
{
    if (!this->processOpen())
        return;
    
    if (MH_Memory::Versions.find(Ver) == MH_Memory::Versions.end()) // Safety Check
        Ver = "Latest";

    _dataPtr = FindDataAddress(_MHProcess, MH_Memory::Versions[Ver]);
    if (_dataPtr == 0)
        return;
    _dataPtr -= 29;
    _slotDist = MH_Memory::CharSlotDist[Ver];

    DEBUG_LOG_HEX(DEBUG,"Address of the Characters Data : "<< _dataPtr);
}

bool MH_Memory::fetchPlayerData(int slot)
{
    if ( !this->processOpen() || !this->dataAddressFound() )
        return false;

    if (slot < 0)
        slot = 0;
    if (slot > 2)
        slot = 2;

    byte *CharDataBuffer = _MHProcess.readMemory(_dataPtr + _slotDist * slot, 28);
    byte *lpBuffer = _MHProcess.readMemory(_dataPtr + _slotDist * slot - 394460, 1);

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

void MH_Memory::setSteamDirectory(const fs::Path &p)
{
    this->_steamPath = p;
    _steamPath /= "userdata";
    _steamPath /= std::to_string(_steamID);
    _steamPath /= "582010";
    _steamFound = true;
    DEBUG_LOG(DEBUG, "Steam Path customly set to "<<_steamPath);
}

std::string GetDateTime()
{
    std::time_t t = std::time(nullptr);

    std::stringstream Result;

    Result << std::put_time(std::localtime(&t), "%Y-%m-%d %H-%M-%S");

    return Result.str();
}

bool MH_Memory::backupSaveData() const
{
    if (!this->steamFound())
    {
        DEBUG_LOG(ERROR,"Steam Dir was not Found");
        return false;
    }

    if (!MH_Memory::backupDir.exists())
        if (!fs::create_directory(MH_Memory::backupDir))
        {
            DEBUG_LOG(ERROR,"Backup Dir was not Found and couldn't created");
            return false;
        }

    auto SourcePath = this->_steamPath;
    SourcePath /= "remote";
    SourcePath /= "SAVEDATA1000";

    if (!SourcePath.exists())
    {
        DEBUG_LOG(ERROR,"Couldn't Find Save Data");
        return false;
    }

    auto DestPath = MH_Memory::backupDir;
    DestPath /= ("Backup " + GetDateTime());
    try
    {
        fs::copy_file(SourcePath, DestPath);
    }
    catch (fs::FilesystemError &e)
    {
        DEBUG_LOG(ERROR,"Couldn't Copy Save data. Error : " << e.what() );
        return false;
    }

    return DestPath.exists();
}

bool MH_Memory::writeArmor(int CharSlot, bool isSafe)
{
    if ( !this->processOpen() || !this->dataAddressFound() )
        return false;
    if (!this->backupSaveData())
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
        Status &= _MHProcess.writeMemoryUInt(_dataPtr + index * 4 + _slotDist * CharSlot,
                                            _Data.getArmorPiece(index));
    }
    
    std::stringstream S;
    for( const auto d : _Data.getData() )
        S << ( (int)d ) << ", ";
    DEBUG_LOG(DEBUG,"Written (" << S.str()<<") to slot " << CharSlot);

    return Status;
}

// ByteArray to search Given version
std::map<std::string, SearchPattern> MH_Memory::Versions{
    {"163956", {BytesToInt({231, 188, 66, 1}), (0x068C) + 29}},
    {"165889", {BytesToInt({174, 190, 66, 1}), (0x068C) + 29}},
    {"166849", {BytesToInt({ 47, 192, 66, 1}), (0x068C) + 29}},
    {"167353", {BytesToInt({ 55, 193, 66, 1}), (0x068C) + 29}},
    {"167541", {BytesToInt({214, 199, 66, 1}), (0x06AC) + 29}},
    {"167796", {BytesToInt({ 87, 200, 66, 1}), (0x06AC) + 29}},
    {"167898", {BytesToInt({103, 200, 66, 1}), (0x06AC) + 29}},
    {"168030", {BytesToInt({119, 200, 66, 1}), (0x06AC) + 29}},
    {"Latest", {BytesToInt({119, 200, 66, 1}), (0x06AC) + 29}}};

// The distance between the address of the data of the character slots
std::map<std::string, int> MH_Memory::CharSlotDist{
    {"163956", 1285888},
    {"165889", 1285888},
    {"166849", 1285888},
    {"167353", 1285888},
    {"167541", 1285920},
    {"167796", 1285920},
    {"167898", 1285920},
    {"168030", 1285920},
    {"Latest", 1285920}};

/// End MH_Memory Member definitions
fs::Path CurrentExecutableDir()
{
    wchar_t Buffer[1024];
    if (GetModuleFileNameW(nullptr, Buffer, 1024) != 0)
    {
        fs::Path Path(Buffer);
        return Path.parent_path();
    }
    DEBUG_LOG(ERROR, "Couldn't retrieve the name of current EXE File.");
    return fs::current_path();
}
