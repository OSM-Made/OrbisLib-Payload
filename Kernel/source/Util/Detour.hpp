#pragma once

class Detour
{
private:
    
    void* StubPtr = 0;
    size_t StubSize = 0;
    void* FunctionPtr = 0;
    void* HookPtr = 0;

public:
    template <typename result, typename... Args>
    result Stub(Args... args)
    {
        result(*Stub_internal)(Args... args) = decltype(Stub_internal)(StubPtr);
        return Stub_internal(args...);
    }
    
    void* DetourFunction(void* FunctionPtr, void* HookPtr, int32_t InstructionSize);
    void RestoreFunction();

    Detour(/*void* FunctionPtr, void* HookPtr, int32_t InstructionSize*/);
    ~Detour();
};

