#include "PlayerData.hpp"
#include <sstream>

/// Begin PlayerData Class Member Functions

PlayerData::PlayerData(byte *CharDataBuffer, bool gender)
{
    for (int i=0;i<5;++i)
        this->_ArmorData[i] = CharDataBuffer[i*4 ];
    this->_empty = false;
}

void PlayerData::setArmorPiece(int num, int value)
{
    if (num > 4 || num < 0)
        num = Armor::HEAD;
    if (value >255 || value < 0)
        value = 255;
    this->_ArmorData[num] = value;
}
int PlayerData::getArmorPiece(int num) const
{
    if (num > 4 || num < 0)
        num = Armor::HEAD;
    return this->_ArmorData[num];
}

std::string PlayerData::Print() const
{
    std::stringstream Base;
    Base << "Character Data Info : " << std::endl;
    Base << "Gender : " << (this->_Gender ? "Female" : "Male") << std::endl;
    for(int i=0;i<5;++i)
    {
        Base << std::to_string(i) << ". ";
        Base << Armor::Names[i] <<"\t";
        Base << (this->_ArmorData[i] ==255 ? "None" : std::to_string(this->_ArmorData[i]));
        Base << std::endl;
    }
    return Base.str();
}

std::ostream &operator<<(std::ostream &out, PlayerData &Play)
{
    out<< Play.Print();
    return out;
}

/// Begin Misc Functions

DWORD FindDataAddress(Process &Proc)
{
    unsigned long long num1 = 0;
    LPVOID Ptr ;
    while (num1 < 140737488355323ULL)
    {
        Ptr = (LPVOID)num1;
        MEMORY_BASIC_INFORMATION lpBuffer1 ;
        VirtualQueryEx(Proc.getHanlder(), Ptr, &lpBuffer1, sizeof(MEMORY_BASIC_INFORMATION) );
        num1 += (long)lpBuffer1.RegionSize - 1;
        if ( lpBuffer1.AllocationProtect == 64 || lpBuffer1.AllocationProtect == 4)
        {
            byte *lpBuffer2 = Proc.ReadMemory(lpBuffer1.AllocationBase, lpBuffer1.RegionSize);
            if (lpBuffer2 == nullptr)
                continue;
            for (int index = 0; index < (lpBuffer1.RegionSize - 3); ++index)
            {
                if (lpBuffer2[index] == (byte)143 && lpBuffer2[index + 1] == (byte)187)
                {
                    if (lpBuffer2[index + 2] != (byte)66)
                        ++index;
                    else if (lpBuffer2[index + 3] != (byte)1)
                        index += 2;
                    else
                    {
                        unsigned long long num2 = (DWORD_PTR)(lpBuffer1.AllocationBase) + (unsigned long long)index;
                        if ( ( (long)num2 & 4095L)  == 1705L )
                        {
                            return num2;
                        }
                    }
                }
            }
        }
    }
    return 0;
}

