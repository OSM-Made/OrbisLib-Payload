#pragma once

#define offsetof(st, m) ((size_t)((char *)&((st *)(0))->m - (char *)0))
#define COUNT_OF(x) ((sizeof(x)/sizeof(0[x])) / ((size_t)(!(sizeof(x) % sizeof(0[x])))))

void HookFunctionCall(uint8_t* p_HookTrampoline, void* p_Function, void* p_Address);

int GetConsoleType(int byte);
char* strrchr(const char *cp, int ch);
char* strchr(const char *s, int c);
void SendStatus(int Socket, int Status);
char* basename(const char *filename);
int Get_Module_Handle_For_Name(struct proc* proc, const char* Name);
const char* Get_Module_Path(struct proc* proc, int hModule);