#include <iomanip>
#include <ctime>
#include <sstream>

#include "MHMemory.hpp"
INITIALIZE_EASYLOGGINGPP

/// Begin MH_Memory Member definitions
MH_Memory::MH_Memory()
{
    INIT_LOGGER(exeDir.absoluteFilePath("CostumeArmor.log").toStdString());
    this->exeDir.mkdir("Backups");
}

bool MH_Memory::openProcess()
{
    _MHProcess.open(this->procName);
    return _MHProcess.isOpen();
}

void MH_Memory::findSteamPath()
{
    if (!_MHProcess.isOpen())
        return;
    
    auto Mod = _MHProcess.getModuleByName(this->steamDLL);
    if (Mod.isEmpty())
    {
        LOG_ENTRY(ERROR, "Module " << this->steamDLL << " NOT FOUND");
        return;
    }

    this->_steamID = _MHProcess.readMemoryUInt(Mod.getBaseAddress() + 237848);
    auto path = get_reg_value(HKEY_CURRENT_USER, "Software\\Valve\\Steam", "SteamPath");
    this->_steamPath.setPath(QString::fromStdString(path));
    this->_steamPath.makeAbsolute();
    this->_steamPath.cd("userdata");
    this->_steamPath.cd(QString::number(_steamID));
    this->_steamPath.cd("582010");
    if (this->_steamPath.exists())
        this->_steamFound = true;
    else
    {
        this->_steamFound = false;
        LOG_ENTRY(ERROR, "Steam ID or Path NOT FOUND");
    }
    LOG_ENTRY(DEBUG, "Steam ID : " << _steamID);
    LOG_ENTRY(DEBUG, "Steam Path : " << _steamPath);
}

void MH_Memory::findDataAddress(QString selected_version)
{
    if (!_MHProcess.isOpen())
        return;

    if (MH_Memory::versions.find(selected_version) == MH_Memory::versions.end()) // Safety Check
        selected_version = "Latest";

    _dataPtr = find_data_address(_MHProcess, MH_Memory::versions[selected_version]);
    if (_dataPtr == 0)
        return;
    _dataPtr -= 29;
    _slotDist = MH_Memory::charSlotDist[selected_version];

    LOG_ENTRY_HEX(DEBUG,"Address of the Characters Data : "<< _dataPtr);
}

bool MH_Memory::validateProcess()
{
    if (_MHProcess.isOpen())
        return true;
    _MHProcess.close();
    _dataPtr = 0;
    return false;
}

bool MH_Memory::readArmor(int slot)
{
    if (!this->_MHProcess.isOpen() || !this->dataAddressFound())
        return false;
    
    byte *charDataBuffer = _MHProcess.readMemory(_dataPtr + _slotDist * slot, 20);

    if (charDataBuffer)
    {
        for (int i = 0; i < 5; ++i)
            this->_data[i] = BytesToUInt(charDataBuffer + (i * 4));
        return true;
    }

    delete[] charDataBuffer;
    return false;
}

bool MH_Memory::writeArmor(int char_slot, bool no_backup_ok)
{
    if (!_MHProcess.isOpen() || !this->dataAddressFound())
        return false;

    if (!this->backupSaveData())
    {
        LOG_ENTRY(ERROR, "Couldn't Backup the Save Data");
        if (!no_backup_ok)
        {
            LOG_ENTRY(WARNING, "Check \"Do not Backup Save Files\" to be able to write without backup");
            return false;
        }
    }

    bool status = true;
    for (int index = 0; index < 5; ++index)
    {
        status &= _MHProcess.writeMemoryUInt(_dataPtr + (index * 4) + _slotDist * char_slot,
                                             _data[index]);
    }

    std::stringstream S;
    for (const auto d : _data)
        S << d << ", ";
    LOG_ENTRY(DEBUG, "Written (" << S.str() << ") to slot " << char_slot+1);

    return status;
}

void MH_Memory::setSteamDirectory(const QDir &p)
{
    this->_steamPath = p;
    this->_steamFound = true;
    LOG_ENTRY(DEBUG, "Steam Path customly set to " << _steamPath);
}

void MH_Memory::unSetSteamDirectory()
{
    this->_steamPath.setPath("");
    this->_steamFound = false;
}

std::string get_date_time()
{
    std::time_t t = std::time(nullptr);

    std::stringstream Result;

    Result << std::put_time(std::localtime(&t), "%Y-%m-%d %H-%M-%S");

    return Result.str();
}

