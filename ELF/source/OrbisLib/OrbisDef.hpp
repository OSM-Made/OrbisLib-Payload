#pragma once

enum API_COMMANDS
{
    NULL_PACKET = 0,
    API_TEST_COMMS,
    
    /* ####### Proc functions ####### */
    API_PROC_GET_LIST,
    API_PROC_ATTACH,
    API_PROC_DETACH,
    API_PROC_GET_CURRENT,
    API_PROC_READ,
    API_PROC_WRITE,
    API_PROC_KILL,
    API_PROC_GET_INFO,
    API_PROC_LOAD_ELF,
    API_PROC_SIGNAL,
    API_PROC_CALL,

    /* Remote Library functions */
    API_PROC_LOAD_SPRX,
    API_PROC_UNLOAD_SPRX,
    API_PROC_RELOAD_SPRX_NAME,
    API_PROC_RELOAD_SPRX_HANDLE,
    API_PROC_MODULE_LIST,
    /* ############################## */
    

    /* ##### Debugger functions ##### */
    API_DBG_START, /* Debugger attach to target */
    API_DBG_STOP, /* Debugger detach from target */
    API_DBG_BREAK,
    API_DBG_RESUME,
    API_DBG_STEP,
    API_DBG_STEP_OVER,
    API_DBG_STEP_OUT,
    API_DBG_GET_REG,
    API_DBG_SET_REG,
    API_DBG_GET_FREG,
    API_DBG_SET_FREG,
    API_DBG_GET_DBGREG,
    API_DBG_SET_DBGREG,

    /* Thread Management */
    API_DBG_THREAD_LIST,
    API_DBG_THREAD_STOP,
    API_DBG_THREAD_RESUME,

    /* Breakpoint functions */
    API_DBG_BREAKPOINT_GETFREE,
    API_DBG_BREAKPOINT_SET,
    API_DBG_BREAKPOINT_UPDATE,
    API_DBG_BREAKPOINT_REMOVE,
    API_DBG_BREAKPOINT_GETINFO,
    API_DBG_BREAKPOINT_LIST,

    /* Watchpoint functions */
    API_DBG_WATCHPOINT_SET,
    API_DBG_WATCHPOINT_UPDATE,
    API_DBG_WATCHPOINT_REMOVE,
    API_DBG_WATCHPOINT_GETINFO,
    API_DBG_WATCHPOINT_LIST,
    /* ############################## */

    /* Kernel functions */
    API_KERN_BASE,
    API_KERN_READ,
    API_KERN_WRITE,
    
    /* Target functions */
    API_TARGET_INFO,
    API_TARGET_SHUTDOWN,
    API_TARGET_REBOOT,
    API_TARGET_NOTIFY,
    API_TARGET_BEEP,
    API_TARGET_SET_LED,
    API_TARGET_GET_LED,
    API_TARGET_DUMP_PROC,
    //API_TARGET_LOAD_VSH_MODULE

};

enum
{
	API_OK = 0,
	API_ERROR_NOT_CONNECTED,
	API_ERROR_NOT_REACHABLE,
	API_ERROR_NOT_ATTACHED,
	API_ERROR_LOST_PROC,

	API_ERROR_FAIL,
	API_ERROR_INVALID_ADDRESS,
};

struct API_Packet_s
{
    //int8_t PacketVersion;
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
        struct 
        {
            int MessageType;
            char Message[100];
        }Target_Notify;
        struct 
        {
           int Count;
        }Target_Beep;
        
    };
};

