#pragma once
#include "Process.hpp"

class PlayerData
{
public:
    PlayerData(int Head, int Body, int Arms, int Waist, int Legs, bool gender);
    PlayerData(){}
    bool isEmpty() const { return _Head == -1; }
    std::string getGender() const { return _Gender ? "Female" :  "Male"; }

    int getHead () const { return _Head ;}
    int getBody () const { return _Body ;}
    int getArms () const { return _Arms ;}
    int getWaist() const { return _Waist;}
    int getLegs () const { return _Legs ;}
    
    void setHead (int val) { this->_Head =val;}
    void setBody (int val) { this->_Body =val;}
    void setArms (int val) { this->_Arms =val;}
    void setWaist(int val) { this->_Waist=val;}
    void setLegs (int val) { this->_Legs =val;}

private:
    int _Head  = -1;
    int _Body  = -1;
    int _Arms  = -1;
    int _Waist = -1;
    int _Legs  = -1;
    bool _Gender; // true is female
};

DWORD FindDataAddress(Process &Proc);

PlayerData GetCharData(Process &Proc, DWORD charDataAddr, int slot);