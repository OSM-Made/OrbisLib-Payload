#include "../Main.hpp"
#include "OrbisDef.hpp"
#include "OrbisTarget.hpp"

OrbisTarget::OrbisTarget()
{
	DebugLog(LOGTYPE_INFO, "Initialization!!");

}

OrbisTarget::~OrbisTarget()
{
	DebugLog(LOGTYPE_INFO, "Destruction!!");
}

void OrbisTarget::Info(int Socket)
{
    RESP_TargetInfo TargetInfo;

    //kern.sdk_version
    //machdep.upd_version
    //machdep.idps
    //machdep.openpsid
    //machdep.openpsid_for_sys
	
	Log("[CONSOLE INFO]");

	char PSID[16] = { 0 };
	size_t PSIDlen = 16;
	int ret = kernel_sysctlbyname(curthread(), "machdep.openpsid", (char*)&PSID, &PSIDlen, NULL, NULL, NULL, 0);

	Log("PSID: %02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X", 
					 (PSID[0] & 0xffU), 
					 (PSID[1] & 0xffU), 
					 (PSID[2] & 0xffU), 
					 (PSID[3] & 0xffU), 
					 (PSID[4] & 0xffU), 
					 (PSID[5] & 0xffU), 
					 (PSID[6] & 0xffU), 
					 (PSID[7] & 0xffU), 
					 (PSID[8] & 0xffU), 
					 (PSID[9] & 0xffU), 
					 (PSID[10] & 0xffU), 
					 (PSID[11] & 0xffU), 
					 (PSID[12] & 0xffU), 
					 (PSID[13] & 0xffU), 
					 (PSID[14] & 0xffU), 
					 (PSID[15] & 0xffU));

	char IDPS[16] = { 0 };
	memcpy(IDPS, resolve(0x1CD0688), 16);

	Log("IDPS: %02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X", 
					 (IDPS[0] & 0xffU), 
					 (IDPS[1] & 0xffU), 
					 (IDPS[2] & 0xffU), 
					 (IDPS[3] & 0xffU), 
					 (IDPS[4] & 0xffU), 
					 (IDPS[5] & 0xffU), 
					 (IDPS[6] & 0xffU), 
					 (IDPS[7] & 0xffU), 
					 (IDPS[8] & 0xffU), 
					 (IDPS[9] & 0xffU), 
					 (IDPS[10] & 0xffU), 
					 (IDPS[11] & 0xffU), 
					 (IDPS[12] & 0xffU), 
					 (IDPS[13] & 0xffU), 
					 (IDPS[14] & 0xffU), 
					 (IDPS[15] & 0xffU));

	int ConsoleType = GetConsoleType(IDPS[5] & 0xffU);

	int sdk_version;
	size_t sdk_versionlen = 4;
	ret = kernel_sysctlbyname(curthread(), "kern.sdk_version", (char*)&sdk_version, &sdk_versionlen, NULL, NULL, NULL, 0);

	Log("sdk_version: %01X.%03X.%03X", (sdk_version >> 24) & 0xFF, (sdk_version >> 12) & 0xFFF, sdk_version & 0xFFF);

	int upd_version;
	size_t upd_versionlen = 4;
	ret = kernel_sysctlbyname(curthread(), "machdep.upd_version", (char*)&upd_version, &upd_versionlen, NULL, NULL, NULL, 0);

	Log("upd_version: %01X.%02X", (sdk_version >> 24) & 0xFF, (sdk_version >> 16) & 0xFF);

	/*char cputemp[100] = { 0 };
	size_t testlen = 100;
	ret = kernel_sysctlbyname(curthread(), "dev.amdtemp.1.core0.sensor0", (char*)&cputemp, &testlen, NULL, NULL, NULL, 0);
	Log("%d", ret);
	Log("%d", testlen);
	Log("%s", cputemp);*/

    proc* proc = GetCurrentGame();
	if(proc)
		strcpy(TargetInfo.CurrentTitleID, proc->titleId);
	else
	    strcpy(TargetInfo.CurrentTitleID, "XMB");

    Log("TitleID: %s", TargetInfo.CurrentTitleID);

    //Fill Response packet.
    TargetInfo.SDKVersion = sdk_version;
    TargetInfo.SoftwareVersion = upd_version;
    //CPU TEMPS 
	//Console Name
    memcpy(TargetInfo.IDPS, IDPS, 16);
    memcpy(TargetInfo.PSID, PSID, 16);
    TargetInfo.ConsoleType = ConsoleType;

	SendStatus(Socket, API_OK);

	Send(Socket, (char*)&TargetInfo, sizeof(RESP_TargetInfo));
}

