#pragma once

class OrbisLib
{
private:
    bool IsRunning = false;
    proc* kOrbisProc = 0;
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

        API_PROC_GET_LIST,
        API_PROC_ATTACH,
        API_PROC_DETACH,
        API_PROC_READ,
        API_PROC_WRITE,
        

    };

    struct API_Packet_s
    {
        API_COMMANDS cmd;
        
    };

    static void OrbisLibClientThread(void* arg);
    static void OrbisLibProcThread(void *arg);

public:
    OrbisLib();
    ~OrbisLib();

};
