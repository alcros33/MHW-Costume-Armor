#pragma once
#include "Process.hpp"

namespace Armor
{
    const int HEAD = 0;
    const int BODY = 1;
    const int ARMS = 2;
    const int WAIST= 3;
    const int LEGS = 4;
    const std::vector<std::string> Names{"Head ","Body ", "Arms ","Waist","Legs "};
}

class PlayerData
{
public:
    PlayerData(byte *CharDataBuffer, bool gender);
    PlayerData(){}

    bool isEmpty() const { return _empty; }

    std::string getGender() const { return _Gender ? "Female" :  "Male"; }
    void setArmorPiece(int num, int value);
    int getArmorPiece(int num) const;

    std::string Print() const;
    std::vector<std::string> getDataString() const;

private:
    byte _ArmorData[5] = {255,255,255,255,255};
    bool _Gender; // true is female
    bool _empty = true;
};

std::ostream &operator<<(std::ostream &out, PlayerData &Play);

DWORD FindDataAddress(Process &Proc);

PlayerData GetCharData(Process &Proc, DWORD charDataAddr, int slot);