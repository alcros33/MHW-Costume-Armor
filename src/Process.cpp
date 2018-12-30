#include "Process.hpp"

/// Start Module Class Member Definitions

Module::Module(const HMODULE &HMod, const std::string &moduleName) : _hMod(HMod) , _Name(moduleName) {}

std::ostream &operator<<(std::ostream &out, Module &Mod)
{
    out << "Name : " << Mod.getName();
    out << "\tAt -> " << Mod.getBaseAddress();
    return out;
}

/// Start Process Class Member Definitions

Process::Process(const std::string &processName)
{
    PROCESSENTRY32 entry;
    entry.dwSize = sizeof(PROCESSENTRY32);

    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    if (Process32First(snapshot, &entry) == TRUE)
    {
        while (Process32Next(snapshot, &entry) == TRUE)
        {
            if (stricmp(entry.szExeFile, processName.c_str()) == 0)
            {
                this->_id = entry.th32ProcessID;
                this->_handleProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, entry.th32ProcessID);
            }
        }
    }
    CloseHandle(snapshot);
}

Process::Process(const DWORD64 &processID)
{
    this->_id = processID;
    this->_handleProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processID);
}

std::vector<Module> Process::getModuleList() const 
{
    HMODULE hMods[1024];
    DWORD cbNeeded;
    std::vector<Module> _Modules;

    if (!this->isOpen())
        return _Modules;

    if (EnumProcessModules(this->_handleProcess, hMods, sizeof(hMods), &cbNeeded))
    {
        for (int i = 0; i < (cbNeeded / sizeof(HMODULE)); ++i)
        {
            TCHAR Buffer[MAX_PATH];
            if (GetModuleBaseNameA(this->_handleProcess, hMods[i], Buffer, MAX_PATH)!= 0)
            {

                _Modules.emplace_back(hMods[i], std::string(Buffer));
            }
        }
    }
    return _Modules;
}

bool Process::isOpen() const
{
    if (!this->_id || !this->_handleProcess)
        return false;
    return true;
}

Process::~Process()
{
    CloseHandle(Process::_handleProcess);
}

// -------- READ THINGS ---------------

byte* Process::ReadMemory(DWORD64 address, int bytesToRead) const
{
    byte *buffer = new byte[bytesToRead];
    if (ReadProcessMemory(this->_handleProcess, (LPVOID)address, buffer, bytesToRead, nullptr))
        return buffer;
    delete[] buffer;
    return nullptr;
}

byte* Process::ReadMemory(LPVOID address, int bytesToRead) const 
{
    byte* buffer = new byte[bytesToRead] ;
    if (ReadProcessMemory(this->_handleProcess, address, buffer, bytesToRead, nullptr))
        return buffer;
    delete[] buffer;
    return nullptr;
}

int Process::ReadMemoryInt(DWORD64 address) const
{
    byte *buffer = this->ReadMemory( address, 4);
    if ( !buffer )
        return 0;
    int Val = BytesToInt(buffer);
    delete[] buffer;
    return Val;
}

// -------- Write THINGS ---------------

bool Process::WriteMemory(DWORD64 address, byte Buffer[], int bytesToWrite)
{
    return WriteProcessMemory(this->_handleProcess, (LPVOID)address, Buffer, bytesToWrite, nullptr);
}
bool Process::WriteMemoryInt(DWORD64 address, int value)
{
    return WriteProcessMemory(this->_handleProcess, (LPVOID)address, &value, 4, nullptr);
}
bool Process::WriteMemoryUInt(DWORD64 address, u_int value)
{
    return WriteProcessMemory(this->_handleProcess, (LPVOID)address, &value, 4, nullptr);
}

Module Process::getModuleByName(const std::string &ModuleName) const
{
    for (auto Mod : this->getModuleList())
        if (Mod.getName() == ModuleName)
            return Mod;
    return Module();
}

/// Start Misc Functions Definitions

int BytesToInt(const byte buffer[4] )
{
    return int(buffer[3] << 24 | buffer[2] << 16 | buffer[1] << 8 | buffer[0] );
}

byte* IntToBytes(const int val )
{
    byte* buffer = new byte[4];
    buffer[3] = val >> 24;
    buffer[2] = val >> 16;
    buffer[1] = val >> 8;
    buffer[0] = (byte)val;
    return buffer;
}


std::string GetRegKeyValue(HKEY RootKey, const std::string &SubKey, const std::string &Value)
{
    char value[255];
    DWORD BufferSize = 255;
    auto status = RegGetValue(RootKey, SubKey.c_str(), Value.c_str(), RRF_RT_REG_SZ, NULL, (PVOID)&value, &BufferSize);
    if (status != ERROR_SUCCESS)
    {
        DEBUG_LOG_HEX(ERROR,"Couldn't Get Registry value with error code : " << status);
        return std::string();
    }
    return std::string(value);
}