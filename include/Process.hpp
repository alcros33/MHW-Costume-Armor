#pragma once
#include <string>
#include <windows.h>
#include <tlhelp32.h>
#include <psapi.h>
#include <vector>
#include <iostream>

using byte = unsigned char;

int BytesToInt(const byte[4] );

class Module
{
public:
    Module(const HMODULE &HMod, const std::string &moduleName);
    Module(){}

    std::string getName() const { return _Name; }
    DWORD_PTR getBaseAddress() const { return (DWORD_PTR)_hMod; }
    HMODULE getModuleHandler() const { return _hMod; }

private:
    HMODULE _hMod;
    std::string _Name;
};

std::ostream &operator<<(std::ostream &out, Module &Mod);

class Process
{
public:
    Process(const std::string &processName);
    Process(const DWORD &processID);
    ~Process();

    byte* ReadMemory(DWORD address, int bytesToRead) const;
    byte* ReadMemory(LPVOID address, int bytesToRead) const;
    int ReadMemoryInt(DWORD address) const;

    std::vector<Module> getModuleList() const;
    DWORD getID() const { return this->_id; }
    HANDLE getHanlder() const { return this->_handleProcess; }
    bool isOpen() const;

private:
    HANDLE _handleProcess = 0;
    DWORD _id = 0;
};

