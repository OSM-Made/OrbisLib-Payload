#pragma once
#include "OrbisDef.hpp"

class OrbisTarget
{
private:


public:
    OrbisTarget();
    ~OrbisTarget();

    void Info(int Socket);
    void Shutdown(int Socket);
    void Reboot(int Socket);
    void Notify(int Socket, int Type, char* Message);
    void Beep(int Socket, int Count);
    void SetLED(int Socket);
    void GetLED(int Socket);
    void DumpProcess(int Socket, const char* ProcessName);

    void APIHandle(int Socket, API_Packet_s* Packet);
};