bool MH_Memory::backupSaveData() const
{
    if (!this->_steamFound)
    {
        LOG_ENTRY(ERROR,"Steam Dir was not Found");
        return false;
    }

    if (!this->backupDir.exists())
    {
        LOG_ENTRY(ERROR,"Backup Dir was not Found and couldn't created");
        return false;
    }

    auto sourcePath = this->_steamPath;
    sourcePath.cd("remote");

    if (!sourcePath.exists("SAVEDATA1000"))
    {
        LOG_ENTRY(ERROR,"Couldn't Find Save Data");
        return false;
    }
    QString saveFileName("Backup ");
    saveFileName.append(get_date_time().c_str());
    return QFile::copy(sourcePath.absoluteFilePath("SAVEDATA1000"),
                       this->backupDir.absoluteFilePath(saveFileName));
}

// ByteArray to search Given version
std::map<QString, SearchPattern> MH_Memory::versions{
    {"163956", {BytesToUInt({231, 188, 66, 1}), (0x68C) + 29}},
    {"165889", {BytesToUInt({174, 190, 66, 1}), (0x68C) + 29}},
    {"166849", {BytesToUInt({ 47, 192, 66, 1}), (0x68C) + 29}},
    {"167353", {BytesToUInt({ 55, 193, 66, 1}), (0x68C) + 29}},
    {"167541", {BytesToUInt({214, 199, 66, 1}), (0x6AC) + 29}},
    {"167796", {BytesToUInt({ 87, 200, 66, 1}), (0x6AC) + 29}},
    {"167898", {BytesToUInt({103, 200, 66, 1}), (0x6AC) + 29}},
    {"168030", {BytesToUInt({119, 200, 66, 1}), (0x6AC) + 29}},
    {"400974", {BytesToUInt({ 32, 246, 66, 1}), (0x4B4) + 29}},
    {"401727", {BytesToUInt({224, 241, 66, 1}), (0x4B4) + 29}},
    {"Latest", {BytesToUInt({224, 241, 66, 1}), (0x4B4) + 29}}};

// The distance between the address of the data of the character slots
std::map<QString, int> MH_Memory::charSlotDist{
    {"163956", 1285888},
    {"165889", 1285888},
    {"166849", 1285888},
    {"167353", 1285888},
    {"167541", 1285920},
    {"167796", 1285920},
    {"167898", 1285920},
    {"168030", 1285920},
    {"400974", 2615792},
    {"401727", 2615792},
    {"Latest", 2615792}};

/// End MH_Memory Member definitions

/// Begin Misc Functions
// Most important function.
// Searches the memory for a pattern (that changes each version)
DWORD64 find_data_address(Process &Proc, SearchPattern Pa)
{
    byte PatternBuffer[4];
    u_int lastBits = Pa.lastBits;
    u_int uintPattern = Pa.uintPattern;

    DWORD64 baseAddr = 0;
    byte *readBuffer = nullptr;
    MEMORY_BASIC_INFORMATION MemBuffer;

    while (baseAddr < 0x7fffffffffffLL)
    {
        if (VirtualQueryEx(Proc.getHanlder(), (LPCVOID)baseAddr, &MemBuffer, sizeof(MemBuffer)) == 0)
        {
            LOG_ENTRY(ERROR, "VirtualQueryEx Returned Error Code : " << std::to_string(GetLastError()));
            return 0;
        }

        baseAddr += (DWORD64)MemBuffer.RegionSize - 1; // Advance to next Memory Region

        if (MemBuffer.AllocationProtect == 64 || MemBuffer.AllocationProtect == 4)
        {
            readBuffer = Proc.readMemory(MemBuffer.AllocationBase, MemBuffer.RegionSize);
            if (!readBuffer)
                continue;

            // Get 12 least significant bits
            DWORD64 index = (DWORD_PTR)MemBuffer.AllocationBase & ((1 << 12) - 1);
            // Ensure Index + AllocationBase has the least significant bits we're searching for
            if (index < lastBits)
                index = lastBits - index;
            else
                index = ((1 << 12) - index) + lastBits;

            // On the for loop we only add ones at the 13th bit position
            // To ensure we keep least significant bits value
            for (; index <= (MemBuffer.RegionSize - 4); index += (1 << 12))
            {
                std::copy(readBuffer + index, readBuffer + (index + 4), PatternBuffer);
                if (uintPattern == BytesToUInt(PatternBuffer))
                {
                    return (DWORD_PTR)(MemBuffer.AllocationBase) + (DWORD64)index;
                }
            }
            delete[] readBuffer;
        }
    }
    return 0;
}