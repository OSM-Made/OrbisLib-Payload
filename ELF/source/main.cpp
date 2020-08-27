#include "Main.hpp"
#include "Util/Detour.hpp"
#include "Util/Kernel.hpp"
#include "Util/FileIO.hpp"

extern "C"
{
	#include "sys/mount.h"
}

OrbisLib* orbisLib = NULL;
HelperManager* pHelperManager = NULL;

extern "C" void _main(uint64_t* p)
{
	Install_Patches();
	ResolveFunctions();

	//Start up the main orbis lib API.
	orbisLib = new OrbisLib();

	//Start up the Helper Manager
	pHelperManager = new HelperManager();

	//Start up the DebugLogger
    StartDebugLogger();

	/*auto beepmeboss = (void(*)(char))resolve(0x1042E0);
	beepmeboss(2);*/

	/*struct
	{
		uint8_t unk; //0x00
		uint8_t unk1; //0x02
		uint8_t unk2; //0x04
		uint8_t unk3; //0x08
		uint8_t unk4; //0x0A
		uint8_t unk5; //0x0C
		uint8_t unk6; //0x0E
		uint8_t unk7; //0x10
		uint8_t unk8; //0x12
		uint8_t unk9; //0x14
		uint8_t unk10; //0x16
		uint8_t unk11; //0x18
	}somethingcolours;

	auto getledsboss = (void(*)(uint64_t*))resolve(0x104B20);
	getledsboss((uint64_t*)&somethingcolours);

	somethingcolours.unk = 0xFF;
	somethingcolours.unk1 = 0xFF;
	somethingcolours.unk2 = 0xFF;
	somethingcolours.unk3 = 0xFF;
	somethingcolours.unk4 = 0xFF;
	somethingcolours.unk5 = 0xFF;
	somethingcolours.unk6 = 0xFF;
	somethingcolours.unk7 = 0xFF;
	somethingcolours.unk8 = 0xFF;
	somethingcolours.unk9 = 0xFF;
	somethingcolours.unk10 = 0xFF;
	somethingcolours.unk11 = 0xFF;

	DebugLog(LOGTYPE_INFO, "unk = %llX", somethingcolours.unk);
	DebugLog(LOGTYPE_INFO, "unk1 = %llX", somethingcolours.unk1);
	DebugLog(LOGTYPE_INFO, "unk2 = %llX", somethingcolours.unk2);
	DebugLog(LOGTYPE_INFO, "unk3 = %llX", somethingcolours.unk3);
	DebugLog(LOGTYPE_INFO, "unk4 = %llX", somethingcolours.unk4);
	DebugLog(LOGTYPE_INFO, "unk5 = %llX", somethingcolours.unk5);
	DebugLog(LOGTYPE_INFO, "unk6 = %llX", somethingcolours.unk6);
	DebugLog(LOGTYPE_INFO, "unk7 = %llX", somethingcolours.unk7);
	DebugLog(LOGTYPE_INFO, "unk8 = %llX", somethingcolours.unk8);
	DebugLog(LOGTYPE_INFO, "unk9 = %llX", somethingcolours.unk9);
	DebugLog(LOGTYPE_INFO, "unk10 = %llX", somethingcolours.unk10);
	DebugLog(LOGTYPE_INFO, "unk11 = %llX", somethingcolours.unk11);

	auto setledsboss = (void(*)(uint64_t*))resolve(0x1049B0);
	setledsboss((uint64_t*)&somethingcolours);*/

	//TODO: Write Shell Code for ShellCore to do ftp and do notifies.
	
	Log("Hello from Kernel Land!!!");
}
