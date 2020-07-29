#pragma once

#include "OrbisProc.hpp"

class OrbisLib
{
private:
    bool IsRunning = false;
    proc* kOrbisProc = 0;
    OrbisProc* orbisProc;
    int HostIPAddr;

    struct ClientThreadArgs
    {
        int Socket;
        OrbisLib* orbisLib;
    };

    enum API_COMMANDS
    {
        NULL_PACKET = 0,
        API_TEST_COMMS,
        
        /* Proc functions */
        API_PROC_GET_LIST,
        API_PROC_ATTACH,
        API_PROC_DETACH,
        API_PROC_GET_CURRENT,
        API_PROC_READ,
        API_PROC_WRITE,
        
        /* Remote Library functions */
        API_PROC_LOAD_SPRX,
        API_PROC_UNLOAD_SPRX,
    };

    struct API_Packet_s
    {
        API_COMMANDS cmd;
        char ProcName[0x20];
        union 
        {
            struct
            {
               uint64_t Address;
               size_t len;
            }PROC_RW;
            
        };
        
    };

    static void OrbisLibClientThread(void* arg);
    static void OrbisLibProcThread(void *arg);

public:
    OrbisLib();
    ~OrbisLib();
};
