#include <sstream>
#include <QCoreApplication>
#include "PlayerData.hpp"

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
        return;

    if (value >= _byteLimit || value <=0 )
        value = _byteLimit;

    this->_ArmorData[num] = value;
}
u_int PlayerData::getArmorPiece(int num) const
{
    if (num > 4 || num < 0)
        num = Armor::HEAD;
    u_int value = this->_ArmorData[num];

    if (value >= _byteLimit)
        value = std::numeric_limits<u_int>::max();
    return value;
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

std::vector<std::string> PlayerData::getDataString() const
{
    std::vector<std::string> Result(5);
    for(int i=0;i<5;++i)
        Result[i] = (this->_ArmorData[i] == 255 ? "" : std::to_string(this->_ArmorData[i]) );
    return Result;
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
    byte* lpBuffer2 = nullptr;
    while (num1 < 140737488355323ULL)
    {
        Ptr = (LPVOID)num1;
        MEMORY_BASIC_INFORMATION lpBuffer1;
        VirtualQueryEx(Proc.getHanlder(), Ptr, &lpBuffer1, sizeof(MEMORY_BASIC_INFORMATION) );
        num1 += (long)lpBuffer1.RegionSize - 1;

        QCoreApplication::processEvents();

        if ( lpBuffer1.AllocationProtect == 64 || lpBuffer1.AllocationProtect == 4)
        {
            lpBuffer2 = Proc.ReadMemory(lpBuffer1.AllocationBase, lpBuffer1.RegionSize);
            if (!lpBuffer2)
                continue;
            for (int index = 0; index < (lpBuffer1.RegionSize - 3); ++index)
            {
                if (lpBuffer2[index] == (byte)231 && lpBuffer2[index + 1] == (byte)188)
                {
                    if (lpBuffer2[index + 2] != (byte)66)
                        ++index;
                    else if (lpBuffer2[index + 3] != (byte)1)
                        index += 2;
                    else
                    {
                        unsigned long long num2 = (DWORD_PTR)(lpBuffer1.AllocationBase) + (unsigned long long)index;
                        if ( ( (long)num2 & 4095L)  == 1705L )
                            return num2;
                    }
                }
            }
            delete lpBuffer2;
        }
    }
    return 0;
}