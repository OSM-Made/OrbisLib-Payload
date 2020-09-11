#include "../Main.hpp"
#include "Utilites.hpp"

int GetConsoleType(int byte)
{
	int ConsoleType = 0;
	switch(byte)
	{
		case 0x80:
			ConsoleType = DIAG;
			break;
		
		case 0x81:
			ConsoleType = DEVKIT;
			break;

		case 0x82:
			ConsoleType = TESTKIT;
			break;

		case 0x83 ... 0x8F:
			ConsoleType = RETAIL;
			break;

		case 0xA0:
			ConsoleType = KRATOS;
			break;
	}
	return ConsoleType;
}

char* strrchr(const char *cp, int ch)
{
    char *save;
    char c;

    for (save = (char *) 0; (c = *cp); cp++) {
	if (c == ch)
	    save = (char *) cp;
    }

    return save;
}

char* strchr(const char *s, int c)
{
  do {
    if (*s == c)
      {
        return (char*)s;
      }
  } while (*s++);
  return (0);
}

void SendStatus(int Socket, int Status)
{
	DebugLog(LOGTYPE_INFO, "API Status: %s", API_ERRORS_STR[Status]);
    Send(Socket, (char*)&Status, 0x4);
}

char* basename(const char *filename)
{
  char *p = strrchr(filename, '/');
  return p ? p + 1 : (char *)filename;
}

void HookFunctionCall(uint8_t* p_HookTrampoline, void* p_Function, void* p_Address)
{
    uint8_t* s_HookPayload = p_HookTrampoline;
    uint16_t* s_TempAddress = reinterpret_cast<uint16_t*>(p_HookTrampoline);
    s_TempAddress++;

    uint64_t* s_FunctionAddress = reinterpret_cast<uint64_t*>(s_TempAddress);

    uint64_t CR0 = __readcr0();
 	__writecr0(CR0 & ~CR0_WP);

    // mov rax
    s_HookPayload[0] = 0x48;
    s_HookPayload[1] = 0xB8;

    *s_FunctionAddress = reinterpret_cast<uint64_t>(p_Function);

    s_HookPayload[0x0A] = 0xFF;
    s_HookPayload[0x0B] = 0xE0;

    int32_t s_CallAddress = (int32_t)(p_HookTrampoline - (uint8_t*)p_Address) - 5;
    s_HookPayload = reinterpret_cast<uint8_t*>(p_Address);
    s_HookPayload++;
    int32_t* s_Pointer = reinterpret_cast<int32_t*>(s_HookPayload);
    *s_Pointer = s_CallAddress;

    __writecr0(CR0);
}

/*int Get_Module_Handle_For_Name(struct proc* proc, const char* Name)
{
	int ModuleHandle = -1;
	struct proc_ldt* m_library = proc->p_md->md_ldt;
	while(m_library != 0)
	{
		if(!strcmp(basename(m_library->LibraryPath), Name))
		{
			ModuleHandle = m_library->ModuleHandle;
			break;
		}

		m_library = m_library->md_ldt_next;
	}

	return ModuleHandle;
}

const char* Get_Module_Path(struct proc* proc, int hModule)
{
	struct proc_ldt* m_library = proc->p_md->md_ldt;
	while(m_library != 0)
	{
		if(m_library->ModuleHandle == hModule)
			return m_library->LibraryPath;
			
		m_library = m_library->md_ldt_next;
	}
	return "Error";
}*/