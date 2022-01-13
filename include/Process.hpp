#pragma once
#include <string>
#include <memory>
#include <vector>
#include <iostream>
#include <windows.h>
#include <tlhelp32.h>
#include <psapi.h>

#include "logger.hpp"

using byte = unsigned char;

class Module
{
public:
    Module(const HMODULE &HMod, const std::string &moduleName);
    Module(){}

    std::string getName() const { return _Name; }
    DWORD_PTR getBaseAddress() const { return (DWORD_PTR)_hMod; }
    HMODULE getModuleHandler() const { return _hMod; }

    bool isEmpty() const { return _Name.empty(); }

private:
    HMODULE _hMod;
    std::string _Name;
};

std::ostream &operator<<(std::ostream &out, Module &Mod);

class Process
{
public:
    Process();
    ~Process();
    bool open(const std::string &process_name);
    bool open(const DWORD64 &processID);
    bool isOpen() const;

    void close();

    DWORD64 getID() const { return this->_id; }
    HANDLE getHanlder() const { return this->_processHandler; }

    byte* readMemory(DWORD64 address, int num_bytes) const;
    byte* readMemory(LPVOID address, int num_bytes) const;
    uint readMemoryUInt(DWORD64 address) const;

    bool writeMemory(DWORD64 address, byte buffer[], int num_bytes);
    bool writeMemory(LPVOID address, byte buffer[], int num_bytes);
    bool writeMemoryUInt(DWORD64 address, uint value);

    std::vector<Module> getModuleList() const;
    Module getModuleByName(const std::string &ModuleName) const;

private:
    HANDLE _processHandler = 0;
    DWORD64 _id = 0;
};

///

std::wstring to_wstring(const std::string &str);
std::string get_reg_value(HKEY root_key, const std::string &sub_key, const std::string &value);

uint BytesToUInt(const byte[4]);
uint BytesToUInt(const std::array<byte,4> &buffer);