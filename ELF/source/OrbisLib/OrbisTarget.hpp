#pragma once
#include "OrbisDef.hpp"

enum ConsoleTypes
{
    UNK,
    DIAG, //0x80
    DEVKIT, //0x81
    TESTKIT, //0x82
    RETAIL, //0x83 -> 0x8F
    KRATOS, //0xA0 IMPOSSIBLE??
};

struct RESP_TargetInfo
{
    int32_t SDKVersion;
    int32_t SoftwareVersion;
    int32_t CPUTemp[8];
    char CurrentTitleID[10];
    char ConsoleName[100];
    char IDPS[16];
    char PSID[16];
    int32_t ConsoleType;
};

class OrbisTarget
{
private:
    /* data */
public:
    OrbisTarget();
    ~OrbisTarget();

    void Info(int Socket);
    void Shutdown(int Socket);
    void Reboot(int Socket);
    void Notify(int Socket, int Type, const char* Message);
    void Beep(int Socket, int Count);
    void SetLED(int Socket);
    void GetLED(int Socket);
    void DumpProcess(int Socket, const char* ProcessName);

    void APIHandle(int Socket, API_Packet_s* Packet);
};
