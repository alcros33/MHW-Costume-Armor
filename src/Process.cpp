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
bool Process::open(const std::string &process_name)
{
    PROCESSENTRY32 entry;
    entry.dwSize = sizeof(PROCESSENTRY32);

    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (Process32First(snapshot, &entry) == TRUE)
    {
        while (Process32Next(snapshot, &entry) == TRUE)
        {
            if (std::string(entry.szExeFile) == process_name)
            {
                this->_id = entry.th32ProcessID;
                this->_processHandler = OpenProcess(PROCESS_ALL_ACCESS, FALSE, entry.th32ProcessID);
            }
        }
    }
    CloseHandle(snapshot);
    return this->isOpen();
}

bool Process::open(const DWORD64 &processID)
{
    this->_id = processID;
    this->_processHandler = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processID);
    return this->isOpen();
}

bool Process::isOpen() const
{
    if (!this->_id || !this->_processHandler)
        return false;
    DWORD exit_code;
    if (GetExitCodeProcess(this->_processHandler, &exit_code) == 0)
        return false;
    return exit_code == STILL_ACTIVE;
}

std::vector<Module> Process::getModuleList() const 
{
    HMODULE hMods[1024];
    DWORD cbNeeded;
    std::vector<Module> _Modules;

    if (!this->isOpen())
        return _Modules;

    if (EnumProcessModules(this->_processHandler, hMods, sizeof(hMods), &cbNeeded))
    {
        for (int i = 0; i < (cbNeeded / sizeof(HMODULE)); ++i)
        {
            char Buffer[MAX_PATH];
            if (GetModuleBaseNameA(this->_processHandler, hMods[i], Buffer, MAX_PATH)!= 0)
            {

                _Modules.emplace_back(hMods[i], std::string(Buffer));
            }
        }
    }
    return _Modules;
}

void Process::close()
{
    CloseHandle(this->_processHandler);
    this->_processHandler = 0;
}

Process::Process() {}
Process::~Process()
{
    this->close();
}

// -------- READ THINGS ---------------
byte* Process::readMemory(LPVOID address, int num_bytes) const 
{
    byte* buffer = new byte[num_bytes] ;
    if (ReadProcessMemory(this->_processHandler, address, buffer, num_bytes, nullptr))
        return buffer;
    delete[] buffer;
    return nullptr;
}
byte* Process::readMemory(DWORD64 address, int num_bytes) const
{
   return this->readMemory((LPVOID) address, num_bytes);
}
u_int Process::readMemoryUInt(DWORD64 address) const
{
    u_int value;
    if (ReadProcessMemory(this->_processHandler, (LPVOID)address, &value, 4, nullptr))
        return value;
    return 0;
}

// -------- WRITE THINGS ---------------
bool Process::writeMemory(LPVOID address, byte buffer[], int num_bytes)
{
    return WriteProcessMemory(this->_processHandler, address, buffer, num_bytes, nullptr);
}
bool Process::writeMemory(DWORD64 address, byte buffer[], int num_bytes)
{
    return this->writeMemory((LPVOID)address, buffer, num_bytes);
}
bool Process::writeMemoryUInt(DWORD64 address, u_int value)
{
    return WriteProcessMemory(this->_processHandler, (LPVOID)address, &value, 4, nullptr);
}

Module Process::getModuleByName(const std::string &module_name) const
{
    for (auto Mod : this->getModuleList())
        if (Mod.getName() == module_name)
            return Mod;
    return Module();
}

/// Start Misc Functions Definitions
u_int BytesToUInt(const byte buffer[4])
{
    return u_int(buffer[3] << 24 | buffer[2] << 16 | buffer[1] << 8 | buffer[0]);
}

u_int BytesToUInt(const std::array<byte,4> &buffer)
{
    return u_int(buffer[3] << 24 | buffer[2] << 16 | buffer[1] << 8 | buffer[0]);
}

std::string get_reg_value(HKEY root_key, const std::string &sub_key, const std::string &Value)
{
    char result[255];
    DWORD BufferSize = 255;
    auto status = RegGetValueA(root_key, sub_key.c_str(), Value.c_str(), RRF_RT_REG_SZ, NULL,
                               (PVOID)&result, &BufferSize);
    if (status != ERROR_SUCCESS)
    {
        LOG_ENTRY(ERROR,"Couldn't Get Registry value with error code : " << status);
        return std::string();
    }
    return std::string(result);
}

std::wstring to_wstring(const std::string &str)
{
    int size = MultiByteToWideChar(CP_UTF8, 0, &str[0], (u_int)str.size(), NULL, 0);
    std::wstring result(size, 0);
    MultiByteToWideChar(CP_UTF8, 0, &str[0], (u_int)str.size(), &result[0], size);
    return result;
}