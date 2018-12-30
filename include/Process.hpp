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
    Process(const std::string &processName);
    Process(const DWORD64 &processID);
    ~Process();

    byte* ReadMemory(DWORD64 address, int bytesToRead) const;
    byte* ReadMemory(LPVOID address, int bytesToRead) const;
    int ReadMemoryInt(DWORD64 address) const;

    bool WriteMemory(DWORD64 address, byte Buffer[], int bytesToWrite);
    bool WriteMemoryInt(DWORD64 address, int value);
    bool WriteMemoryUInt(DWORD64 address, u_int value);

    std::vector<Module> getModuleList() const;
    Module getModuleByName(const std::string &ModuleName) const;
    DWORD64 getID() const { return this->_id; }
    HANDLE getHanlder() const { return this->_handleProcess; }
    bool isOpen() const;

private:
    HANDLE _handleProcess = 0;
    DWORD64 _id = 0;
};

///

std::string GetRegKeyValue(HKEY RootKey,const std::string &SubKey,const std::string &Value);

int BytesToInt(const byte[4]);
byte *IntToBytes(const int val);