void OrbisTarget::Shutdown(int Socket)
{
   DebugLog(LOGTYPE_WARN, "Not Implimented!");
   SendStatus(Socket, API_ERROR_FAIL);
}

void OrbisTarget::Reboot(int Socket)
{
    DebugLog(LOGTYPE_WARN, "Not Implimented!");
	SendStatus(Socket, API_ERROR_FAIL);
}

void OrbisTarget::Notify(int Socket, int Type, const char* Message)
{
    if(Type == -1)
        pHelperManager->pUserlandHelper->sceSysUtilSendNotificationRequest(Message);
    else
        pHelperManager->pUserlandHelper->sceSysUtilSendSystemNotificationWithText(Type, Message);
    
    SendStatus(Socket, API_OK);
}

void OrbisTarget::Beep(int Socket, int Count)
{
    if(Count > 4)
    {
        DebugLog(LOGTYPE_WARN, "Beep count cant be over 4.");

        SendStatus(Socket, API_ERROR_FAIL);

        return;
    }

    auto icc_indicator_set_buzzer = (void(*)(char))resolve(addr_icc_indicator_set_buzzer);
	icc_indicator_set_buzzer(Count);

    SendStatus(Socket, API_OK);
}

void OrbisTarget::SetLED(int Socket)
{
    DebugLog(LOGTYPE_WARN, "Not Implimented!");
	SendStatus(Socket, API_ERROR_FAIL);

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
}

void OrbisTarget::GetLED(int Socket)
{
    DebugLog(LOGTYPE_WARN, "Not Implimented!");

	SendStatus(Socket, API_ERROR_FAIL);
}

void OrbisTarget::DumpProcess(int Socket, const char* ProcessName)
{
	int64_t Size = 0;
	char* DumpedData = 0;
	size_t n = 0;
	int err = 0;
	proc* proc = proc_find_by_name(ProcessName);
	if(!proc)
	{
		DebugLog(LOGTYPE_ERR, "Failed to get process \"%s\".", ProcessName);

		SendStatus(Socket, API_ERROR_FAIL);

		return;
	}

	Size = (proc->p_vmspace->vm_tsize + proc->p_vmspace->vm_dsize) * PAGE_SIZE;

	DumpedData = (char*)_malloc(Size);
	if(!DumpedData)
	{
		DebugLog(LOGTYPE_ERR, "Failed to allocate space for Dump.");
		
		SendStatus(Socket, API_ERROR_FAIL);

		goto Cleanup;
	}

	err = proc_rw_mem(proc, proc->p_vmspace->vm_taddr, proc->p_vmspace->vm_tsize * PAGE_SIZE, DumpedData, &n, 0);
    if(err)
    {
        DebugLog(LOGTYPE_ERR, "Failed to read Text Segment Data: %d(%d).", err, n);

		SendStatus(Socket, API_ERROR_FAIL);

		goto Cleanup;

	}

	n = 0;
	err = proc_rw_mem(proc, proc->p_vmspace->vm_daddr, proc->p_vmspace->vm_dsize * PAGE_SIZE, DumpedData + (proc->p_vmspace->vm_tsize * PAGE_SIZE), &n, 0);
    if(err)
    {
        DebugLog(LOGTYPE_ERR, "Failed to read Data Segment Data: %d(%d).", err, n);

		SendStatus(Socket, API_ERROR_FAIL);

		goto Cleanup;
	}

	SendStatus(Socket, API_OK);

	Send(Socket, (char*)&Size, 0x8);
	Send(Socket, DumpedData, Size);

Cleanup:
	_free(DumpedData);

	char Buffer[0x200];
	sprintf(Buffer, "Dumped: %s(0x%llX).", ProcessName, Size);
	pHelperManager->pUserlandHelper->sceSysUtilSendNotificationRequest(Buffer);
}

void OrbisTarget::APIHandle(int Socket, API_Packet_s* Packet)
{
	switch(Packet->cmd)
	{
	case API_TARGET_INFO:
		Info(Socket);
		break;

	case API_TARGET_SHUTDOWN:
		Shutdown(Socket);
		break;

	case API_TARGET_REBOOT:
		Reboot(Socket);
		break;

	case API_TARGET_NOTIFY:
		Notify(Socket, Packet->Target_Notify.MessageType, Packet->Target_Notify.Message);
		break;

	case API_TARGET_BEEP:
		Beep(Socket, Packet->Target_Beep.Count);
		break;

	case API_TARGET_SET_LED:
		SetLED(Socket);
		break;

	case API_TARGET_GET_LED:
		GetLED(Socket);
		break;

	case API_TARGET_DUMP_PROC:
		DumpProcess(Socket, Packet->ProcName);
		break;
	}

}