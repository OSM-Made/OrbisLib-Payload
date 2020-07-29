#include "../Main.hpp"
#include "Utilites.hpp"

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

char* basename(const char *filename)
{
  char *p = strrchr(filename, '/');
  return p ? p + 1 : (char *)filename;
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