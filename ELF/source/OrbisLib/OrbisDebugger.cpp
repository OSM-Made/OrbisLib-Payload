#include "../Main.hpp"
#include "OrbisDebugger.hpp"

struct trapframe_s {
	register_t	tf_rdi;
	register_t	tf_rsi;
	register_t	tf_rdx;
	register_t	tf_rflags;
	register_t	tf_rsp;
	register_t	tf_ss;
};

void OnTrapFatalHook(trapframe_s *frame)
{
	DebugLog(LOGTYPE_INFO, "Trap Fatal Hit!");

    for(;;){}
}

OrbisDebugger::OrbisDebugger(OrbisProc* orbisProc)
{
    DebugLog(LOGTYPE_INFO, "Initialization!!");

    this->orbisProc = orbisProc;

    //Initialize Breakpoints
    for(int i = 0; i < BREAKPOINTS_MAX; i++)
        this->Breakpoints[i] = new OrbisBreakPoint();

    //Initialize Watchpoints
    for(int i = 0; i < WATCHPOINTS_MAX; i++)
        this->Watchpoints[i] = new OrbisWatchPoint();

    Detour* OnTrapFatalDetour = new Detour((void*)resolve(addr_trap_fatalHook), (void*)OnTrapFatalHook, 17);
}

OrbisDebugger::~OrbisDebugger()
{
    DebugLog(LOGTYPE_INFO, "Destruction!!");

    //Free the Breakpoint Classes
    for(int i = 0; i < BREAKPOINTS_MAX; i++)
        delete this->Breakpoints[i];

    //Free the Watchpoint Classes
    for(int i = 0; i < WATCHPOINTS_MAX; i++)
        delete this->Watchpoints[i];
}

void OrbisDebugger::APIHandle(int Socket, API_Packet_s* Packet)
{
	switch(Packet->cmd)
	{
        
    }
}