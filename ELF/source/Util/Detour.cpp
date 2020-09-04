#include "../Main.hpp"
#include "Detour.hpp"

void WriteJump(void* Address, void* Destination)
{
    uint8_t JumpInstructions[] = {
       0xFF, 0x25, 0x00, 0x00, 0x00, 0x00, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, // jmp QWORD PTR[Address]
    };

    //Write the address of our hook to the instruction.
    *(uint64_t*)(JumpInstructions + 6) = (uint64_t)Address;
    //uint64_t* jumpBufferAddress = (uint64_t*)(JumpInstructions + 6);
    //*jumpBufferAddress = (uint64_t)Address;

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
    this->FunctionPtr = FunctionPtr;
    this->HookPtr = HookPtr;

    /*this->RestoreInstructionsSize = InstructionSize;
    this->RestoreInstructions = (uint8_t*)_malloc(InstructionSize);
    if(this->RestoreInstructions == NULL)
    {
        DebugLog(LOGTYPE_ERR, "Failed to allocate space to store old instructions.");
        return (void*)0;
    }

    DebugLog(LOGTYPE_INFO, "Here");

    //Clear memory and write our back up instructions.
    memset(this->RestoreInstructions, 0, InstructionSize);
    memcpy(this->RestoreInstructions, (void*)FunctionPtr, InstructionSize);

    //Allocate Executable memory for stub and write instructions to stub and a jump back to original execution.
    this->StubSize = (InstructionSize + 14);
	this->StubPtr = (void*)kmmap(0, this->StubSize, 1 | 2 | 4, 0x1000 | 0x2, -1, 0);
    memcpy(StubPtr, (void*)FunctionPtr, InstructionSize);
    WriteJump((void*)(this->StubPtr + InstructionSize), HookPtr + InstructionSize);*/
    
    //Write jump from function to hook.
    WriteJump(FunctionPtr, HookPtr);

    DebugLog(LOGTYPE_INFO, "Detour(%llX %llX) Written Successfully!", FunctionPtr, this->HookPtr);

    return this->StubPtr;
}

void Detour::RestoreFunction()
{
    if(this->RestoreInstructions)
    {  
        EnterCriticalSection();
        uint64_t CR0 = __readcr0();
 	    __writecr0(CR0 & ~CR0_WP);

        memcpy((void*)this->FunctionPtr, this->RestoreInstructions, this->RestoreInstructionsSize);

        __writecr0(CR0);
        ExitCriticalSection();
    }
}

Detour::Detour(void* FunctionPtr, void* HookPtr, int32_t InstructionSize)
{
    DetourFunction(FunctionPtr, HookPtr, InstructionSize);
}

Detour::~Detour()
{
    RestoreFunction();

    //Clean up
    _free(this->RestoreInstructions);
    kmunmap((caddr_t)this->StubPtr, this->StubSize);

    DebugLog(LOGTYPE_INFO, "Detour(%llX) has been Removed Successfully!", this->FunctionPtr);
}