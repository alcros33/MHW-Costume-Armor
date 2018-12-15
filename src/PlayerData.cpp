#include "PlayerData.hpp"

PlayerData::PlayerData(int Head, int Body, int Arms, int Waist, int Legs, bool gender) : _Head(Head), _Body(Body), _Arms(Arms), _Waist(Waist), _Legs(Legs), _Gender(gender) {}

///

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

PlayerData GetCharData(Process &Proc, DWORD charDataAddr, int slot)
{
    if (slot < 1)
        slot = 1;
    if (slot > 3)
        slot = 3;
    slot -= 1;
    byte *CharDataBuffer = Proc.ReadMemory(charDataAddr + 1285888*slot, 28);
    byte *lpBuffer = Proc.ReadMemory(charDataAddr + 1285888 * slot - 394460, 1);
    
    if (!CharDataBuffer || !lpBuffer)
        return PlayerData();

    return PlayerData(CharDataBuffer[0], CharDataBuffer[4], CharDataBuffer[8], CharDataBuffer[12], CharDataBuffer[16], lpBuffer[0]);
}

