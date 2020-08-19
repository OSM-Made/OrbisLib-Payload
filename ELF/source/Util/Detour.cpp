#include "../Main.hpp"
#include "Detour.hpp"

Detour::Detour(uint64_t FunctionPtr, uint64_t HookPtr, int32_t InstructionSize)
{
    if(FunctionPtr == NULL || HookPtr == NULL)
    {
        DebugLog(LOGTYPE_ERR, "FunctionPtr or HookPtr NULL (%llX %llX)", FunctionPtr, HookPtr);
        return;
    }

    if(InstructionSize < 14)
    {
        DebugLog(LOGTYPE_ERR, "Hooking Requires a minimum of 14 bytes to write jump!");
        return;
    }

    //Save Pointers for later
    this->FunctionPtr = FunctionPtr;
    this->HookPtr = HookPtr;
    
    //Save Instructions to restore later.
    /*this->RestoreInstructionsSize = InstructionSize;
    this->RestoreInstructions = (uint8_t*)_malloc(InstructionSize);
    if(this->RestoreInstructions == NULL)
    {
        DebugLog(LOGTYPE_ERR, "Failed to allocate space to store old instructions.");
        return;
    }

    //Clear memory and write our back up instructions.
    memset(this->RestoreInstructions, 0, InstructionSize);
    memcpy(this->RestoreInstructions, (void*)FunctionPtr, InstructionSize);*/

    uint8_t JumpInstructions[] = {
       0xFF, 0x25, 0x00, 0x00, 0x00, 0x00, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, // jmp QWORD PTR[Address]
    };

    //Write the address of our hook to the instruction.
    *(uint64_t*)(JumpInstructions + 6) = this->HookPtr;
    
    EnterCriticalSection();
    uint64_t CR0 = __readcr0();
 	__writecr0(CR0 & ~CR0_WP);

    //Write the jump instruction and a buffer nops as required.
    memcpy((void*)FunctionPtr, JumpInstructions, sizeof(JumpInstructions));
    DebugLog(LOGTYPE_INFO, "Bytes = %02X, %02X, %02X, %02X, %02X, %02X, %02X, %02X, %02X, %02X, %02X, %02X, %02X, %02X", JumpInstructions[0], JumpInstructions[1], JumpInstructions[2], JumpInstructions[3], JumpInstructions[4], JumpInstructions[5], JumpInstructions[6], JumpInstructions[7], JumpInstructions[8], JumpInstructions[9], JumpInstructions[10], JumpInstructions[11], JumpInstructions[12], JumpInstructions[13]);
    /*if(InstructionSize > 14)
        memset((void*)(FunctionPtr + 14), 0x90, InstructionSize - 14);*/

    __writecr0(CR0);
    ExitCriticalSection();

    //TODO: Allocate Space for a stub and write instructions to stub.

    DebugLog(LOGTYPE_INFO, "Detour(%llX %llX) Written Successfully!", FunctionPtr, this->HookPtr);
}

Detour::~Detour()
{
    EnterCriticalSection();
    uint64_t CR0 = __readcr0();
 	__writecr0(CR0 & ~CR0_WP);

    memcpy((void*)this->FunctionPtr, this->RestoreInstructions, this->RestoreInstructionsSize);

    __writecr0(CR0);
    ExitCriticalSection();

    //Clean up
    _free(this->RestoreInstructions);
    _free(this->StubPtr);

    DebugLog(LOGTYPE_INFO, "Detour(%llX) has been Removed Successfully!", this->FunctionPtr);
}