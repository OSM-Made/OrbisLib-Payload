#pragma once

class Detour
{
private:
    
    uint8_t* StubPtr = 0;
    uint64_t FunctionPtr = 0;
    uint64_t HookPtr = 0;

    uint8_t* RestoreInstructions = 0;
    int32_t RestoreInstructionsSize = 0;
public:
    Detour(uint64_t FunctionPtr, uint64_t HookPtr, int32_t InstructionSize);
    ~Detour();
};

