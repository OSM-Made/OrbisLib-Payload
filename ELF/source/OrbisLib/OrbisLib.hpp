#pragma once

#include "OrbisProc.hpp"

class OrbisLib
{
private:
    bool IsRunning = false;
    OrbisProc* orbisProc;
    int LastHostIPAddr;

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
        API_PROC_KILL,

        /* Breakpoint functions */
        API_BREAKPOINT_GETFREE,
        API_BREAKPOINT_SET,
        API_BREAKPOINT_UPDATE,
        API_BREAKPOINT_REMOVE,
        API_BREAKPOINT_GETINFO,
        API_BREAKPOINT_LIST,

        /* Watchpoint functions */
        API_WATCHPOINT_SET,
        API_WATCHPOINT_UPDATE,
        API_WATCHPOINT_REMOVE,
        API_WATCHPOINT_GETINFO,
        API_WATCHPOINT_LIST,
        
        /* Remote Library functions */
        API_PROC_LOAD_SPRX,
        API_PROC_UNLOAD_SPRX,
        API_PROC_RELOAD_SPRX_NAME,
        API_PROC_RELOAD_SPRX_HANDLE,
        API_PROC_MODULE_LIST,
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
            struct
            {
                char ModuleDir[0x100];
                int hModule;
                int Flags;
            }PROC_SPRX;
            struct
            {
                int32_t Index;
                uint64_t Address;
                bool Enable;

            }Breakpoint;
            
        };
    };

    static void ClientThread(void* arg);
    static void ProcThread(void *arg);

public:
    proc* kOrbisProc = 0;
    int HostIPAddr;

    OrbisLib();
    ~OrbisLib();
};
