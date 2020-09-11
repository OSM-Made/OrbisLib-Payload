#pragma once

class Detour
{
private:
    
    void* StubPtr = 0;
    size_t StubSize = 0;
    void* FunctionPtr = 0;
    void* HookPtr = 0;

    uint8_t* RestoreInstructions = 0;
    int32_t RestoreInstructionsSize = 0;
public:
    void* DetourFunction(void* FunctionPtr, void* HookPtr, int32_t InstructionSize);
    void RestoreFunction();

    Detour(void* FunctionPtr, void* HookPtr, int32_t InstructionSize);
    ~Detour();
};

