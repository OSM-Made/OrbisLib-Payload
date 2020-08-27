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

unsigned int g_monthdays[13] = {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365};
unsigned int g_lpmonthdays[13] = {0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335, 366};

#define DAYSPERYEAR 365
#define DAYSPER4YEARS (4*DAYSPERYEAR+1)
#define DAYSPER100YEARS (25*DAYSPER4YEARS-1)
#define DAYSPER400YEARS (4*DAYSPER100YEARS+1)
#define SECONDSPERDAY (24*60*60)
#define SECONDSPERHOUR (60*60)
#define LEAPDAY 59

#define DIFFTIME 0x19db1ded53e8000ULL
#define DIFFDAYS (3 * DAYSPER100YEARS + 17 * DAYSPER4YEARS + 1 * DAYSPERYEAR)

static long leapyears_passed(long days)
{
	long quadcenturies, centuries, quadyears;
	quadcenturies = days / DAYSPER400YEARS;
	days -= quadcenturies;
	centuries = days / DAYSPER100YEARS;
	days += centuries;
	quadyears = days / DAYSPER4YEARS;
	return quadyears - centuries + quadcenturies;
}

static long leapdays_passed(long days)
{
	return leapyears_passed(days + DAYSPERYEAR - LEAPDAY + 1);
}

static long years_passed(long days)
{
	return (days - leapdays_passed(days)) / 365;
}

 struct tm * _gmtime_worker(struct tm *ptm, time_t time)
 {
     unsigned int days, daystoyear, dayinyear, leapdays, leapyears, years, month;
     unsigned int secondinday, secondinhour;
     unsigned int *padays;
 
     if (time < 0)
     {
         return 0;
     }
 
     /* Divide into date and time */
     days = (unsigned int)(time / SECONDSPERDAY);
     secondinday = time % SECONDSPERDAY;
 
     /* Shift to days from 1.1.1601 */
     days += DIFFDAYS;
 
     /* Calculate leap days passed till today */
     leapdays = leapdays_passed(days);
 
     /* Calculate number of full leap years passed */
     leapyears = leapyears_passed(days);
 
     /* Are more leap days passed than leap years? */
     if (leapdays > leapyears)
     {
         /* Yes, we're in a leap year */
         padays = g_lpmonthdays;
     }
     else
     {
         /* No, normal year */
         padays = g_monthdays;
     }
 
     /* Calculate year */
     years = (days - leapdays) / 365;
     ptm->tm_year = years - 299;
 
     /* Calculate number of days till 1.1. of this year */
     daystoyear = years * 365 + leapyears;
 
     /* Calculate the day in this year */
     dayinyear = days - daystoyear;
 
     /* Shall we do DST corrections? */
     ptm->tm_isdst = 0;
 
     ptm->tm_yday = dayinyear;
 
     /* dayinyear < 366 => terminates with i <= 11 */
     for (month = 0; dayinyear >= padays[month+1]; month++)
         ;
 
     /* Set month and day in month */
     ptm->tm_mon = month;
     ptm->tm_mday = 1 + dayinyear - padays[month];
 
     /* Get weekday */
     ptm->tm_wday = (days + 1) % 7;
 
     /* Calculate hour and second in hour */
     ptm->tm_hour = secondinday / SECONDSPERHOUR;
     secondinhour = secondinday % SECONDSPERHOUR;
 
     /* Calculate minute and second */
     ptm->tm_min = secondinhour / 60;
     ptm->tm_sec = secondinhour % 60;
 
     return ptm;
 }

struct tm *gmtime_s(const time_t *timep, struct tm *result)
{
	/* no thread safety. */
	result = _gmtime_worker(result, *timep);
	return result;
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