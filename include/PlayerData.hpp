#pragma once
#include "Process.hpp"
#include <climits>
#include <array>

namespace Armor
{
    const int HEAD = 0;
    const int BODY = 1;
    const int ARMS = 2;
    const int WAIST= 3;
    const int LEGS = 4;
    const std::array<std::string,5> Names{"Head","Body", "Arms","Waist","Legs"};
}

struct SearchPattern
{
    int int_pattern;
    int last_bits;
};

class PlayerData
{
public:
    PlayerData(byte *CharDataBuffer, bool gender);
    PlayerData(){}

    bool isEmpty() const { return _empty; }

    std::string getGender() const { return _Gender ? "Female" :  "Male"; }
    void setArmorPiece(int num, int value);
    u_int getArmorPiece(int num) const;

    std::string print() const;
    std::array<std::string,5> getDataString() const;
    std::array<byte,5> getData() const { return _ArmorData; }

private:
    std::array<byte,5> _ArmorData = {255,255,255,255,255};
    bool _Gender; // true is female
    bool _empty = true;
};

std::ostream &operator<<(std::ostream &out, PlayerData &Play);

DWORD64 FindDataAddress(Process &Proc, SearchPattern Pa);

PlayerData GetCharData(Process &Proc, DWORD64 charDataAddr, int slot);