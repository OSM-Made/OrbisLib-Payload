#include "../Main.hpp"
#include "Detour.hpp"

void WriteJump(void* Address, void* Destination)
{
    uint8_t JumpInstructions[] = {
       0xFF, 0x25, 0x00, 0x00, 0x00, 0x00, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, // jmp QWORD PTR[Address]
    };

    //Write the address of our hook to the instruction.
    *(uint64_t*)(JumpInstructions + 6) = (uint64_t)Destination;

    EnterCriticalSection();
    uint64_t CR0 = __readcr0();
 	__writecr0(CR0 & ~CR0_WP);
    
    memcpy(Address, JumpInstructions, sizeof(JumpInstructions));

    __writecr0(CR0);
    ExitCriticalSection();
}

void* Detour::DetourFunction(void* FunctionPtr, void* HookPtr, int32_t InstructionSize)
{
    if(FunctionPtr == NULL || HookPtr == NULL)
    {
        DebugLog(LOGTYPE_ERR, "FunctionPtr or HookPtr NULL (%llX %llX)", FunctionPtr, HookPtr);
        return (void*)0;
    }

    if(InstructionSize < 14)
    {
        DebugLog(LOGTYPE_ERR, "Hooking Requires a minimum of 14 bytes to write jump!");
        return (void*)0;
    }

    //Save Pointers for later
    this->FunctionPtr = (void*)FunctionPtr;
    this->HookPtr = HookPtr;

    //Allocate Executable memory for stub and write instructions to stub and a jump back to original execution.
    this->StubSize = (InstructionSize + 14);
	this->StubPtr = (void*)kmmap(0, this->StubSize, VM_PROT_ALL, 0x1000 | 0x2, -1, 0);
    memcpy(StubPtr, (void*)FunctionPtr, InstructionSize);
    WriteJump((void*)(this->StubPtr + InstructionSize), (void*)(FunctionPtr + InstructionSize));

    //Write jump from function to hook.
    WriteJump((void*)FunctionPtr, HookPtr);

    DebugLog(LOGTYPE_INFO, "Detour(%llX %llX) Written Successfully!", FunctionPtr, this->HookPtr);

    return this->StubPtr;
}

void Detour::RestoreFunction()
{
    if(this->StubPtr)
    {  
        EnterCriticalSection();
        uint64_t CR0 = __readcr0();
 	    __writecr0(CR0 & ~CR0_WP);

        memcpy((void*)this->FunctionPtr, this->StubPtr, this->StubSize - 14);

        __writecr0(CR0);
        ExitCriticalSection();
    }
}

Detour::Detour()
{

}

Detour::~Detour()
{
    RestoreFunction();

    //Clean up
    kmunmap((caddr_t)this->StubPtr, this->StubSize);

    DebugLog(LOGTYPE_INFO, "Detour(%llX) has been Removed Successfully!", this->FunctionPtr);
}